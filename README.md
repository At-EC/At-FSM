# <img src="https://github.com/At-EC/At-FSM/blob/main/.github/picture/@-EC.png" alt="@-EC" width="25" height="25" />[At-FSM](https://github.com/At-EC/At-FSM) &middot; [![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/At-EC/At-FSM/blob/main/LICENSE) [![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](https://github.com/At-EC/At-FSM/tree/main/.github/Welcome_PRs.md) <a href="https://github.com/At-EC/At-FSM/stargazers"><img alt="GitHub stars" src="https://img.shields.io/github/stars/At-EC/At-FSM?color=success"></a> [![Discord](https://img.shields.io/discord/1208405601994932274?logo=discord)](https://discord.gg/AxfF9aH58G) <a href="https://github.com/At-EC/At-FSM/actions"><img alt="Build" src="https://github.com/At-EC/At-FSM/workflows/Build/badge.svg"></a> [![GitHub Release](https://img.shields.io/github/v/release/At-EC/At-FSM)](./release-note.md)

At-FSM ([@ Finite State Machine](https://github.com/At-EC/At-FSM)) is an open and user-friendly embedded Finite State Machine(FSM) designed for various embedded controllers' supporting, as it's beginning it will be an important member of the At-EC family.

The goal of the system is to explore and try to provide a range of features and capabilities to support and simplify your embedded firmware implementation.

We hope the At-EC will be a popular community-based embedded controller's project in the future. If this project was useful to you, give it a ⭐. It is very important for us to know it is useful to you and your attention will encourage us. 

Welcome PRs! If you are interested in contributing, Pls join us at [Discord](https://discord.gg/AxfF9aH58G). 

<p align="center">

<img src="https://socialify.git.ci/At-EC/At-FSM/image?description=1&descriptionEditable=%40%20Embedded%20System%2C%20%40%20Finite%20State%20Machine.&font=KoHo&issues=1&name=1&owner=1&pattern=Circuit%20Board&pulls=1&stargazers=1&theme=Auto" alt="At-FSM" width="640" height="320" />

</p>

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
