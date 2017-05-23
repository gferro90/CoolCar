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
#include "HighResolutionTimer.h"
#include "stdio.h"
#include "string.h"
/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/
#define AdjustCounterDirection(encoder, dir)  (uint32)((uint16) ((dir==-1)*0x10000+dir*encoder))

extern uint32 sentPacketNumber;
/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

CoolCarControlGAM::CoolCarControlGAM() {
    // Auto-generated constructor stub for CoolCarControlGAM
    // TODO Verify if manual additions are needed

    pwmMotor = NULL;
    pwmDrive = NULL;
    refs = NULL;
    usb = NULL;
    timer = NULL;
    stops = NULL;
    encoder =NULL;

    maxMotorIn = 0;
    minMotorIn = 0;
    maxDriveIn = 0;
    minDriveIn = 0;
    noObstacle = 0;
    obstacle = 0;
    obstacleDetected = NULL;
    numberOfStops = 0;
    receiveOnlyRange = 0u;
    encoderStore=0u;
    counterDirection=1;
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
        if (!data.Read("ReceiveOnlyRange", receiveOnlyRange)) {
            receiveOnlyRange = 0u;
        }
        if (!data.Read("CounterDirection", counterDirection)) {
            counterDirection = 1u;
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
    encoder = (uint32*) (refs +1);

    pwmMotor = (uint32 *) GetOutputSignalsMemory();
    pwmDrive = (uint32 *) GetOutputSignalsMemory() + 1;
    usb = (int32*) GetOutputSignalsMemory() + 2;
    tic=HighResolutionTimer::Counter();
}

bool CoolCarControlGAM::Execute() {

    uint32 motorRec = (uint32)((*refs) >> 8);
    uint32 driveRec = (uint32)((*refs) & 0xff);

    float dt=(HighResolutionTimer::Counter()-tic)*1.0e-6;
    tic=HighResolutionTimer::Counter();


    //REPORT_ERROR_PARAMETERS(ErrorManagement::Warning,"\nExecuting %f %d\n", dt, tic);
    usb[0]=sentPacketNumber;
    sentPacketNumber++;
    usb[1] = *timer;
    usb[2] = motorRec;
    usb[3] = driveRec;
    uint32 storeEncoder = *encoder;

    //encoder position and speed
    //do the difference with the previous
    *encoder=AdjustCounterDirection(*encoder, counterDirection);
    *encoder=(uint32)(encoderStore+(int16)((*encoder)-encoderStore));
    usb[4]= *encoder;

    float speed=0.;
    if(dt!=0.){
        speed=((int32)(*encoder-encoderStore))/dt;
    }
    memcpy(&usb[5], &speed, sizeof(float));
    //usb[5]= speed;
    encoderStore=*encoder;
    *encoder=storeEncoder;

    if (receiveOnlyRange != 0u) {
        if (motorRec != 0) {
            motorRec += minMotorIn;
        }
        if (driveRec != 0) {
            driveRec += minDriveIn;
        }
    }

    bool isObstacle = false;

    //histeresys
    for (uint32 i = 0u; i < numberOfStops; i++) {
        if ((stops[i] >= obstacle) && (obstacleDetected[i] == 0u)) {
            //exit if one obstacle has been detected
            obstacleDetected[i] = 1u;
            isObstacle = true;
        }
        if ((stops[i] < noObstacle) && (obstacleDetected[i] == 1u)) {
            obstacleDetected[i] = 0u;
        }
    }

    if ((motorRec >= minMotorIn) && (motorRec <= maxMotorIn)) {
        *pwmMotor = motorRec;

    }
    if ((driveRec >= minDriveIn) && (driveRec <= maxDriveIn)) {
        *pwmDrive = driveRec;

    }
    // if one of the sensors has detected an obstacle stop the car
    if (isObstacle) {
        *pwmMotor = (maxMotorIn + minMotorIn) / 2;

    }


    //write on pwm (done by output broker)
    return true;
}

CLASS_REGISTER(CoolCarControlGAM, "1.0")
