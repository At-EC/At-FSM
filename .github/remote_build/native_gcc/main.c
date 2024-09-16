/**
 * Copyright (c) Riven Zheng (zhengheiot@gmail.com).
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 **/

#include "psm.h"
#include "hsm.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The PSM user specific signal */
enum {
    PSM_SIGNAL_1 = PSM_SIGNAL_USER_DEFINE,
    PSM_SIGNAL_2,
    PSM_SIGNAL_3,
};

/* The PSM state instance id */
enum {
    PSM_INST_0 = 0,
    PSM_INST_1,
    PSM_INST_2,
};

/* The HSM user specific signal */
enum {
    HSM_SIGNAL_1 = HSM_SIGNAL_USER_DEFINE,
    HSM_SIGNAL_2,
    HSM_SIGNAL_3,
};

/* The HSM user specific signal */
enum {
    HSM_INST_0 = 0,
    HSM_INST_1,
    HSM_INST_2,
    HSM_INST_20,
    HSM_INST_21,
    HSM_INST_210,
    HSM_INST_10,
    HSM_INST_100,
};

static void* psm_state_1(psm_state_input_t input);
static void* psm_state_2(psm_state_input_t input);
static void* psm_state_3(psm_state_input_t input);
static i32_t psm_transducer_handler(const psm_state_t *pStateContext, psm_instance_t from, psm_instance_t to, psm_state_input_t input);

static hsm_result_t hsm_state_0(hsm_state_input_t input);
static hsm_result_t hsm_state_1(hsm_state_input_t input);
static hsm_result_t hsm_state_2(hsm_state_input_t input);
static hsm_result_t hsm_state_20(hsm_state_input_t input);
static hsm_result_t hsm_state_21(hsm_state_input_t input);
static hsm_result_t hsm_state_210(hsm_state_input_t input);
static hsm_result_t hsm_state_10(hsm_state_input_t input);
static hsm_result_t hsm_state_100(hsm_state_input_t input);
static hsm_result_t hsm_transducer_handler(const hsm_state_t *pStateContext, hsm_instance_t from, hsm_instance_t to, hsm_state_input_t input);

/* The PSM states' init tables */
static psm_state_t g_psm_state_init[] = {
    [PSM_INST_0] = {.instance = PSM_INST_0,
                    .id = 0u,
                    .pName = "psm_state_1",
                    .pEntryFunc = psm_state_1 },

    [PSM_INST_1] = {.instance = PSM_INST_1,
                    .id = 1u,
                    .pName = "psm_state_2",
                    .pEntryFunc = psm_state_2 },
    [PSM_INST_2] = {.instance = PSM_INST_2,
                    .id = 2u,
                    .pName = "psm_state_1",
                    .pEntryFunc = psm_state_3 },
};

/* The HSM states' init tables */
static hsm_state_t g_hsm_state_init[] = {
    [HSM_INST_0] = {.pMasterState = NULL,
                    .instance = HSM_INST_0,
                    .id = 0,
                    .pName = "hsm_state_0",
                    .pEntryFunc = hsm_state_0 },

    [HSM_INST_1] = {.pMasterState = NULL,
                    .instance = HSM_INST_1,
                    .id = 1,
                    .pName = "hsm_state_1",
                    .pEntryFunc = hsm_state_1 },
    [HSM_INST_2] = {.pMasterState = NULL,
                    .instance = HSM_INST_2,
                    .id = 2,
                    .pName = "hsm_state_2",
                    .pEntryFunc = hsm_state_2 },
    [HSM_INST_20] = {.pMasterState = &g_hsm_state_init[HSM_INST_2],
                     .instance = HSM_INST_20,
                     .id = 20,
                     .pName = "hsm_state_20",
                     .pEntryFunc = hsm_state_20 },
    [HSM_INST_21] = {.pMasterState = &g_hsm_state_init[HSM_INST_2],
                     .instance = HSM_INST_21,
                     .id = 21,
                     .pName = "hsm_state_21",
                     .pEntryFunc = hsm_state_21 },
    [HSM_INST_210] = {.pMasterState = &g_hsm_state_init[HSM_INST_21],
                     .instance = HSM_INST_210,
                     .id = 210,
                     .pName = "hsm_state_210",
                     .pEntryFunc = hsm_state_210 },
    [HSM_INST_10] = {.pMasterState = &g_hsm_state_init[HSM_INST_1],
                     .instance = HSM_INST_10,
                     .id = 10,
                     .pName = "hsm_state_10",
                     .pEntryFunc = hsm_state_10 },
    [HSM_INST_100] = {.pMasterState = &g_hsm_state_init[HSM_INST_10],
                      .instance = HSM_INST_100,
                      .id = 100,
                      .pName = "hsm_state_100",
                      .pEntryFunc = hsm_state_100 },

};

/* The PSM state manager */
static psm_state_manager_t g_psm_mngr_context = {0u};

/* The HSM state manager */
static hsm_state_manager_t g_hsm_mngr_context = {0u};

int main(void)
{
    psm_init(&g_psm_mngr_context, &g_psm_state_init[0], PSM_INST_0, psm_transducer_handler);

    psm_state_input_t data = {0};
    data.signal = PSM_SIGNAL_1;
    psm_activities(&g_psm_mngr_context, data);

    hsm_init(&g_hsm_mngr_context, &g_hsm_state_init[0], HSM_INST_210, hsm_transducer_handler);

    hsm_state_input_t input = {0};
    input.signal = HSM_SIGNAL_INIT;
    hsm_activities(&g_hsm_mngr_context, input);

    input.signal = HSM_SIGNAL_1;
    hsm_activities(&g_hsm_mngr_context, input);

    while(1) {};
}

static i32_t psm_transducer_handler(const psm_state_t *pStateContext, psm_instance_t from, psm_instance_t to, psm_state_input_t input)
{
	if (pStateContext) {
		printf("psm_transducer_handler: from %d to %d, signal: %d\n", from, to, input.signal);
	}
    return PSM_RESULT_PASS;
}

static void* psm_state_1(psm_state_input_t input)
{
    switch(input.signal)
    {
        case PSM_SIGNAL_ENTRY:
        {
            break;
        }
        case PSM_SIGNAL_EXIT:
        {
            break;
        }
        case PSM_SIGNAL_1:
        {
            return psm_transition(&g_psm_mngr_context, PSM_INST_1);
        }
        case PSM_SIGNAL_2:
        {
            break;
        }
        case PSM_SIGNAL_3:
        {
            break;
        }
        default:
            break;
    }

    return PSM_ACTION_DONE;
}

static void* psm_state_2(psm_state_input_t input)
{
    switch(input.signal)
    {
        case PSM_SIGNAL_ENTRY:
        {
            break;
        }
        case PSM_SIGNAL_EXIT:
        {
            break;
        }
        case PSM_SIGNAL_1:
        {
            break;
        }
        case PSM_SIGNAL_2:
        {
            break;
        }
        case PSM_SIGNAL_3:
        {
            break;
        }
        default:
            break;
    }

    return PSM_ACTION_DONE;
}

static void* psm_state_3(psm_state_input_t input)
{
    switch(input.signal)
    {
        case PSM_SIGNAL_ENTRY:
        {
            break;
        }
        case PSM_SIGNAL_EXIT:
        {
            break;
        }
        case PSM_SIGNAL_1:
        {
            break;
        }
        case PSM_SIGNAL_2:
        {
            break;
        }
        case PSM_SIGNAL_3:
        {
            break;
        }
        default:
            break;
    }

    return PSM_ACTION_DONE;
}

static hsm_result_t hsm_transducer_handler(const hsm_state_t *pStateContext, hsm_instance_t from, hsm_instance_t to, hsm_state_input_t input)
{
	if (pStateContext) {
		printf("hsm_transducer_handler: from %d to %d, signal: %d\n", from, to, input.signal);
	}
    return HSM_RESULT_PASS;
}

static hsm_result_t hsm_state_0(hsm_state_input_t input)
{
    switch(input.signal)
    {
        case HSM_SIGNAL_ENTRY:
        {
            break;
        }
        case HSM_SIGNAL_EXIT:
        {
            break;
        }
        case HSM_SIGNAL_INIT:
        {
            break;
        }
        case HSM_SIGNAL_1:
        {
            break;
        }
        case HSM_SIGNAL_2:
        {
            break;
        }
        case HSM_SIGNAL_3:
        {
            break;
        }
        default:
            break;
    }

    return HSM_ACTION_DONE;
}

static hsm_result_t hsm_state_1(hsm_state_input_t input)
{
    switch(input.signal)
    {
        case HSM_SIGNAL_ENTRY:
        {
            break;
        }
        case HSM_SIGNAL_EXIT:
        {
            break;
        }
        case HSM_SIGNAL_INIT:
        {
            break;
        }
        case HSM_SIGNAL_1:
        {
            break;
        }
        case HSM_SIGNAL_2:
        {
            break;
        }
        case HSM_SIGNAL_3:
        {
            break;
        }
        default:
            break;
    }

    return HSM_ACTION_DONE;
}

static hsm_result_t hsm_state_2(hsm_state_input_t input)
{
    switch(input.signal)
    {
        case HSM_SIGNAL_ENTRY:
        {
            break;
        }
        case HSM_SIGNAL_EXIT:
        {
            break;
        }
        case HSM_SIGNAL_INIT:
        {
            break;
        }
        case HSM_SIGNAL_1:
        {
            break;
        }
        case HSM_SIGNAL_2:
        {
            break;
        }
        case HSM_SIGNAL_3:
        {
            break;
        }
        default:
            break;
    }

    return HSM_ACTION_DONE;
}

static hsm_result_t hsm_state_20(hsm_state_input_t input)
{
    switch(input.signal)
    {
        case HSM_SIGNAL_ENTRY:
        {
            break;
        }
        case HSM_SIGNAL_EXIT:
        {
            break;
        }
        case HSM_SIGNAL_INIT:
        {
            break;
        }
        case HSM_SIGNAL_1:
        {
            break;
        }
        case HSM_SIGNAL_2:
        {
            break;
        }
        case HSM_SIGNAL_3:
        {
            break;
        }
        default:
            break;
    }

    return HSM_ACTION_DONE;
}

static hsm_result_t hsm_state_21(hsm_state_input_t input)
{
    switch(input.signal)
    {
        case HSM_SIGNAL_ENTRY:
        {
            break;
        }
        case HSM_SIGNAL_EXIT:
        {
            break;
        }
        case HSM_SIGNAL_INIT:
        {
            break;
        }
        case HSM_SIGNAL_1:
        {
            //return hsm_transition(&g_hsm_mngr_context, HSM_INST_2);
            break;
        }
        case HSM_SIGNAL_2:
        {
            break;
        }
        case HSM_SIGNAL_3:
        {
            break;
        }
        default:
            break;
    }

    return HSM_ACTION_DONE;
}

static hsm_result_t hsm_state_210(hsm_state_input_t input)
{
    switch(input.signal)
    {
        case HSM_SIGNAL_ENTRY:
        {
            break;
        }
        case HSM_SIGNAL_EXIT:
        {
            break;
        }
        case HSM_SIGNAL_INIT:
        {
            break;
        }
        case HSM_SIGNAL_1:
        {
            return hsm_transition(&g_hsm_mngr_context, HSM_INST_100);
        }
        case HSM_SIGNAL_2:
        {
            break;
        }
        case HSM_SIGNAL_3:
        {
            break;
        }
        default:
            break;
    }

    return HSM_ACTION_DONE;
}

static hsm_result_t hsm_state_10(hsm_state_input_t input)
{
    switch(input.signal)
    {
        case HSM_SIGNAL_ENTRY:
        {
            break;
        }
        case HSM_SIGNAL_EXIT:
        {
            break;
        }
        case HSM_SIGNAL_INIT:
        {
            break;
        }
        case HSM_SIGNAL_1:
        {
            break;
        }
        case HSM_SIGNAL_2:
        {
            break;
        }
        case HSM_SIGNAL_3:
        {
            break;
        }
        default:
            break;
    }

    return HSM_ACTION_DONE;
}

static hsm_result_t hsm_state_100(hsm_state_input_t input)
{
    switch(input.signal)
    {
        case HSM_SIGNAL_ENTRY:
        {
            break;
        }
        case HSM_SIGNAL_EXIT:
        {
            break;
        }
        case HSM_SIGNAL_INIT:
        {
            break;
        }
        case HSM_SIGNAL_1:
        {
            break;
        }
        case HSM_SIGNAL_2:
        {
            break;
        }
        case HSM_SIGNAL_3:
        {
            break;
        }
        default:
            break;
    }

    return HSM_ACTION_DONE;
}

#ifdef __cplusplus
}
#endif
