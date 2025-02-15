/**
 * Copyright (c) Riven Zheng (zhengheiot@gmail.com).
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 **/
#ifndef _PSM_H_
#define _PSM_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define EOR_INVALID_ARGUMENT (-1)
#define EOR_INVALID_DATA     (-2)
#define EOR_FAULT_ERROR      (-3)

enum psm_signal {
    PSM_SIGNAL_UNKNOWN = 0u,
    PSM_SIGNAL_ENTRY,
    PSM_SIGNAL_EXIT,
    PSM_SIGNAL_USER_DEFINE,
};
typedef unsigned int psm_signal_t;

typedef unsigned short psm_instance_t;
#define PSM_STATE_INSTANCE_INVALID (0xFFFFu)
#define PSM_FAULT_ERROR            (0xFFFFFFFFu)
#define PSM_ACTION_DONE            (NULL)

typedef struct {
    psm_signal_t signal;
    void *pUserContext;
} psm_state_input_t;

typedef void *(*pPsmEntryFunc_t)(psm_state_input_t);

typedef struct psm_state {
    psm_instance_t instance;

    unsigned int id;

    const char *pName;

    pPsmEntryFunc_t pEntryFunc;
} psm_state_t;

typedef signed int (*pPsmTransducerFunc_t)(const psm_state_t *, psm_instance_t, psm_instance_t, psm_state_input_t);

typedef struct {
    const psm_state_t *pInitState;

    unsigned short number;

    psm_instance_t previous;

    psm_instance_t current;

    psm_signal_t exit_signal;

    pPsmTransducerFunc_t pTransucerFunc;
} psm_state_manager_t;

signed int psm_init(psm_state_manager_t *pInitManager, const psm_state_t *pInitStateList, unsigned short number,
                    psm_instance_t initInstance, pPsmTransducerFunc_t pTransucerFunc);
signed int psm_state_inst_isInvalid(psm_state_manager_t *pStateManager, psm_instance_t instance);
const char *psm_state_nameGet(psm_state_manager_t *pStateManager, psm_instance_t instance);
signed int psm_state_idGet(psm_state_manager_t *pStateManager, psm_instance_t instance);
psm_instance_t psm_inst_current_get(psm_state_manager_t *pStateManager);
signed int psm_activities(psm_state_manager_t *pStateManager, psm_state_input_t input);
void *psm_transition(psm_state_manager_t *pStateManager, psm_instance_t next);

#endif /* _PSM_H_ */
