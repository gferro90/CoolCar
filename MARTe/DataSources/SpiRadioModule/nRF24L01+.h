/**
 * @file nRF24L01+.h
 * @brief Header file for class nRF24L01+
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

 * @details This header file contains the declaration of the class nRF24L01+
 * with all of its public, protected and private members. It may also include
 * definitions for inline methods which need to be visible to the compiler.
 */

#ifndef MARTE_DATASOURCES_NRF24L01__H_
#define MARTE_DATASOURCES_NRF24L01__H_

/*---------------------------------------------------------------------------*/
/*                        Standard header includes                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                        Project header includes                            */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/*                           Class declaration                               */
/*---------------------------------------------------------------------------*/

#define CONFIG 0x00 //configuration register
#define MASK_RX_DR 6 //enable interrupt on receiving a packet
#define MASK_TX_DS 5 //enable interrupt on transmitting a packet
#define MASK_MAX_RT 4
#define EN_CRC 3 //enable CRC
#define CRCO 2
#define PWR_UP 1
#define PRIM_RX 0 //1 for RX, 0 for TX

#define EN_AA 0x01 //enhanced shock burst register
#define ENAA_P5 5 //enable auto-ack data pipe x
#define ENAA_P4 4
#define ENAA_P3 3
#define ENAA_P2 2
#define ENAA_P1 1
#define ENAA_P0 0

#define EN_RXADDR 0x02 //enabled RX addresses
#define ERX_P5 5 //enable data pipe x
#define ERX_P4 4
#define ERX_P3 3
#define ERX_P2 2
#define ERX_P1 1
#define ERX_P0 0

#define SETUP_AW 0x03 //detup addresses widths
#define AW 0 //2-bits to set the witdth up to 5 bytes

#define SETUP_RETR 0x04 //setup automatic retransmission
#define ARD 4 //4 bits to set the delay of the auto retransmission
#define ARC 0 //4 bits to set how many retransmissions

#define RF_CH 0x05 //radio frequency channel

#define RF_SETUP 0x06 //radio frequency setup register
#define CONT_WAVE 7 //enables continuous carrier transmit
#define RF_DR_LOW 5
#define PLL_LOCK 4
#define RF_DR_HIGH 3//select the air data rates
#define RF_PWR 1 //2 bit selecting output power

#define STATUS 0x07 //the status register
#define RX_DR 6//data ready
#define TX_DS 5//data sent
#define MAX_RT 4//max number of retransmissions
#define RX_P_NO 1//3 bits to assert the data pipe number
#define TX_FULL 0//the tx fifo is full

#define OBSERVE_TX 0x08//transmit observe register
#define PLOS_CNT 4//4 bits to count the lost packets
#define ARC_CNT 0//4 bits to count the retransmitted packets

#define RPD 0x09//receiver power detector

#define RX_ADDR_P0 0x0A//receive address data pype 0 (5 bytes max with the LSByte first)

#define RX_ADDR_P1 0x0B//receive address data pype 1 (5 bytes max with the LSByte first)

#define RX_ADDR_P2 0x0C//receive address data pype 2 (LSByte)

#define RX_ADDR_P3 0x0D//receive address data pype 3 (LSByte)

#define RX_ADDR_P4 0x0E//receive address data pype 4 (LSByte)

#define RX_ADDR_P5 0x0F//receive address data pype 5 (LSByte)

#define TX_ADDR 0x10//transmit address used only on the transmitter (5 bytes max)

#define RX_PW_P0 0x11 //the number of bytes in RX payload in data pipe 0 (1-32 bytes)

#define RX_PW_P1 0x12 //the number of bytes in RX payload in data pipe 1 (1-32 bytes)

#define RX_PW_P2 0x13 //the number of bytes in RX payload in data pipe 2 (1-32 bytes)

#define RX_PW_P3 0x14 //the number of bytes in RX payload in data pipe 3 (1-32 bytes)

#define RX_PW_P4 0x15 //the number of bytes in RX payload in data pipe 4 (1-32 bytes)

#define RX_PW_P5 0x16 //the number of bytes in RX payload in data pipe 5 (1-32 bytes)

#define FIFO_STATUS 0x17 //the fifo status register
#define TX_REUSE 6
#define TX_FIFO_FULL 5//check is tx fifo is full
#define TX_FIFO_EMPTY 4//check if tx fifo is empty
#define RX_FIFO_FULL 1//check if rx fifo is full
#define RX_FIFO_EMPTY 0//check if rx fifo is empty

//commands
#define READ_REGISTER(reg_addr) (0x1f & reg_addr) //read a register
#define WRITE_REGISTER(reg_addr) (0x1f & reg_addr)|(0x20) //write to a register
#define R_RX_PAYLOAD 0x61 //read the rx payload
#define W_TX_PAYLOAD 0xa0 //write the tx payload
#define FLUSH_TX 0xe1 //flush the tx fifo
#define FLUSH_RX 0xe2 //flush the rx fifo
#define REUSE_TX_PL 0xe3 //reuse the last transmitted payload
#define R_RX_PL_WID 0x60 //read the last payload width
#define W_ACK_PAYLOAD(pipe) (0xa8)|(pipe & 0x07)
#define W_TX_PAYLOAD_NO_ACK 0xb0 //disables autoack for this packet
#define NOP 0xff

#define TX_BUFFER_LENGTH 33
#define RX_BUFFER_LENGTH 33

/*---------------------------------------------------------------------------*/
/*                        Inline method definitions                          */
/*---------------------------------------------------------------------------*/

		#endif /* MARTE_DATASOURCES_NRF24L01__H_ */
	
