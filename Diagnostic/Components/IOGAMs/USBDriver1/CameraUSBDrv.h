/**
 * @file CameraUSBDrv.h
 * @brief Header file for class CameraUSBDrv
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

 * @details This header file contains the declaration of the class CameraUSBDrv
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef CAMERAUSBDRV_H_
#define CAMERAUSBDRV_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/


#define int64 int64_t
#define uint64 uint64_t
#include "Utilities.h"
#undef int64
#undef uint64
#include "USBDrv.h"
#include "CamModule.h"
#include "System.h"
#include "GenericAcqModule.h"
#include "File.h"

/// Number buffers for data storage
OBJECT_DLL (CameraUSBDrv)

// Callback declaration
void ReceiverCallback(void *userData);

/// USB Module Class
class CameraUSBDrv: public USBDrv {
    OBJECT_DLL_STUFF (CameraUSBDrv)


/// Receiver CallBack
    friend void ReceiverCallback(void *userData);

public:

    /** Constructor */
    CameraUSBDrv();

    /** Deconstructor */
    virtual ~CameraUSBDrv();

    /** Load Object Parameters from the ConfigurationDataBase */
    virtual bool ObjectLoadSetup(ConfigurationDataBase &cdbData,
                                 StreamInterface *err);


    /** Object Description */
    virtual bool ObjectDescription(StreamInterface &s,
                                   bool full,
                                   StreamInterface *err);

///////////////////////////////////////////////////////////////////////////////
//                           From Time Module                                //
///////////////////////////////////////////////////////////////////////////////


    /** Gets Data From the Module to the DDB
     @param usecTime Microseconds Time
     @return -1 on Error, 1 on success
     */
    int32 GetData(uint32 usecTime,
                  int32 *buffer,
                  int32 bufferNumber = 0);


    virtual bool Poll();

///////////////////////////////////////////////////////////////////////////////
//                                  General                                  //
///////////////////////////////////////////////////////////////////////////////

// Serve webpage
    bool ProcessHttpMessage(HttpStream &hStream);


private:

    int32 cameraId;

    int32 calibrateCycles;
    int32 showMode;

    char* dataWriteBuffer;
    int32 outputBufferSize;
    // sub-objects
    GCRTemplate<CamModule> *modules;
    uint32 numberOfModules;

    //graphic objects
    VideoCapture *capture;

    Mat *frameMat;
};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* CAMERAUSBDRV_H_ */

