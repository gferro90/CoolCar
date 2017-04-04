/**
 * @file TurnLeftSign.cpp
 * @brief Source file for class TurnLeftSign
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
 * the class TurnLeftSign (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "TurnLeftSign.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

TurnLeftSign::TurnLeftSign():RoadSign() {
    // Auto-generated constructor stub for TurnLeftSign
    // TODO Verify if manual additions are needed
}

TurnLeftSign::~TurnLeftSign() {
    // Auto-generated destructor stub for TurnLeftSign
    // TODO Verify if manual additions are needed
}

float TurnLeftSign::SignalFunction(int & status){
    status = FOLLOW_LEFT;
    return standardSpeed;
}
OBJECTLOADREGISTER(TurnLeftSign, "$Id:TurnLeftSign.cpp,v 1.1.1.1 2010-01-20 12:26:47 pc Exp $")
