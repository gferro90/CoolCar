/**
 * @file RemoteDriverClient.cpp
 * @brief Source file for class RemoteDriverClient
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

 * @details This source file contains the definition of all the methods for
 * the class RemoteDriverClient (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "RemoteDriverClient.h"
#include "base64.h"
/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

RemoteDriverClient::RemoteDriverClient() {

    capture = NULL;
    //initialization
    port = 0;
    positionIndex = 0;
    directionIndex = 0;

    zeroSpeedControl = 0.;
    zeroDriveControl = 0.;

    driveControlMin = 0.;
    driveControlMax = 0.;
    drivePwmMin = 0;
    drivePwmMax = 0;

    speedControlMin = 0.;
    speedControlMax = 0.;
    speedPwmMin = 0;
    speedPwmMax = 0;

    minRowIndex = -1;
    maxRowIndex = -1;
    minColIndex = -1;
    maxColIndex = -1;

    packetSize = 1024;

    positionFactor = 0.;
    directionFactor = 0.;
    positionMinIn = 0.;
    positionMinOut = 0.;
    directionMinIn = 0.;
    directionMinOut = 0.;

    chassisLength = 0.;
    initialXposition = 0.;
    initialYposition = 0.;
    initialOrientation = 0.;

    //state
    frameMat = NULL;
    newFrame = NULL;
    //outputBuffer
    outputBuffer = NULL;
    inputBuffer = NULL;
    socketFd = -1;
    position = NULL;
    direction = NULL;

    x = 0.;
    y = 0.;
    theta = 0.;
    speed = 0.;
    omega = 0.;

    pos_1 = 0.;
    lastUpdateTime = 0;
}

RemoteDriverClient::~RemoteDriverClient() {

    if (newFrame != NULL) {
        delete newFrame;
        newFrame = NULL;
    }
}

bool RemoteDriverClient::ObjectLoadSetup(ConfigurationDataBase &cdbData,
                                         StreamInterface * err) {
    bool ret = GCReferenceContainer::ObjectLoadSetup(cdbData, err);

    CDBExtended cdb(cdbData);
    if (ret) {

        if (!cdb.ReadInt32(positionIndex, "PositionIndex", 1)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s positionIndex not specified. Using default: %d", Name(), positionIndex);
        }

        if (!cdb.ReadInt32(directionIndex, "DirectionIndex", 2)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s directionIndex not specified. Using default: %d", Name(), directionIndex);
        }

        if (!cdb.ReadFString(ipAddress, "ServerIPaddress", "127.0.0.1")) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s ServerIPaddress not specified. Using default: %s", Name(),
                                 ipAddress.Buffer());
        }

        if (!cdb.ReadInt32(port, "Port", 4444)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s Port not specified. Using default: %d", Name(), port);
        }

        //the factor to transform encoders in cm or m
        if (!cdb.ReadFloat(positionFactor, "PositionFactor", 1.)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s positionFactor not specified. Using default: %f", Name(), positionFactor);
        }

        //the factor to transform direction input to grades or radiants
        if (!cdb.ReadFloat(directionFactor, "DirectionFactor", 1.)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s directionFactor not specified. Using default: %f", Name(), directionFactor);
        }

        if (!cdb.ReadFloat(positionMinIn, "PositionMinIn", 0.)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s positionMinIn not specified. Using default: %f", Name(), positionMinIn);
        }

        if (!cdb.ReadFloat(positionMinOut, "PositionMinOut", 0.)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s positionMinOut not specified. Using default: %f", Name(), positionMinOut);
        }

        if (!cdb.ReadFloat(directionMinIn, "DirectionMinIn", 0.)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s directionMinIn not specified. Using default: %f", Name(), directionMinIn);
        }

        if (!cdb.ReadFloat(directionMinOut, "DirectionMinOut", 0.)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s directionMinOut not specified. Using default: %f", Name(), directionMinOut);
        }

        if (!cdb.ReadFloat(chassisLength, "ChassisLength", 10.)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s chassisLength not specified. Using default: %f", Name(), chassisLength);
        }

        if (!cdb.ReadFloat(initialXposition, "InitialXposition", 0.)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s initialXposition not specified. Using default: %f", Name(),
                                 initialXposition);
        }

        if (!cdb.ReadFloat(initialYposition, "InitialYposition", 0.)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s initialYposition not specified. Using default: %f", Name(),
                                 initialYposition);
        }

        if (!cdb.ReadFloat(initialOrientation, "InitialOrientation", 0.)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s initialOrientation not specified. Using default: %f", Name(),
                                 initialOrientation);
        }

        if (!cdb.ReadFloat(driveControlMin, "MinDriveControl", -5000.)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s driveControlMin not specified. Using default: %f", Name(), driveControlMin);
        }
        if (!cdb.ReadFloat(driveControlMax, "MaxDriveControl", 5000.)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s driveControlMax not specified. Using default: %f", Name(), driveControlMax);
        }

        if (!cdb.ReadInt32(drivePwmMin, "MinDrivePwm", 140)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s drivePwmMin not specified. Using default: %d", Name(), drivePwmMin);
        }

        if (!cdb.ReadInt32(drivePwmMax, "MaxDrivePwm", 340)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s drivePwmMax not specified. Using default: %d", Name(), drivePwmMax);
        }

        if (!cdb.ReadFloat(speedControlMin, "MinSpeedControl", -7200.)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s speedControlMin not specified. Using default: %f", Name(), speedControlMin);
        }
        if (!cdb.ReadFloat(speedControlMax, "MaxSpeedControl", 7200.)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s speedControlMax not specified. Using default: %f", Name(), speedControlMax);
        }

        if (!cdb.ReadInt32(speedPwmMin, "MinSpeedPwm", 200)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s speedPwmMin not specified. Using default: %d", Name(), speedPwmMin);
        }

        if (!cdb.ReadInt32(speedPwmMax, "MaxSpeedPwm", 400)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s speedPwmMin not specified. Using default: %d", Name(), speedPwmMin);
        }

        if (!cdb.ReadFloat(zeroSpeedControl, "ZeroSpeedControl", 0.)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s zeroSpeedControl not specified. Using default: %f", Name(),
                                 zeroSpeedControl);
        }

        if (!cdb.ReadFloat(zeroDriveControl, "ZeroDriveControl", 0.)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s zeroDriveControl not specified. Using default: %f", Name(),
                                 zeroDriveControl);
        }

        if (!cdb.ReadInt32(minRowIndex, "FromRow", -1)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s FromRow not specified. Transfer from the first row", Name());
        }

        if (!cdb.ReadInt32(maxRowIndex, "ToRow", -1)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s FromRow not specified. Transfer up to the last row", Name());
        }

        if (!cdb.ReadInt32(minColIndex, "FromColumn", -1)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s FromColumn not specified. Transfer from the first column", Name());
        }

        if (!cdb.ReadInt32(maxColIndex, "ToColumn", -1)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s FromColumn not specified. Transfer up to the last column", Name());
        }

        if (!cdb.ReadInt32(packetSize, "PacketSize", 1024)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s PacketSize not specified. Using Default %d", Name(), packetSize);
        }

    }
    return ret;
}

bool RemoteDriverClient::Init(Mat *frameMatIn,
                              VideoCapture *captureIn,
                              int showModeIn,
                              char *bufferIn,
                              char *bufferOut) {

    capture = captureIn;
    frameMat = frameMatIn;

    inputBuffer = bufferIn;
    outputBuffer = bufferOut;

    if ((capture == NULL) || (frameMat == NULL) || (inputBuffer == NULL) || (outputBuffer == NULL)) {
        return false;
    }

    if (minRowIndex < 0) {
        minRowIndex = 0;
    }

    if ((maxRowIndex > (frameMat->rows)) || (maxRowIndex < 0)) {
        maxRowIndex = (frameMat->rows);
    }

    if (minColIndex < 0) {
        minColIndex = 0;
    }

    if ((maxColIndex > (frameMat->cols)) || (maxColIndex < 0)) {
        maxColIndex = (frameMat->cols);
    }

    if (maxRowIndex <= minRowIndex) {
        maxRowIndex = minRowIndex + 1;
        AssertErrorCondition(Warning, "RemoteDriverClient::maxRowIndex<=minRowIndex, consider maxRowIndex=minRowIndex+1", Name());
    }

    if (maxColIndex <= minColIndex) {
        maxColIndex = minColIndex + 1;
        AssertErrorCondition(Warning, "RemoteDriverClient::maxColIndex<=minColIndex, consider maxColIndex=minColIndex+1", Name());
    }

    //create the reduced image
    Rect rect(minColIndex, minRowIndex, (maxColIndex - minColIndex), (maxRowIndex - minRowIndex));
    newFrame = new Mat(*frameMat, rect);

    //accelerator to input pointers
    position = (int32*) (inputBuffer + positionIndex);
    direction = (int32*) (inputBuffer + directionIndex);

    //normal client connection to the server ip
    struct sockaddr_in client;
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0) {
        AssertErrorCondition(InitialisationError, "RemoteDriverClient::Init: Failed creation of socket = %d", socketFd);
        printf("\nfailed socket creation\n");
        return false;
    }
    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    inet_aton(ipAddress.Buffer(), &(client.sin_addr));
    printf("\nConnecting ip=%s port=%d...\n", ipAddress.Buffer(), port);

    if (connect(socketFd, (struct sockaddr*) &client, sizeof(client)) == -1) {
        AssertErrorCondition(InitialisationError, "RemoteDriverClient::Init: Failed connection to server ip %s on port %d", ipAddress.Buffer(), port);
        printf("\nfailed connect\n");
        return false;
    }

    //send to the server the packet size
    write(socketFd, &packetSize, sizeof(int32));

    //read the initial controls
    uint16 controls = 0;
    read(socketFd, &controls, sizeof(controls));

    //printf("\nread controls %d %d\n", controls&0xff, controls>>8);

    memcpy(outputBuffer, &controls, sizeof(controls));

    x = initialXposition;
    y = initialYposition;
    theta = initialOrientation;
    /*
     int32 speedControl = PWM_SPEED_REMAP(zeroSpeedControl);
     int32 driveControl = PWM_DRIVE_REMAP(zeroDriveControl);
     speedControl -= speedPwmMin;
     driveControl -= drivePwmMin;
     uint16 controls = (speedControl << 8);
     controls |= driveControl;

     memcpy(outputBuffer, &controls, sizeof(controls));
     */
    return true;
}

bool RemoteDriverClient::Execute() {

    //remap inputs to the useful measurements
    float pos = RemapInput((float) (*position), positionMinIn, positionMinOut, positionFactor, false);
    float alpha = 0.;
    if (*direction != 0) {
        alpha = RemapInput((float) (*direction) + drivePwmMin, directionMinIn, directionMinOut, directionFactor, true);
    }

    float dt = 0.;
    if (lastUpdateTime != 0) {
        dt = (HRT::HRTCounter() - lastUpdateTime) * HRT::HRTPeriod();
    }

    lastUpdateTime = HRT::HRTCounter();

    if (dt != 0.) {
        speed = (pos - pos_1) / dt;

        //compute speed using position and cycle time
        float dx = speed * cos(theta);
        float dy = speed * sin(theta);
        omega = (speed * tan(alpha)) / chassisLength;

        x += dx * dt;
        y += dy * dt;
        theta += omega * dt;
        pos_1 = pos;
    }

    float diagnosticData[] = { x, y, theta, speed, omega };
    //write the diagnostic data
    //printf("\nwriting d data...\n");
    write(socketFd, diagnosticData, 5 * sizeof(float));

    //read the mode
    int32 mode = 0;
    read(socketFd, &mode, sizeof(int32));
    string encoded;
    switch (mode) {
    //no image to send
    case 0: {
        encoded = "";
    }
        break;
    case 1: {
        vector < uchar > buf;
        Mat *grayFrame = new Mat(newFrame->size(), CV_8UC1);
        cvtColor(*newFrame, *grayFrame, CV_BGR2GRAY);
        imencode(".jpg", *grayFrame, buf);
        encoded = base64_encode(&buf[0], buf.size());
        delete grayFrame;
    }
        break;
    case 2: {
        vector < uchar > buf;
        imencode(".jpg", *newFrame, buf);
        encoded = base64_encode(&buf[0], buf.size());
    }

    }

    //the total size to be transferred
    uint32 totalSize = strlen(encoded.c_str());
    //how many packets
    uint32 numberOfPackets = CEIL(totalSize, packetSize);

    //printf("\nnumberOfPackets=%d, totalSize=%d\n", numberOfPackets, totalSize);
    uint32 sizeToWrite = 0;
    uint32 k = 0u;
    for (k = 0u; k < numberOfPackets; k++) {
        //transfer packet per packet. The other part understand that the transmission ended
        //by checking if the size is minor than packetSize.
        const char* buffer = encoded.c_str() + k * packetSize;
        sizeToWrite = (strlen(buffer) < packetSize) ? (strlen(buffer)) : (packetSize);

        write(socketFd, buffer, sizeToWrite);
    }
    //send null char if the other part cannot see the end of transmission
    uint8 termChar = 0x1a;
    write(socketFd, &termChar, 1);

    //read controls
    uint16 controls = 0;
    //printf("\nreading controls...\n");
    read(socketFd, &controls, sizeof(controls));
    // printf("\nread controls %d %d\n", controls&0xff, controls>>8);

    memcpy(outputBuffer, &controls, sizeof(controls));

    return true;
}

bool RemoteDriverClient::ProcessHttpMessage(HttpStream &hStream) {
           hStream.SSPrintf("OutputHttpOtions.Content-Type", "text/html");
           hStream.Printf("<html>\n"
                   "<head>\n"
                   "<meta charset=\"UTF-8\">\n"
                   "</head>\n"
                   "<body>\n"
                   "<script language=\"javascript\" type=\"text/javascript\" src=\"/P5_DIR/libraries/p5.js\"></script>\n"
                   "<script language=\"javascript\" type=\"text/javascript\" src=\"/P5_DIR/sketch.js\"></script>\n"
                   "<style> body {padding: 0; margin: 0;} </style>\n"
                   "</body>\n"
                   "</html>\n",
                   10);
       hStream.WriteReplyHeader(True);
    return true;

}

OBJECTLOADREGISTER(RemoteDriverClient, "$Id:RemoteDriverClient.cpp,v 1.1.1.1 2010-01-20 12:26:47 pc Exp $")
