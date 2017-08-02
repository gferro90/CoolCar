/**
 * @file CameraUSBDrv.cpp
 * @brief Source file for class CameraUSBDrv
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

 * @details This source file contains the definition of all the methods for
 * the class CameraUSBDrv (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "CameraUSBDrv.h"
#include "Endianity.h"
#include "CDBExtended.h"
#include "FastPollingMutexSem.h"
#include "Sleep.h"
#include "stdio.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

CameraUSBDrv::CameraUSBDrv() :
        USBDrv() {
    // Auto-generated constructor stub for CameraUSBDrv
    // TODO Verify if manual additions are needed

    cameraId = 0;
    calibrateCycles = 0;
    showMode = 0;

    //graphic objects
    capture = NULL;

    frameMat = NULL;

    modules = NULL;
    numberOfModules = 0;
    dataWriteBuffer = NULL;
    outputBufferSize = 0;
}

CameraUSBDrv::~CameraUSBDrv() {
    // Auto-generated destructor stub for CameraUSBDrv
    // TODO Verify if manual additions are needed
    if (capture != NULL) {
        delete capture;
        capture = NULL;
    }

    if (frameMat != NULL) {
        delete frameMat;
        frameMat = NULL;
    }
    if (dataWriteBuffer != NULL) {
        delete[] dataWriteBuffer;
        dataWriteBuffer = NULL;
    }
}

/**
 * ObjectLoadSetup
 */

bool CameraUSBDrv::ObjectLoadSetup(ConfigurationDataBase &cdbData,
                                   StreamInterface *err) {

    bool ret = USBDrv::ObjectLoadSetup(cdbData, err);
    CDBExtended cdb(cdbData);

    if (ret) {
        ret = cdb.ReadInt32(outputBufferSize, "OutputBufferSize", 1);
        if (ret) {
            printf("\nallocated %d for dataWriteBuffer\n", outputBufferSize);
            dataWriteBuffer = new char[outputBufferSize];
            memset(dataWriteBuffer, 0, outputBufferSize);
        }
        else {
            AssertErrorCondition(InitialisationError, "CameraUSBDrv::ObjectLoadSetup: %s OutputBufferSize not specified", Name());
        }

    }

    int32 sleepAfterInit=0;
    //Initialisation
    if (ret) {
        printf("\nCam 1\n");

        if (!cdb.ReadInt32(cameraId, "CameraID", 0)) {
            AssertErrorCondition(Warning, "CameraUSBDrv::ObjectLoadSetup: %s CameraID not specified. Using default: %d", Name(), cameraId);
        }

        if (!cdb.ReadInt32(calibrateCycles, "CalibrateCycles", 60)) {
            AssertErrorCondition(Warning, "CameraUSBDrv::ObjectLoadSetup: %s calibrateCycles not specified. Using default: %d", Name(), calibrateCycles);
        }

        // 0 no shows
        // 1 show before calibration
        // 2 show during execution
        // 3 show all images during ex
        if (!cdb.ReadInt32(showMode, "ShowMode", 0)) {
            AssertErrorCondition(Warning, "CameraUSBDrv::ObjectLoadSetup: %s showMode not specified. Using default: %d", Name(), showMode);
        }
        if (!cdb.ReadInt32(sleepAfterInit, "SleepAfterInit", 0)) {
            AssertErrorCondition(Warning, "CameraUSBDrv::ObjectLoadSetup: %s showMode not specified. Using default: %d", Name(), showMode);
        }
    }


    // camera intialisation
    if (ret) {
        printf("\nCam 2\n");

        // ranges of HSV boundaries
        int t1min = 0, t1max = 0, t2min = 0, t2max = 0, t3min = 0, t3max = 0;

        // Open capture device. 0 is /dev/video0, 1 is /dev/video1, etc.
        capture = new VideoCapture(cameraId);

        // Create a window in which the captured images will be presented
        // grab an image from the capture
        frameMat = new Mat();
        for (int i = 0; i < calibrateCycles; i++) {
            (*capture) >> (*frameMat);
        }

        if (frameMat->empty()) {
            AssertErrorCondition(InitialisationError, "CameraUSBDrv::Capture is NULL");
            return false;
        }
    }

    if (ret) {
        printf("\nCam 3\n");

        uint32 mySize = Size();

        for (uint32 i = 0u; i < mySize; i++) {
            GCRTemplate<CamModule> ref = Find(i);
            if (ref.IsValid()) {
                numberOfModules++;
            }
        }

        if (numberOfModules > 0) {
            modules = new GCRTemplate<CamModule> [numberOfModules];
        }

        uint32 n=0;
        for (uint32 i = 0u; i < mySize; i++) {
            GCRTemplate<CamModule> ref = Find(i);
            if (ref.IsValid()) {
                modules[n] = ref;
                if(!modules[n]->Init(frameMat, capture, showMode, (char*)dataBuffer, dataWriteBuffer)){
                    AssertErrorCondition(Warning, "CameraUSBDrv::Failed initialisation of %s", modules[n]->Name());
                }
                n++;
            }
        }
        uint32 defaultSize = ((numberOfInputChannels - 1) * sizeof(uint32));
        *dataBuffer = cycleCounter;
       /* while(read(usbFile, dataBuffer + 1, defaultSize)>0){

        }*/
        if (write(usbFile, dataWriteBuffer, outputBufferSize) < 0) {
            AssertErrorCondition(Warning, "CameraUSBDrv::USB write error");
        }
        SleepMsec(sleepAfterInit);

    }
    return ret;
}

/**
 * GetData
 */
int32 CameraUSBDrv::GetData(uint32 usecTime,
                            int32 *buffer,
                            int32 bufferNumber) {

    int32 ret = USBDrv::GetData(usecTime, buffer, bufferNumber);
    return ret;
}

/**
 * ObjectDescription
 */
bool CameraUSBDrv::ObjectDescription(StreamInterface &s,
                                     bool full,
                                     StreamInterface *err) {
    s.Printf("%s %s\n", ClassName(), Version());
    // Module name
    s.Printf("Module Name --> %s\n", Name());

    return true;
}

bool CameraUSBDrv::Poll() {
    //read diagnostics
    uint32 defaultSize = ((numberOfInputChannels - 1) * sizeof(uint32));
    *dataBuffer = cycleCounter;
    while(read(usbFile, dataBuffer + 1, defaultSize)>0){

    }
    /*
    int32 nRead = 0;
    while ((nRead += read(usbFile, dataBuffer + 1 + nRead, defaultSize)) < defaultSize) {
        defaultSize -= nRead;
    }*/
    // Get one frame
    (*capture) >> (*frameMat);

    //Execute modules
    for (uint32 i = 0u; i < numberOfModules; i++) {
        modules[i]->Execute();
    }
    //TODO Write the buffer on USB

    if (write(usbFile, dataWriteBuffer, outputBufferSize) < 0) {
        AssertErrorCondition(Warning, "CameraUSBDrv::USB write error");
    }

    cycleCounter++;
    for (int i = 0; i < nOfTriggeringServices; i++) {
        triggerService[i].Trigger();
    }

    //TODO Do everything here!!

    return true;
}

bool CameraUSBDrv::ProcessHttpMessage(HttpStream &hStream) {
#if 0
    hStream.SSPrintf("OutputHttpOtions.Content-Type", "text/html");
    hStream.keepAlive = false;

    FString changeStateStr;
    changeStateStr.SetSize(0);
    if (hStream.Switch("InputCommands.changeState")) {
        hStream.Seek(0);
        hStream.GetToken(changeStateStr, "");
        hStream.Switch((uint32) 0);
    }
    FString sendStr;
    sendStr.SetSize(0);
    if (hStream.Switch("InputCommands.send")) {
        hStream.Seek(0);
        hStream.GetToken(sendStr, "");
        hStream.Switch((uint32) 0);
    }

    FString increaseStr;
    increaseStr.SetSize(0);
    if (hStream.Switch("InputCommands.increase")) {
        hStream.Seek(0);
        hStream.GetToken(increaseStr, "");
        hStream.Switch((uint32) 0);
    }

    FString decreaseStr;
    decreaseStr.SetSize(0);
    if (hStream.Switch("InputCommands.decrease")) {
        hStream.Seek(0);
        hStream.GetToken(decreaseStr, "");
        hStream.Switch((uint32) 0);
    }

    if (changeStateStr == "1") {
        inputsFromHttp[0] = (int) -1;
        FString stateStr;
        stateStr.SetSize(0);
        if (hStream.Switch("InputCommands.state")) {
            hStream.Seek(0);
            hStream.GetToken(stateStr, "");
            hStream.Switch((uint32) 0);
            inputsFromHttp[1] = (int) atoi(stateStr.Buffer());
        }

        bool boardState = boardIsOn;
        boardIsOn = (inputsFromHttp[1] != -1);
        if (!boardState && boardIsOn) {
            (printf("\nSend Initial packet...!\n"));
            const uint32 packetSize = 63;
            const uint32 buffSize = 64;

            char buff[buffSize];
            uint32 sizeToW = buffSize;

            uint32 written = 0;
            memset(buff, '@', sizeToW);
            do {
                sizeToW -= written;
                written += write(usbFile, buff + written, sizeToW);
            }
            while (written < sizeToW);
        }
        boardIsOn ? (printf("\nBoard Is On!\n")) : (printf("\nBoard Is Off!\n"));

    }
    else {
        FString motorNumberStr;
        motorNumberStr.SetSize(0);
        if (hStream.Switch("InputCommands.motorNumber")) {
            hStream.Seek(0);
            hStream.GetToken(motorNumberStr, "");
            hStream.Switch((uint32) 0);
            inputsFromHttp[0] = (int) atoi(motorNumberStr.Buffer());
        }
        if (sendStr == "1") {
            FString motorReferenceStr;
            motorReferenceStr.SetSize(0);
            if (hStream.Switch("InputCommands.motorReference")) {
                hStream.Seek(0);
                hStream.GetToken(motorReferenceStr, "");
                hStream.Switch((uint32) 0);
                inputsFromHttp[1] = (int) atoi(motorReferenceStr.Buffer());
            }
        }

        if (increaseStr == "1") {
            inputsFromHttp[1] = 1;
        }
        if (decreaseStr == "1") {
            inputsFromHttp[1] = -1;
        }
    }

    hStream.Printf("<html><head><title>CameraUSBDrv</title></head>\n");
    hStream.Printf("<body>\n");

    hStream.Printf("Motor number:<br>\n");
    hStream.Printf("<form><input type=\"text\" name=\"motorNumber\">");
    hStream.Printf("<button type=\"submit\" name=\"increase\" value=\"1\">+</button>\n");
    hStream.Printf("<button type=\"submit\" name=\"decrease\" value=\"1\">-</button><br>\n");

    hStream.Printf("Reference:<br>\n");
    hStream.Printf("<input type=\"text\" name=\"motorReference\"><br>");

    hStream.Printf("<button type=\"submit\" name=\"send\" value=\"1\">Send</button><br>\n");

    hStream.Printf("State:<br>\n");
    hStream.Printf("<input type=\"text\" name=\"state\"><br>");
    hStream.Printf("<button type=\"submit\" name=\"changeState\" value=\"1\">Change State</button>");
    hStream.Printf("</form>\n</body></html>\n");
    //copy to the client
    hStream.WriteReplyHeader(true);

    write(usbFile, inputsFromHttp, (2 * sizeof(int)));
    printf("\n %d %d\n", inputsFromHttp[0], inputsFromHttp[1]);
#endif
    return true;
}

OBJECTLOADREGISTER(CameraUSBDrv, "$Id: CameraUSBDrv.cpp 3 2012-01-15 16:26:07Z aneto $")
