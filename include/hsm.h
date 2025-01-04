/**
 * Copyright (c) Riven Zheng (zhengheiot@gmail.com).
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 **/
#ifndef _HSM_H_
#define _HSM_H_

#include "type_def.h"

#define EOR_INVALID_ARGUMENT (-1)
#define EOR_INVALID_DATA     (-2)
#define EOR_FAULT_ERROR      (-3)

#define HSM_ACTION_DONE (0)

enum hsm_signal {
    HSM_SIGNAL_UNKNOWN = (0u),
    HSM_SIGNAL_ENTRY,
    HSM_SIGNAL_INIT,
    HSM_SIGNAL_EXIT,
    HSM_SIGNAL_USER_DEFINE,
};
typedef u32_t hsm_signal_t;

typedef u16_t hsm_instance_t;
#define HSM_STATE_INSTANCE_ROOT    (0xFFFEu)
#define HSM_STATE_INSTANCE_INVALID (0xFFFFu)

typedef struct {
    hsm_signal_t signal;

    void *pUserContext;
} hsm_state_input_t;

typedef i32_t (*pEntryFunc_t)(hsm_state_input_t);

typedef struct hsm_state {
    struct hsm_state *pMasterState;

    hsm_instance_t instance;

    u32_t id;

    const char_t *pName;

    pEntryFunc_t pEntryFunc;
} hsm_state_t;

typedef i32_t (*pHsmTransducerFunc_t)(const hsm_state_t *, hsm_instance_t, hsm_instance_t, hsm_state_input_t);

typedef struct {
    const hsm_state_t *pInitState;

    u16_t number;

    hsm_instance_t current;

    hsm_instance_t middleware;

    pHsmTransducerFunc_t pTransucerFunc;
} hsm_state_manager_t;

i32_t hsm_init(hsm_state_manager_t *pInitManagerContext, const hsm_state_t *pInitStateList, hsm_instance_t initInstance,
               pHsmTransducerFunc_t pTransucerFunc);
i32_t hsm_inst_isInvalid(hsm_state_manager_t *pStateManager, hsm_instance_t instance);
const char_t *hsm_inst_nameGet(hsm_state_manager_t *pStateManager, hsm_instance_t instance);
u32_t hsm_inst_idGet(hsm_state_manager_t *pStateManager, hsm_instance_t instance);
hsm_instance_t hsm_inst_middleware_get(hsm_state_manager_t *pStateManager);
hsm_instance_t hsm_inst_current_get(hsm_state_manager_t *pStateManager);
i32_t hsm_activities(hsm_state_manager_t *pStateManager, hsm_state_input_t input);
i32_t hsm_transition(hsm_state_manager_t *pStateManager, hsm_instance_t next);

#endif /* _HSM_H_ */
