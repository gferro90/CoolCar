/**
 * @file LineFollower.cpp
 * @brief Source file for class LineFollower
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
 * the class LineFollower (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "LineFollower.h"

#define PWM_SPEED_REMAP(control) PwmRemapping(control, speedControlMin, speedControlMax, speedPwmMin, speedPwmMax)
#define PWM_DRIVE_REMAP(control) PwmRemapping(control, driveControlMin, driveControlMax, drivePwmMin, drivePwmMax)

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

static int PwmRemapping(float control,
                        float minIn,
                        float maxIn,
                        int minOut,
                        int maxOut) {
    if (control > maxIn) {
        control = maxIn;
    }
    if (control < minIn) {
        control = minIn;
    }

    float rangeIn = maxIn - minIn;
    int rangeOut = maxOut - minOut;
    return (int) (((control - minIn) * rangeOut) / rangeIn) + minOut;
}

int LineFollower::DetectSignal(Mat &hsv_frame,
                               Mat &thresholded) {
    float speedControl = standardSpeedControl;

    for (uint32 n = 0; n < numberOfSignals; n++) {

        // Load threshold from the slider bars in these 2 parameters
        Scalar hsv_min = signals[n]->range1;
        Scalar hsv_max = signals[n]->range2;

        // Filter out colors which are out of range.
        inRange(hsv_frame, hsv_min, hsv_max, thresholded);

        int middleY = (int) (thresholded.rows / 2.);
        int consecutiveWhiteCnt = 0;
        int maxConsecutiveWhite = 0;
        //sarch white points in the middle line
        for (uint32 i = 0; i < thresholded.cols; i++) {

            if (thresholded.at < uint8_t > (middleY, i) == 255) {
                consecutiveWhiteCnt++;
            }
            else {
                if (consecutiveWhiteCnt > maxConsecutiveWhite) {
                    maxConsecutiveWhite = consecutiveWhiteCnt;
                }
                consecutiveWhiteCnt = 0;
            }
        }
        if (maxConsecutiveWhite > signalMinWidth) {
            speedControl = signals[n]->SignalFunction((int&) status);
            //TODO signals[i]->Name()
            //printf("\ndetected signal for %s\n", signals[i]->signalFunction.functionName);
            break;
        }
    }
    return PWM_SPEED_REMAP(speedControl);

}

void LineFollower::StateMachine(int minX,
                                int maxX,
                                int &minX_used,
                                int &maxX_used,
                                float curvature,
                                int linesCounter,
                                int imgMin,
                                int imgMax,
                                int initialMinX,
                                int initialMaxX,
                                int &speedControl) {

    // printf("\nstatus is %d", lineStatus);
    switch (lineStatus) {

    case (TWO_LINES): {
        if (linesCounter >= 2) {
            maxX_used = maxX;
            minX_used = minX;
            numberCyclesInStatus++;
        }
        else if (linesCounter == 1) {
            bool turnRight = true;
            // printf("\ncurvature = %f!!!\n", curvature);

            if (ABS_VAL(curvature) > curvatureThres) {
                turnRight = (curvature < 0.);
            }
            else {
                //work on derivate
                int gapLeft = ABS_VAL(minX - minX_1);
                int gapRight = ABS_VAL(maxX - maxX_1);
                turnRight = (gapLeft < gapRight);
            }
            if (turnRight) {
                //printf("\nOnly Left Line!! %d %d %d %d %d %d\n", minX, maxX, minX_1, maxX_1, gapRight, gapLeft);

                minX_used = minX;
                //assume the line as left line
                maxX_used = imgMax;
                lineStatus = LEFT_LINE;
            }
            else {
                //printf("\nOnly Right Line!! %d %d %d %d %d %d\n", minX, maxX, minX_1, maxX_1, gapRight, gapLeft);

                //assume the line as right line
                minX_used = imgMin;
                maxX_used = maxX;
                lineStatus = RIGHT_LINE;
            }
            numberCyclesInStatus = 0;
        }
        else if (linesCounter == 0) {
            numberCyclesInStatus++;
            if (numberCyclesInStatus >= zeroLineCycles) {
                lineStatus = STOP;
                numberCyclesInStatus = 0;
            }
        }
    }
        break;

    case (LEFT_LINE): {
        if (linesCounter >= 2) {
            maxX_used = maxX;
            minX_used = minX;
            lineStatus = TWO_LINES;
            numberCyclesInStatus = 0;
        }
        else if (linesCounter == 1) {
            minX_used = minX;
            //assume the line as left line
            maxX_used = imgMax;
            numberCyclesInStatus++;
        }
        else if (linesCounter == 0) {
            minX_used = imgMax;
            maxX_used = imgMax;
            lineStatus = ZERO_LEFT;
            numberCyclesInStatus = 0;
        }
    }
        break;

    case (RIGHT_LINE): {
        if (linesCounter >= 2) {
            maxX_used = maxX;
            minX_used = minX;
            lineStatus = TWO_LINES;
            numberCyclesInStatus = 0;
        }
        else if (linesCounter == 1) {
            //assume the line as right line
            minX_used = imgMin;
            maxX_used = maxX;
            numberCyclesInStatus++;
        }
        else if (linesCounter == 0) {
            minX_used = imgMin;
            maxX_used = imgMin;
            lineStatus = ZERO_RIGHT;
            numberCyclesInStatus = 0;
        }
    }
        break;

    case (ZERO_LEFT): {
        if (linesCounter >= 2) {
            maxX_used = maxX;
            minX_used = minX;
            lineStatus = TWO_LINES;
            numberCyclesInStatus = 0;
        }
        else if (linesCounter == 1) {
            minX_used = minX;
            //assume the line as left line
            maxX_used = imgMax;
            lineStatus = LEFT_LINE;
            numberCyclesInStatus = 0;
        }
        else if (linesCounter == 0) {
            minX_used = imgMax;
            maxX_used = imgMax;
            numberCyclesInStatus++;
            if (numberCyclesInStatus >= zeroLineCycles) {
                lineStatus = STOP;
                numberCyclesInStatus = 0;
            }
        }
    }
        break;

    case (ZERO_RIGHT): {
        if (linesCounter >= 2) {
            maxX_used = maxX;
            minX_used = minX;
            lineStatus = TWO_LINES;
            numberCyclesInStatus = 0;
        }
        else if (linesCounter == 1) {
            //assume the line as right line
            minX_used = imgMin;
            maxX_used = maxX;
            lineStatus = RIGHT_LINE;
            numberCyclesInStatus = 0;
        }
        else if (linesCounter == 0) {
            minX_used = imgMin;
            maxX_used = imgMin;
            numberCyclesInStatus++;
            if (numberCyclesInStatus >= zeroLineCycles) {
                lineStatus = STOP;
                numberCyclesInStatus = 0;
            }
        }
    }
        break;
    case (STOP): {
        if (linesCounter >= 2) {
            maxX_used = maxX;
            minX_used = minX;
            lineStatus = TWO_LINES;
            numberCyclesInStatus = 0;
        }
        else {
            minX_used = initialMinX;
            maxX_used = initialMaxX;
            numberCyclesInStatus++;
            speedControl = PWM_SPEED_REMAP(zeroSpeedControl);
        }
    }

    }

}

bool LineFollower::Calibrate(Mat &lineBandGreyThres) {

    int initialMinX = lineBandGreyThres.cols;
    int initialMaxX = 0;

    int initialMinY = lineBandGreyThres.rows;
    int initialMaxY = 0;

    lineHeight = lineBandGreyThres.rows;
    //get the contours

    vector < vector<Point> > contours;
    vector < Vec4i > hierarchy;

    if (showMode == 3) {
        RNG rng(12345);
        Mat temp = lineBandGreyThres.clone();
        findContours(temp, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    }
    else {
        findContours(lineBandGreyThres, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    }

    int maxX = initialMaxX;
    int minX = initialMinX;
    int maxX_used = initialMaxX;
    int minX_used = initialMinX;

    //search the minimum and maximum x points on all contours

    int linesCounter = 0;

    //printf("\nnContours=%d\n",contours.size() );
    for (int i = 0; i < contours.size() && (linesCounter < 2); i++) {
        int minX_temp = initialMinX;
        int maxX_temp = initialMaxX;
        int minY_temp = initialMinY;
        int maxY_temp = initialMaxY;
        int yinMaxX = initialMaxY;
        int yinMinX = initialMinY;

        for (int j = 0; j < contours[i].size(); j++) {
            //take max and min on the middle line

            if ((contours[i])[j].x < minX_temp) {
                minX_temp = (contours[i])[j].x;
                yinMinX = (contours[i])[j].y;
            }
            if ((contours[i])[j].x > maxX_temp) {
                maxX_temp = (contours[i])[j].x;
                yinMaxX = (contours[i])[j].y;
            }
            if ((contours[i])[j].y < minY_temp) {
                minY_temp = (contours[i])[j].y;
            }
            if ((contours[i])[j].y > maxY_temp) {
                maxY_temp = (contours[i])[j].y;
            }
        }

        //printf("\nrangeH=%d %d %d \n",  lineHeight + lineHeightTolMin, lineHeight + lineHeightTolMax, maxY_temp - minY_temp);

        //printf("\nrangeW=%d %d\n",  lineWidth + lineWidthTolMin, lineWidth + lineWidthTolMax);

        //lineWidth and lineHeight of the contours are compliants with the line
        if (InRange(maxY_temp - minY_temp, lineHeight + lineHeightTolMin, lineHeight + lineHeightTolMax)) {

            lineWidth = maxX_temp - minX_temp;
            if (InRange(maxX_temp - minX_temp, lineWidth + lineWidthTolMin, lineWidth + lineWidthTolMax)) {

                //printf("\nLINE DETECTED w=%d h=%d rows=%d!!!\n", maxX_temp - minX_temp, maxY_temp - minY_temp, lineBandGreyThres.rows);
                int average = (maxX_temp + minX_temp) / 2;

                //

                if (average < minX) {
                    minX = average;
                }
                if (average > maxX) {
                    maxX = average;
                }
                linesCounter++;
            }
        }
    }

    refLeft = minX;
    refRight = maxX;

    minX_1 = minX;
    maxX_1 = maxX;
    printf("\nlinesCounter=%d\n",linesCounter);
    if ((linesCounter >= 2) && (lineWidth > 0)) {
        return true;
    }
    return false;
}

//to be called after Initialise
int LineFollower::FollowLine(Mat &lineBandGreyThres,
                             int &speedControl) {

    int initialMinX = lineBandGreyThres.cols;
    int initialMaxX = 0;

    int initialMinY = lineBandGreyThres.rows;
    int initialMaxY = 0;

    int lineHeight = lineBandGreyThres.rows;

//get the contours

    vector < vector<Point> > contours;
    vector < Vec4i > hierarchy;
    RNG rng(12345);
    if (showMode == 3) {

        Mat temp = lineBandGreyThres.clone();
        findContours(temp, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    }
    else {
        findContours(lineBandGreyThres, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    }

    int maxX = initialMaxX;
    int minX = initialMinX;
    int maxX_used = initialMaxX;
    int minX_used = initialMinX;

    float curvature = 0.;
    //search the minimum and maximum x points on all contours

    int linesCounter = 0;

    for (int i = 0; i < contours.size() && (linesCounter < 2); i++) {
        int minX_temp = initialMinX;
        int maxX_temp = initialMaxX;
        int minY_temp = initialMinY;
        int maxY_temp = initialMaxY;
        int yinMaxX = initialMaxY;
        int yinMinX = initialMinY;

        for (int j = 0; j < contours[i].size(); j++) {
            //take max and min on the middle line

            if ((contours[i])[j].x < minX_temp) {
                minX_temp = (contours[i])[j].x;
                yinMinX = (contours[i])[j].y;
            }
            if ((contours[i])[j].x > maxX_temp) {
                maxX_temp = (contours[i])[j].x;
                yinMaxX = (contours[i])[j].y;
            }
            if ((contours[i])[j].y < minY_temp) {
                minY_temp = (contours[i])[j].y;
            }
            if ((contours[i])[j].y > maxY_temp) {
                maxY_temp = (contours[i])[j].y;
            }
        }

        //lineWidth and lineHeight of the contours are compliants with the line
        if (InRange(maxY_temp - minY_temp, lineHeight + lineHeightTolMin, lineHeight + lineHeightTolMax)) {

            if (InRange(maxX_temp - minX_temp, lineWidth + lineWidthTolMin, lineWidth + lineWidthTolMax)) {

                //printf("\nLINE DETECTED w=%d h=%d rows=%d!!!\n", maxX_temp - minX_temp, maxY_temp - minY_temp, lineBandGreyThres.rows);
                int average = (maxX_temp + minX_temp) / 2;

                //
                curvature +=
                        ((maxX_temp - minX_temp) > lineWidth) ? (SIGN_VAL(yinMaxX - yinMinX) * ((maxX_temp - minX_temp) / ((float) lineWidth) - 1.)) : (0.);

                if (average < minX) {
                    minX = average;
                }
                if (average > maxX) {
                    maxX = average;
                }
                linesCounter++;
            }
        }
    }
    curvature /= linesCounter;

    if (showMode == 3) {
        //draw contours on the band
        for (int i = 0; i < contours.size(); i++) {
            Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
            drawContours(lineBandGreyThres, contours, i, color, 2, 8, hierarchy, 0, Point());
        }
    }

    StateMachine(minX, maxX, minX_used, maxX_used, curvature, linesCounter, 0, lineBandGreyThres.cols, initialMinX, initialMaxX, speedControl);

    // something has been detected
    float control = zeroDriveControl;

    if (minX_used <= maxX_used) {
        switch (status) {
        case (FOLLOW_LEFT): {
            //assume left line detected
            //if status is follow left
            float error = refLeft - minX_used;

            //get the drive control on this error
            control = controller->Execute(error, 0.);
        }
            break;
        case (FOLLOW_RIGHT): {
            //assume right line detected
            //if status is follow right
            float error = refRight - maxX_used;
            //printf("\nerror=%f\n", error);
            //get the drive control on this error
            control = controller->Execute(error, 0.);
        }
            break;
        default: {
            control = zeroDriveControl;                        //??
        }
        }
    }

    // if (linesCounter >= 2) {
    minX_1 = minX;
    maxX_1 = maxX;
    //}

    // printf("\nline status=%d", lineStatus);
    //printf("Min=%d Max=%d\n", minX, maxX);
    return PWM_DRIVE_REMAP(control);
}

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

LineFollower::LineFollower() {
    // Auto-generated constructor stub for LineFollower
    // TODO Verify if manual additions are needed

    thresType = 0;
    lineThres = 0;
    lineBandXFactor = 0;
    lineBandYFactor = 0;
    lineBandHeightFactor = 0;

    lineBandX = 0;
    lineBandY = 0;
    lineBandHeight = 0;
    lineBandWidth = 0;
    //configuration parameters
    lineHeightTolMax = 0;
    lineHeightTolMin = 0;

    lineWidthTolMax = 0;
    lineWidthTolMin = 0;

    driveControlMin = 0.;
    driveControlMax = 0.;
    drivePwmMin = 0;
    drivePwmMax = 0;

    speedControlMin = 0.;
    speedControlMax = 0.;
    speedPwmMin = 0;
    speedPwmMax = 0;

    zeroSpeedControl = 0.;
    zeroDriveControl = 0.;

    standardSpeedControl = 0.;

    zeroLineCycles = 0;
    signalMinWidth = 0;

    curvatureThres = 0.;
    bufferStart = 0;

    //status parameters
    minX_1 = 0;
    maxX_1 = 0;

    lineWidth = 0;
    lineHeight = 0;

    refLeft = 0;
    refRight = 0;

    status = FOLLOW_RIGHT;
    lineStatus = TWO_LINES;
    numberCyclesInStatus = 0;

    showMode = 0;

    numberOfSignals = 0;

    //graphics

    capture = NULL;

    frameMat = NULL;

    lineBand = NULL;

    hsv_frame = NULL;
    thresholded = NULL;

    lineBandGrey = NULL;
    lineBandGreyThres = NULL;

    //buffer
    buffer = NULL;

    signals = NULL;
}

LineFollower::~LineFollower() {
    // Auto-generated destructor stub for LineFollower
    // TODO Verify if manual additions are needed

    if (lineBand != NULL) {
        delete lineBand;
        lineBand = NULL;
    }

    if (hsv_frame != NULL) {
        delete hsv_frame;
        hsv_frame = NULL;
    }

    if (thresholded != NULL) {
        delete thresholded;
        thresholded = NULL;
    }

    if (lineBandGrey != NULL) {
        delete lineBandGrey;
        lineBandGrey = NULL;
    }

    if (lineBandGreyThres != NULL) {
        delete lineBandGreyThres;
        lineBandGreyThres = NULL;
    }

    if (signals != NULL) {
        delete[] signals;
    }
}

bool LineFollower::ObjectLoadSetup(ConfigurationDataBase &cdbData,
                                   StreamInterface * err) {

    bool ret = CamModule::ObjectLoadSetup(cdbData, err);

    CDBExtended cdb(cdbData);
    if (ret) {
        //default black line
        if (!cdb.ReadInt32(thresType, "ThresType", 1)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s thresType not specified. Using default: %d", Name(), thresType);
        }
        if (!cdb.ReadInt32(lineThres, "LineThres", 15)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s lineThres not specified. Using default: %d", Name(), lineThres);
        }

        //these factor will be remapped
        if (!cdb.ReadFloat(lineBandXFactor, "LineBandXFactor", 0.)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s lineBandXFactor not specified. Using default: %f", Name(), lineBandXFactor);
        }
        if (!cdb.ReadFloat(lineBandYFactor, "LineBandYFactor", 0.5)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s lineBandYFactor not specified. Using default: %f", Name(), lineBandYFactor);
        }

        if (!cdb.ReadFloat(lineBandHeightFactor, "LineBandHeightFactor", 0.1)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s lineBandHeight not specified. Using default: %f", Name(), lineBandHeightFactor);
        }

        if (!cdb.ReadInt32(lineHeightTolMax, "LineHeightTolMax", -3)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s lineHeightTolMax not specified. Using default: %d", Name(), lineHeightTolMax);
        }


        if (!cdb.ReadInt32(lineWidthTolMax, "LineWidthTolMax", 1000)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s lineWidthTolMax not specified. Using default: %d", Name(), lineWidthTolMax);
        }

        if (!cdb.ReadInt32(lineHeightTolMin, "LineHeightTolMin", -3)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s lineHeightTolMin not specified. Using default: %d", Name(), lineHeightTolMin);
        }


        if (!cdb.ReadInt32(lineWidthTolMin, "LineWidthTolMin", -8)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s lineWidthTolMin not specified. Using default: %d", Name(), lineWidthTolMin);
        }



        if (!cdb.ReadFloat(driveControlMin, "MinDriveControl", -5000.)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s driveControlMin not specified. Using default: %f", Name(), driveControlMin);
        }
        if (!cdb.ReadFloat(driveControlMax, "MaxDriveControl", 5000.)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s driveControlMax not specified. Using default: %f", Name(), driveControlMax);
        }

        if (!cdb.ReadInt32(drivePwmMin, "MinDrivePwm", 140)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s drivePwmMin not specified. Using default: %d", Name(), drivePwmMin);
        }

        if (!cdb.ReadInt32(drivePwmMax, "MaxDrivePwm", 340)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s drivePwmMax not specified. Using default: %d", Name(), drivePwmMax);
        }

        if (!cdb.ReadFloat(speedControlMin, "MinSpeedControl", -7200.)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s speedControlMin not specified. Using default: %f", Name(), speedControlMin);
        }
        if (!cdb.ReadFloat(driveControlMax, "MaxSpeedControl", 7200.)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s speedControlMax not specified. Using default: %f", Name(), speedControlMax);
        }

        if (!cdb.ReadInt32(speedPwmMin, "MinSpeedPwm", 200)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s speedPwmMin not specified. Using default: %d", Name(), speedPwmMin);
        }

        if (!cdb.ReadInt32(speedPwmMax, "MaxSpeedPwm", 400)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s speedPwmMin not specified. Using default: %d", Name(), speedPwmMin);
        }

        if (!cdb.ReadFloat(zeroSpeedControl, "ZeroSpeedControl", 0.)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s zeroSpeedControl not specified. Using default: %f", Name(), zeroSpeedControl);
        }

        if (!cdb.ReadFloat(zeroDriveControl, "ZeroDriveControl", 0.)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s zeroDriveControl not specified. Using default: %f", Name(), zeroDriveControl);
        }

        int32 standardSpeedPwmDelta = 12;
        if (!cdb.ReadInt32(standardSpeedPwmDelta, "StandardSpeedControlPwmDelta", 12)) {

            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s standardSpeedPwmDelta not specified. Using default: %d", Name(),
                                 standardSpeedPwmDelta);
        }
        standardSpeedControl = zeroSpeedControl + standardSpeedPwmDelta * (speedControlMax - speedControlMin) / (speedPwmMax - speedPwmMin);
        //number of cycles before stop if no lines detected
        if (!cdb.ReadInt32(zeroLineCycles, "ZeroLineCycles", 60)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s zeroLineCycles not specified. Using default: %d", Name(), zeroLineCycles);
        }

        if (!cdb.ReadInt32(signalMinWidth, "SignalMinWidth", 20)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s signalMinWidth not specified. Using default: %d", Name(), signalMinWidth);
        }
        if (!cdb.ReadFloat(curvatureThres, "CurvatureThres", 0.3)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s curvatureThres not specified. Using default: %f", Name(), curvatureThres);
        }

        if (!cdb.ReadInt32(bufferStart, "BufferIndex", 0)) {
            AssertErrorCondition(Warning, "LineFollower::ObjectLoadSetup: %s bufferStart not specified. Using default: %d", Name(), bufferStart);
        }
        //Fill the RoadSign accelerator
        //GCReference Find(i)
        numberOfSignals = 0;
        uint32 mySize = Size();
        for (uint32 i = 0; i < mySize; i++) {
            GCRTemplate<RoadSign> ref = Find(i);
            if (!controller.IsValid()) {

                controller = Find(i);
            }

            if (ref.IsValid()) {
                numberOfSignals++;
            }
        }
        if (!controller.IsValid()) {
            AssertErrorCondition(InitialisationError, "LineFollower::ObjectLoadSetup: %s No controllers specified", Name());
            ret = false;
        }
        if (ret) {
            if (numberOfSignals > 0) {
                signals = new GCRTemplate<RoadSign> [numberOfSignals];
            }
            uint32 n = 0;
            for (uint32 i = 0; i < mySize; i++) {
                GCRTemplate<RoadSign> ref = Find(i);
                if (ref.IsValid()) {
                    signals[n] = ref;
                    signals[n]->Init(speedControlMax, speedControlMin, driveControlMax, driveControlMin, zeroSpeedControl, zeroDriveControl,
                                     standardSpeedControl);
                    n++;
                }
            }
            status = FOLLOW_RIGHT;
            lineStatus = TWO_LINES;
            numberCyclesInStatus = 0;
        }

    }
    return ret;
}

bool LineFollower::Init(Mat *frameMatIn,
                        VideoCapture *captureIn,
                        int showModeIn,
                        char* bufferIn) {

    capture = captureIn;
    frameMat = frameMatIn;

    buffer = bufferIn;
    showMode = showModeIn;

    lineBandX = (frameMat->cols) * lineBandXFactor;
    lineBandY = (frameMat->rows) * lineBandYFactor;
    lineBandHeight = (frameMat->rows) * lineBandHeightFactor;
    lineBandWidth = (frameMat->cols) - 2 * lineBandX;

    //create the image band
    Rect rect(lineBandX, lineBandY, lineBandWidth, lineBandHeight);
    lineBand = new Mat(*frameMat, rect);

    hsv_frame = new Mat(lineBand->size(), CV_8UC3);
    thresholded = new Mat(lineBand->size(), CV_8UC1);

    cvtColor(*lineBand, *hsv_frame, CV_BGR2HSV);

    //create images for gryscale and thresholded grayscale
    lineBandGrey = new Mat(lineBand->size(), CV_8UC1);
    lineBandGreyThres = new Mat(lineBand->size(), CV_8UC1);
    if (showMode > 0) {
        //show all images
        if (showMode == 3) {
            namedWindow("Camera", WINDOW_AUTOSIZE);
            namedWindow("HSV", WINDOW_AUTOSIZE);
            namedWindow("Thresholded", WINDOW_AUTOSIZE);
        }

        namedWindow("lineBandGreyThres", WINDOW_AUTOSIZE);

        // Create trackbars to threshold the line for the line follower
        char* trackbar_value = "Value";
        int const max_value = 255;

        createTrackbar(trackbar_value, "lineBandGreyThres", &lineThres, max_value, NULL);

        // wait adjustements before calibration
        while (1) {
            // Get one frame
            (*capture) >> (*frameMat);
            //convert RGB to grayscale
            cvtColor(*lineBand, *lineBandGrey, CV_BGR2GRAY);

            //threshold the grayscale to detect the line
            threshold(*lineBandGrey, *lineBandGreyThres, lineThres, 255, thresType);

            imshow("lineBandGreyThres", *lineBandGreyThres);
            if ((cvWaitKey(5) & 255) == 27) {
                //destroyWindow("lineBandGreyThres");
                break;
            }
        }
    }

    for (uint32 i=0; i<zeroLineCycles; i++){
        (*capture) >> (*frameMat);
    }

    // Covert color space to HSV as it is much easier to filter colors in the HSV color-space.
    cvtColor(*lineBand, *hsv_frame, CV_BGR2HSV);

    //convert RGB to grayscale
    cvtColor(*lineBand, *lineBandGrey, CV_BGR2GRAY);

    //threshold the grayscale to detect the line
    threshold(*lineBandGrey, *lineBandGreyThres, lineThres, 255, thresType);

    //calibrate

    int speedControl = PWM_SPEED_REMAP(standardSpeedControl);
    int driveControl = PWM_DRIVE_REMAP(zeroDriveControl);
    uint16 controls = 0;

    //calibration

    bool ret = Calibrate(*lineBandGreyThres);
    if (ret) {
        printf("Calibration Results: \nrefLeft=%d \nrefRight=%d \nlineWidth=%d \nlineHeight=%d", refLeft, refRight, lineWidth, lineHeight);

        speedControl = PWM_SPEED_REMAP(zeroSpeedControl);
        driveControl = PWM_DRIVE_REMAP(zeroDriveControl);
        speedControl -= speedControlMin;
        driveControl -= driveControlMin;
        controls = (speedControl << 8);
        controls |= driveControl;


        memcpy(buffer + bufferStart, &controls, sizeof(controls));

        sleep(10);

        printf("\nSTART!!\n");
    }
    else {
        printf("Calibration Error!\n");
    }
    return ret;
}

bool LineFollower::Execute() {

    // Covert color space to HSV as it is much easier to filter colors in the HSV color-space.
    //lineBand is already a band of frameMat
    cvtColor(*lineBand, *hsv_frame, CV_BGR2HSV);

    int speedControl = DetectSignal(*hsv_frame, *thresholded);

    //convert RGB to grayscale
    cvtColor(*lineBand, *lineBandGrey, CV_BGR2GRAY);

    //threshold the grayscale to detect the line
    threshold(*lineBandGrey, *lineBandGreyThres, lineThres, 255, thresType);

    //FollowLine
    int driveControl = FollowLine(*lineBandGreyThres, speedControl);

    speedControl -= speedPwmMin;
    driveControl -= drivePwmMin;

    uint16 controls = (speedControl << 8);
    controls |= (driveControl);

    //printf("controls %d %d\n", speedControl, driveControl);

    //write on buffer
    memcpy(buffer + bufferStart, &controls, sizeof(controls));
    if (showMode > 1) {
        imshow("lineBandGreyThres", *lineBandGreyThres);
        if (showMode == 3) {
            imshow("Camera", *frameMat); // Original stream with detected ball overlay
            imshow("HSV", *hsv_frame); // Original stream in the HSV color space
            imshow("Thresholded", *thresholded); // Original stream in the HSV color space
        }

        if ((cvWaitKey(5) & 255) == 27) {
            speedControl = PWM_SPEED_REMAP(zeroSpeedControl);
            driveControl = PWM_DRIVE_REMAP(zeroDriveControl);

            speedControl -= speedPwmMin;
            driveControl -= drivePwmMin;

            controls = (speedControl << 8);
            controls |= driveControl;

            memcpy(buffer + bufferStart, &controls, sizeof(controls));

        }
    }

    return true;
}

OBJECTLOADREGISTER(LineFollower, "$Id:LineFollower.cpp,v 1.1.1.1 2010-01-20 12:26:47 pc Exp $")
