/**
 * @file RoadSign.h
 * @brief Header file for class RoadSign
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

 * @details This header file contains the declaration of the class RoadSign
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef ROADSIGN_H_
#define ROADSIGN_H_

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
/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

class RoadSign: public GCReferenceContainer {
public:
    RoadSign();
    virtual ~RoadSign();

    virtual bool ObjectLoadSetup(ConfigurationDataBase &cdbData,
                                 StreamInterface * err);

    virtual bool Init(float maxSpeedIn,
                      float minSpeedIn,
                      float maxDriveIn,
                      float minDriveIn,
                      float zeroSpeedIn,
                      float zeroDriveIn,
                      float standardSpeedIn);

    virtual float SignalFunction(int & status)=0;

    FString colorName;
    Scalar range1;
    Scalar range2;
    float maxSpeed;
    float minSpeed;
    float standardSpeed;
    float maxDrive;
    float minDrive;
    float zeroDrive;
    float zeroSpeed;

};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* ROADSIGN_H_ */

