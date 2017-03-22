/**
 * @file TransmitterGAM.h
 * @brief Header file for class TransmitterGAM
 * @date 19/ott/2016
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

 * @details This header file contains the declaration of the class TransmitterGAM
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef MARTE_GAMS_TRANSMITTER_TRANSMITTERGAM_H_
#define MARTE_GAMS_TRANSMITTER_TRANSMITTERGAM_H_

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

class TransmitterGAM: public GAM {
public:
    CLASS_REGISTER_DECLARATION()

    TransmitterGAM();
    virtual ~TransmitterGAM();

    virtual bool Initialise(StructuredDataI &data);

    virtual void Setup();

    virtual bool Execute();

private:
    uint32 *adcMotor;
    uint32 *adcDrive;
    uint16 *pwms;
    uint32 *usb[5];
    uint32 *timer;
    uint32 minMotorRef;
    uint32 rangeMotorRef;
    uint32 minMotorIn;
    uint32 rangeMotorIn;
    uint32 minDriveRef;
    uint32 rangeDriveRef;
    uint32 minDriveIn;
    uint32 rangeDriveIn;
    uint8 transmitOnlyRange;
};
/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* MARTE_GAMS_TRANSMITTER_TRANSMITTERGAM_H_ */

