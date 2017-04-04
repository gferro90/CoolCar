/**
 * @file PIDController.cpp
 * @brief Source file for class PIDController
 * @date 23/feb/2017
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
 * the class PIDController (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "PIDController.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

PIDController::PIDController() :
        Controller() {
    Kp = 0.;
    Ki = 0.;
    Kd = 0.;
    integralTerm = 0.;
    error_1=0.;
}

PIDController::~PIDController() {
    // Auto-generated destructor stub for PIDController
    // TODO Verify if manual additions are needed

}

bool PIDController::ObjectLoadSetup(ConfigurationDataBase &cdbData,
                                    StreamInterface * err) {

    bool ret = Controller::ObjectLoadSetup(cdbData, err);

    CDBExtended cdb(cdbData);

    if (ret) {
        //default black line
        if (!cdb.ReadFloat(Kp, "Kp", 0.)) {
            AssertErrorCondition(Warning, "PIDController::ObjectLoadSetup: %s Kp not specified. Using default: %f", Name(), Kp);
        }
        if (!cdb.ReadFloat(Ki, "Ki", 0.)) {
            AssertErrorCondition(Warning, "PIDController::ObjectLoadSetup: %s Ki not specified. Using default: %f", Name(), Ki);
        }
        if (!cdb.ReadFloat(Kd, "Kd", 0.)) {
            AssertErrorCondition(Warning, "PIDController::ObjectLoadSetup: %s Kd not specified. Using default: %f", Name(), Kd);
        }
        if (!cdb.ReadFloat(integralTerm, "IntegralCI", 0.)) {
            AssertErrorCondition(Warning, "PIDController::ObjectLoadSetup: %s integralTerm not specified. Using default: %f", Name(), integralTerm);
        }
    }
    return ret;
}

float PIDController::Execute(float error,
                             float dt) {

    float propTerm = Kp * error;
    float derTerm = 0.;
    if (dt > 0.) {
        derTerm = Kd * (error - error_1) / dt;
    }
    integralTerm += Ki * error * dt;
    error_1 = error;
    return propTerm + derTerm + integralTerm;
    //TODO Ki and Kd

}

OBJECTLOADREGISTER(PIDController, "$Id:PIDController.cpp,v 1.1.1.1 2010-01-20 12:26:47 pc Exp $")
