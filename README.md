<p align="center"> <img src="https://github.com/At-EC/At-RTOS/blob/main/.github/picture/At-EC.jpg" alt="@-EC" width="150" height="150" /> </p>

> @ Embedded Controllers.

# [At-FSM](https://github.com/At-EC/At-FSM) &middot; [![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/At-EC/At-FSM/blob/main/LICENSE) [![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](https://github.com/At-EC/At-FSM/tree/main/.github/Welcome_PRs.md) <a href="https://github.com/At-EC/At-FSM/stargazers"><img alt="GitHub stars" src="https://img.shields.io/github/stars/At-EC/At-FSM?color=success"></a> [![Discord](https://img.shields.io/discord/1208405601994932274?logo=discord)](https://discord.gg/AxfF9aH58G) <a href="https://github.com/At-EC/At-FSM/actions"><img alt="Build" src="https://github.com/At-EC/At-FSM/workflows/Build/badge.svg"></a> [![GitHub Release](https://img.shields.io/github/v/release/At-EC/At-FSM)](./release-note.md)

At-FSM ([@ Finite State Machine](https://github.com/At-EC/At-FSM)) is an open and user-friendly embedded Finite State Machine(FSM) designed for various embedded controllers' supporting, as it's beginning it will be an important member of the At-EC family.

The goal of the system is to explore and try to provide a range of features and capabilities to support and simplify your embedded firmware implementation.

We hope the At-EC will be a popular community-based embedded controller's project in the future. If this project was useful to you, give it a ⭐. It is very important for us to know it is useful to you and your attention will encourage us. 

Welcome PRs! If you are interested in contributing, Pls join us at [Discord](https://discord.gg/AxfF9aH58G). 

## Introduce

We specify a transducer (a process that takes as input a sequence of values which serve as inputs to the state machine, and returns as output the set of outputs of the machine for each input) as a state machine by specifying:
 - a set of states, S,
 - a set of inputs, I, also called the input signal,
 - a set of outputs, O, also called the output result,
 - a state input action, O = o(s, i), that maps the current state accept a input signal, that triggers a state transition.
 - a next-state transition, O = n(s, i), that maps the current state accept a input signal, that triggers a state transition.
 
 Here is a Primitive State Machine (PSM), to give you an idea of the usage of the systems we are considering.

<p align="center">

<img src="https://github.com/At-EC/At-FSM/blob/main/.github/picture/psm_diagram.png" />

</p>

 Here is a Hierarchical State Machine (HSM), to give you an idea of the usage of the systems we are considering.

<p align="center">

<img src="https://github.com/At-EC/At-FSM/blob/main/.github/picture/hsm_diagram.png" />

</p>

In the primitive state machine system, it alway has an active state which can accept the current system event signal. There are no inherit rules betweent states, so the event signal only can drive the active state.
The state transition can be triggered by the user specific event signal. when the transition is happening, the current state will receive the system defined signal `PSM_SIGNAL_EXIT`, and the next state will receive another system defined signal `PSM_SIGNAL_ENTRY`. after that the next state will be enabled. 

<p align="center">

<img src="https://github.com/At-EC/At-FSM/blob/main/.github/picture/psm_state.png" />

</p>

The hierarchical state machine system also have the system defined signal `HSM_SIGNAL_EXIT` and `HSM_SIGNAL_ENTRY` for the system transition internal use. A bit difference is that the HSM has another system specific signal `HSM_SIGNAL_INIT` for the next state init purpose.
The HSM signal direction always is from master to slave, The higher layer state always has a high priority to accept the signal. it means if the higher layer state accept the signal and start to transition to another state, this signal will be blocked at the higher layer state, and the lower layer state has no choice to receive it. 
If the direction of the transition is from the slave to master, the children states will receive the signal `HSM_SIGNAL_EXIT`. otherwise, the children states will accept the signal `HSM_SIGNAL_ENTRY` and the target state will accept the signal `HSM_SIGNAL_INIT`.

## PSM Examples

The [main.c](./.github/remote_build/native_gcc/main.c) has a sample code for github action gcc build check. You can check it how to use and perform it in your embedded controller system.

The following sample codes illustrate how to define a PSM state init tables and a demo state entry function:

```c

#include "psm.h"

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

/* The PSM state manager */
static psm_state_manager_t g_psm_mngr_context = {0u};

```

After you implementing the PSM states init tables as above exmaple code, you should focus on your application design for the state entry function and the input signal management.

```c

/* The PSM state entry function */
static void* psm_state_1(psm_state_input_t input)
{
    switch(input.signal)
    {
        case PSM_SIGNAL_ENTRY: /* The system internal defined signal for transition */
        case PSM_SIGNAL_EXIT: 
        {
            break;
        }
        case PSM_SIGNAL_1: /* User defined event signal */
        {
            /* Transition: To the state PSM_INST_2 */
            return psm_transition(&g_psm_mngr_context, PSM_INST_2); 
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

    return PSM_ACTION_DONE; /* No futher state action */
}

```

## HSM Examples

The following sample codes illustrate how to define a HSM state init tables and a demo state entry function:

```c

#include "hsm.h"

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

/* The HSM states' init tables */
static hsm_state_t g_hsm_state_init[] = {
    [HSM_INST_0] = {.pMasterState = NULL, /* The master pointer is NULL for the root state */
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
    [HSM_INST_20] = {.pMasterState = &g_hsm_state_init[HSM_INST_2], /* This state is HSM_INST_2 children */
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

/* The HSM state manager */
static hsm_state_manager_t g_hsm_mngr_context = {0u};


```

After you implementing the PSM states init tables as above exmaple code, you should focus on your application design for the state entry function and the input signal management.

```c

/* The HSM state entry function */
static hsm_result_t hsm_state_210(hsm_state_input_t input)
{
    switch(input.signal)
    {
        case HSM_SIGNAL_ENTRY: /* The system internal defined signal for transition */
        case HSM_SIGNAL_EXIT:
        {
            break;
        }
        case HSM_SIGNAL_INIT: /* The system internal defined signal for transition */
        {
            break;
        }
        case HSM_SIGNAL_1:
        {
            /* Transition: to the state HSM_INST_100 */
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

    return HSM_ACTION_DONE; /* No futher state action */
}

```

Note that: The details are shared in the [main.c](./.github/remote_build/native_gcc/main.c).

## License

The At-FSM is completely open-source, can be used in commercial applications for free, does not require the disclosure of code, and has no potential commercial risk. License information and copyright information can generally be seen at the beginning of the code:

```c
/**
 * Copyright (c) Riven Zheng (zhengheiot@gmail.com).
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
**/
```

The MIT License is [here](./LICENSE).
