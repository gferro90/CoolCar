/**
 * @file RemoteDriverClient.h
 * @brief Header file for class RemoteDriverClient
 * @date Jul 24, 2017
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

 * @details This header file contains the declaration of the class RemoteDriverClient
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef REMOTEDRIVERCLIENT_H_
#define REMOTEDRIVERCLIENT_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#define int64 int64_t
#define uint64 uint64_t
#include "Utilities.h"
#undef int64
#undef uint64
#include "System.h"
#include "GenericAcqModule.h"
#include "File.h"
#include "CamModule.h"
#include "sys/select.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/
OBJECT_DLL (RemoteDriverClient)

class RemoteDriverClient: public CamModule {
    OBJECT_DLL_STUFF (RemoteDriverClient)

public:
    RemoteDriverClient();

    virtual ~RemoteDriverClient();

    virtual bool ObjectLoadSetup(ConfigurationDataBase &cdbData,
                                 StreamInterface * err);

    virtual bool Init(Mat * frameMatIn,
                      VideoCapture* captureIn,
                      int showModeIn,
                      char *bufferIn,
                      char *bufferOut);

    virtual bool Execute();

    virtual bool ProcessHttpMessage(HttpStream &hStream);

private:

    void SendAndReceive(const char* question,
                        char* response,
                        int32 respSize,
                        uint32 usecTimeout = 0,
                        uint32 secTimeout = 2,
                        uint32 maxWaitLoops = 100,
                        bool print = true);

    void WriteToGsm(const void* data,
                    uint32 size,
                    bool terminate = true);

    void ReadFromGsm(void* data,
                     uint32 size);

    VideoCapture *capture;

    //initialization
    FString ipAddress;
    int32 port;

    int32 positionIndex;
    int32 directionIndex;

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

    int32 minRowIndex;
    int32 maxRowIndex;
    int32 minColIndex;
    int32 maxColIndex;

    int32 packetSize;
    //state
    Mat *frameMat;
    Mat *newFrame;
    //outputBuffer
    char* outputBuffer;
    char* inputBuffer;

    int32 socketFd;

    int32 *position;
    int32 *direction;


    float x;
    float y;
    float theta;
    float speed;
    float omega;
    float pos_1;
    uint64 lastUpdateTime;

    float positionFactor;
    float directionFactor;
    float positionMinIn;
    float positionMinOut;
    float directionMinIn;
    float directionMinOut;

    float chassisLength;
    float initialXposition;
    float initialYposition;
    float initialOrientation;


};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* REMOTEDRIVER_H_ */

