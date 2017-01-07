/**
 * @file SpiRadioModule.cpp
 * @brief Source file for class SpiRadioModule
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
 * the class SpiRadioModule (public, protected, and private). Be aware that some 
 * methods, such as those inline could be defined on the header file, instead.
 */

/*---------------------------------------------------------------------------*/
/*                         Standard header includes                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                         Project header includes                           */
/*---------------------------------------------------------------------------*/

#include "SpiRadioModule.h"
#include "SpiRadioReader.h"
#include "SpiRadioWriter.h"
#include "AdvancedErrorManagement.h"

/*---------------------------------------------------------------------------*/
/*                           Static definitions                              */
/*---------------------------------------------------------------------------*/

#define CE_L HAL_GPIO_WritePin(ce, cePinMask, GPIO_PIN_RESET)

#define CE_H HAL_GPIO_WritePin(ce, cePinMask, GPIO_PIN_SET)

#define CSN_L HAL_GPIO_WritePin(csn, csnPinMask, GPIO_PIN_RESET)

#define CSN_H HAL_GPIO_WritePin(csn, csnPinMask, GPIO_PIN_SET)

#define INFINITE_TIMEOUT 0xffffffffu
#define SPI_TX_RX(txBuffer, rxBuffer, size)    HAL_SPI_TransmitReceive(spi, txBuffer, rxBuffer, size, INFINITE_TIMEOUT)

extern "C" {
extern void *GetHwHandle(const char* id);
}

/*---------------------------------------------------------------------------*/
/*                           Method definitions                              */
/*---------------------------------------------------------------------------*/

SpiRadioModule::SpiRadioModule() :
        DataSourceI() {
    spi = NULL;
    csn = NULL;
    ce = NULL;
    cePinMask = 0u;
    csnPinMask = 0u;
    listening = false;
    timeout = 0u;
    delayAfterStop=10u;
}

SpiRadioModule::~SpiRadioModule() {
    // Auto-generated destructor stub for SpiRadioModule
    // TODO Verify if manual additions are needed

}

bool SpiRadioModule::Initialise(StructuredDataI &data) {
    bool ret = DataSourceI::Initialise(data);

    if (ret) {

        //get the ce pin
        StreamString cePortId;
        ret = data.Read("CE_Port", cePortId);
        if (ret) {
            ce = (GPIO_TypeDef *) GetHwHandle(cePortId.Buffer());
            ret = (ce != NULL);
            if (ret) {
                uint8 cePinId;
                ret = data.Read("CE_Pin", cePinId);
                if (ret) {
                    cePinMask = (1 << cePinId);
                }
                else {
                    REPORT_ERROR(ErrorManagement::FatalError, "Undefined CE pin");
                }
            }
        }
        else {
            REPORT_ERROR(ErrorManagement::FatalError, "Undefined CE port");
        }

        if (ret) {
            //get the ce pin
            StreamString csnPortId;
            ret = data.Read("CSN_Port", csnPortId);
            if (ret) {
                csn = (GPIO_TypeDef *) GetHwHandle(csnPortId.Buffer());
                ret = (csn != NULL);
                if (ret) {
                    uint8 csnPinId;
                    ret = data.Read("CSN_Pin", csnPinId);
                    if (ret) {
                        csnPinMask = (1 << csnPinId);
                    }
                    else {
                        REPORT_ERROR(ErrorManagement::FatalError, "Undefined CSN pin");
                    }
                }
            }
            else {
                REPORT_ERROR(ErrorManagement::FatalError, "Undefined CSN port");
            }
        }

        if (ret) {
            StreamString spiId;
            ret = data.Read("SPI", spiId);
            if (ret) {
                //init the radio module
                spi = (SPI_HandleTypeDef *) GetHwHandle(spiId.Buffer());
                ret = (spi != NULL);
            }
            else {
                REPORT_ERROR(ErrorManagement::FatalError, "Undefined SPI");
            }
        }

        if (ret) {
            CSN_H;
            Standby();

            //put the module in power up mode
            MemoryOperationsHelper::Set(txBuffer, 0, TX_BUFFER_LENGTH);
            MemoryOperationsHelper::Set(rxBuffer, 0, RX_BUFFER_LENGTH);
            HAL_Delay(1);
            txBuffer[0] = WRITE_REGISTER(CONFIG);
            txBuffer[1] = ((1 << EN_CRC) | (1 << CRCO) | (1 << PWR_UP));
            CSN_L;
            SPI_TX_RX(txBuffer, rxBuffer, 2);
            CSN_H;

            //set the payload size

            MemoryOperationsHelper::Set(txBuffer, 0, TX_BUFFER_LENGTH);
            MemoryOperationsHelper::Set(rxBuffer, 0, RX_BUFFER_LENGTH);
            HAL_Delay(1);

            uint8 payloadSize;
            if (!data.Read("PayloadSize", payloadSize)) {
                REPORT_ERROR(ErrorManagement::Warning, "TxAddress not set: use by default 16");
                txBuffer[0] = WRITE_REGISTER(RX_PW_P0);
                txBuffer[1] = 16;
                CSN_L;
                SPI_TX_RX(txBuffer, rxBuffer, 2);
                CSN_H;

            }
            else {
                txBuffer[0] = WRITE_REGISTER(RX_PW_P0);
                txBuffer[1] = payloadSize;
                CSN_L;
                SPI_TX_RX(txBuffer, rxBuffer, 2);
                CSN_H;
            }

            //set the speed 1MHz todo configurable?
            MemoryOperationsHelper::Set(txBuffer, 0, TX_BUFFER_LENGTH);
            MemoryOperationsHelper::Set(rxBuffer, 0, RX_BUFFER_LENGTH);
            HAL_Delay(1);
            txBuffer[0] = WRITE_REGISTER(RF_SETUP);
            txBuffer[1] = 0x06;
            CSN_L;
            SPI_TX_RX(txBuffer, rxBuffer, 2);
            CSN_H;

            //set the auto retransmission delay to 750us todo configurable??
            MemoryOperationsHelper::Set(txBuffer, 0, TX_BUFFER_LENGTH);
            MemoryOperationsHelper::Set(rxBuffer, 0, RX_BUFFER_LENGTH);
            HAL_Delay(1);
            txBuffer[0] = WRITE_REGISTER(SETUP_RETR);
            txBuffer[1] = 0x02;
            CSN_L;
            SPI_TX_RX(txBuffer, rxBuffer, 2);
            CSN_H;

            uint64 txAddr;
            if (!data.Read("TxAddress", txAddr)) {
                REPORT_ERROR(ErrorManagement::Warning, "TxAddress not set: use the default");
            }
            else {
                //set tx addr
                MemoryOperationsHelper::Set(txBuffer, 0, TX_BUFFER_LENGTH);
                MemoryOperationsHelper::Set(rxBuffer, 0, RX_BUFFER_LENGTH);
                HAL_Delay(1);
                txBuffer[0] = WRITE_REGISTER(RX_ADDR_P0);
                for (uint32 i = 0; i < 5; i++) {
                    txBuffer[i + 1] = ((txAddr >> i) & 0xff);
                }
                CSN_L;
                SPI_TX_RX(txBuffer, rxBuffer, 6);
                CSN_H;
            }
            uint64 rxAddr;
            if (!data.Read("RxAddress", rxAddr)) {
                REPORT_ERROR(ErrorManagement::Warning, "TxAddress not set: use the default");
            }
            else {
                //set rx addr
                MemoryOperationsHelper::Set(txBuffer, 0, TX_BUFFER_LENGTH);
                MemoryOperationsHelper::Set(rxBuffer, 0, RX_BUFFER_LENGTH);
                HAL_Delay(1);

                txBuffer[0] = WRITE_REGISTER(TX_ADDR);
                for (uint32 i = 0; i < 5; i++) {
                    txBuffer[i + 1] = ((rxAddr >> i) & 0xff);
                }
                CSN_L;
                SPI_TX_RX(txBuffer, rxBuffer, 6);
                CSN_H;
            }

            if (!data.Read("Timeout", timeout)) {
                timeout = 0u;
                REPORT_ERROR(ErrorManagement::Warning, "Set non-blocking operations by default");
            }
            listening = false;
            uint8 startReading;
            if (data.Read("StartListening", startReading)) {
                if (startReading > 0) {
                    StartListening();
                }
            }

            if (!data.Read("DelayAfterStop", delayAfterStop)) {
                delayAfterStop=50u;
            }
        }
    }

    return ret;
}

bool SpiRadioModule::Synchronise() {
    return true;
}

bool SpiRadioModule::AllocateMemory() {
    return true;
}

uint32 SpiRadioModule::GetNumberOfMemoryBuffers() {
    return 1u;
}

bool SpiRadioModule::SetConfiguredDatabase(MARTe::StructuredDataI & data) {
    bool ret = DataSourceI::SetConfiguredDatabase(data);
    //todo need to check if the signal size matches with the payload size

    /*if (ret) {
     uint32 numberOfSignals = GetNumberOfSignals();
     for (uint32 i = 0u; i < numberOfSignals && ret; i++) {
     ret = (GetSignalType(i) == TypeDescriptor::GetTypeDescriptorFromTypeName("uint32"));
     if (!ret) {
     REPORT_ERROR(ErrorManagement::InitialisationError, "The SpiRadioModule signal type has to be uint32");
     }
     }
     }*/
    return ret;
}

bool SpiRadioModule::GetSignalMemoryBuffer(const uint32 signalIdx,
                                           const uint32 bufferIdx,
                                           void *&signalAddress) {
    signalAddress = NULL;
    return true;
}

const char8 *SpiRadioModule::GetBrokerName(StructuredDataI &data,
                                           const SignalDirection direction) {
    if (direction == InputSignals) {
        return "SpiRadioReader";
    }
    else {
        return "SpiRadioWriter";
    }

    return "";
}

bool SpiRadioModule::GetInputBrokers(ReferenceContainer &inputBrokers,
                                     const char8* const functionName,
                                     void * const gamMemPtr) {
    ReferenceT<SpiRadioReader> broker("SpiRadioReader");
    bool ret = broker.IsValid();
    if (ret) {
        ret = broker->Init(InputSignals, *this, functionName, gamMemPtr);
    }
    if (ret) {
        ret = inputBrokers.Insert(broker);
    }
    return ret;
}

bool SpiRadioModule::GetOutputBrokers(ReferenceContainer &outputBrokers,
                                      const char8* const functionName,
                                      void * const gamMemPtr) {
    ReferenceT<SpiRadioWriter> broker("SpiRadioWriter");
    bool ret = broker.IsValid();
    if (ret) {
        ret = broker->Init(OutputSignals, *this, functionName, gamMemPtr);
    }
    if (ret) {
        ret = outputBrokers.Insert(broker);
    }
    return ret;
}

bool SpiRadioModule::PrepareNextState(const MARTe::char8 * const currentStateName,
                                      const MARTe::char8 * const nextStateName) {
    return true;
}

bool SpiRadioModule::ReadPacket(char8 * buffer,
                                uint32 length) {

    static uint8 kk = 0u;
    //go in Rx mode
    StartListening();

    uint8 status = 0;
    bool received = false;
    //  while (!received) {
    CE_L;

    MemoryOperationsHelper::Set(txBuffer, 0, TX_BUFFER_LENGTH);
    MemoryOperationsHelper::Set(rxBuffer, 0, RX_BUFFER_LENGTH);
    //HAL_Delay(1);
    txBuffer[0] = READ_REGISTER(FIFO_STATUS);
    CSN_L;
    SPI_TX_RX(txBuffer, rxBuffer, 2);
    CSN_H;
    status = rxBuffer[0];

    MemoryOperationsHelper::Set(txBuffer, 0, TX_BUFFER_LENGTH);
    MemoryOperationsHelper::Set(rxBuffer, 0, RX_BUFFER_LENGTH);
    //HAL_Delay(1);

    received = (status & 0x40);    // || (!(rxBuffer[1] & 0x1));
    if (received) {
        //read something
        txBuffer[0] = R_RX_PAYLOAD;
        CSN_L;
        SPI_TX_RX(txBuffer, rxBuffer, length + 1);
        CSN_H;
        status = rxBuffer[0];
        //copy anyway
        MemoryOperationsHelper::Copy(buffer, rxBuffer + 1, length);
        kk = 0u;
    }
    else {
        kk++;
    }
    if(kk>=delayAfterStop){
        // copy 0
        MemoryOperationsHelper::Set(buffer, 0, length);
        kk=0u;
    }

    //}
    //if (received) {
    MemoryOperationsHelper::Set(txBuffer, 0, TX_BUFFER_LENGTH);
    MemoryOperationsHelper::Set(rxBuffer, 0, RX_BUFFER_LENGTH);
    //HAL_Delay(1);
    txBuffer[0] = WRITE_REGISTER(STATUS);
    //the first byte of rx should always be the status reg.
    txBuffer[1] = (status | (status & 0x70));
    CSN_L;
    SPI_TX_RX(txBuffer, rxBuffer, 2);
    CSN_H;
    //}

    //HAL_Delay(1);
    MemoryOperationsHelper::Set(txBuffer, 0, TX_BUFFER_LENGTH);
    MemoryOperationsHelper::Set(rxBuffer, 0, RX_BUFFER_LENGTH);
    //HAL_Delay(1);
    txBuffer[0] = FLUSH_RX;
    CSN_L;
    SPI_TX_RX(txBuffer, rxBuffer, 1);
    CSN_H;
    //HAL_Delay(1);
    //restart listening
    CE_H;

    return true;
}

bool SpiRadioModule::WritePacket(const char8* buffer,
                                 uint32 length) {
    //go in Tx mode
    StopListening();

    MemoryOperationsHelper::Set(txBuffer, 0, TX_BUFFER_LENGTH);
    MemoryOperationsHelper::Set(rxBuffer, 0, RX_BUFFER_LENGTH);
    //HAL_Delay(1);
    txBuffer[0] = FLUSH_TX;
    CSN_L;
    SPI_TX_RX(txBuffer, rxBuffer, 1);
    CSN_H;

    MemoryOperationsHelper::Set(txBuffer, 0, TX_BUFFER_LENGTH);
    MemoryOperationsHelper::Set(rxBuffer, 0, RX_BUFFER_LENGTH);
    //HAL_Delay(1);
    txBuffer[0] = W_TX_PAYLOAD;
    MemoryOperationsHelper::Copy(txBuffer + 1, buffer, length);
    //we should obtain in rxBuffer the status register
    CSN_L;
    SPI_TX_RX(txBuffer, rxBuffer, length + 1);
    CSN_H;

    uint8 status = rxBuffer[0];

    MemoryOperationsHelper::Set(txBuffer, 0, TX_BUFFER_LENGTH);
    MemoryOperationsHelper::Set(rxBuffer, 0, RX_BUFFER_LENGTH);
    //HAL_Delay(1);
    txBuffer[0] = READ_REGISTER(FIFO_STATUS);
    CSN_L;
    SPI_TX_RX(txBuffer, rxBuffer, 2);
    CSN_H;
    status = rxBuffer[0];
    if (!(rxBuffer[1] & 0x10)) {
        CE_H;
        HAL_Delay(1);
        CE_L;
    }
    else {
        MemoryOperationsHelper::Set((void*) buffer, 'n', length);
    }

    bool transmitted = false;
    while (!transmitted) {
        MemoryOperationsHelper::Set(txBuffer, 0, TX_BUFFER_LENGTH);
        MemoryOperationsHelper::Set(rxBuffer, 0, RX_BUFFER_LENGTH);
        //HAL_Delay(1);
        txBuffer[0] = READ_REGISTER(STATUS);
        //the first byte of rx should always be the status reg.
        CSN_L;
        SPI_TX_RX(txBuffer, rxBuffer, 2);
        CSN_H;
        transmitted = (rxBuffer[0] & 0x30);
        //HAL_Delay(1);

    }

    MemoryOperationsHelper::Set(txBuffer, 0, TX_BUFFER_LENGTH);
    MemoryOperationsHelper::Set(rxBuffer, 0, RX_BUFFER_LENGTH);
    //HAL_Delay(1);
    txBuffer[0] = WRITE_REGISTER(STATUS);
    //the first byte of rx should always be the status reg.
    txBuffer[1] = (status | (status & 0x70));
    CSN_L;
    SPI_TX_RX(txBuffer, rxBuffer, 2);
    CSN_H;

    return true;
}

void SpiRadioModule::StartListening() {

    if (!listening) {
        CE_L;

        MemoryOperationsHelper::Set(txBuffer, 0, TX_BUFFER_LENGTH);
        MemoryOperationsHelper::Set(rxBuffer, 0, RX_BUFFER_LENGTH);
        //
        //receiver config
        //set PRIM_RX=1
        /*  txBuffer[0] = READ_REGISTER(CONFIG);
         CSN_L;
         SPI_TX_RX((char8*)txBuffer, (char8*)rxBuffer, 2);
         CSN_H;*/
        //HAL_Delay(1);
        txBuffer[0] = WRITE_REGISTER(CONFIG);
        //the first byte of rx should always be the status reg.
        txBuffer[1] = ((1 << EN_CRC) | (1 << CRCO) | (1 << PWR_UP) | (1 << PRIM_RX));
        CSN_L;
        SPI_TX_RX(txBuffer, rxBuffer, 2);
        CSN_H;
        //HAL_Delay(1);
        CE_H;
        listening = true;
    }

}

void SpiRadioModule::StopListening() {
    if (listening) {
        CE_L;

        MemoryOperationsHelper::Set(txBuffer, 0, TX_BUFFER_LENGTH);
        MemoryOperationsHelper::Set(rxBuffer, 0, RX_BUFFER_LENGTH);
        //standby mode

        //HAL_Delay(1);
        txBuffer[0] = WRITE_REGISTER(CONFIG);
        //the first byte of rx should always be the status reg.
        txBuffer[1] = ((1 << EN_CRC) | (1 << CRCO) | (1 << PWR_UP));
        CSN_L;
        SPI_TX_RX(txBuffer, rxBuffer, 2);
        CSN_H;
        //HAL_Delay(1);
        listening = false;
    }
}

void SpiRadioModule::Standby() {
    CE_L;
    listening = false;
}

void SpiRadioModule::ReadRegister(uint8 reg,
                                  uint8 &status,
                                  void *data,
                                  uint32 size) {
    MemoryOperationsHelper::Set(txBuffer, 0, TX_BUFFER_LENGTH);
    MemoryOperationsHelper::Set(rxBuffer, 0, RX_BUFFER_LENGTH);

    txBuffer[0] = READ_REGISTER(reg);
    CSN_L;
    SPI_TX_RX(txBuffer, rxBuffer, size + 1);
    CSN_H;
    status = rxBuffer[0];
    MemoryOperationsHelper::Copy(data, rxBuffer + 1, size);
}

bool SpiRadioModule::Listening() const {
    return listening;
}

uint32 SpiRadioModule::Timeout() const {
    return timeout;
}

CLASS_REGISTER(SpiRadioModule, "1.0")
