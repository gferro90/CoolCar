/**
 * @file Utilities.h
 * @brief Header file for class Utilities
 * @date 29/mar/2017
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

 * @details This header file contains the declaration of the class Utilities
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#ifndef NO_OPENCV
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include <opencv2/core/core.hpp>
//#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

#define PWM_SPEED_REMAP(control) PwmRemapping(control, speedControlMin, speedControlMax, speedPwmMin, speedPwmMax)
#define PWM_DRIVE_REMAP(control) PwmRemapping(control, driveControlMin, driveControlMax, drivePwmMin, drivePwmMax)


inline float ABS_VAL(float a) {
    return ((a) > 0) ? (a) : (-(a));
}

inline int SIGN_VAL(float a) {
    return (a > 0) ? (1) : (-1);
}

inline bool InRange(float input,
                    float min,
                    float max) {
    return (input >= min) && (input <= max);
}

inline int CEIL(int a, int b){
    return (a/b)+(a%b>0);
}

inline int FIX(int a, int b){
    return (a/b)+((float)(a/b)-(a/b)>0.5);
}

enum GenericStatus {
    FOLLOW_RIGHT = 0, FOLLOW_LEFT
};


enum LineStatus {
    STOP = 0, ZERO_LEFT, ZERO_RIGHT, LEFT_LINE, RIGHT_LINE, TWO_LINES
};

inline int PwmRemapping(float control,
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


inline float RemapInput(float input,
                        float minInput,
                        float minOutput,
                        float factor,
                        bool constrained = false) {
    return (input < minInput && constrained) ? (minOutput) : (((input - minInput) * factor) + minOutput);
}


inline int GetBaudRate(int speedVal) {

    int speeds[] = { B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, 0 };
    int speedValues[] = { 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 0 };

    int i = 0;
    while (speedValues[i] != 0) {
        if (speedValues[i] == speedVal) {
            return speeds[i];
        }
        i++;
    }
    return 0;

}
#ifndef NO_OPENCV
using namespace cv;
#endif
using namespace std;
/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* UTILITIES_H_ */

