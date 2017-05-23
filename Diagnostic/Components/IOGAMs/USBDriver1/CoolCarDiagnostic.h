/**
 * @file CoolCarDiagnostic.h
 * @brief Header file for class CoolCarDiagnostic
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

 * @details This header file contains the declaration of the class CoolCarDiagnostic
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef COOLCARDIAGNOSTIC_H_
#define COOLCARDIAGNOSTIC_H_

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

//TODO Opencv stuff

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

OBJECT_DLL (CoolCarDiagnostic)

class CoolCarDiagnostic: public CamModule {

    OBJECT_DLL_STUFF (CoolCarDiagnostic)

public:
    CoolCarDiagnostic();

    virtual ~CoolCarDiagnostic();

    virtual bool ObjectLoadSetup(ConfigurationDataBase &cdbData,
                                 StreamInterface * err);

    virtual bool Init(Mat *,
                      VideoCapture*,
                      int showModeIn,
                      char *bufferIn,
                      char *bufferOut);

    virtual bool Execute();

    virtual bool ProcessHttpMessage(HttpStream &hStream);


private:

    VideoCapture *capture;

    //config params
    int32 positionIndex;
    int32 directionIndex;


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
    int32 httpRefreshTime;

    //state
    Mat *frameMat;
    //outputBuffer
    char* outputBuffer;
    char* inputBuffer;

    float x;
    float y;
    float theta;
    float speed;
    float omega;


    int32 *position;
    float pos_1;
    int32 *direction;
    uint64 lastUpdateTime;
};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* DIAGNOSTIC_COMPONENTS_IOGAMS_CAMERAUSBDRV_COOLCARDIAGNOSTIC_H_ */

