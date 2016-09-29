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

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

CoolCarControlGAM::CoolCarControlGAM() {
    // Auto-generated constructor stub for CoolCarControlGAM
    // TODO Verify if manual additions are needed
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
        }
        else {
            //todo error
        }
        if (ret) {
            rangeMotorRef -= minMotorRef;
        }
        if (ret) {
            ret = data.Read("MinMotorIn", minMotorIn);
        }
        else {
            //todo error
        }
        if (ret) {
            ret = data.Read("MaxMotorIn", rangeMotorIn);
        }
        else{
            //todo error
        }
        if (ret) {
            rangeMotorIn -= minMotorIn;
        }
    }

    return ret;
}

bool CoolCarControlGAM::Execute() {
    //read the ADC (done by input broker)
    //map adc value on pwm duty cycle (to be done here)
    uint32 *adcValue = (uint32 *) GetInputSignalMemory();
    uint32 *pwmValue =(uint32 *) GetOutputSignalMemory()

    *pwmValue = minMotorIn + ((*adcValue - minMotorRef) / (rangeMotorRef)) * rangeMotorIn;
    //write on pwm (done by output broker)
    return true;
}
