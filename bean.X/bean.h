/*
 * File:   bean.h
 * Author: fedor
 *
 * Created on 31 ????? 2022 ?., 15:37
 */

#ifndef BEAN_H
#define	BEAN_H

#ifdef	__cplusplus
extern "C" {
#endif

#define BEANBUFFSIZE 17   // 13 (Data + Header) + ML + CRC + EOM + RSP = 17
    // When more than BEAN_NOT_COND ticks passed it means that transfer terminated
#define BEAN_NO_TR_COND          9

typedef enum {
    BEAN_NO_TR,             // Should be first to equal to 0
    BEAN_NO_TR_DATA_PRESENT, // Np transfer in progress but data has been loaded to tr buffer
    BEAN_TR_IN_PR,
    BEAN_TR_SOF,       // We are just got SOF and some part of ML.
    BEAN_TR_MLINPR,
    BEAN_TR_ML,       // We are getting ML
    BEAN_TR_DSTID,
    BEAN_TR_MESID,
    BEAN_TR_DATA,
    BEAN_TR_CRC8,
    BEAN_TR_EOM,
    BEAN_TR_RSP,

    BEAN_TR_ERR = 0x81,
    BEAN_TR_CRCERR = 0x82,
} BeanTransferState;

typedef struct {
    unsigned char recBuffer1[BEANBUFFSIZE];
    unsigned char recBuffer2[BEANBUFFSIZE];
    unsigned char *intBuffer; // Is used to receive data by bean executable
    unsigned char *buffer; // Is used to get data by user app
    // Bit that has been received. As we receive data in bits
    unsigned char recBit;
    // Position in receive buffer
    unsigned char recBuffPos;
    // Transfer state
    BeanTransferState recBeanState;

    // Do we await staffing bit next?
    unsigned char recIsNextBitStaffing : 1;
    // Is set when rec buffer is full of data by bean exec
    unsigned char recBufferFull : 1;

} RecBeanData;

typedef struct {
    unsigned char sendBuffer[BEANBUFFSIZE];
    // Bit that has been sent. As we send data in bits
    unsigned char sentBit;
    // Position in send buffer
    unsigned char sendBuffPos;
    // Number of ticks to send to bus
    unsigned char cnt;
    // Transfer state
    BeanTransferState sendBeanState;

    // Should next bit be staffing?
    unsigned char sendNextBitStaffing : 1;
    // What bit shpuld be sent to bus (0/1)
    unsigned char bean : 1;

} SendBeanData;

// Receive BEAN data from BEAN bus
void recBean(RecBeanData *pBeanData, char bean, unsigned char cnt);
void resetRecBuffer(RecBeanData *pBeanData);

#ifdef __32MX220F032B__
unsigned char getCntFromTmr(unsigned short tmr, unsigned short t_cnt);
#endif

void sendBean(SendBeanData* pBeanData);
unsigned char initSendBeanData(SendBeanData *pBeanData, unsigned char *buff);
void resetSendBuffer(SendBeanData *pBeanData);
void setSendError(SendBeanData *pBeanData);
unsigned char resetSendError(SendBeanData *pBeanData);
unsigned char isTransferInProgress(SendBeanData *pBeanData);
unsigned char canStartTransfer(BeanTransferState sendBeanState, BeanTransferState recBeanState);

unsigned char Crc8(unsigned char *pcBlock, unsigned char len);

#ifdef	__cplusplus
}
#endif

#endif	/* BEAN_H */

