/**
 * Copyright (c) Riven Zheng (zhengheiot@gmail.com).
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 **/
#ifndef _HSM_H_
#define _HSM_H_

#include <stdbool.h>
#include <stddef.h>

/* Error codes */
#define HSM_OK               (0)
#define EOR_INVALID_ARGUMENT (-1)
#define EOR_INVALID_DATA     (-2)
#define EOR_FAULT_ERROR      (-3)

#define HSM_ACTION_DONE (0)

/* State signals */
enum hsm_signal {
    HSM_SIGNAL_UNKNOWN = 0u,
    HSM_SIGNAL_ENTRY,       /* Called when entering a state */
    HSM_SIGNAL_INIT,        /* Called for initial setup after entry */
    HSM_SIGNAL_EXIT,        /* Called when exiting a state */
    HSM_SIGNAL_USER_DEFINE, /* User-defined signals start here */
};
typedef unsigned int hsm_signal_t;

/* Signal dispatch modes (for future extensibility) */
enum hsm_signal_mode {
    HSM_SIGNAL_MODE_PASS_THROUGH = 0u,   /* Signals propagate from root to leaf through hierarchy */
    HSM_SIGNAL_MODE_CURRENT_NODE,        /* Signals only dispatch to current active state */
};
typedef unsigned int hsm_signal_mode_t;

/* State instance identifiers */
typedef unsigned short hsm_instance_t;
#define HSM_STATE_INSTANCE_ROOT    (0xFFFEu)
#define HSM_STATE_INSTANCE_INVALID (0xFFFFu)

/* Input event structure */
typedef struct {
    hsm_signal_t signal;
    void *pUserContext;
} hsm_state_input_t;

/* State handler function type */
typedef signed int (*hsm_state_handler_t)(hsm_state_input_t);

/* State definition */
typedef struct hsm_state {
    struct hsm_state *pParent;      /* Parent state in hierarchy (NULL for top-level) */
    hsm_instance_t instance;        /* Index in state array */
    unsigned int id;                /* User-defined state ID */
    const char *pName;              /* State name for debugging */
    hsm_state_handler_t pHandler;   /* State handler function */
} hsm_state_t;

/* Transducer callback for state transitions */
typedef signed int (*hsm_transducer_t)(const hsm_state_t *pStates,
                                       hsm_instance_t fromState,
                                       hsm_instance_t toState,
                                       hsm_state_input_t input);

/* State manager context */
typedef struct {
    const hsm_state_t *pStates;     /* Array of state definitions */
    unsigned short stateCount;       /* Number of states in array */
    hsm_instance_t currentState;     /* Currently active state instance */
    hsm_instance_t processingState;  /* State being processed (during transitions) */
    bool passThroughMode;            /* true: pass through mode, false: current node mode */
    hsm_transducer_t pTransducer;    /* Optional transition callback */
} hsm_state_manager_t;

/* Public API */
signed int hsm_init(hsm_state_manager_t *pManager,
                    const hsm_state_t *pStateList,
                    unsigned short stateCount,
                    hsm_instance_t initialState,
                    bool passThrough,
                    hsm_transducer_t pTransducer);
signed int hsm_state_isValid(hsm_state_manager_t *pManager, hsm_instance_t instance);
const char *hsm_state_getName(hsm_state_manager_t *pManager, hsm_instance_t instance);
signed int hsm_state_getId(hsm_state_manager_t *pManager, hsm_instance_t instance);
hsm_instance_t hsm_getProcessingState(hsm_state_manager_t *pManager);
const char *hsm_getCurrentStateName(hsm_state_manager_t *pManager);
hsm_instance_t hsm_getTargetState(hsm_state_manager_t *pManager);
const char *hsm_getTargetStateName(hsm_state_manager_t *pManager);
signed int hsm_dispatch(hsm_state_manager_t *pManager, hsm_state_input_t input);
signed int hsm_transition(hsm_state_manager_t *pManager, hsm_instance_t nextState);

/* Backward compatibility macros */
#define pMasterState          pParent
#define pEntryFunc            pHandler
#define pInitState            pStates
#define number                stateCount
#define current               currentState
#define middleware            processingState
#define pTransucerFunc        pTransducer
#define hsm_activities        hsm_dispatch
#define hsm_state_isInvalid(m, i)   (!hsm_state_isValid((m), (i)) ? 0 : EOR_INVALID_DATA)
#define hsm_state_nameGet     hsm_state_getName
#define hsm_state_idGet       hsm_state_getId
#define hsm_inst_middleware_get  hsm_getProcessingState
#define hsm_inst_current_get     hsm_getTargetState

#endif /* _HSM_H_ */
