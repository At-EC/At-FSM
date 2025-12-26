/**
 * Copyright (c) Riven Zheng (zhengheiot@gmail.com).
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 **/
#include "hsm.h"

/*============================================================================
 * Private Helper Functions
 *============================================================================*/

/**
 * @brief Get state pointer by instance index.
 */
static inline hsm_state_t *hsm_getState(const hsm_state_manager_t *pManager, hsm_instance_t instance)
{
    return (hsm_state_t *)&pManager->pStates[instance];
}

/**
 * @brief Check if HSM is at root (not yet entered any state).
 */
static inline bool hsm_isAtRoot(const hsm_state_manager_t *pManager)
{
    return (pManager->currentState == HSM_STATE_INSTANCE_ROOT);
}

/**
 * @brief Invoke state handler with given signal.
 */
static inline signed int hsm_invokeHandler(hsm_state_manager_t *pManager,
                                           hsm_state_t *pState,
                                           hsm_state_input_t input)
{
    pManager->processingState = pState->instance;
    return pState->pHandler(input);
}

/**
 * @brief Find the Least Common Ancestor (LCA) of two states in the hierarchy.
 *
 * The LCA is the deepest state that is an ancestor of both fromState and toState.
 * This is used to determine which states to exit and enter during a transition.
 *
 * Special cases:
 * - If fromState is ancestor of toState (parent->child transition): returns fromState
 * - If toState is ancestor of fromState (child->parent transition): returns toState
 */
static hsm_state_t *hsm_findLCA(hsm_state_t *pFromState, hsm_state_t *pToState)
{
    /* Check if fromState is an ancestor of toState (e.g., INIT -> PREPARE where PREPARE's parent is INIT) */
    hsm_state_t *pIter = pToState;
    while (pIter != NULL) {
        if (pIter == pFromState) {
            return pFromState;  /* fromState is the LCA */
        }
        pIter = pIter->pParent;
    }

    /* Check if toState is an ancestor of fromState (e.g., PREPARE -> INIT where PREPARE's parent is INIT) */
    pIter = pFromState;
    while (pIter != NULL) {
        if (pIter == pToState) {
            return pToState;  /* toState is the LCA */
        }
        pIter = pIter->pParent;
    }

    /* Neither is ancestor of the other - find common ancestor by comparing hierarchies */
    hsm_state_t *pFromIter = pFromState;
    while (pFromIter != NULL) {
        hsm_state_t *pToIter = pToState;

        while (pToIter != NULL) {
            if (pFromIter == pToIter) {
                return pFromIter;  /* Found common ancestor */
            }
            pToIter = pToIter->pParent;
        }
        pFromIter = pFromIter->pParent;
    }

    return NULL;  /* No common ancestor (both are top-level states) */
}

/**
 * @brief Walk up the hierarchy to find topmost ancestor below target.
 *
 * This ensures we enter states from top-to-bottom in the hierarchy.
 */
static hsm_state_t *hsm_findTopmostBelow(hsm_state_t *pState, hsm_state_t *pTarget)
{
    while (pState->pParent != pTarget) {
        pState = pState->pParent;
    }
    return pState;
}

/**
 * @brief Exit states from current up to (but not including) the LCA.
 */
static signed int hsm_exitToLCA(hsm_state_manager_t *pManager,
                                hsm_state_t *pFromState,
                                hsm_state_t *pLCA,
                                hsm_state_t *pToState,
                                hsm_state_input_t input)
{
    input.signal = HSM_SIGNAL_EXIT;

    while ((pFromState != pLCA) && (pFromState != pToState)) {
        if (hsm_invokeHandler(pManager, pFromState, input)) {
            return EOR_FAULT_ERROR;
        }
        pFromState = pFromState->pParent;
    }

    return HSM_OK;
}

/**
 * @brief Notify transducer of state transition.
 */
static signed int hsm_notifyTransition(hsm_state_manager_t *pManager,
                                       hsm_state_t *pFromState,
                                       hsm_state_input_t input)
{
    if (pManager->pTransducer == NULL) {
        return HSM_OK;
    }

    hsm_instance_t fromInst = (pFromState != NULL) ? pFromState->instance : HSM_STATE_INSTANCE_ROOT;

    return pManager->pTransducer(pManager->pStates, fromInst, pManager->currentState, input);
}

/*============================================================================
 * Public API Implementation
 *============================================================================*/

/**
 * @brief Initialize a new HSM manager.
 *
 * @param pManager        The HSM manager context to initialize.
 * @param pStateList      Array of state definitions.
 * @param stateCount      Number of states in the array.
 * @param initialState    The initial state instance to start in.
 * @param passThrough     True: pass through mode, false: current node mode.
 * @param pTransducer     Optional callback for state transitions (can be NULL).
 *
 * @return HSM_OK on success, error code otherwise.
 */
signed int hsm_init(hsm_state_manager_t *pManager,
                    const hsm_state_t *pStateList,
                    unsigned short stateCount,
                    hsm_instance_t initialState,
                    bool passThrough,
                    hsm_transducer_t pTransducer)
{
    if (pManager == NULL || pStateList == NULL) {
        return EOR_INVALID_ARGUMENT;
    }

    pManager->pStates = pStateList;
    pManager->stateCount = stateCount;
    pManager->currentState = HSM_STATE_INSTANCE_ROOT;
    pManager->processingState = initialState;
    pManager->passThroughMode = passThrough;
    pManager->pTransducer = pTransducer;

    return HSM_OK;
}

/**
 * @brief Check if a state instance is valid.
 *
 * @param pManager  The HSM manager context.
 * @param instance  The state instance to validate.
 *
 * @return HSM_OK if valid, error code otherwise.
 */
signed int hsm_state_isValid(hsm_state_manager_t *pManager, hsm_instance_t instance)
{
    if (pManager == NULL) {
        return EOR_INVALID_ARGUMENT;
    }
    return (instance < pManager->stateCount) ? HSM_OK : EOR_INVALID_DATA;
}

/**
 * @brief Get the name of a state.
 *
 * @param pManager  The HSM manager context.
 * @param instance  The state instance.
 *
 * @return State name string, or NULL if invalid.
 */
const char *hsm_state_getName(hsm_state_manager_t *pManager, hsm_instance_t instance)
{
    if (pManager == NULL || hsm_state_isValid(pManager, instance) != HSM_OK) {
        return NULL;
    }
    return pManager->pStates[instance].pName;
}

/**
 * @brief Get the user-defined ID of a state.
 *
 * @param pManager  The HSM manager context.
 * @param instance  The state instance.
 *
 * @return State ID, or error code if invalid.
 */
signed int hsm_state_getId(hsm_state_manager_t *pManager, hsm_instance_t instance)
{
    if (pManager == NULL || hsm_state_isValid(pManager, instance) != HSM_OK) {
        return EOR_INVALID_ARGUMENT;
    }
    return (signed int)pManager->pStates[instance].id;
}

/**
 * @brief Get the state currently being processed.
 *
 * @param pManager  The HSM manager context.
 *
 * @return Processing state instance, or HSM_STATE_INSTANCE_INVALID if error.
 */
hsm_instance_t hsm_getProcessingState(hsm_state_manager_t *pManager)
{
    if (pManager == NULL) {
        return HSM_STATE_INSTANCE_INVALID;
    }
    return pManager->processingState;
}

/**
 * @brief Get the name of the current state.
 *
 * @param pManager  The HSM manager context.
 *
 * @return Current state name, or NULL if error.
 */
const char *hsm_getCurrentStateName(hsm_state_manager_t *pManager)
{
    if (pManager == NULL) {
        return NULL;
    }
    return pManager->pStates[pManager->processingState].pName;
}

/**
 * @brief Get the name of the target state.
 *
 * @param pManager  The HSM manager context.
 *
 * @return Target state name, or NULL if error.
 */
const char *hsm_getTargetStateName(hsm_state_manager_t *pManager)
{
    if (pManager == NULL) {
        return NULL;
    }
    return pManager->pStates[pManager->currentState].pName;
}

/**
 * @brief Get the target active state.
 *
 * @param pManager  The HSM manager context.
 *
 * @return target state instance, or HSM_STATE_INSTANCE_INVALID if error.
 */
hsm_instance_t hsm_getTargetState(hsm_state_manager_t *pManager)
{
    if (pManager == NULL) {
        return HSM_STATE_INSTANCE_INVALID;
    }
    return pManager->currentState;
}

/**
 * @brief Request a state transition.
 *
 * This should be called from within a state handler to request transition
 * to a new state. The actual transition is processed by hsm_dispatch().
 *
 * @param pManager   The HSM manager context.
 * @param nextState  The target state instance.
 *
 * @return HSM_OK on success, error code otherwise.
 */
signed int hsm_transition(hsm_state_manager_t *pManager, hsm_instance_t nextState)
{
    if (pManager == NULL) {
        return EOR_INVALID_ARGUMENT;
    }

    if (nextState >= pManager->stateCount) {
        return EOR_INVALID_ARGUMENT;
    }

    pManager->currentState = nextState;
    return HSM_OK;
}

/**
 * @brief Dispatch an event to the state machine.
 *
 * This is the main processing function that:
 * 1. On first call: enters from root through hierarchy to initial state
 * 2. Dispatches the input signal to current state handler
 * 3. If handler requested transition: exits old states, enters new states
 *
 * @param pManager  The HSM manager context.
 * @param input     The event to dispatch (signal + optional user context).
 *
 * @return HSM_OK on success, error code otherwise.
 */
signed int hsm_dispatch(hsm_state_manager_t *pManager, hsm_state_input_t input)
{
    if (pManager == NULL) {
        return EOR_INVALID_ARGUMENT;
    }

    hsm_state_t *pCurrentState = NULL;
    hsm_state_t *pWorkingState = NULL;
    hsm_state_t *pEntryTarget = NULL;
    hsm_state_t *pActiveState = NULL;
    hsm_state_input_t savedInput = {0};
    bool isInitialEntry = false;

    /* Determine starting point */
    if (hsm_isAtRoot(pManager)) {
        /* First dispatch: start from initial state, will enter from root */
        pWorkingState = hsm_getState(pManager, pManager->processingState);
        isInitialEntry = true;
    } else {
        /* Normal operation: process from current state */
        pCurrentState = hsm_getState(pManager, pManager->currentState);
        pWorkingState = pCurrentState;
        pActiveState = pCurrentState;
    }
    savedInput = input;

    /* Main state processing loop */
    while ((pCurrentState != pEntryTarget) || hsm_isAtRoot(pManager)) {
        /* Walk up to find topmost ancestor below entry target */
        pWorkingState = hsm_findTopmostBelow(pWorkingState, pEntryTarget);

        if (!hsm_isAtRoot(pManager)) {
            /* System signals (ENTRY, INIT, EXIT) or pass-through mode: dispatch to all states in hierarchy */
            if ((input.signal < HSM_SIGNAL_USER_DEFINE) || pManager->passThroughMode) {
                /* Call state handler (for system signals or pass-through mode) */
                if (hsm_invokeHandler(pManager, pWorkingState, input)) {
                    return EOR_FAULT_ERROR;
                }
            } else {
                /* Current node mode with user-defined signal: dispatch only to active state */
                if (pActiveState != NULL && pActiveState == pWorkingState) {
                    if (hsm_invokeHandler(pManager, pWorkingState, input)) {
                        return EOR_FAULT_ERROR;
                    }
                }
            }
        } else {
            /* First iteration: commit initial state */
            pManager->currentState = pManager->processingState;
        }

        /* After reaching target state with ENTRY signal, send INIT */
        if ((pWorkingState == pCurrentState) && (input.signal == HSM_SIGNAL_ENTRY)) {
            input.signal = HSM_SIGNAL_INIT;
            if (hsm_invokeHandler(pManager, pWorkingState, input)) {
                return EOR_FAULT_ERROR;
            }

            /* On initial entry, also dispatch the original user signal */
            if (isInitialEntry && (savedInput.signal != HSM_SIGNAL_INIT)) { 
                if (hsm_invokeHandler(pManager, pWorkingState, savedInput)) {
                    return EOR_FAULT_ERROR;
                }
            }
        }

        /* Check if state handler requested a transition */
        hsm_state_t *pNewState = hsm_getState(pManager, pManager->currentState);

        if (pCurrentState != pNewState) {
            /* Transition requested: find LCA and perform exit/entry sequence */
            hsm_state_t *pLCA = hsm_findLCA(pCurrentState, pNewState);

            /* Save input for next transition */
            if (input.signal != HSM_SIGNAL_INIT) {
                savedInput = input;
            }

            /* Notify transducer of transition */
            if (hsm_notifyTransition(pManager, pCurrentState, savedInput) != HSM_OK) {
                return EOR_FAULT_ERROR;
            }

            /* Exit states from current up to LCA */
            if (hsm_exitToLCA(pManager, pCurrentState, pLCA, pNewState, input) != HSM_OK) {
                return EOR_FAULT_ERROR;
            }

            /* Prepare to enter new state hierarchy */
            input.signal = HSM_SIGNAL_ENTRY;
            pCurrentState = pNewState;
            pEntryTarget = pLCA;
        } else {
            /* No transition: we're done with this state */
            pEntryTarget = pWorkingState;
        }

        pWorkingState = pCurrentState;
    }

    return HSM_ACTION_DONE;
}
