/**
 * @file CoolCarControlGAM.h
 * @brief Header file for class CoolCarControlGAM
 * @date 28/set/2016
 * @author Giuseppe Ferrò
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

 * @details This header file contains the declaration of the class CoolCarControlGAM
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef GAMS_COOLCARCONTROLGAM_H_
#define GAMS_COOLCARCONTROLGAM_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/

#include "GAM.h"
#include "ConfigurationDatabase.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

using namespace MARTe;

class CoolCarControlGAM: public GAM {
public:
    CLASS_REGISTER_DECLARATION()

    CoolCarControlGAM();
    virtual ~CoolCarControlGAM();

    virtual bool Initialise(StructuredDataI &data);

    virtual void Setup();

    virtual bool Execute();

private:
    //signals
    uint16 *refs;
    uint32 *pwmMotor;
    uint32 *pwmDrive;
    uint32 *usb[3];
    uint32 *timer;
    uint32 *stops;

    //params
    uint8 maxMotorIn;
    uint8 minMotorIn;
    uint8 maxDriveIn;
    uint8 minDriveIn;
    uint32 noObstacle;
    uint32 obstacle;
    uint8 *obstacleDetected;
    uint32 numberOfStops;
};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* GAMS_COOLCARCONTROLGAM_H_ */

