/**
 * Copyright (c) Riven Zheng (zhengheiot@gmail.com).
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 **/
#include "psm.h"

/**
 * @brief Initialize a new PSM manager object.
 *
 * @param pInitManager The PSM manager context pointer.
 * @param pInitStateList The user PSM state list table.
 * @param initInstance The first start state' instance.
 * @param pTransucerFunc The transucer function pointer for state transition handler.
 *
 * @return The value of PSM init operaton result.
 */
i32_t psm_init(psm_state_manager_t *pInitManager, const psm_state_t *pInitStateList, psm_instance_t initInstance,
               pPsmTransducerFunc_t pTransucerFunc)
{
    if (!pInitManager) {
        return EOR_INVALID_ARGUMENT;
    }

    if (!pInitStateList) {
        return EOR_INVALID_ARGUMENT;
    }

    if (DIMOF(pInitStateList) > PSM_STATE_INSTANCE_INVALID) {
        return EOR_INVALID_ARGUMENT;
    }

    pInitManager->pInitState = pInitStateList;
    pInitManager->number = DIMOF(pInitStateList);
    pInitManager->current = initInstance;
    pInitManager->previous = PSM_STATE_INSTANCE_INVALID;
    pInitManager->exit_signal = PSM_SIGNAL_UNKNOWN;
    pInitManager->pTransucerFunc = pTransucerFunc;

    return 0;
}

/**
 * @brief To check if the PSM state instance is invalid.
 *
 * @param pStateManager The PSM manager context pointer.
 * @param instance The instance number of the state.
 *
 * @return The value of 0 indicates the instance is valid.
 */
i32_t psm_state_inst_isInvalid(psm_state_manager_t *pStateManager, psm_instance_t instance)
{
    if (!pStateManager) {
        return EOR_INVALID_ARGUMENT;
    }
    return ((instance < pStateManager->number) ? (0) : (EOR_INVALID_DATA));
}

/**
 * @brief Get the PSM state instance name.
 *
 * @param pStateManager The PSM manager context pointer.
 * @param instance The instance number of the state.
 *
 * @return The value of state name string.
 */
const char_t *psm_state_nameGet(psm_state_manager_t *pStateManager, psm_instance_t instance)
{
    if (!pStateManager) {
        return NULL;
    }
    if (psm_state_inst_isInvalid(pStateManager, instance)) {
        return NULL;
    }
    return pStateManager->pInitState[instance].pName;
}

/**
 * @brief Get the PSM state user defined id number.
 *
 * @param pStateManager The PSM manager context pointer.
 * @param instance The instance number of the state.
 *
 * @return The value of state id number.
 */
i32_t psm_state_idGet(psm_state_manager_t *pStateManager, psm_instance_t instance)
{
    if (!pStateManager) {
        return EOR_INVALID_ARGUMENT;
    }
    if (psm_state_inst_isInvalid(pStateManager, instance)) {
        return EOR_INVALID_ARGUMENT;
    }
    return pStateManager->pInitState[instance].id;
}

/**
 * @brief Get the PSM state current instance.
 *
 * @param pStateManager The PSM manager context pointer.
 *
 * @return The value of current state instance.
 */
psm_instance_t psm_inst_current_get(psm_state_manager_t *pStateManager)
{
    if (!pStateManager) {
        return PSM_STATE_INSTANCE_INVALID;
    }
    return pStateManager->current;
}

/**
 * @brief The PSM state schedule process.
 *
 * @param pStateManager The PSM manager context pointer.
 * @param input The user defined input signal and data context.
 *
 * @return The value of operation result.
 */
i32_t psm_activities(psm_state_manager_t *pStateManager, psm_state_input_t input)
{
    if (!pStateManager) {
        return EOR_INVALID_ARGUMENT;
    }

    pPsmEntryFunc_t pNextEntry = NULL;
    do {
        if (pStateManager->previous != pStateManager->current) {
            pStateManager->exit_signal = input.signal;
            input.signal = PSM_SIGNAL_EXIT;
            if (pStateManager->previous != PSM_STATE_INSTANCE_INVALID) {
                if ((u32_t)pStateManager->pInitState[pStateManager->previous].pEntryFunc(input) == PSM_FAULT_ERROR) {
                    break;
                }
            }

            if (pStateManager->pTransucerFunc) {
                if (pStateManager->pTransucerFunc(pStateManager->pInitState, pStateManager->previous, pStateManager->current, input) ==
                    PSM_FAULT_ERROR) {
                    break;
                }
            }

            input.signal = PSM_SIGNAL_ENTRY;
            if (pStateManager->previous == PSM_STATE_INSTANCE_INVALID) {
                if ((u32_t)pStateManager->pInitState[pStateManager->current].pEntryFunc(input) == PSM_FAULT_ERROR) {
                    break;
                }
                input.signal = pStateManager->exit_signal;
            }

            pStateManager->previous = pStateManager->current;
        }

        pNextEntry = (pPsmEntryFunc_t)pStateManager->pInitState[pStateManager->current].pEntryFunc(input);
    } while (pNextEntry && ((u32_t)pNextEntry != PSM_FAULT_ERROR));

    return (((u32_t)pNextEntry != PSM_FAULT_ERROR) ? (0) : (EOR_FAULT_ERROR));
}

/**
 * @brief The PSM state transition.
 *
 * @param pStateManager The PSM manager context pointer.
 * @param next The expected state instance.
 *
 * @return The value of operation result.
 */
void *psm_transition(psm_state_manager_t *pStateManager, psm_instance_t next)
{
    if (pStateManager->number >= next) {
        return (void *)PSM_FAULT_ERROR;
    }

    pStateManager->current = next;
    return (void *)pStateManager->pInitState[next].pEntryFunc;
}
