/*
 * File: RoachFSM.h
 * Author: Gabriel H Elkaim
 *
 * Simple flat state machine framework for the Roach project for CMPE-118 to get
 * you started on the project. The FSM simply ping pongs between two states on a
 * timer.
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_TattleTale.h"
#include "BOARD.h"
#include "BotFrameworkEvents.h"
//#include "BotFSM.h"
#include <stdio.h>
#include "Bot.h"
#include "Motor_X_FSM.h"
#include "Motor_Y_FSM.h"
#include "Presets_FSM.h"
// #include in any sub-state machines header files here


/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/



/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/


/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/

static Motor_X_State_t CurrentState = InitRState;
static Motor_X_Status_t CurrentStatus = resting;

static uint8_t MyPriority;
static ES_Event memEventX;
static int X_Value = 0, currentPosition = 0, positionDifference = 0, newPosition = 0;
unsigned int initialX = 0;

/* Use the Generate_String_Array_From_Enum program on the desktop to generate the
   StateNames array, this will be used with the code trace function. */
static const char *StateNames[] = {"InitRState",
    "selfCalibrateX",
    "waitingToCalibrate",
    "waitingX",
    "movingBufferX",
    "movingX",
    "movingX_Preset"};

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

uint8_t InitMotorXFSM(uint8_t Priority) {
    //printf("InitMotorXFSM\n");
    MyPriority = Priority;
    // put us into the Initial PseudoState
    CurrentState = InitRState;
    CurrentStatus = resting;

    // post the initial transition event
    if (ES_PostToService(MyPriority, INIT_EVENT) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

uint8_t PostMotorXFSM(ES_Event ThisEvent) {
    return ES_PostToService(MyPriority, ThisEvent);
}

Motor_X_State_t QueryMotorXFSM(void) {
    return (CurrentState);
}

ES_Event RunMotorXFSM(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE;
    Motor_X_State_t nextState;
    ES_Tattle(); // trace call stack
    static unsigned int eventParam = 0;
    static unsigned int i = 0;
    static unsigned int j = 0;

    switch (CurrentState) {
        case InitRState:
            if (ThisEvent.EventType == ES_INIT) {
                nextState = selfCalibrateX;
                makeTransition = TRUE;
            }
            break;


        case selfCalibrateX:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    Stepper_Init(MOTOR_X, 77);
                    reverseX(4000); //reverse to zero point
                    ES_Timer_InitTimer(MOTOR_X_TIMER, 5000);
                    break;

                case X_LIMIT:
                    if (ThisEvent.EventParam == X_MIN_MASK) {
                        ES_Event NewEvent;
                        NewEvent.EventType = CALIBRATED;
                        NewEvent.EventParam = MOTOR_X;
                        PostMotorYFSM(NewEvent);

                        initialX = getFilteredAD(X_SLIDER);
                        newPosition = AD_SCALE_X * initialX;
                        currentPosition = 0;
                        setStepsTaken(MOTOR_X, 0);
                        positionDifference = currentPosition - newPosition;

                        if (positionDifference < 0) {
                            //forwardX(abs(positionDifference));
                        } else if (positionDifference > 0) {
                            1; //do nothing, already at zero
                        }
                        Stepper_Halt(MOTOR_X);
                    }
                    break;
                    //case Y_LIMIT:

                    //TODO: for final, need to set length of timeout s.t there is \
                    an error if we dont hit limit in time
                    //remove calibration throw once this is done
                case ES_TIMEOUT:
                    if (ThisEvent.EventParam == MOTOR_X_TIMER) {
                        ES_Event NewEvent;
                        NewEvent.EventType = CALIBRATED;
                        NewEvent.EventParam = MOTOR_X;
                        PostMotorYFSM(NewEvent);

                        initialX = getFilteredAD(X_SLIDER);
                        newPosition = AD_SCALE_X * initialX;
                        currentPosition = 0;
                        setStepsTaken(MOTOR_X, 0);
                        positionDifference = currentPosition - newPosition;

                        if (positionDifference < 0) {
                            //forwardX(abs(positionDifference));
                        } else if (positionDifference > 0) {
                            1; //do nothing, already at zero
                        }
                        nextState = waitingX;
                        makeTransition = TRUE;
                    }
                    break;

                case ES_EXIT:
                    Stepper_Halt(MOTOR_X);
                    ES_Timer_StopTimer(MOTOR_X_TIMER);
                    break;

                default:
                    break;
            }
            break;

        case waitingX:
            if (ThisEvent.EventType != ES_NO_EVENT) {
                switch (ThisEvent.EventType) {
                    case ES_ENTRY:
                        CurrentStatus = resting;
                        break;

                    case XY_CHANGE:
                        nextState = movingBufferX;
                        makeTransition = TRUE;
                        memEventX = ThisEvent;
                        break;

                    case PRESETCHANGE:
                        eventParam = ThisEvent.EventParam;
                        nextState = movingX_Preset;
                        makeTransition = TRUE;
                        break;

                    case ES_EXIT:
                        Stepper_Resume(MOTOR_X);
                        setStepsRate(MOTOR_X, 77);
                        break;

                    default:
                        break;
                }
            }
            break;

        case movingBufferX:
            if (ThisEvent.EventType != ES_NO_EVENT) {
                switch (ThisEvent.EventType) {
                    case ES_ENTRY:
                        ES_Timer_InitTimer(MOTOR_X_TIMER, 200);
                        break;

                    case XY_CHANGE:
                        nextState = movingBufferX;
                        makeTransition = TRUE;
                        memEventX = ThisEvent;
                        break;

                    case ES_TIMEOUT:
                        if (ThisEvent.EventParam = MOTOR_X_TIMER) {
                            nextState = movingX;
                            makeTransition = TRUE;
                        }
                        break;

                    case ES_EXIT:
                        //setStepsRate(MOTOR_X, 77);
                        break;

                    default:
                        break;
                }
            }
            break;

        case movingX:
            if (ThisEvent.EventType != ES_NO_EVENT) {

                switch (ThisEvent.EventType) {
                    case ES_ENTRY:
                        PORTX05_BIT = 1;
                        Stepper_Resume(MOTOR_X);
                        //Stepper_Init(MOTOR_X, 77); //Dont think this is necessary..
                        //if last state was decelerating, sample the current position
                        newPosition = AD_SCALE_X * ReadXSlider();
                        //newPosition = AD_SCALE_X * memEventX.EventParam;
                        currentPosition = getStepsTaken(MOTOR_X);
                        positionDifference = currentPosition - newPosition;
                        if (positionDifference < 0) {
                            if (CurrentStatus == reversing) {
                                nextState = decelerating;
                                makeTransition = TRUE;
                                goto END_ENTRY_MOVING_X;
                            }
                            setStepsRate(MOTOR_X, 77);
                            forwardX(abs(positionDifference));
                            CurrentStatus = forwarding;
                        } else {
                            if (CurrentStatus == forwarding) {
                                nextState = decelerating;
                                makeTransition = TRUE;
                                goto END_ENTRY_MOVING_X;
                            }
                            setStepsRate(MOTOR_X, 77);
                            reverseX(abs(positionDifference));
                            CurrentStatus = reversing;
                        }
END_ENTRY_MOVING_X:
                        break;

                    case ALMOSTOUTOFSTEPS:
                        //PORTX05_BIT = ~PORTX05_BIT;
                        setStepsRate(MOTOR_X, 5);
                        break;

                    case OUTOFSTEPS:
                        //                    Stepper_Halt_Hard(MOTOR_X);
                        Stepper_Halt(MOTOR_X);
                        ES_Timer_InitTimer(MOTOR_X_TIMER, 50);
                        setStepsRate(MOTOR_X, 77);
                        break;

                        /*
                      case X_LIMIT:
                          Stepper_Halt_Hard(MOTOR_X);
                          nextState = waitingX;
                          makeTransition = TRUE;
                          break;
                         */

                    case ES_TIMEOUT:
                        if (ThisEvent.EventParam == MOTOR_X_TIMER) {
                            Stepper_Halt(MOTOR_X);
                            nextState = waitingX;
                            makeTransition = TRUE;
                        }
                        break;

                    case XY_CHANGE:
                        nextState = movingBufferX;
                        makeTransition = TRUE;
                        memEventX = ThisEvent;
                        break;

                    case ES_EXIT:
                        PORTX05_BIT = 0;

                        //Stepper_End(MOTOR_X);
                        //setStepsRate(MOTOR_Y, 77);
                        ES_Timer_StopTimer(MOTOR_X);
                        break;

                    default:
                        break;
                }
            }
            break;

        case decelerating:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    setStepsRate(MOTOR_X, 5);
                    ES_Timer_InitTimer(MOTOR_X_TIMER, 1000);
                    break;

                case XY_CHANGE:
                    memEventX = ThisEvent;
                    break;
                    /*�

                    case NEARLIMIT:
                        setStepsRate(MOTOR_X, 5);
                        break;

                    case X_LIMIT:
                        Stepper_Halt_Hard(MOTOR_X);
                        nextState = haltHard;
                        makeTransition = TRUE;
                        break;

                     * * */

                case OUTOFSTEPS:
                    Stepper_Halt_Hard(MOTOR_X);
                    nextState = haltHard;
                    makeTransition = TRUE;
                    //ES_Timer_InitTimer(MOTOR_X_TIMER, 50);
                    break;

                case ES_TIMEOUT:
                    if (ThisEvent.EventParam = MOTOR_X_TIMER) {
                        Stepper_Halt_Hard(MOTOR_X);
                        nextState = haltHard;
                        makeTransition = TRUE;
                    }
                    break;

                case ES_EXIT:
                    break;

                default:
                    break;
            }
            break;

            
        case haltHard:
            switch (ThisEvent.EventType) {
                case ES_ENTRY:
                    ES_Timer_InitTimer(MOTOR_X_TIMER, 100);
                    break;

                case XY_CHANGE:
                    memEventX = ThisEvent;
                    break;

                case ES_TIMEOUT:
                    if (ThisEvent.EventParam = MOTOR_X_TIMER) {
                        nextState = movingX;
                        makeTransition = TRUE;
                    }
                    break;

                case ES_EXIT:
                    setStepsRate(MOTOR_X, 77);
                    CurrentStatus = resting;
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    if (makeTransition == TRUE) {
        RunMotorXFSM(EXIT_EVENT);
        CurrentState = nextState;
        RunMotorXFSM(ENTRY_EVENT);
    }
    //printf("Leaving RunMotorXFSM with CurrentState = %d\n", CurrentState);
    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/


