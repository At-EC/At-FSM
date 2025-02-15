/**
 * Copyright (c) Riven Zheng (zhengheiot@gmail.com).
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 **/
#ifndef _HSM_H_
#define _HSM_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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
typedef unsigned int hsm_signal_t;

typedef unsigned short hsm_instance_t;
#define HSM_STATE_INSTANCE_ROOT    (0xFFFEu)
#define HSM_STATE_INSTANCE_INVALID (0xFFFFu)

typedef struct {
    hsm_signal_t signal;

    void *pUserContext;
} hsm_state_input_t;

typedef signed int (*pEntryFunc_t)(hsm_state_input_t);

typedef struct hsm_state {
    struct hsm_state *pMasterState;

    hsm_instance_t instance;

    unsigned int id;

    const char *pName;

    pEntryFunc_t pEntryFunc;
} hsm_state_t;

typedef signed int (*pHsmTransducerFunc_t)(const hsm_state_t *, hsm_instance_t, hsm_instance_t, hsm_state_input_t);

typedef struct {
    const hsm_state_t *pInitState;

    unsigned short number;

    hsm_instance_t current;

    hsm_instance_t middleware;

    pHsmTransducerFunc_t pTransucerFunc;
} hsm_state_manager_t;

signed int hsm_init(hsm_state_manager_t *pInitManagerContext, const hsm_state_t *pInitStateList, unsigned short number,
                    hsm_instance_t initInstance, pHsmTransducerFunc_t pTransucerFunc);
signed int hsm_inst_isInvalid(hsm_state_manager_t *pStateManager, hsm_instance_t instance);
const char *hsm_inst_nameGet(hsm_state_manager_t *pStateManager, hsm_instance_t instance);
signed int hsm_inst_idGet(hsm_state_manager_t *pStateManager, hsm_instance_t instance);
hsm_instance_t hsm_inst_middleware_get(hsm_state_manager_t *pStateManager);
hsm_instance_t hsm_inst_current_get(hsm_state_manager_t *pStateManager);
signed int hsm_activities(hsm_state_manager_t *pStateManager, hsm_state_input_t input);
signed int hsm_transition(hsm_state_manager_t *pStateManager, hsm_instance_t next);

#endif /* _HSM_H_ */
