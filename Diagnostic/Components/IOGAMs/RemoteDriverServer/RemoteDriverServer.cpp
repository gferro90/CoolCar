/**
 * @file RemoteDriverServer.cpp
 * @brief Source file for class RemoteDriverServer
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
 * the class RemoteDriverServer (public, protected, and private). Be aware that some
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "RemoteDriverServer.h"
#include "base64.h"
#include "Sleep.h"
#include <math.h>

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

RemoteDriverServer::RemoteDriverServer() :
        GenericAcqModule() {
    // Auto-generated constructor stub for RemoteDriverServer
    // TODO Verify if manual additions are needed

    //graphics

    //config params

    httpRefreshTime = 1000;
    driveRefStep = 0.;
    speedRefStep = 0.;

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

    port = 0;
    camMode = 0;
    //state

    x = 0.;
    y = 0.;
    theta = 0.;

    lastUpdateTime = 0u;

    omega = 0.;
    speed = 0.;
    showCamera = false;
    manualDrive = false;
    useModel = false;
    speedReference = 0.;
    driveReference = 0.;

    socketFd = -1;
    inputBuffer = NULL;
    inputByteSize = 0;
    cpuMask = 0;
    packetSize = 0u;
    readBuffer = NULL;
    eventSem.Create();
    eventSem.Reset();
    speedRefModelSat = 0.;
    directionMaxOut = 0.;

    speedStepModelStep = 0.;
    driveStepModelStep = 0.;
}

RemoteDriverServer::~RemoteDriverServer() {
    // Auto-generated destructor stub for RemoteDriverServer
    // TODO Verify if manual additions are needed
    if (inputBuffer != NULL) {
        free((void *&) inputBuffer);
        inputBuffer = NULL;
    }
    if (readBuffer != NULL) {
        delete[] readBuffer;
    }
}

bool RemoteDriverServer::ObjectLoadSetup(ConfigurationDataBase &cdbData,
                                         StreamInterface * err) {

    bool ret = true;
    // Parent class Object load setup
    CDBExtended cdb(cdbData);
    if (!GenericAcqModule::ObjectLoadSetup(cdbData, err)) {
        AssertErrorCondition(InitialisationError, "RemoteDriverServer::ObjectLoadSetup: %s GenericAcqModule::ObjectLoadSetup Failed", Name());
        ret = false;
    }

    if (ret) {

        //Read destinationServerAddress destinationServerPort serverPort....
        if (numberOfInputChannels < 1) {
            AssertErrorCondition(InitialisationError, "RemoteDriverServer::EnableAcquisition: At least 1 input channels must be specified for the header");
            ret = false;
        }
    }
    if (ret) {

        // Read cpu mask
        if (!cdb.ReadInt32(cpuMask, "CpuMask", 0xFFFF)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s CpuMask was not specified. Using default: %d", Name(), cpuMask);
        }

        // Create Data Buffers. Compute total size and allocate storing buffer
        inputByteSize = numberOfInputChannels * sizeof(int32);
        inputBuffer = (int32 *) malloc(inputByteSize);

        if (!cdb.ReadInt32(httpRefreshTime, "HttpRefreshTime", 1000)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s HttpRefreshTime not specified. Using default: %d", Name(), httpRefreshTime);
        }

        if (!cdb.ReadInt32((int32&) showCamera, "ShowCamera", 0)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s ShowCamera not specified. Using default: %d", Name(), showCamera);
        }
        if (showCamera != 0) {
            showCamera = 1;
        }

        if (!cdb.ReadFloat(driveRefStep, "DriveRefStep", 0.)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s DriveRefStep not specified. Using default: %f", Name(), driveRefStep);
        }

        if (!cdb.ReadFloat(speedRefStep, "SpeedRefStep", 0.)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s SpeedRefStep not specified. Using default: %f", Name(), speedRefStep);
        }

        if (!cdb.ReadFloat(driveControlMin, "MinDriveControl", -5000.)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s driveControlMin not specified. Using default: %f", Name(), driveControlMin);
        }
        if (!cdb.ReadFloat(driveControlMax, "MaxDriveControl", 5000.)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s driveControlMax not specified. Using default: %f", Name(), driveControlMax);
        }

        if (!cdb.ReadInt32(drivePwmMin, "MinDrivePwm", 140)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s drivePwmMin not specified. Using default: %d", Name(), drivePwmMin);
        }

        if (!cdb.ReadInt32(drivePwmMax, "MaxDrivePwm", 340)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s drivePwmMax not specified. Using default: %d", Name(), drivePwmMax);
        }

        if (!cdb.ReadFloat(speedControlMin, "MinSpeedControl", -7200.)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s speedControlMin not specified. Using default: %f", Name(), speedControlMin);
        }
        if (!cdb.ReadFloat(speedControlMax, "MaxSpeedControl", 7200.)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s speedControlMax not specified. Using default: %f", Name(), speedControlMax);
        }

        if (!cdb.ReadInt32(speedPwmMin, "MinSpeedPwm", 200)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s speedPwmMin not specified. Using default: %d", Name(), speedPwmMin);
        }

        if (!cdb.ReadInt32(speedPwmMax, "MaxSpeedPwm", 400)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s speedPwmMin not specified. Using default: %d", Name(), speedPwmMin);
        }

        if (!cdb.ReadFloat(zeroSpeedControl, "ZeroSpeedControl", 0.)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s zeroSpeedControl not specified. Using default: %f", Name(),
                                 zeroSpeedControl);
        }

        if (!cdb.ReadFloat(zeroDriveControl, "ZeroDriveControl", 0.)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s zeroDriveControl not specified. Using default: %f", Name(),
                                 zeroDriveControl);
        }

        if (!cdb.ReadInt32(port, "Port", 4444)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s Port not specified. Using default: %d", Name(), port);
        }

        if (!cdb.ReadInt32(camMode, "CamMode", 2)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s CamMode not specified. Using default: %d", Name(), camMode);
        }

        if (!cdb.ReadFloat(speedRefModelSat, "SpeedRefModelSat", 1000.)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s SpeedRefRemoteSat not specified. Using default: %f", Name(),
                                 speedRefModelSat);
        }
        if (!cdb.ReadFloat(speedStepModelStep, "SpeedStepModel", 20.)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s SpeedStepModelStep not specified. Using default: %f", Name(),
                                 speedStepModelStep);
        }
        if (!cdb.ReadFloat(driveStepModelStep, "DriveStepModel", 50.)) {
            AssertErrorCondition(Warning, "RemoteDriverClient::ObjectLoadSetup: %s DriveStepModelStep not specified. Using default: %f", Name(),
                                 driveStepModelStep);
        }
        if (!cdb.ReadFloat(directionMaxOut, "DirectionMaxOut", 26.39)) {
            AssertErrorCondition(Warning, "RemoteDriverServer::ObjectLoadSetup: %s directionMaxOut not specified. Using default: %f", Name(), directionMaxOut);
        }
        if (ret) {
            //setup the server
            struct sockaddr_in server_opts;
            socketFd = socket(AF_INET, SOCK_STREAM, 0);
            if (socketFd < 0) {
                AssertErrorCondition(InitialisationError, "RemoteDriverClient::Init: Failed creation of socket = %d", socketFd);
                ret = false;
            }
            if (ret) {
                server_opts.sin_family = AF_INET;
                server_opts.sin_port = htons(port);
                server_opts.sin_addr.s_addr = INADDR_ANY;
                if (bind(socketFd, (struct sockaddr*) &server_opts, sizeof(server_opts)) != 0) {
                    AssertErrorCondition(InitialisationError, "RemoteDriverClient::Init: Failed bind");
                    ret = false;
                }
            }
            if (ret) {
                if (listen(socketFd, 1) != 0) {
                    AssertErrorCondition(InitialisationError, "RemoteDriverClient::Init: Failed listen");
                    ret = false;
                }
            }
            if (ret) {
                //accept the connection from the car
                struct sockaddr_in client_opts;
                int32 clientOptsLen = sizeof(client_opts);
                printf("\nAccepting...\n");
                clientFd = accept(socketFd, (struct sockaddr*) &client_opts, (socklen_t*) &clientOptsLen);
                printf("\nAccepted!!!\n");

                //first read: tell me how big is the packet and how many packets per frame
                read(clientFd, &packetSize, sizeof(uint32));
                printf("\nreceived packetSize=%d\n", packetSize);
                readBuffer = new char[packetSize + 1];

            }
        }

        if (ret) {
            int32 speedControl = PWM_SPEED_REMAP(zeroSpeedControl);
            int32 driveControl = PWM_DRIVE_REMAP(zeroDriveControl);
            speedControl -= speedPwmMin;
            driveControl -= drivePwmMin;
            uint16 controls = (speedControl << 8);
            controls |= driveControl;
            write(clientFd, &controls, sizeof(controls));

            lastUpdateTime = HRT::HRTCounter();
        }

    }
    return ret;
}

bool RemoteDriverServer::ProcessHttpMessage(HttpStream &hStream) {

    FString ajaxStr;
    ajaxStr.SetSize(0);

    eventSem.ResetWait();
    //mutex.Lock();
    ajaxStr = camData;
    //mutex.UnLock();
    eventSem.Post();

    FString showImageStr;
    showImageStr.SetSize(0);

    if (hStream.Switch("InputCommands.showImage")) {
        hStream.Seek(0);
        hStream.GetToken(showImageStr, "");
        hStream.Switch((uint32) 0);
    }

    if (showImageStr == "1") {
        showCamera = !showCamera;
    }

    FString useModelStr;
    useModelStr.SetSize(0);
    hStream.Seek(0);
    if (hStream.Switch("InputCommands.useModel")) {
        hStream.Seek(0);
        hStream.GetToken(useModelStr, "");
        hStream.Switch((uint32) 0);
    }

    if (useModelStr == "1") {
        useModel = !useModel;
    }

    FString manualStr;
    manualStr.SetSize(0);
    hStream.Seek(0);
    if (hStream.Switch("InputCommands.manualDriveButton")) {
        hStream.Seek(0);
        hStream.GetToken(manualStr, "");
        hStream.Switch((uint32) 0);
    }
    if (manualStr == "1") {
        manualDrive = !manualDrive;
    }

    FString refreshData;
    refreshData.SetSize(0);
    hStream.Seek(0);
    if (hStream.Switch("InputCommands.ImgRefreshData")) {
        hStream.Seek(0);
        hStream.GetToken(refreshData, "");
        hStream.Switch((uint32) 0);
        hStream.Printf("%s\n", ajaxStr.Buffer());
        hStream.WriteReplyHeader(True);
        return True;
    }

    FString upStr;
    upStr.SetSize(0);
    hStream.Seek(0);
    if (hStream.Switch("InputCommands.Up")) {
        hStream.Seek(0);
        hStream.GetToken(upStr, "");
        hStream.Switch((uint32) 0);
        speedReference += speedRefStep;
    }

    FString downStr;
    downStr.SetSize(0);
    hStream.Seek(0);
    if (hStream.Switch("InputCommands.Down")) {
        hStream.Seek(0);
        hStream.GetToken(downStr, "");
        hStream.Switch((uint32) 0);
        speedReference -= speedRefStep;
    }

    FString rightStr;
    rightStr.SetSize(0);
    hStream.Seek(0);
    if (hStream.Switch("InputCommands.Right")) {
        hStream.Seek(0);
        hStream.GetToken(rightStr, "");
        hStream.Switch((uint32) 0);
        driveReference -= driveRefStep;
    }

    FString leftStr;
    leftStr.SetSize(0);
    hStream.Seek(0);
    if (hStream.Switch("InputCommands.Left")) {
        hStream.Seek(0);
        hStream.GetToken(leftStr, "");
        hStream.Switch((uint32) 0);
        driveReference += driveRefStep;
    }

    FString stopSpeedStr;
    stopSpeedStr.SetSize(0);
    hStream.Seek(0);
    if (hStream.Switch("InputCommands.stopSpeed")) {
        hStream.Seek(0);
        hStream.GetToken(stopSpeedStr, "");
        hStream.Switch((uint32) 0);
        speedReference = 0;
    }

    FString stopDriveStr;
    stopDriveStr.SetSize(0);
    hStream.Seek(0);
    if (hStream.Switch("InputCommands.stopDrive")) {
        hStream.Seek(0);
        hStream.GetToken(stopDriveStr, "");
        hStream.Switch((uint32) 0);
        driveReference = 0;
    }

    FString camModeStr;
    camModeStr.SetSize(0);
    hStream.Seek(0);
    if (hStream.Switch("InputCommands.camMode")) {
        hStream.Seek(0);
        hStream.GetToken(camModeStr, "");
        hStream.Switch((uint32) 0);
        camMode = atoi(camModeStr.Buffer());
    }

    if (!manualDrive) {
        FString driveRefStr;
        driveRefStr.SetSize(0);
        hStream.Seek(0);
        if (hStream.Switch("InputCommands.driveRefInBox")) {
            hStream.Seek(0);
            hStream.GetToken(driveRefStr, "");
            hStream.Switch((uint32) 0);
            driveReference = atoi(driveRefStr.Buffer());
        }
        FString speedRefStr;
        speedRefStr.SetSize(0);
        hStream.Seek(0);
        if (hStream.Switch("InputCommands.speedRefInBox")) {
            hStream.Seek(0);
            hStream.GetToken(speedRefStr, "");
            hStream.Switch((uint32) 0);
            speedReference = atoi(speedRefStr.Buffer());
        }
    }

    hStream.SSPrintf("OutputHttpOtions.Content-Type", "text/html");

    hStream.keepAlive = False;
    hStream.WriteReplyHeader(False);

    hStream.Printf("<html><head><title>CoolCar-Diagnostics</title></head>\n");
    hStream.Printf("<script src=\"http://ajax.googleapis.com/ajax/libs/jquery/1.10.2/jquery.min.js\"></script>\n");

    hStream.Printf("<body onload=\"timedUpdate();\">\n");
    if (useModel) {
        hStream.Printf("<script language=\"javascript\" type=\"text/javascript\" src=\"/P5_DIR/libraries/p5.js\"></script>\n");
        hStream.Printf("<script>\n");

        hStream.Printf("const PI=3.14159265358979323846;\n"
                       "const canvasWidth=710;\n"
                       "const canvasHeight=400;\n"
                       "const carLength = 200;\n"
                       "const carWidth = 100;\n"
                       "const carHeight = 50;\n"
                       "const wheelRadius = 45;\n"
                       "const wheelHeight=25;\n"
                       "const rightDriveSat=-PI/5;\n"
                       "const leftDriveSat=PI/5;\n"
                       "const carPitch=PI/4;\n"
                       "var driveRef=0;\n"
                       "var speedGraphVal=0;\n"
                       "const speedGraphStep=0.1;\n"
                       "var speedRef=0;\n"
                       "var driveGraphVal=0;\n");
        hStream.Printf("const speedStep=50;\n", driveStepModelStep);
        hStream.Printf("const driveStep=%f;\n", driveStepModelStep);
        hStream.Printf("const driveRefSat=%f;\n", driveControlMax);
        hStream.Printf("const speedRefSat=%f;\n", speedRefModelSat);
        hStream.Printf("const driveFactor=(%f*PI/180)/%f;\n", directionMaxOut, driveControlMax);
        hStream.Printf("const driveGraphFactor=leftDriveSat/%f;\n", driveControlMax);

        hStream.Printf("</script>\n");

        hStream.Printf("<script language=\"javascript\" type=\"text/javascript\" src=\"/P5_DIR/sketch.js\"></script>\n");
    }
    if (showCamera) {
        hStream.Printf("<img src=\"\" id=\"myimage\" />\n");
    }
    hStream.Printf("<script>\n");

    hStream.Printf("var request = new XMLHttpRequest();\n");
    hStream.Printf("var requestPost = new XMLHttpRequest();\n");
    hStream.Printf("var camModeVar = 0;\n");
    hStream.Printf("var driveRefVar = 0;\n");
    hStream.Printf("var speedRefVar = 0;\n");
    if (!useModel) {
        hStream.Printf("var driveRef=0;\n");
        hStream.Printf("var speedRef=0;\n");
    }
    hStream.Printf("function timedUpdate() {\n");
    hStream.Printf("if ((driveRefVar!=driveRef)||(speedRefVar!=speedRef)");
    if (showCamera) {
        hStream.Printf("||(camModeVar!=document.getElementById(\"camModeId\").value)");
    }
    hStream.Printf(") {\n");
    hStream.Printf("requestPost.open('POST', \"\", true);\n");
    hStream.Printf("requestPost.setRequestHeader(\"Content-type\", \"application/x-www-form-urlencoded\");\n");
    hStream.Printf("requestPost.onreadystatechange = function() {\n");
    hStream.Printf("if(requestPost.readyState == 4 && requestPost.status == 200) {\n");
    hStream.Printf("}\n}\n");

    hStream.Printf("var posturl=\"driveRefInBox=\"+eval(driveRef)+\"&&speedRefInBox=\"+eval(speedRef);\n"
                   "driveRefVar=driveRef;\n"
                   "speedRefVar=speedRef;\n");
    if (showCamera) {
        hStream.Printf("camModeVar=document.getElementById(\"camModeId\").value;\n"
                       "posturl+=\"&&camMode=\"+eval(camModeVar);\n");
    }
    hStream.Printf("requestPost.send(posturl);\n");
    hStream.Printf("}\n");
    hStream.Printf("var url = \"?ImgRefreshData\";\n");
    hStream.Printf("request.open('GET', url, true);\n");
    hStream.Printf("request.onreadystatechange = imgManager;\n");
    hStream.Printf("request.send(null);\n");
    hStream.Printf("window.setTimeout(timedUpdate, %d);\n", httpRefreshTime);
    hStream.Printf("}\n");

    hStream.Printf("function imgManager() {\n");
    hStream.Printf("if(this.readyState != 4 || this.status != 200) {\n");
    hStream.Printf("return;\n");
    hStream.Printf("}\n");
    hStream.Printf("eval(request.responseText);\n");
//hStream.Printf("img.src=imgData;\n");
    if (showCamera) {
        hStream.Printf("document.getElementById(\"myimage\").src=imgData;\n");
        //hStream.Printf("document.getElementById(\"camModeId\").value=\"1\";\n");
    }
    if (useModel) {

        hStream.Printf("document.getElementById(\"driveRefInBoxId\").value=eval(driveRef);\n");
        hStream.Printf("document.getElementById(\"speedRefInBoxId\").value=eval(speedRef);\n");
    }
    hStream.Printf("for(i=0; i<5; i++){\n"
                   "document.getElementById(\"cell\"+i).innerHTML=diagnosticData[i];\n"
                   "}\n");

    /*      hStream.Printf("var canvas = document.getElementById(\"canvas\");\n"
     "var context = canvas.getContext(\"2d\");\n"
     "context.drawImage(img, 0, 0);\n");
     */
    hStream.Printf("}\n");

    hStream.Printf("</script>\n");

    hStream.Printf("<h1>Current diagnostics:</h1><br />");
    hStream.Printf("<p>Data was updated %f seconds ago<p/>", ((float) (HRT::HRTCounter() - lastUpdateTime) * (float) HRT::HRTPeriod()));

    hStream.Printf("<table border=\"1\"><tr><th></th><th>Last value</th></tr>");

    hStream.Printf("<tr><th>%s</th>", "X position");
    hStream.Printf("<td id=\"cell0\">%.3e</td></tr>", x);
    hStream.Printf("<tr><th>%s</th>", "Y position");
    hStream.Printf("<td id=\"cell1\">%.3e</td></tr>", y);
    hStream.Printf("<tr><th>%s</th>", "Theta");
    hStream.Printf("<td id=\"cell2\">%.3e</td></tr>", theta);
    hStream.Printf("<tr><th>%s</th>", "Speed");
    hStream.Printf("<td id=\"cell3\">%.3e</td></tr>", speed);
    hStream.Printf("<tr><th>%s</th>", "Omega");
    hStream.Printf("<td id=\"cell4\">%.3e</td></tr>", omega);

    hStream.Printf("</table><br />");

    hStream.Printf("<form>");
    hStream.Printf("<button type=\"submit\" name=\"showImage\" value=\"1\">Show/Hide Camera</button>\n");
    hStream.Printf("<button type=\"submit\" name=\"useModel\" value=\"1\">Show/Hide Model</button><br>\n");
    hStream.Printf("<button type=\"submit\" name=\"manualDriveButton\" value=\"1\">Manual Drive</button><br>\n");

    if (manualDrive) {
        hStream.Printf("<button type=\"submit\" name=\"Up\" value=\"1\">UP</button>\n");
        hStream.Printf("<button type=\"submit\" name=\"Left\" value=\"1\">LEFT</button>\n");
        hStream.Printf("<button type=\"submit\" name=\"Right\" value=\"1\">RIGHT</button>\n");
        hStream.Printf("<button type=\"submit\" name=\"Down\" value=\"1\">DOWN</button><br>\n");

        hStream.Printf("<button type=\"submit\" name=\"stopSpeed\" value=\"1\">STOP SPEED</button>\n");
        hStream.Printf("<button type=\"submit\" name=\"stopDrive\" value=\"1\">STOP DRIVE</button>\n");
    }
    if (showCamera) {
        hStream.Printf("<br><input type=\"number\" id=\"camModeId\" name=\"camMode\"><br>\n");
    }
    hStream.Printf("<br><input type=\"text\" id=\"driveRefInBoxId\" name=\"driveRefInBox\">\n");
    hStream.Printf("<br><input type=\"text\" id=\"speedRefInBoxId\" name=\"speedRefInBox\"><br>\n");
    hStream.Printf("</form>\n\n");

    hStream.Printf("</body></html>");
    hStream.WriteReplyHeader(True);

#if 0
    if (hStream.Switch("InputCommands.TankHeight")) {
        hStream.SSPrintf("OutputHttpOtions.Content-Type", "text/html");
        hStream.Switch((uint32) 0);
        hStream.Printf("tankHeight=%f\n", lastHeight);
    }
    else {
        hStream.SSPrintf("OutputHttpOtions.Content-Type", "text/html");
        hStream.Printf("<html>\n"
                "<head>\n"
                "<meta charset=\"UTF-8\">\n"
                "</head>\n"
                "<body onload=\"timedUpdate();\">\n"
                "<script language=\"javascript\" type=\"text/javascript\" src=\"/P5_DIR/libraries/p5.js\"></script>\n"
                "<script>\n"
                "var variable=0\n"
                "var request = new XMLHttpRequest();\n"
                "function timedUpdate() {\n"
                "var url = \"?TankHeight\";\n"
                "request.open('GET', url, true);\n"
                "request.onreadystatechange = imgManager;\n"
                "request.send(null);\n"
                "window.setTimeout(timedUpdate, %d);\n"
                "}\n"
                "function imgManager() {\n"
                "if(this.readyState != 4 || this.status != 200) {\n"
                "return;\n"
                "}\n"
                "eval(request.responseText);\n"
                "variable=tankHeight;\n"
                "}\n"
                "</script>\n"
                "<script language=\"javascript\" type=\"text/javascript\" src=\"/P5_DIR/sketch.js\"></script>\n"
                "<style> body {padding: 0; margin: 0;} </style>\n"
                "</body>\n"
                "</html>\n",
                10);
    }
    hStream.WriteReplyHeader(True);
#endif

    return True;
}

bool RemoteDriverServer::ObjectSaveSetup(ConfigurationDataBase &info,
                                         StreamInterface *err) {
    return true;
}

bool RemoteDriverServer::SetInputBoardInUse(bool on) {
    return true;
}

bool RemoteDriverServer::SetOutputBoardInUse(bool on) {
    return true;
}

bool RemoteDriverServer::ObjectDescription(StreamInterface &s,
                                           bool full,
                                           StreamInterface *err) {
    s.Printf("%s %s\n", ClassName(), Version());
// Module name
    s.Printf("Module Name --> %s\n", Name());

    return true;
}

bool RemoteDriverServer::PulseStart() {
    return True;
}

bool RemoteDriverServer::Poll() {

//read diagnostic data
//printf("\nreading d data...\n");
    read(clientFd, inputBuffer, inputByteSize);
    memcpy(&x, inputBuffer, sizeof(float));
    memcpy(&y, inputBuffer + 1, sizeof(float));
    memcpy(&theta, inputBuffer + 2, sizeof(float));
    memcpy(&speed, inputBuffer + 3, sizeof(float));
    memcpy(&omega, inputBuffer + 4, sizeof(float));
//printf("\nvar diagnosticData = [%f, %f, %f, %f, %f];\n", x, y, theta, speed, omega);

//write the cam visualization mode
    write(clientFd, &camMode, sizeof(int32));

    camData.Seek(0);
    camData = "var imgData = \"data:image/jpg;base64,";
    int32 readSize = packetSize;
    uint32 numberOfPackets = 0;
    uint32 totalSize = 0;
    while (1) {
        memset(readBuffer, 0, packetSize + 1);
        readSize = read(clientFd, readBuffer, packetSize);
        //printf("\nread %d p=%d\n", readSize, numberOfPackets);

        if (readSize >= 0) {
            if (readBuffer[readSize - 1] == 0x1a) {
                readBuffer[readSize - 1] = 0;
                camData += readBuffer;
                break;
            }
            readBuffer[readSize] = 0;
            camData += readBuffer;
        }
        numberOfPackets++;
        totalSize += readSize;
    }

//printf("\nread %d\n", readSize);
//printf("\nnumberOfPackets=%d, totalSize=%d\n", numberOfPackets, totalSize);
    camData += "\";\n";
    camData.Printf("var diagnosticData = [%f, %f, %f, %f, %f];\n", x, y, theta, speed, omega);
//inputBuffer[0] = 0;
    int32 speedControl = PWM_SPEED_REMAP(zeroSpeedControl + speedReference);
    int32 driveControl = PWM_DRIVE_REMAP(zeroDriveControl + driveReference);

    speedControl -= speedPwmMin;
    driveControl -= drivePwmMin;

    uint16 controls = (speedControl << 8);
    controls |= (driveControl);

    write(clientFd, &controls, sizeof(controls));
    lastUpdateTime = HRT::HRTCounter();

    eventSem.Post();
    eventSem.Reset();
    eventSem.fastWait(100);

    return true;
}

int32 RemoteDriverServer::GetData(uint32 usecTime,
                                  int32 *buffer,
                                  int32 bufferNumber) {
    memcpy(buffer, inputBuffer, inputByteSize);
    return 1;
}

bool RemoteDriverServer::WriteData(uint32 usecTime,
                                   const int32 *buffer) {
    return false;
}

/*
 int64  RemoteDriverServer::GetUsecTime() {
 return 0;
 }*/

OBJECTLOADREGISTER(RemoteDriverServer, "$Id:RemoteDriverServer.cpp,v 1.1.1.1 2010-01-20 12:26:47 pc Exp $")

//hStream.Printf("<meta http-equiv=\"refresh\" content=\"0.2\" >");
#if 0
hStream.Printf("<script>\n"
        "window.addEventListener(\"load\", function() {\n"
        "var camera = document.getElementById(\"camera\");\n"
        "var play = document.getElementById(\"play\");\n"
        "var pause = document.getElementById(\"pause\");\n"
        "var stop = document.getElementById(\"stop\");\n"
        "var constraints = {audio:true, video:true};\n"
        "function success(stream) {\n"
        "  camera.mozSrcObject = stream;\n"
        "  camera.play();\n"
        "  disableButtons(true, false, false);\n"
        "}\n"
        "function failure(error) {\n"
        "  alert(JSON.stringify(error));\n"
        "}\n"
        "function disableButtons(disPlay, disPause, disStop) {\n"
        "  play.disabled = disPlay;\n"
        "  pause.disabled = disPause;\n"
        "  stop.disabled = disStop;\n"
        "}\n"
        "disableButtons(true, true, true);\n"
        "if (navigator.mozGetUserMedia)\n"
        "  navigator.mozGetUserMedia(constraints, success, failure);\n"
        "else\n"
        "  alert(\"Your browser does not support getUserMedia()\");\n"
        "play.addEventListener(\"click\", function() {\n"
        "  disableButtons(true, false, false);\n"
        "  camera.play();\n"
        "}, false);\n"
        "pause.addEventListener(\"click\", function() {\n"
        "  disableButtons(false, true, false);\n"
        "  camera.pause();\n"
        "}, false);\n"
        "stop.addEventListener(\"click\", function() {\n"
        "  disableButtons(true, true, true);\n"
        "  camera.pause();\n"
        "  camera.src = "";\n"
        "}, false);\n"
        "}, false);\n"
        "</script>\n");

hStream.Printf("<body>\n");
hStream.Printf("<button id=\"play\">Play</button>\n"
        "<button id=\"pause\">Pause</button>\n"
        "<button id=\"stop\">Stop</button>\n"
        "<br />\n"
        "<video id=\"camera\"></video>\n");

#endif

#if 0
hStream.Printf("<img src=\"\" id=\"myimage\" />\n");

hStream.Printf("function timedUpdate() {\n");
hStream.Printf("window.setTimeout(timedUpdate, %d);\n", httpRefreshTime);
hStream.Printf("window.location.reload(true);");
hStream.Printf("}\n");

vector < uchar > buf;
imencode(".jpg", *frameMat, buf);

uchar *enc_msg = new uchar[buf.size()];
for (int i = 0; i < buf.size(); i++) {
    enc_msg[i] = buf[i];
}
string encoded = base64_encode(enc_msg, buf.size());
hStream.Printf("var testjpg=\"data:image/jpg;base64,%s\";\n", encoded.c_str());
hStream.Printf("document.getElementById(\"myimage\").src=testjpg;\n");

hStream.Printf("</script>\n");
#endif

#if 0

hStream.Printf("<canvas id=\"canvas\" width=\"1600\" height=\"1600\"/>\n");

hStream.Printf("<script type=\"text/JavaScript\">\n");

hStream.Printf("var img = new Image();\n");

hStream.Printf("img.onload = function() {\n"
        "var canvas = document.getElementById(\"canvas\");\n"
        "var context = canvas.getContext(\"2d\");\n"
        "context.drawImage(img, 0, 0);\n"
        "setTimeout(timedUpdate,%d);\n"
        "}\n",httpRefreshTime);

hStream.Printf("function timedUpdate() {\n");
//hStream.Printf("window.setTimeout(timedUpdate, %d);\n", httpRefreshTime);
//hStream.Printf("window.location.reload(true);");

vector < uchar > buf;
imencode(".jpg", *frameMat, buf);

uchar *enc_msg = new uchar[buf.size()];
for (int i = 0; i < buf.size(); i++) {
    enc_msg[i] = buf[i];
}
string encoded = base64_encode(enc_msg, buf.size());
hStream.Printf("var testjpg=\"data:image/jpg;base64;\"\n");
hStream.Printf("testjpg=testjpg+Date.now()\n");
hStream.Printf("testjpg=testjpg+\",%s\";\n", encoded.c_str());
hStream.Printf("img.src=testjpg\n");
hStream.Printf("}\n");
#endif
