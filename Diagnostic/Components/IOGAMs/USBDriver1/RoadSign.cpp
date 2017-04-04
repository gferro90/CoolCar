/**
 * @file RoadSign.cpp
 * @brief Source file for class RoadSign
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

 * @details This source file contains the definition of all the methods for
 * the class RoadSign (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "RoadSign.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

RoadSign::RoadSign() {
    // Auto-generated constructor stub for RoadSign
    // TODO Verify if manual additions are needed

    maxSpeed = 0.;
    minSpeed = 0.;
    standardSpeed = 0.;
    maxDrive = 0.;
    minDrive = 0.;

    zeroDrive=0.;
    zeroSpeed=0.;
}


RoadSign::~RoadSign() {
    // Auto-generated destructor stub for RoadSign
    // TODO Verify if manual additions are needed
}

bool RoadSign::ObjectLoadSetup(ConfigurationDataBase &cdbData,
                               StreamInterface * err) {
    bool ret = GCReferenceContainer::ObjectLoadSetup(cdbData, err);

    CDBExtended cdb(cdbData);

    if (ret) {
        int maxDims = 1;
        int size[1] = {0};

        // read rgb range1
        if (!cdb->GetArrayDims(size, maxDims, "RGBmin")) {
            AssertErrorCondition(InitialisationError, "RoadSign::Initialise: %s RGBmin not found HERE", Name());
            ret = false;
        }

        if (ret) {
            ret = (size[0] == 3);
        }

        if (ret) {
            int rgbMin[3];
            if (!cdb.ReadInt32Array(rgbMin, size, maxDims, "RGBmin")) {
                AssertErrorCondition(InitialisationError, "RoadSign::Initialise: %s RGBmin not found", Name());
                ret = false;
            }
            else {
                range1 = Scalar(rgbMin[0], rgbMin[1], rgbMin[2]);
            }
        }

        if (ret) {
            // read rgb range2
            if (!cdb->GetArrayDims(size, maxDims, "RGBmax")) {
                AssertErrorCondition(InitialisationError, "RoadSign::Initialise: %s RGBmax not found", Name());
                ret = false;
            }
        }

        if (ret) {
            ret = (size[0] == 3);
        }

        if (ret) {
            int rgbMax[3];
            if (!cdb.ReadInt32Array(rgbMax, size, maxDims, "RGBmax")) {
                AssertErrorCondition(InitialisationError, "RoadSign::Initialise: %s RGBmax not found", Name());
                ret = false;
            }
            else {
                range2 = Scalar(rgbMax[0], rgbMax[1], rgbMax[2]);
            }
        }

        if (ret) {
            if (!cdb.ReadFString(colorName, "Color", "Orange")) {
                AssertErrorCondition(InitialisationError, "RoadSign::Initialise: %s Color not found", Name());
                ret = false;
            }
        }

        // take two RGB arrays for color range

    }

    return ret;
}

bool RoadSign::Init(float maxSpeedIn,
                    float minSpeedIn,
                    float maxDriveIn,
                    float minDriveIn,
                    float zeroSpeedIn,
                    float zeroDriveIn,
                    float standardSpeedIn) {

    maxSpeed = maxSpeedIn;
    minSpeed = minSpeedIn;
    standardSpeed = standardSpeedIn;
    maxDrive = maxDriveIn;
    minDrive = minDriveIn;
    zeroDrive = zeroDriveIn;
    zeroSpeed = zeroSpeedIn;
    return true;
}

