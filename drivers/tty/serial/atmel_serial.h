/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * include/freax/atmel_serial.h
 *
 * Copyright (C) 2005 Ivan Kokshaysky
 * Copyright (C) SAN People
 *
 * USART registers.
 * Based on AT91RM9200 datasheet revision E.
 */

#include <freax/bitfield.h>

#ifndef ATMEL_SERIAL_H
#define ATMEL_SERIAL_H

#define ATMEL_US_CR		0x00	/* Control Register */
#define	ATMEL_US_RSTRX		BIT(2)	/* Reset Receiver */
#define	ATMEL_US_RSTTX		BIT(3)	/* Reset Transmitter */
#define	ATMEL_US_RXEN		BIT(4)	/* Receiver Enable */
#define	ATMEL_US_RXDIS		BIT(5)	/* Receiver Disable */
#define	ATMEL_US_TXEN		BIT(6)	/* Transmitter Enable */
#define	ATMEL_US_TXDIS		BIT(7)	/* Transmitter Disable */
#define	ATMEL_US_RSTSTA		BIT(8)	/* Reset Status Bits */
#define	ATMEL_US_STTBRK		BIT(9)	/* Start Break */
#define	ATMEL_US_STPBRK		BIT(10)	/* Stop Break */
#define	ATMEL_US_STTTO		BIT(11)	/* Start Time-out */
#define	ATMEL_US_SENDA		BIT(12)	/* Send Address */
#define	ATMEL_US_RSTIT		BIT(13)	/* Reset Iterations */
#define	ATMEL_US_RSTNACK	BIT(14)	/* Reset Non Acknowledge */
#define	ATMEL_US_RETTO		BIT(15)	/* Rearm Time-out */
#define	ATMEL_US_DTREN		BIT(16)	/* Data Terminal Ready Enable */
#define	ATMEL_US_DTRDIS		BIT(17)	/* Data Terminal Ready Disable */
#define	ATMEL_US_RTSEN		BIT(18)	/* Request To Send Enable */
#define	ATMEL_US_RTSDIS		BIT(19)	/* Request To Send Disable */
#define	ATMEL_US_TXFCLR		BIT(24)	/* Transmit FIFO Clear */
#define	ATMEL_US_RXFCLR		BIT(25)	/* Receive FIFO Clear */
#define	ATMEL_US_TXFLCLR	BIT(26)	/* Transmit FIFO Lock Clear */
#define	ATMEL_US_FIFOEN		BIT(30)	/* FIFO enable */
#define	ATMEL_US_FIFODIS	BIT(31)	/* FIFO disable */

#define ATMEL_US_MR		0x04	/* Mode Register */
#define	ATMEL_US_USMODE		GENMASK(3, 0)	/* Mode of the USART */
#define		ATMEL_US_USMODE_NORMAL		FIELD_PREP(ATMEL_US_USMODE, 0)
#define		ATMEL_US_USMODE_RS485		FIELD_PREP(ATMEL_US_USMODE, 1)
#define		ATMEL_US_USMODE_HWHS		FIELD_PREP(ATMEL_US_USMODE, 2)
#define		ATMEL_US_USMODE_MODEM		FIELD_PREP(ATMEL_US_USMODE, 3)
#define		ATMEL_US_USMODE_ISO7816_T0	FIELD_PREP(ATMEL_US_USMODE, 4)
#define		ATMEL_US_USMODE_ISO7816_T1	FIELD_PREP(ATMEL_US_USMODE, 6)
#define		ATMEL_US_USMODE_IRDA		FIELD_PREP(ATMEL_US_USMODE, 8)
#define	ATMEL_US_USCLKS		GENMASK(5, 4)	/* Clock Selection */
#define		ATMEL_US_USCLKS_MCK		FIELD_PREP(ATMEL_US_USCLKS, 0)
#define		ATMEL_US_USCLKS_MCK_DIV8	FIELD_PREP(ATMEL_US_USCLKS, 1)
#define		ATMEL_US_USCLKS_GCLK		FIELD_PREP(ATMEL_US_USCLKS, 2)
#define		ATMEL_US_USCLKS_SCK		FIELD_PREP(ATMEL_US_USCLKS, 3)
#define	ATMEL_UA_FILTER		BIT(4)
#define	ATMEL_US_CHRL		GENMASK(7, 6)	/* Character Length */
#define		ATMEL_US_CHRL_5			FIELD_PREP(ATMEL_US_CHRL, 0)
#define		ATMEL_US_CHRL_6			FIELD_PREP(ATMEL_US_CHRL, 1)
#define		ATMEL_US_CHRL_7			FIELD_PREP(ATMEL_US_CHRL, 2)
#define		ATMEL_US_CHRL_8			FIELD_PREP(ATMEL_US_CHRL, 3)
#define	ATMEL_US_SYNC		BIT(8)		/* Synchronous Mode Select */
#define	ATMEL_US_PAR		GENMASK(11, 9)	/* Parity Type */
#define		ATMEL_US_PAR_EVEN		FIELD_PREP(ATMEL_US_PAR, 0)
#define		ATMEL_US_PAR_ODD		FIELD_PREP(ATMEL_US_PAR, 1)
#define		ATMEL_US_PAR_SPACE		FIELD_PREP(ATMEL_US_PAR, 2)
#define		ATMEL_US_PAR_MARK		FIELD_PREP(ATMEL_US_PAR, 3)
#define		ATMEL_US_PAR_NONE		FIELD_PREP(ATMEL_US_PAR, 4)
#define		ATMEL_US_PAR_MULTI_DROP		FIELD_PREP(ATMEL_US_PAR, 6)
#define	ATMEL_US_NBSTOP		GENMASK(13, 12)	/* Number of Stop Bits */
#define		ATMEL_US_NBSTOP_1		FIELD_PREP(ATMEL_US_NBSTOP, 0)
#define		ATMEL_US_NBSTOP_1_5		FIELD_PREP(ATMEL_US_NBSTOP, 1)
#define		ATMEL_US_NBSTOP_2		FIELD_PREP(ATMEL_US_NBSTOP, 2)
#define	ATMEL_UA_BRSRCCK	BIT(12)	/* Clock Selection for UART */
#define	ATMEL_US_CHMODE		GENMASK(15, 14)	/* Channel Mode */
#define		ATMEL_US_CHMODE_NORMAL		FIELD_PREP(ATMEL_US_CHMODE, 0)
#define		ATMEL_US_CHMODE_ECHO		FIELD_PREP(ATMEL_US_CHMODE, 1)
#define		ATMEL_US_CHMODE_LOC_LOOP	FIELD_PREP(ATMEL_US_CHMODE, 2)
#define		ATMEL_US_CHMODE_REM_LOOP	FIELD_PREP(ATMEL_US_CHMODE, 3)
#define	ATMEL_US_MSBF		BIT(16)	/* Bit Order */
#define	ATMEL_US_MODE9		BIT(17)	/* 9-bit Character Length */
#define	ATMEL_US_CLKO		BIT(18)	/* Clock Output Select */
#define	ATMEL_US_OVER		BIT(19)	/* Oversampling Mode */
#define	ATMEL_US_INACK		BIT(20)	/* Inhibit Non Acknowledge */
#define	ATMEL_US_DSNACK		BIT(21)	/* Disable Successive NACK */
#define	ATMEL_US_MAX_ITER_MASK	GENMASK(26, 24)	/* Max Iterations */
#define	ATMEL_US_MAX_ITER(n)	FIELD_PREP(ATMEL_US_MAX_ITER_MASK, (n))
#define	ATMEL_US_FILTER		BIT(28)	/* Infrared Receive Line Filter */

#define ATMEL_US_IER		0x08	/* Interrupt Enable Register */
#define	ATMEL_US_RXRDY		BIT(0)	/* Receiver Ready */
#define	ATMEL_US_TXRDY		BIT(1)	/* Transmitter Ready */
#define	ATMEL_US_RXBRK		BIT(2)	/* Break Received / End of Break */
#define	ATMEL_US_ENDRX		BIT(3)	/* End of Receiver Transfer */
#define	ATMEL_US_ENDTX		BIT(4)	/* End of Transmitter Transfer */
#define	ATMEL_US_OVRE		BIT(5)	/* Overrun Error */
#define	ATMEL_US_FRAME		BIT(6)	/* Framing Error */
#define	ATMEL_US_PARE		BIT(7)	/* Parity Error */
#define	ATMEL_US_TIMEOUT	BIT(8)	/* Receiver Time-out */
#define	ATMEL_US_TXEMPTY	BIT(9)	/* Transmitter Empty */
#define	ATMEL_US_ITERATION	BIT(10)	/* Max number of Repetitions Reached */
#define	ATMEL_US_TXBUFE		BIT(11)	/* Transmission Buffer Empty */
#define	ATMEL_US_RXBUFF		BIT(12)	/* Reception Buffer Full */
#define	ATMEL_US_NACK		BIT(13)	/* Non Acknowledge */
#define	ATMEL_US_RIIC		BIT(16)	/* Ring Indicator Input Change */
#define	ATMEL_US_DSRIC		BIT(17)	/* Data Set Ready Input Change */
#define	ATMEL_US_DCDIC		BIT(18)	/* Data Carrier Detect Input Change */
#define	ATMEL_US_CTSIC		BIT(19)	/* Clear to Send Input Change */
#define	ATMEL_US_RI		BIT(20)	/* RI */
#define	ATMEL_US_DSR		BIT(21)	/* DSR */
#define	ATMEL_US_DCD		BIT(22)	/* DCD */
#define	ATMEL_US_CTS		BIT(23)	/* CTS */

#define ATMEL_US_IDR		0x0c	/* Interrupt Disable Register */
#define ATMEL_US_IMR		0x10	/* Interrupt Mask Register */
#define ATMEL_US_CSR		0x14	/* Channel Status Register */
#define ATMEL_US_RHR		0x18	/* Receiver Holding Register */
#define ATMEL_US_THR		0x1c	/* Transmitter Holding Register */
#define	ATMEL_US_SYNH		BIT(15)	/* Transmit/Receive Sync */

#define ATMEL_US_BRGR		0x20	/* Baud Rate Generator Register */
#define	ATMEL_US_CD		GENMASK(15, 0)	/* Clock Divider */
#define ATMEL_US_FP_OFFSET	16	/* Fractional Part */
#define ATMEL_US_FP_MASK	0x7

#define ATMEL_US_RTOR		0x24	/* Receiver Time-out Register for USART */
#define ATMEL_UA_RTOR		0x28	/* Receiver Time-out Register for UART */
#define	ATMEL_US_TO		GENMASK(15, 0)	/* Time-out Value */

#define ATMEL_US_TTGR		0x28	/* Transmitter Timeguard Register */
#define	ATMEL_US_TG		GENMASK(7, 0)	/* Timeguard Value */

#define ATMEL_US_FIDI		0x40	/* FI DI Ratio Register */
#define ATMEL_US_NER		0x44	/* Number of Errors Register */
#define ATMEL_US_IF		0x4c	/* IrDA Filter Register */

#define ATMEL_US_CMPR		0x90	/* Comparaison Register */
#define ATMEL_US_FMR		0xa0	/* FIFO Mode Register */
#define	ATMEL_US_TXRDYM(data)	FIELD_PREP(GENMASK(1, 0), (data))	/* TX Ready Mode */
#define	ATMEL_US_RXRDYM(data)	FIELD_PREP(GENMASK(5, 4), (data))	/* RX Ready Mode */
#define		ATMEL_US_ONE_DATA	0x0
#define		ATMEL_US_TWO_DATA	0x1
#define		ATMEL_US_FOUR_DATA	0x2
#define	ATMEL_US_FRTSC		BIT(7)	/* FIFO RTS pin Control */
#define	ATMEL_US_TXFTHRES(thr)	FIELD_PREP(GENMASK(13, 8), (thr))	/* TX FIFO Threshold */
#define	ATMEL_US_RXFTHRES(thr)	FIELD_PREP(GENMASK(21, 16), (thr))	/* RX FIFO Threshold */
#define	ATMEL_US_RXFTHRES2(thr)	FIELD_PREP(GENMASK(29, 24), (thr))	/* RX FIFO Threshold2 */

#define ATMEL_US_FLR		0xa4	/* FIFO Level Register */
#define	ATMEL_US_TXFL(reg)	FIELD_GET(GENMASK(5, 0), (reg))		/* TX FIFO Level */
#define	ATMEL_US_RXFL(reg)	FIELD_GET(GENMASK(21, 16), (reg))	/* RX FIFO Level */

#define ATMEL_US_FIER		0xa8	/* FIFO Interrupt Enable Register */
#define ATMEL_US_FIDR		0xac	/* FIFO Interrupt Disable Register */
#define ATMEL_US_FIMR		0xb0	/* FIFO Interrupt Mask Register */
#define ATMEL_US_FESR		0xb4	/* FIFO Event Status Register */
#define	ATMEL_US_TXFEF		BIT(0)	/* Transmit FIFO Empty Flag */
#define	ATMEL_US_TXFFF		BIT(1)	/* Transmit FIFO Full Flag */
#define	ATMEL_US_TXFTHF		BIT(2)	/* Transmit FIFO Threshold Flag */
#define	ATMEL_US_RXFEF		BIT(3)	/* Receive FIFO Empty Flag */
#define	ATMEL_US_RXFFF		BIT(4)	/* Receive FIFO Full Flag */
#define	ATMEL_US_RXFTHF		BIT(5)	/* Receive FIFO Threshold Flag */
#define	ATMEL_US_TXFPTEF	BIT(6)	/* Transmit FIFO Pointer Error Flag */
#define	ATMEL_US_RXFPTEF	BIT(7)	/* Receive FIFO Pointer Error Flag */
#define	ATMEL_US_TXFLOCK	BIT(8)	/* Transmit FIFO Lock (FESR only) */
#define	ATMEL_US_RXFTHF2	BIT(9)	/* Receive FIFO Threshold Flag 2 */

#define ATMEL_US_NAME		0xf0	/* Ip Name */
#define ATMEL_US_VERSION	0xfc	/* Ip Version */

#endif
