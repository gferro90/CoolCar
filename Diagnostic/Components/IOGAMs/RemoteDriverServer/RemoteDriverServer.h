/**
 * @file RemoteDriverServer.h
 * @brief Header file for class RemoteDriverServer
 * @date 28/mar/2017
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

 * @details This header file contains the declaration of the class RemoteDriverServer
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef REMOTEDRIVERSERVER_H_
#define REMOTEDRIVERSERVER_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#define NO_OPENCV

#define int64 int64_t
#define uint64 uint64_t
#include "Utilities.h"
#undef int64
#undef uint64
#include "System.h"
#include "GenericAcqModule.h"
#include "File.h"
#include "EventSem.h"
//TODO Opencv stuff

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

OBJECT_DLL (RemoteDriverServer)

class RemoteDriverServer: public GenericAcqModule {

    OBJECT_DLL_STUFF (RemoteDriverServer)

public:
    RemoteDriverServer();

    virtual ~RemoteDriverServer();

    virtual bool ObjectLoadSetup(ConfigurationDataBase &cdbData,
                            StreamInterface * err);


    virtual bool ProcessHttpMessage(HttpStream &hStream);

    virtual bool ObjectSaveSetup(ConfigurationDataBase &info,
                                 StreamInterface *err);



    virtual bool ObjectDescription(StreamInterface &s,
                                   bool full,
                                   StreamInterface *err);

    virtual bool SetInputBoardInUse(bool on = True);

    virtual bool SetOutputBoardInUse(bool on = True);


//    int64 GetUsecTime();

    virtual bool Poll();

    bool PulseStart();



    int32 GetData(uint32 usecTime,
                  int32 *buffer,
                  int32 bufferNumber = 0);

    bool WriteData(uint32 usecTime,
                   const int32 *buffer);
private:

    //config params

    int32 httpRefreshTime;

    float speedRefStep;
    float driveRefStep;

    float zeroSpeedControl;
    float zeroDriveControl;

    float driveControlMin;
    float driveControlMax;
    int32 drivePwmMin;
    int32 drivePwmMax;

    float speedControlMin;
    float speedControlMax;
    int32 speedPwmMin;
    int32 speedPwmMax;

    FString ipAddress;
    int32 port;

    //state
    //outputBuffer
    float lastUpdateTime;
    int32 *inputBuffer;
    int32 inputByteSize;
    int32 cpuMask;

    float x;
    float y;
    float theta;
    float speed;
    float omega;

    bool showCamera;
    bool manualDrive;
    bool useModel;
    float driveReference;
    float speedReference;

    int32 socketFd;
    int32 clientFd;

    uint32 packetSize;
    FString camData;
    char * readBuffer;
    EventSem eventSem;
    int32 camMode;
    float speedRefModelSat;
    float directionMaxOut;
    float speedStepModelStep;
    float driveStepModelStep;
};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* DIAGNOSTIC_COMPONENTS_IOGAMS_CAMERAUSBDRV_COOLCARDIAGNOSTIC_H_ */

