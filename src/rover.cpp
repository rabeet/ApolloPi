//////////////////////////////////////////////////////////////////////////////
// Model: apollo_pi.qm
// File:  ./src/rover.cpp
//
// This file has been generated automatically by QP Modeler (QM).
// DO NOT EDIT THIS FILE MANUALLY.
//
// Please visit www.state-machine.com/qm for more information.
//////////////////////////////////////////////////////////////////////////////
/**
 *
 */

#include "qp_port.h"   // QP framework
#include "apollo_pi.h" // APi framework
#include "rover.h"     // application
#include "bsp.h"       // board support package

#include <iostream>

#include <wiringPi.h>

namespace ApolloPi {

/* Local scope -- Event queues */
static QP::QEvent const * l_serverQueueSto[SERVER_QUEUE_SIZE];
static QP::QEvent const * l_statusLedQueueSto[STATUS_LED_QUEUE_SIZE];
static QP::QEvent const * l_dispatcherQueueSto[DISPATCHER_QUEUE_SIZE];
static QP::QEvent const * l_wifiDongleQueueSto[WIFI_DONGLE_QUEUE_SIZE];

/* Dynamic Event Allocation Pools */
static SendEvt l_smlPoolSto[SMALL_POOL_SIZE];

/* no publish-subscribe implementation yet */
static QP::QSubscrList l_subscrSto[MAX_PUB_SIG];

extern "C" int main(int argc, char *argv[]) {
    /* initialize WiringPi */
    std::cout << "Initialized WiringPi" << std::endl;
    wiringPiSetup();

    /* initialize the framework */
    QP::QF::init();

    QP::QF::poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));

    /* initialize publish-subscribe event mechanism */
    QP::QF::psInit(l_subscrSto, Q_DIM(l_subscrSto));

    /* start all AOs, starting with the highest priority object */
    int priority = QF_MAX_ACTIVE;

    AO_WebSocketServer->start(priority--,
                              l_serverQueueSto,
                              Q_DIM(l_serverQueueSto),
                              (void *)0, 0,
                              (QP::QEvent *)0);
    std::cout << "***Server started***" << std::endl;

    AO_Dispatcher->start(     priority--,
                              l_dispatcherQueueSto,
                              Q_DIM(l_dispatcherQueueSto),
                              (void *)0, 0,
                              (QP::QEvent *)0);
    std::cout << "***Dispatcher started***" << std::endl;

    AO_MotorController->start(priority--,
                              l_wifiDongleQueueSto,
                              Q_DIM(l_wifiDongleQueueSto),
                              (void *)0, 0,
                              (QP::QEvent *)0);
    std::cout << "***MotorController started***" << std::endl;

    AO_WifiDongle->start(     priority--,
                              l_wifiDongleQueueSto,
                              Q_DIM(l_wifiDongleQueueSto),
                              (void *)0, 0,
                              (QP::QEvent *)0);
    std::cout << "***WifiDongle started***" << std::endl;

    AO_StatusLed->start(      priority--,
                              l_statusLedQueueSto,
                              Q_DIM(l_statusLedQueueSto),
                              (void *)0, 0,
                              (QP::QEvent *)0);
    std::cout << "***StatusLed started***" << std::endl;

    /* intialize any F/Hsms */
    HSM_Camera->init();

    /* run the application according to statemachine semantics */
    std::cout << std::endl << "Running Apollo Pi Rover!" << std::endl << std::endl;
    return QP::QF::run();
}

} /* namespace ApolloPi */
