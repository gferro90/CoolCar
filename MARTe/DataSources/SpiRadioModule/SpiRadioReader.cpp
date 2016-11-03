/**
 * @file SpiRadioReader.cpp
 * @brief Source file for class SpiRadioReader
 * @date 18/ott/2016
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
 * the class SpiRadioReader (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "SpiRadioReader.h"
#include "SpiRadioModule.h"
/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

SpiRadioReader::SpiRadioReader() :MemoryMapInputBroker(){
    // Auto-generated constructor stub for SpiRadioReader
    // TODO Verify if manual additions are needed
    radio=NULL;
}

SpiRadioReader::~SpiRadioReader() {
    // Auto-generated destructor stub for SpiRadioReader
    // TODO Verify if manual additions are needed
}

bool SpiRadioReader::Execute() {
    uint32 n;
    bool ret = true;
    for (n = 0u; (n < numberOfCopies) && (ret); n++) {
        if (copyTable != NULL_PTR(MemoryMapBrokerCopyTableEntry *)) {
            radio->ReadPacket((char8 *)copyTable[n].gamPointer, copyTable[n].copySize);
        }
    }
    return ret;
}

bool SpiRadioReader::Init(const SignalDirection direction,
                     DataSourceI &dataSourceIn,
                     const char8 * const functionName,
                     void * const gamMemoryAddress) {
    bool ret = MemoryMapInputBroker::Init(direction, dataSourceIn, functionName, gamMemoryAddress);
    if (ret) {
        radio = dynamic_cast<SpiRadioModule*>(dataSource);
        ret=(radio!=NULL);
        if(!ret){
            REPORT_ERROR(ErrorManagement::FatalError, "Failed dynamic_cast from DataSourceI* to SpiRadioModule*");
        }
    }
    return ret;
}

CLASS_REGISTER(SpiRadioReader, "1.0")
