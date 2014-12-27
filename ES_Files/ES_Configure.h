/****************************************************************************
 Module
     ES_Configure.h
 Description
     This file contains macro definitions that are edited by the user to
     adapt the Events and Services framework to a particular application.
 Notes
     
 History
 When           Who     What/Why
 -------------- ---     --------
 01/15/12 10:03 jec      started coding
 *****************************************************************************/

#ifndef CONFIGURE_H
#define CONFIGURE_H


//defines for keyboard input
//#define USE_KEYBOARD_INPUT
//What State machine are we testing
//#define POSTFUNCTION_FOR_KEYBOARD_INPUT PostmotorXHSM

//define for TattleTale
//#define USE_TATTLETALE

//uncomment to supress the entry and exit events
//#define SUPPRESS_EXIT_ENTRY_IN_TATTLE

/****************************************************************************/
// Name/define the events of interest
// Universal events occupy the lowest entries, followed by user-defined events

typedef enum {
    ES_NO_EVENT = 0,
    ES_ERROR, /* used to indicate an error from the service */
    ES_INIT, /* used to transition from initial pseudo-state */
    ES_ENTRY,
    ES_EXIT, /* used to exit a state*/
    ES_KEYINPUT, /* used to signify a key has been pressed*/
    ES_LISTEVENTS, /* used to list events in keyboard input, does not get posted to fsm*/
    ES_TIMEOUT, /* signals that the timer has expired */
    ES_TIMERACTIVE, /* signals that a timer has become active */
    ES_TIMERSTOPPED, /* signals that a timer has stopped*/
    /* User-defined events start here */
    //10

    OUTOFSTEPS,
    XY_CHANGE,
    AZIMUTH_ELEVATION_CHANGE,
    PRESETCHANGE,
    LONG_PRESETCHANGE, //15
    PRESETMATCHX,
    PRESETMATCHY,
    PRESETMATCHA,
    NOPRESETMATCHX,
    NOPRESETMATCHY,

    NOPRESETMATCHA,
    X_LIMIT,
    Y_LIMIT,
    XY_POT_ZERO,
    XY_POT_MAX,
    CALIBRATE,
    CALIBRATED,
    ALMOSTOUTOFSTEPS,
    SPEEDMATCH,
    NEARLIMIT,

    DECELPOINT,
    ACCELPOINT


} ES_EventTyp_t;

static const char *EventNames[] = {"ES_NO_EVENT",
    "ES_ERROR",
    "ES_INIT",
    "ES_ENTRY",
    "ES_EXIT",
    "ES_KEYINPUT",
    "ES_LISTEVENTS",
    "ES_TIMEOUT",
    "ES_TIMERACTIVE",
    "ES_TIMERSTOPPED",

    "OUTOFSTEPS",
    "XY_CHANGE",
    "AZIMUTH_ELEVATION_CHANGE",
    "PRESETCHANGE",
    "LONGPRESETCHANGE",
    "PRESETMATCHX",
    "PRESETMATCHY"
    "PRESETMATCHA",
    "NOPRESETMATCHX",
    "NOPRESETMATCHY",

    "NOPRESETMATCHA",
    "X_LIMIT",
    "Y_LIMIT",
    "XY_POT_ZERO",
    "XY_POT_MAX",
    "CALIBRATE",
    "CALIBRATED",
    "ALMOSTOUTOFSTEPS",
    "SPEEDMATCH",
    "NEARLIMIT",

    "DECELPOINT",
    "ACCELPOINT"};

#define NUMBEROFEVENTS (32)

/****************************************************************************/
// This are the name of the Event checking funcion header file.
#define EVENT_CHECK_HEADER "BotFrameworkEvents.h"

/****************************************************************************/
// This is the list of event checking functions
#define EVENT_CHECK_LIST check_Steps, check_Almost_Steps, check_XY_Sliders, check_Azimuth, check_Limits

/**********************************************************************/
// These are the definitions for the post functions to be executed when the
// correspnding timer expires. All 8 must be defined. If you are not using
// a timers, then you can use TIMER_UNUSED
#define TIMER_UNUSED ((pPostFunc)0)
#define TIMER0_RESP_FUNC PostTimerService
#define TIMER1_RESP_FUNC PostTimerService
#define TIMER2_RESP_FUNC PostTimerService
#define TIMER3_RESP_FUNC PostTimerService
#define TIMER4_RESP_FUNC PostTimerService
#define TIMER5_RESP_FUNC PostTimerService
#define TIMER6_RESP_FUNC PostTimerService
#define TIMER7_RESP_FUNC PostTimerService
//user modifiable timers start below here
#define TIMER8_RESP_FUNC PostMotorXFSM
#define TIMER9_RESP_FUNC PostMotorYFSM
#define TIMER10_RESP_FUNC PostServoAzimuthFSM
#define TIMER11_RESP_FUNC TIMER_UNUSED
#define TIMER12_RESP_FUNC TIMER_UNUSED
#define TIMER13_RESP_FUNC TIMER_UNUSED
#define TIMER14_RESP_FUNC TIMER_UNUSED
#define TIMER15_RESP_FUNC TIMER_UNUSED


/****************************************************************************/
// Give the timer numbers symbolc names to make it easier to move them
// to different timers if the need arises. Keep these definitons close to the
// definitions for the response functions to make it easire to check that
// the timer number matches where the timer event will be routed
#define MOTOR_X_TIMER 8
#define MOTOR_Y_TIMER 9
#define SERVO_A_TIMER 10
//#define PRESET_TIMER 11

/****************************************************************************/
// The maximum number of services sets an upper bound on the number of 
// services that the framework will handle. Reasonable values are 8 and 16
// HOWEVER: at this time only a value of 8 is supported.
#define MAX_NUM_SERVICES 8
/****************************************************************************/
// This macro determines that nuber of services that are *actually* used in
// a particular application. It will vary in value from 1 to MAX_NUM_SERVICES
#define NUM_SERVICES 5

/****************************************************************************/
// These are the definitions for Service 0, the lowest priority service
// every Events and Services application must have a Service 0. Further 
// services are added in numeric sequence (1,2,3,...) with increasing 
// priorities
// the header file with the public fuction prototypes
#define SERV_0_HEADER "ES_TimerService.h"
// the name of the Init function
#define SERV_0_INIT InitTimerService
// the name of the run function
#define SERV_0_RUN RunTimerService
// How big should this service's Queue be?
#define SERV_0_QUEUE_SIZE 9

/****************************************************************************/
// These are the definitions for Service 1
#if NUM_SERVICES > 1
// the header file with the public fuction prototypes
#define SERV_1_HEADER "ES_KeyboardInput.h"
// the name of the Init function
#define SERV_1_INIT InitKeyboardInput
// the name of the run function
#define SERV_1_RUN RunKeyboardInput
// How big should this services Queue be?
#define SERV_1_QUEUE_SIZE 9
#endif
/****************************************************************************/
// These are the definitions for Service 2
#if NUM_SERVICES > 2
// the header file with the public fuction prototypes
#define SERV_2_HEADER "Motor_Y_FSM.h"
// the name of the Init function
#define SERV_2_INIT InitMotorYFSM
// the name of the run function
#define SERV_2_RUN RunMotorYFSM
// How big should this services Queue be?
#define SERV_2_QUEUE_SIZE 3
#endif
/****************************************************************************/
// These are the definitions for Service 3
#if NUM_SERVICES > 3
// the header file with the public fuction prototypes
#define SERV_3_HEADER "Motor_X_FSM.h"
// the name of the Init function
#define SERV_3_INIT InitMotorXFSM
// the name of the run function
#define SERV_3_RUN RunMotorXFSM
// How big should this services Queue be?
#define SERV_3_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 4
#if NUM_SERVICES > 4
// the header file with the public fuction prototypes
#define SERV_4_HEADER "Servo_Azimuth_FSM.h"
// the name of the Init function
#define SERV_4_INIT InitServoAzimuthFSM
// the name of the run function
#define SERV_4_RUN RunServoAzimuthFSM
// How big should this services Queue be?
#define SERV_4_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 5
#if NUM_SERVICES > 5
// the header file with the public fuction prototypes
#define SERV_5_HEADER "Motor_X_Accel_FSM.h"
// the name of the Init function
#define SERV_5_INIT InitMotorX_AccelFSM
// the name of the run function
#define SERV_5_RUN RunMotorX_AccelFSM
// How big should this services Queue be?
#define SERV_5_QUEUE_SIZE 3
#endif

/****************************************************************************/
// These are the definitions for Service 6
#if NUM_SERVICES > 6
// the header file with the public fuction prototypes
#define SERV_6_HEADER "Motor_Y_Accel_FSM.h"
// the name of the Init function
#define SERV_6_INIT InitMotorY_AccelFSM
// the name of the run function
#define SERV_6_RUN RunMotorY_AccelFSM
// How big should this services Queue be?
#define SERV_6_QUEUE_SIZE 3
#endif

/****************************************************************************/
//TODO: Consider running UART as a service
#if NUM_SERVICES > 7
// the header file with the public fuction prototypes
#define SERV_7_HEADER "TestService.h"
// the name of the Init function
#define SERV_7_INIT TestServiceInit
// the name of the run function
#define SERV_7_RUN TestServiceRun
// How big should this services Queue be?
#define SERV_7_QUEUE_SIZE 3
#endif

/****************************************************************************/
// the name of the posting function that you want executed when a new 
// keystroke is detected.
// The default initialization distributes keystrokes to all state machines
#define POST_KEY_FUNC ES_PostAll



/****************************************************************************/
// These are the definitions for the Distribution lists. Each definition
// should be a comma seperated list of post functions to indicate which
// services are on that distribution list.
#define NUM_DIST_LISTS 0
#if NUM_DIST_LISTS > 0 
#define DIST_LIST0 PostmotorXHSM, PostmotorYHSM
#endif
#if NUM_DIST_LISTS > 1 
#define DIST_LIST1 PostTemplateFSM
#endif
#if NUM_DIST_LISTS > 2 
#define DIST_LIST2 PostTemplateFSM
#endif
#if NUM_DIST_LISTS > 3 
#define DIST_LIST3 PostTemplateFSM
#endif
#if NUM_DIST_LISTS > 4 
#define DIST_LIST4 PostTemplateFSM
#endif
#if NUM_DIST_LISTS > 5 
#define DIST_LIST5 PostTemplateFSM
#endif
#if NUM_DIST_LISTS > 6 
#define DIST_LIST6 PostTemplateFSM
#endif
#if NUM_DIST_LISTS > 7 
#define DIST_LIST7 PostTemplateFSM
#endif



#endif /* CONFIGURE_H */