/**
 * @file CoolCarDiagnostic.cpp
 * @brief Source file for class CoolCarDiagnostic
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
 * the class CoolCarDiagnostic (public, protected, and private). Be aware that some
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "CoolCarDiagnostic.h"
#include "base64.h"
#include <math.h>

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

static float RemapInput(float input,
                        float minInput,
                        float minOutput,
                        float factor,
                        bool constrained = false) {
    return (input < minInput && constrained) ? (0.) : (((input - minInput) * factor) + minOutput);
}

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

CoolCarDiagnostic::CoolCarDiagnostic() {
    // Auto-generated constructor stub for CoolCarDiagnostic
    // TODO Verify if manual additions are needed

    //graphics

    capture = NULL;

    //config params
    positionIndex = 0;
    directionIndex = 0;

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
    httpRefreshTime=1000;

    frameMat = NULL;

    //outputBuffer
    outputBuffer = NULL;

    inputBuffer = NULL;

    x = 0.;
    y = 0.;
    theta = 0.;

    position = NULL;
    direction = NULL;
    lastUpdateTime = 0u;

    pos_1 = 0.;

    omega = 0.;
    speed = 0.;
}

CoolCarDiagnostic::~CoolCarDiagnostic() {
    // Auto-generated destructor stub for CoolCarDiagnostic
    // TODO Verify if manual additions are needed

}

bool CoolCarDiagnostic::ObjectLoadSetup(ConfigurationDataBase &cdbData,
                                        StreamInterface * err) {

    bool ret = GCReferenceContainer::ObjectLoadSetup(cdbData, err);

    CDBExtended cdb(cdbData);
    if (ret) {

        if (!cdb.ReadInt32(positionIndex, "PositionIndex", 1)) {
            AssertErrorCondition(Warning, "CoolCarDiagnostic::ObjectLoadSetup: %s positionIndex not specified. Using default: %d", Name(), positionIndex);
        }

        if (!cdb.ReadInt32(directionIndex, "DirectionIndex", 2)) {
            AssertErrorCondition(Warning, "CoolCarDiagnostic::ObjectLoadSetup: %s directionIndex not specified. Using default: %d", Name(), directionIndex);
        }

        //the factor to transform encoders in cm or m
        if (!cdb.ReadFloat(positionFactor, "PositionFactor", 1.)) {
            AssertErrorCondition(Warning, "CoolCarDiagnostic::ObjectLoadSetup: %s positionFactor not specified. Using default: %f", Name(), positionFactor);
        }

        //the factor to transform direction input to grades or radiants
        if (!cdb.ReadFloat(directionFactor, "DirectionFactor", 1.)) {
            AssertErrorCondition(Warning, "CoolCarDiagnostic::ObjectLoadSetup: %s directionFactor not specified. Using default: %f", Name(), directionFactor);
        }

        if (!cdb.ReadFloat(positionMinIn, "PositionMinIn", 0.)) {
            AssertErrorCondition(Warning, "CoolCarDiagnostic::ObjectLoadSetup: %s positionMinIn not specified. Using default: %f", Name(), positionMinIn);
        }

        if (!cdb.ReadFloat(positionMinOut, "PositionMinOut", 0.)) {
            AssertErrorCondition(Warning, "CoolCarDiagnostic::ObjectLoadSetup: %s positionMinOut not specified. Using default: %f", Name(), positionMinOut);
        }

        if (!cdb.ReadFloat(directionMinIn, "DirectionMinIn", 0.)) {
            AssertErrorCondition(Warning, "CoolCarDiagnostic::ObjectLoadSetup: %s directionMinIn not specified. Using default: %f", Name(), directionMinIn);
        }

        if (!cdb.ReadFloat(directionMinOut, "DirectionMinOut", 0.)) {
            AssertErrorCondition(Warning, "CoolCarDiagnostic::ObjectLoadSetup: %s directionMinOut not specified. Using default: %f", Name(), directionMinOut);
        }

        if (!cdb.ReadFloat(chassisLength, "ChassisLength", 10.)) {
            AssertErrorCondition(Warning, "CoolCarDiagnostic::ObjectLoadSetup: %s chassisLength not specified. Using default: %f", Name(), chassisLength);
        }

        if (!cdb.ReadFloat(initialXposition, "InitialXposition", 0.)) {
            AssertErrorCondition(Warning, "CoolCarDiagnostic::ObjectLoadSetup: %s initialXposition not specified. Using default: %f", Name(), initialXposition);
        }

        if (!cdb.ReadFloat(initialYposition, "InitialYposition", 0.)) {
            AssertErrorCondition(Warning, "CoolCarDiagnostic::ObjectLoadSetup: %s initialYposition not specified. Using default: %f", Name(), initialYposition);
        }

        if (!cdb.ReadFloat(initialOrientation, "InitialOrientation", 0.)) {
            AssertErrorCondition(Warning, "CoolCarDiagnostic::ObjectLoadSetup: %s initialOrientation not specified. Using default: %f", Name(),
                                 initialOrientation);
        }

        if (!cdb.ReadInt32(httpRefreshTime, "HttpRefreshTime", 1000)) {
            AssertErrorCondition(Warning, "CoolCarDiagnostic::ObjectLoadSetup: %s HttpRefreshTime not specified. Using default: %d", Name(),
                                 httpRefreshTime);
        }

    }
    return ret;
}

bool CoolCarDiagnostic::Init(Mat *frameMatIn,
                             VideoCapture *captureIn,
                             int showModeIn,
                             char* bufferIn,
                             char *bufferOut) {

    capture = captureIn;
    frameMat = frameMatIn;

    inputBuffer = bufferIn;
    outputBuffer = bufferOut;

    if ((capture == NULL) || (frameMat == NULL) || (inputBuffer == NULL) || (outputBuffer == NULL)) {

        return false;
    }

    //accelerator to input pointers
    position = (int32*) (inputBuffer + positionIndex);
    direction = (int32*) (inputBuffer + directionIndex);

    x = initialXposition;
    y = initialYposition;
    theta = initialOrientation;

    return true;
}

bool CoolCarDiagnostic::Execute() {

    //remap inputs to the useful measurements
    float pos = RemapInput((float) (*position), positionMinIn, positionMinOut, positionFactor, false);
    float alpha = RemapInput((float) (*direction), directionMinIn, directionMinOut, directionFactor, true);

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

    return true;
}

bool CoolCarDiagnostic::ProcessHttpMessage(HttpStream &hStream) {

    vector < uchar > buf;
    imencode(".jpg", *frameMat, buf);

    string encoded = base64_encode(&buf[0], buf.size());

    FString ajaxString;
    ajaxString.SetSize(0);

    ajaxString.Printf("var imgData = \"data:image/jpg;base64,%s\";\n", encoded.c_str());
    ajaxString.Printf("var diagnosticData = [%f, %f, %f, %f, %f];\n", x, y, theta, speed, omega);

    FString refreshData;
    refreshData.SetSize(0);
    hStream.Seek(0);
    if (hStream.Switch("InputCommands.ImgRefreshData")) {
        hStream.Seek(0);
        hStream.GetToken(refreshData, "");
        hStream.Switch((uint32) 0);
        hStream.Printf("%s\n", ajaxString.Buffer());
        hStream.WriteReplyHeader(True);
        return True;
    }

    hStream.SSPrintf("OutputHttpOtions.Content-Type", "text/html");

    hStream.keepAlive = False;
    hStream.WriteReplyHeader(False);


    hStream.Printf("<html><head><title>CoolCar-Diagnostics</title></head>\n");
    hStream.Printf("<script src=\"http://ajax.googleapis.com/ajax/libs/jquery/1.10.2/jquery.min.js\"></script>\n");


    hStream.Printf("<body onload=\"timedUpdate();\">\n");

    hStream.Printf("<img src=\"\" id=\"myimage\" />\n");
    hStream.Printf("<script>\n");

    hStream.Printf("var request = new XMLHttpRequest();\n");

    hStream.Printf("function timedUpdate() {\n");
      hStream.Printf("var url = \"?ImgRefreshData\";\n");
      hStream.Printf("request.open('GET', url, true);\n");
      hStream.Printf("request.onreadystatechange = imgManager;\n");
      hStream.Printf("request.send(null);\n");
      hStream.Printf("window.setTimeout(timedUpdate, %d);\n",
                     httpRefreshTime);
      hStream.Printf("}\n");

      hStream.Printf("function imgManager() {\n");
      hStream.Printf("if(this.readyState != 4 || this.status != 200) {\n");
      hStream.Printf("return;\n");
      hStream.Printf("}\n");
      hStream.Printf("eval(request.responseText);\n");
      //hStream.Printf("img.src=imgData;\n");
      hStream.Printf("document.getElementById(\"myimage\").src=imgData;\n");

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
    hStream.Printf("<p>Data was updated %f seconds ago<p/>", ((HRT::HRTCounter() - lastUpdateTime) * HRT::HRTPeriod()));

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

    hStream.Printf("</body></html>");
    hStream.WriteReplyHeader(True);

    return True;
}

OBJECTLOADREGISTER(CoolCarDiagnostic, "$Id:CoolCarDiagnostic.cpp,v 1.1.1.1 2010-01-20 12:26:47 pc Exp $")





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
