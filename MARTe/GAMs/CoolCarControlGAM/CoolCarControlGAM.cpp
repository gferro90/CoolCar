/**
 * @file CoolCarControlGAM.cpp
 * @brief Source file for class CoolCarControlGAM
 * @date 28/set/2016
 * @author pc
 *
 * @copyright Copyright 2015 F4E | European Joint Undertaking for ITER and
 * the Development of Fusion Energy ('Fusion for Energy').
 * Licensed under the EUPL, Version 1.1 or - as soon they will be approved
 * by the European Commission - subsequent versions of the EUPL (the "Licence")
 * You may not use this work except in compliance with the Licence.
 * You may obtain a copy of the Licence at: http://ec.europa.eu/idabc/eupl
 *
 * @warning Unless required by applicable law or agreed to in writing, 
 * software distributed under the Licence is distributed on an "AS IS"
 * basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the Licence permissions and limitations under the Licence.

 * @details This source file contains the definition of all the methods for
 * the class CoolCarControlGAM (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "CoolCarControlGAM.h"
#include "AdvancedErrorManagement.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

CoolCarControlGAM::CoolCarControlGAM() {
    // Auto-generated constructor stub for CoolCarControlGAM
    // TODO Verify if manual additions are needed

    adcValue = NULL;
    pwmValue = NULL;
    usb[0] = NULL;
    usb[1] = NULL;
    usb[2] = NULL;
    timer = NULL;
    minMotorRef = 0u;
    rangeMotorRef = 0u;
    minMotorIn = 0u;
    rangeMotorIn = 0u;

}

CoolCarControlGAM::~CoolCarControlGAM() {
    // Auto-generated destructor stub for CoolCarControlGAM
    // TODO Verify if manual additions are needed
}

bool CoolCarControlGAM::Initialise(StructuredDataI &data) {
    bool ret = GAM::Initialise(data);
    //todo custom initialisation

    if (ret) {
        ret = data.Read("MinMotorRef", minMotorRef);
        if (ret) {
            ret = data.Read("MaxMotorRef", rangeMotorRef);
            if (!ret) {
                REPORT_ERROR(ErrorManagement::InitialisationError, "The maximum motor speed reference value is not defined");
            }
        }
        else {
            REPORT_ERROR(ErrorManagement::InitialisationError, "The minimum motor speed reference value is not defined");
        }
        if (ret) {
            rangeMotorRef -= minMotorRef;
        }
        if (ret) {
            ret = data.Read("MinMotorIn", minMotorIn);
        }
        else {
            REPORT_ERROR(ErrorManagement::InitialisationError, "The minimum motor speed control value is not defined");
        }
        if (ret) {
            ret = data.Read("MaxMotorIn", rangeMotorIn);
        }
        else {
            REPORT_ERROR(ErrorManagement::InitialisationError, "The maximum motor speed control value is not defined");
        }
        if (ret) {
            rangeMotorIn -= minMotorIn;
        }
    }

    return ret;
}

bool CoolCarControlGAM::SetConfiguredDatabase(StructuredDataI & data) {
    bool ret = GAM::SetConfiguredDatabase(data);

    if (ret) {
        //assign here the pointer to signals
        timer = (uint32*) GetInputSignalsMemory();
        adcValue = (uint32 *) GetInputSignalsMemory() + 1;

        usb[0] = (uint32*) GetOutputSignalsMemory();
        usb[1] = (uint32 *) GetOutputSignalsMemory() + 1;
        usb[2] = (uint32 *) GetOutputSignalsMemory() + 2;
        pwmValue = (uint32 *) GetOutputSignalsMemory() + 3;
    }
    return ret;
}

bool CoolCarControlGAM::Execute() {
    //read the ADC (done by input broker)
    //map adc value on pwm duty cycle (to be done here)
    REPORT_ERROR_PARAMETERS(ErrorManagement::Warning, "Executing... %d %d", rangeMotorRef, rangeMotorIn);

    *(uint32*) GetOutputSignalsMemory() = *(uint32*) GetInputSignalsMemory();
    float32 factor = ((*((uint32 *) GetInputSignalsMemory() + 1) - minMotorRef) / ((float32)rangeMotorRef));
    float32 calc = minMotorIn + factor * rangeMotorIn;
    *((uint32 *) GetOutputSignalsMemory() + 3) = (uint32) calc;
    *((uint32 *) GetOutputSignalsMemory() + 1) = *((uint32 *) GetInputSignalsMemory() + 1);
    *((uint32 *) GetOutputSignalsMemory() + 2) = *((uint32 *) GetOutputSignalsMemory() + 3);

    //write on pwm (done by output broker)
    return true;
}

CLASS_REGISTER(CoolCarControlGAM, "1.0")
