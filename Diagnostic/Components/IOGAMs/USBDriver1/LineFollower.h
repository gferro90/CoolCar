/**
 * @file LineFollower.h
 * @brief Header file for class LineFollower
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

 * @details This header file contains the declaration of the class LineFollower
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef LINEFOLLOWER_H_
#define LINEFOLLOWER_H_

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
#include "Controller.h"
#include "RoadSign.h"
#include "CamModule.h"

//TODO Opencv stuff

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

OBJECT_DLL (LineFollower)

class LineFollower: public CamModule {

    OBJECT_DLL_STUFF (LineFollower)

public:
    LineFollower();

    virtual ~LineFollower();

    virtual bool ObjectLoadSetup(ConfigurationDataBase &cdbData,
                                 StreamInterface * err);

    virtual bool Init(Mat *, VideoCapture*,
                      int showModeIn, char *bufferIn);

    virtual bool Execute();


    //public attributes


    GenericStatus status;
    LineStatus lineStatus;
private:

    int FollowLine(Mat &lineBandGreyThres, int& speedControl);

    int DetectSignal(Mat &hsv_frame,
                     Mat &thresholded);

    void StateMachine(int minX,
                      int maxX,
                      int &minX_used,
                      int &maxX_used,
                      float curvature,
                      int linesCounter,
                      int imgMin,
                      int imgMax,
                      int initialMinX,
                      int initialMaxX,
                      int &speedControl);

    bool Calibrate(Mat &lineBandGreyThres);

    int32 thresType;
    int32 lineThres;

    float lineBandXFactor;
    float lineBandYFactor;
    float lineBandHeightFactor;

    int lineBandX;
    int lineBandY;
    int lineBandHeight;
    int lineBandWidth;
    //configuration parameters
    int lineHeightTolMax;
    int lineHeightTolMin;

    int lineWidthTolMax;
    int lineWidthTolMin;

    float zeroSpeedControl;
    float zeroDriveControl;

    float standardSpeedControl;

    float driveControlMin;
    float driveControlMax;
    int32 drivePwmMin;
    int32 drivePwmMax;

    float speedControlMin;
    float speedControlMax;
    int32 speedPwmMin;
    int32 speedPwmMax;



    int zeroLineCycles;
    int signalMinWidth;

    float curvatureThres;
    int bufferStart;


    //status parameters
    int minX_1;
    int maxX_1;

    int lineWidth;
    int lineHeight;

    int refLeft;
    int refRight;


    int numberCyclesInStatus;

    int showMode;

    //TODO accelerator to RoadSign
    GCRTemplate<RoadSign> *signals;
    int numberOfSignals;

    GCRTemplate<Controller> controller;

    //graphics

    VideoCapture *capture;

    Mat *frameMat;

    Mat *lineBand;

    Mat *hsv_frame;
    Mat *thresholded;

    Mat *lineBandGrey;
    Mat *lineBandGreyThres;

    //buffer
    char* buffer;

};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* DIAGNOSTIC_COMPONENTS_IOGAMS_CAMERAUSBDRV_LINEFOLLOWER_H_ */

