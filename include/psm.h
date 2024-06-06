/**
 * Copyright (c) Riven Zheng (zhengheiot@gmail.com).
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 **/

#ifndef _PSM_H_
#define _PSM_H_

#include "typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

enum psm_result {
    PSM_RESULT_PASS = 0u,
    PSM_RESULT_INVALID_ARGUMENT,
    PSM_RESULT_INVALID_DATA,
    PSM_RESULT_FAULT_ERROR,
};
typedef u32_t psm_result_t;

enum psm_signal {
    PSM_SIGNAL_UNKNOWN = 0u,
    PSM_SIGNAL_ENTRY,
    PSM_SIGNAL_EXIT,
    PSM_SIGNAL_USER_DEFINE,
};
typedef u32_t psm_signal_t;

typedef u16_t psm_instance_t;
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

    u32_t id;

    const char_t *pName;

    pPsmEntryFunc_t pEntryFunc;
} psm_state_t;

typedef u32p_t (*pPsmTransducerFunc_t)(const psm_state_t *, psm_instance_t, psm_instance_t, psm_state_input_t);

typedef struct {
    const psm_state_t *pInitState;

    u16_t number;

    psm_instance_t previous;

    psm_instance_t current;

    psm_signal_t exit_signal;

    pPsmTransducerFunc_t pTransucerFunc;
} psm_state_manager_t;

psm_result_t psm_init(psm_state_manager_t *pInitManager, const psm_state_t *pInitStateList, psm_instance_t initInstance,
                      pPsmTransducerFunc_t pTransucerFunc);
psm_result_t psm_state_inst_isInvalid(psm_state_manager_t *pStateManager, psm_instance_t instance);
const char_t *psm_state_nameGet(psm_state_manager_t *pStateManager, psm_instance_t instance);
psm_result_t psm_state_idGet(psm_state_manager_t *pStateManager, psm_instance_t instance);
psm_instance_t psm_inst_current_get(psm_state_manager_t *pStateManager);
psm_result_t psm_activities(psm_state_manager_t *pStateManager, psm_state_input_t input);
void *psm_transaction(psm_state_manager_t *pStateManager, psm_instance_t next);

#ifdef __cplusplus
}
#endif

#endif /* _PSM_H_ */
