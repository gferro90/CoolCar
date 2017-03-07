/**
 * @file TransmitterGAM.cpp
 * @brief Source file for class TransmitterGAM
 * @date 19/ott/2016
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
 * the class TransmitterGAM (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "TransmitterGAM.h"
#include "AdvancedErrorManagement.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

TransmitterGAM::TransmitterGAM() {
    // Auto-generated constructor stub for TransmitterGAM
    // TODO Verify if manual additions are needed

    adcMotor = NULL;
    pwms = NULL;
    adcDrive = NULL;
    usb[0] = NULL;
    usb[1] = NULL;
    usb[2] = NULL;
    usb[3] = NULL;
    usb[4] = NULL;

    timer = NULL;
    minMotorRef = 0u;
    rangeMotorRef = 0u;
    minMotorIn = 0u;
    rangeMotorIn = 0u;
    minDriveRef = 0u;
    rangeDriveRef = 0u;
    minDriveIn = 0u;
    rangeDriveIn = 0u;

}

TransmitterGAM::~TransmitterGAM() {
    // Auto-generated destructor stub for TransmitterGAM
    // TODO Verify if manual additions are needed
}

bool TransmitterGAM::Initialise(StructuredDataI &data) {
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
            if (!ret) {
                REPORT_ERROR(ErrorManagement::InitialisationError, "The minimum motor speed control value is not defined");
            }
        }
        if (ret) {
            ret = data.Read("MaxMotorIn", rangeMotorIn);
            if (!ret) {
                REPORT_ERROR(ErrorManagement::InitialisationError, "The maximum motor speed control value is not defined");
            }
        }

        if (ret) {
            rangeMotorIn -= minMotorIn;
        }

        if (ret) {
            ret = data.Read("MaxDriveRef", rangeDriveRef);
            if (!ret) {
                REPORT_ERROR(ErrorManagement::InitialisationError, "The maximum drive reference value is not defined");
            }
        }
        if (ret) {
            ret = data.Read("MinDriveRef", minDriveRef);
            if (!ret) {
                REPORT_ERROR(ErrorManagement::InitialisationError, "The minimum drive reference value is not defined");

            }
        }
        if (ret) {
            rangeDriveRef -= minDriveRef;
        }
        if (ret) {
            ret = data.Read("MinDriveIn", minDriveIn);
            if (!ret) {
                REPORT_ERROR(ErrorManagement::InitialisationError, "The minimum drive control value is not defined");
            }
        }

        if (ret) {
            ret = data.Read("MaxDriveIn", rangeDriveIn);
            if (!ret) {
                REPORT_ERROR(ErrorManagement::InitialisationError, "The maximum motor speed control value is not defined");
            }
        }
        if (ret) {
            rangeDriveIn -= minDriveIn;
        }

    }

    return ret;
}

void TransmitterGAM::Setup() {
    //assign here the pointer to signals
    timer = (uint32*) GetInputSignalsMemory();
    adcMotor = (uint32 *) GetInputSignalsMemory() + 1;
    adcDrive = (uint32 *) GetInputSignalsMemory() + 2;

    usb[0] = (uint32*) GetOutputSignalsMemory();
    usb[1] = (uint32 *) GetOutputSignalsMemory() + 1;
    usb[2] = (uint32 *) GetOutputSignalsMemory() + 2;
    usb[3] = (uint32 *) GetOutputSignalsMemory() + 3;
    usb[4] = (uint32 *) GetOutputSignalsMemory() + 4;
    pwms = (uint16 *) GetOutputSignalsMemory() + 10;
}

bool TransmitterGAM::Execute() {
    //read the ADC (done by input broker)
    //map adc value on pwm duty cycle (to be done here)
    //REPORT_ERROR_PARAMETERS(ErrorManagement::Warning, "Executing... %d %d", rangeMotorRef, rangeMotorIn);

    *usb[0] = *timer;
    *usb[1] = *adcMotor;
    *usb[2] = *adcDrive;

    float32 factor = (*adcMotor - minMotorRef) / ((float32) rangeMotorRef);
    float32 calc = minMotorIn + (factor * rangeMotorIn);
    *pwms = (uint16) calc;
    factor = (*adcDrive - minDriveRef) / ((float32) rangeDriveRef);
    calc = minDriveIn + (factor * rangeDriveIn);
    *usb[3] = *pwms;

    *pwms <<= 8;
    *pwms |= (uint16) calc;

    *usb[4] = ((*pwms) & 0xff);
    //write on pwm (done by output broker)
    return true;
}

CLASS_REGISTER(TransmitterGAM, "1.0")
