/**
 * Copyright (c) Riven Zheng (zhengheiot@gmail.com).
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 **/

#include "hsm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize a new HSM manager object.
 *
 * @param pInitManager The PSM manager context pointer.
 * @param pInitStateList The user PSM state list table.
 * @param initInstance The first start state' instance.
 * @param pTransucerFunc The transucer function pointer for state transaction handler.
 *
 * @return The value of PSM init operaton result.
 */
hsm_result_t hsm_init(hsm_state_manager_t *pInitManagerContext, const hsm_state_t *pInitStateList, hsm_instance_t initInstance,
                      pHsmTransducerFunc_t pTransucerFunc)
{
    if (!pInitManagerContext) {
        return HSM_RESULT_INVALID_ARGUMENT;
    }

    if (!pInitStateList) {
        return HSM_RESULT_INVALID_ARGUMENT;
    }

    if (DIMOF(pInitStateList) >= (hsm_signal_t)HSM_STATE_INSTANCE_INVALID) {
        return HSM_RESULT_INVALID_ARGUMENT;
    }

    pInitManagerContext->pInitState = pInitStateList;
    pInitManagerContext->number = DIMOF(pInitStateList);
    pInitManagerContext->current = HSM_STATE_INSTANCE_ROOT;
    pInitManagerContext->middleware = initInstance;
    pInitManagerContext->pTransucerFunc = pTransucerFunc;

    return HSM_RESULT_PASS;
}

/**
 * @brief To check if the HSM state instance is invalid.
 *
 * @param pStateManager The HSM manager context pointer.
 * @param instance The instance number of the state.
 *
 * @return The value of PSM_RESULT_PASS indicates the instance is valid.
 */
hsm_result_t hsm_state_isInvalid(hsm_state_manager_t *pStateManager, hsm_instance_t instance)
{
    if (!pStateManager) {
        return HSM_RESULT_INVALID_ARGUMENT;
    }
    return ((instance < pStateManager->number) ? (HSM_RESULT_PASS) : (HSM_RESULT_INVALID_DATA));
}

/**
 * @brief Get the HSM state instance name.
 *
 * @param pStateManager The HSM manager context pointer.
 * @param instance The instance number of the state.
 *
 * @return The value of state name string.
 */
const char_t *hsm_state_nameGet(hsm_state_manager_t *pStateManager, hsm_instance_t instance)
{
    if (!pStateManager) {
        return NULL;
    }
    if (hsm_state_isInvalid(pStateManager, instance)) {
        return NULL;
    }
    return pStateManager->pInitState[instance].pName;
}

/**
 * @brief Get the HSM state user defined id number.
 *
 * @param pStateManager The HSM manager context pointer.
 * @param instance The instance number of the state.
 *
 * @return The value of state id number.
 */
u32_t hsm_state_idGet(hsm_state_manager_t *pStateManager, hsm_instance_t instance)
{
    if (!pStateManager) {
        return HSM_RESULT_INVALID_ARGUMENT;
    }
    if (hsm_state_isInvalid(pStateManager, instance)) {
        return HSM_RESULT_INVALID_ARGUMENT;
    }
    return pStateManager->pInitState[instance].id;
}

/**
 * @brief Get the HSM state current instance.
 *
 * @param pStateManager The HSM manager context pointer.
 *
 * @return The value of middleware state instance.
 */
hsm_instance_t hsm_inst_middleware_get(hsm_state_manager_t *pStateManager)
{
    if (!pStateManager) {
        return HSM_STATE_INSTANCE_INVALID;
    }
    return pStateManager->middleware;
}

/**
 * @brief Get the HSM state current instance.
 *
 * @param pStateManager The HSM manager context pointer.
 *
 * @return The value of current state instance.
 */
hsm_instance_t hsm_inst_current_get(hsm_state_manager_t *pStateManager)
{
    if (!pStateManager) {
        return HSM_STATE_INSTANCE_INVALID;
    }
    return pStateManager->current;
}

/**
 * @brief The HSM state schedule process.
 *
 * @param pStateManager The HSM manager context pointer.
 * @param input The user defined input signal and data context.
 *
 * @return The value of operation result.
 */
hsm_result_t hsm_activities(hsm_state_manager_t *pStateManager, hsm_state_input_t input)
{
    if (!pStateManager) {
        return HSM_RESULT_INVALID_ARGUMENT;
    }

    hsm_state_t *pCurState = NULL;
    hsm_state_t *pTmpState = NULL;
    hsm_state_t *pTmpTarget = NULL;
    hsm_state_t *pNewState = NULL;
    hsm_state_input_t sv_input = {0u};
    b_t root = FALSE;
    hsm_result_t ret = HSM_ACTION_DONE;

    if (pStateManager->current == HSM_STATE_INSTANCE_ROOT) {
        pTmpState = (hsm_state_t *)&pStateManager->pInitState[pStateManager->middleware];
        sv_input = input;
        root = TRUE;
    } else {
        pCurState = (hsm_state_t *)&pStateManager->pInitState[pStateManager->current];
        pTmpState = pCurState;
    }

    while (((pCurState != pTmpTarget) && (ret == HSM_ACTION_DONE)) || (pStateManager->current == HSM_STATE_INSTANCE_ROOT)) {
        while (pTmpState->pMasterState != pTmpTarget) {
            pTmpState = pTmpState->pMasterState;
        }

        if (pStateManager->current != HSM_STATE_INSTANCE_ROOT) {
            pStateManager->middleware = pTmpState->instance;
            if (pTmpState->pEntryFunc(input)) {
                return HSM_RESULT_FAULT_ERROR;
            }
        } else {
            pStateManager->current = pStateManager->middleware;
        }

        if ((pTmpState == pCurState) && (input.signal == HSM_SIGNAL_ENTRY)) {
            pStateManager->middleware = pTmpState->instance;
            input.signal = HSM_SIGNAL_INIT;
            if (pTmpState->pEntryFunc(input)) {
                return HSM_RESULT_FAULT_ERROR;
            }

            if ((root) && (sv_input.signal != HSM_SIGNAL_INIT)) {
                input = sv_input;
                if (pTmpState->pEntryFunc(input)) {
                    return HSM_RESULT_FAULT_ERROR;
                }
            }
        }

        pNewState = (hsm_state_t *)&pStateManager->pInitState[pStateManager->current];
        if (pCurState != pNewState) {
            hsm_state_t *pSameMasterState = NULL;
            hsm_state_t *pTmpFromState = pCurState;
            hsm_state_t *pTmpToState = pNewState;

            while ((pTmpFromState) && (!pSameMasterState)) {
                while ((pTmpToState) && (!pSameMasterState)) {
                    if (pTmpFromState->pMasterState == pTmpToState->pMasterState) {
                        pSameMasterState = pTmpFromState->pMasterState;
                    }
                    pTmpToState = pTmpToState->pMasterState;
                }
                pTmpFromState = pTmpFromState->pMasterState;
            }

            pTmpFromState = pCurState;
            sv_input = input;
            input.signal = HSM_SIGNAL_EXIT;
            while ((pTmpFromState != pSameMasterState) && (pTmpFromState != pNewState) && (ret == HSM_ACTION_DONE)) {
                pStateManager->middleware = pTmpFromState->instance;
                if (pTmpFromState->pEntryFunc(input)) {
                    return HSM_RESULT_FAULT_ERROR;
                }
                pTmpFromState = pTmpFromState->pMasterState;
            }

            if (pStateManager->pTransucerFunc) {
                hsm_instance_t inst = (pCurState) ? (pCurState->instance) : (HSM_STATE_INSTANCE_ROOT);
                if (pStateManager->pTransucerFunc(pStateManager->pInitState, inst, pStateManager->current, sv_input)) {
                    return HSM_RESULT_FAULT_ERROR;
                }
            }

            input.signal = HSM_SIGNAL_ENTRY;
            pCurState = pNewState;
            pTmpTarget = pSameMasterState;
        } else {
            pTmpTarget = pTmpState;
        }

        pTmpState = pCurState;
    }

    return ret;
}

/**
 * @brief The HSM state transaction.
 *
 * @param pStateManager The PSM manager context pointer.
 * @param next The expected state instance.
 *
 * @return The value of operation result.
 */
hsm_result_t hsm_transaction(hsm_state_manager_t *pStateManager, hsm_instance_t next)
{
    if (next <= pStateManager->number) {
        return HSM_RESULT_INVALID_ARGUMENT;
    }

    pStateManager->current = next;
    return HSM_RESULT_PASS;
}

#ifdef __cplusplus
}
#endif
