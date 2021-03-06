//////////////////////////////////////////////////////////////////////////////
// Model: apollo_pi.qm
// File:  ./src/dispatcher.cpp
//
// This file has been generated automatically by QP Modeler (QM).
// DO NOT EDIT THIS FILE MANUALLY.
//
// Please visit www.state-machine.com/qm for more information.
//////////////////////////////////////////////////////////////////////////////
#include "qp_port.h"
#include "apollo_pi.h"
#include "rover.h"

#include <json/json.h>
#include <iostream>
#include <cstring>

namespace ApolloPi {

// @(/1/1) ...................................................................
// @(/1/1/0) .................................................................
Dispatcher::Dispatcher() 
  : QActive(Q_STATE_CAST(&Dispatcher::initial))
{
}

// @(/1/1/1) .................................................................
bool Dispatcher::dispatch_message(const char * target, json_object * json_msg) {
    if (std::strcmp(target, "MotorController") == 0)
    {
        MotorControllerEvt *mce
            Q_NEW(MotorControllerEvt, UPDATE_MOTORS_SIG);

        int16_t ctrl = json_object_get_int(json_msg);

        std::cout << "Raw motor control value: 0x" << std::hex
            << ctrl << std::dec << std::endl;

        //mce->set_left_speed ( ctrl >> 8 );
        mce->set_right_speed( ctrl );

        std::cout << "Posting event to MotorController" << std::endl;
        AO_MotorController->postFIFO(mce);
    }
    else if (std::strcmp(target, "Camera") == 0)
    {

    }
    else if (std::strcmp(target, "Console") == 0)
    {

    }
    else
    {
        char response[21] = "Unrecognized target.";

        SendEvt * se = Q_NEW(SendEvt, SEND_SIG);

        se->set_frame(
            WebSocketFrame::make_frame(
                response, std::strlen(response)));

        AO_WebSocketServer->postFIFO(se);
    }
}
// @(/1/1/2) .................................................................
// @(/1/1/2/0)
QP::QState Dispatcher::initial(Dispatcher * const me, QP::QEvt const * const e) {
    return Q_TRAN(&Dispatcher::active);
}
// @(/1/1/2/1) ...............................................................
QP::QState Dispatcher::active(Dispatcher * const me, QP::QEvt const * const e) {
    QP::QState status;
    switch (e->sig) {
        // @(/1/1/2/1/0)
        case DISPATCH_MESSAGE_SIG: {
            /* interpret json */
            char *msg = ((DispatchEvt *)e)->get_message();

            std::cout << "Dispatcher: interpretting '" << msg << "'" << std::endl;

            json_object *jobj;

            jobj = json_tokener_parse(msg);

            // done with message
            delete [] msg;

            if (jobj == NULL)
            {
                char err_msg[BUF_SIZE];

                std::strcpy(err_msg,
                    "{\"tgt\":\"Console\","
                    "\"msg\":\"error: msg received was not valid JSON\"}");
                SendEvt *se = Q_NEW(SendEvt, SEND_SIG);
                se->set_frame(
                    WebSocketFrame::make_frame(
                        err_msg, std::strlen(err_msg)));

                AO_WebSocketServer->postFIFO(se);
            }
            else
            {
                std::cout << "tokener_parse() success!" << std::endl;
                std::cout
                    << "Parsed: "
                    << json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_PRETTY) << std::endl;

                // obtain json "tgt" key
                json_object *json_tgt = json_object_object_get(
                    jobj, "tgt");

                // only continue if there is a target
                if (json_tgt != NULL)
                {
                    // convert object into string
                    const char * tgt = json_object_get_string(json_tgt);

                    // obtain "msg" object
                    json_object *json_msg = json_object_object_get(
                        jobj, "msg");

                    me->dispatch_message(tgt, json_msg);
                }
            }
            status = Q_HANDLED();
            break;
        }
        default: {
            status = Q_SUPER(&QHsm::top);
            break;
        }
    }
    return status;
}

// @(/3/5) ...................................................................
// @(/3/5/1) .................................................................
char * DispatchEvt::get_message() {
    return message_;
}
// @(/3/5/2) .................................................................
void DispatchEvt::set_message(char * msg) {
    message_ = msg;
}

static Dispatcher l_dispatcher;
QP::QActive * const AO_Dispatcher = (QP::QActive *)&l_dispatcher;

} /* namespace ApolloPi */
