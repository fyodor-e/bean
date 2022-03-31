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

#define BEANBUFFSIZE 63
    // When more than BEAN_NOT_COND ticks passed it means that transfer terminated
#define BEAN_NO_TR_COND          7

typedef enum {
    BEAN_NO_TR,
    BEAN_TR_SOF,       // We are just got SOF and some part of ML.
//#define BEAN_ERR_NO_SOF         0x11
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

    BEAN_TR_NEXT_STAFF_BIT = 0x80,   // 5 same bits received
    BEAN_TR_NO_STAFFING = 0x40,
} BeanTransferState;

typedef struct {
    unsigned char recBuffer[3][BEANBUFFSIZE];
    unsigned char *pRecBuffer;
    // Bit that has been received. As we receive data in bits
    unsigned char recBit;
    // Position in receive buffer
    unsigned char recBuffPos;
    // Byte count in transfer as received from lower 4 bits of MK byte
    unsigned char recBytesCount;
    // Transfer state
    BeanTransferState recBeanState;
    
    // Do we await staffing bit next?
    unsigned char recIsNextBitStaffing : 1;
    
} BeanData;

// Receive BEAN data from BEAN bus
void recBean(BeanData *pBeanData, char bean, unsigned char cnt);


#ifdef	__cplusplus
}
#endif

#endif	/* BEAN_H */

