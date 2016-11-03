/**
 * @file SpiRadioModule.h
 * @brief Header file for class SpiRadioModule
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

 * @details This header file contains the declaration of the class SpiRadioModule
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef MARTE_DATASOURCES_SPIRADIOMODULE_H_
#define MARTE_DATASOURCES_SPIRADIOMODULE_H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/
#include "DataSourceI.h"
#include "stm32f4xx_hal.h"
#include "nRF24L01+.h"
/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

using namespace MARTe;

class SpiRadioModule:public DataSourceI {
public:

    CLASS_REGISTER_DECLARATION()

    SpiRadioModule();
    virtual ~SpiRadioModule();


    /*DataSourceI functions*/
    virtual bool Initialise(StructuredDataI &data);

    bool Synchronise();

    bool AllocateMemory();

    uint32 GetNumberOfMemoryBuffers();

    virtual bool SetConfiguredDatabase(MARTe::StructuredDataI & data);

    bool GetSignalMemoryBuffer(const uint32 signalIdx,
                               const uint32 bufferIdx,
                               void *&signalAddress);

    virtual const char8 *GetBrokerName(StructuredDataI &data,
                                       const SignalDirection direction);

    virtual bool GetInputBrokers(ReferenceContainer &inputBrokers,
                                 const char8* const functionName,
                                 void * const gamMemPtr);

    virtual bool GetOutputBrokers(ReferenceContainer &outputBrokers,
                                  const char8* const functionName,
                                  void * const gamMemPtr);

    virtual bool PrepareNextState(const char8 * const currentStateName,
                                  const char8 * const nextStateName);



    bool ReadPacket(char8 * buffer, uint32 length);

    bool WritePacket(const char8* buffer, uint32 length);

    void ReadRegister(uint8 reg, uint8 &status, void *data, uint32 size=1);

    bool Listening() const;
    uint32 Timeout() const;
    void StartListening();
    void StopListening();
    void Standby();




private:
    SPI_HandleTypeDef *spi;
    GPIO_TypeDef *csn;
    GPIO_TypeDef *ce;
    uint16 cePinMask;
    uint16 csnPinMask;
    uint8 txBuffer[TX_BUFFER_LENGTH];
    uint8 rxBuffer[RX_BUFFER_LENGTH];
    bool listening;
    uint32 timeout;
    uint8 delayAfterStop;
};

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

#endif /* MARTE_DATASOURCES_SPIRADIOMODULE_H_ */

