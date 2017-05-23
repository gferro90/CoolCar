/**
 * @file CamModule.h
 * @brief Header file for class CamModule
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

 * @details This header file contains the declaration of the class CamModule
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef CAMMODULE_H_
#define CAMMODULE_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/

#include "System.h"
#include "GenericAcqModule.h"
#include "File.h"
#include "Utilities.h"
#include "HttpStream.h"
#include "HttpInterface.h"
#include "HtmlStream.h"

/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

class CamModule: public GCReferenceContainer, public HttpInterface {
public:
    CamModule(){

    }
    virtual ~CamModule(){

    }
    virtual bool Init(Mat *, VideoCapture*,
                      int showModeIn, char *bufferIn, char* bufferOut)=0;

    virtual bool Execute()=0;

};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* DIAGNOSTIC_COMPONENTS_IOGAMS_CAMERAUSBDRV_CAMMODULE_H_ */

