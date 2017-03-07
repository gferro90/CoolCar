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

    pwmMotor = NULL;
    pwmDrive = NULL;
    refs = NULL;
    usb[0] = NULL;
    usb[1] = NULL;
    usb[2] = NULL;
    timer = NULL;
    stops = NULL;

    maxMotorIn = 0;
    minMotorIn = 0;
    maxDriveIn = 0;
    minDriveIn = 0;
    noObstacle = 0;
    obstacle = 0;
    obstacleDetected = NULL;
    numberOfStops = 0;
}

CoolCarControlGAM::~CoolCarControlGAM() {
    // Auto-generated destructor stub for CoolCarControlGAM
    // TODO Verify if manual additions are needed
    if (obstacleDetected != NULL) {
        delete[] obstacleDetected;
        obstacleDetected = NULL;
    }
}

bool CoolCarControlGAM::Initialise(StructuredDataI &data) {
    bool ret = GAM::Initialise(data);

    //custom initialisation
    if (ret) {
        if (!data.Read("MaxMotorIn", maxMotorIn)) {
            maxMotorIn = 20;
        }
        if (!data.Read("MinMotorIn", minMotorIn)) {
            minMotorIn = 10;
        }
        if (!data.Read("MaxDriveIn", maxDriveIn)) {
            maxDriveIn = 17;
        }
        if (!data.Read("MinDriveIn", minDriveIn)) {
            minDriveIn = 7;
        }
        if (!data.Read("ObstacleADC", obstacle)) {
            obstacle = 2000;
        }
        if (!data.Read("NoObstacleADC", noObstacle)) {
            noObstacle = 5;
        }
        if (!data.Read("NumberOfStops", numberOfStops)) {
            numberOfStops = 2;
        }
        if (obstacleDetected != NULL) {
            delete[] obstacleDetected;
        }
        obstacleDetected = new uint8[numberOfStops];
        for (uint32 i = 0u; i < numberOfStops; i++) {
            obstacleDetected = 0u;
        }
    }
    return ret;
}

void CoolCarControlGAM::Setup() {
    //assign here the pointer to signals
    timer = (uint32*) GetInputSignalsMemory();
    stops = (uint32 *) GetInputSignalsMemory() + 1;
    refs = (uint16 *) stops + 2 * numberOfStops;

    usb[0] = (uint32*) GetOutputSignalsMemory();
    usb[1] = (uint32 *) GetOutputSignalsMemory() + 1;
    usb[2] = (uint32 *) GetOutputSignalsMemory() + 2;
    pwmMotor = (uint32 *) GetOutputSignalsMemory() + 3;
    pwmDrive = (uint32 *) GetOutputSignalsMemory() + 4;
}

bool CoolCarControlGAM::Execute() {

    *usb[0] = *timer;
    *usb[1] = (uint32)((*refs) >> 8);
    *usb[2] = (uint32)((*refs) & 0xff);

    bool isObstacle = false;

    //histeresys
    for (uint32 i = 0u; i < numberOfStops; i++) {
        if ((stops[i] >= obstacle) && (obstacleDetected[i]==0u)) {
            //exit if one obstacle has been detected
            obstacleDetected[i] = 1u;
            isObstacle = true;
        }
        if ((stops[i] < noObstacle) && (obstacleDetected[i]==1u)) {
            obstacleDetected[i] = 0u;
        }
    }

    if ((*usb[1] >= minMotorIn) && (*usb[1] <= maxMotorIn)) {
        *pwmMotor = *usb[1];
    }
    if ((*usb[2] >= minDriveIn) && (*usb[2] <= maxDriveIn)) {
        *pwmDrive = *usb[2];
    }
    // if one of the sensors has detected an obstacle stop the car
    if (isObstacle) {
        *pwmMotor = (maxMotorIn + minMotorIn) / 2;
    }
   // REPORT_ERROR_PARAMETERS(ErrorManagement::Warning, "Received %d %d", *usb[1], *usb[2]);

    //write on pwm (done by output broker)
    return true;
}

CLASS_REGISTER(CoolCarControlGAM, "1.0")
