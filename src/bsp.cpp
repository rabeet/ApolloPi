//////////////////////////////////////////////////////////////////////////////
// Model: apollo_pi.qm
// File:  ./src/bsp.cpp
//
// This file has been generated automatically by QP Modeler (QM).
// DO NOT EDIT THIS FILE MANUALLY.
//
// Please visit www.state-machine.com/qm for more information.
//////////////////////////////////////////////////////////////////////////////
#include "qp_port.h"
#include "apollo_pi.h"
#include "bsp.h"

#include <cstdlib>
#include <iostream>

#include <stdio.h>

using namespace std;

namespace ApolloPi {



} /* namespace ApolloPi */

namespace QP {

extern "C" void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    fprintf(stderr, "Assertion failed in %s, line %d", file, line);
    exit(EXIT_FAILURE);
}

void QF::onStartup(void)
{
    QF_setTickRate(ApolloPi::BSP_TICKS_PER_SEC);
}

void QF::onCleanup(void)
{
    // TODO
    ((ApolloPi::WebSocketServer *)ApolloPi::AO_WebSocketServer)->close();
    QF::publish(Q_NEW(ApolloPi::DisconnectedEvt, ApolloPi::DISCONNECTED_SIG));
}

void QF_onClockTick(void)
{
    QF::TICK((void *)0);
}

} /* namespace QP */
