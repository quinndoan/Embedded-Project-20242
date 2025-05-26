/*
 * mmcsd.h
 *
 *  Created on: May 19, 2025
 *      Author: Nitro Tiger
 */

#ifndef __MMCSD_H
#define __MMCSD_H
#include "diskio.h"
//#include <stdint.h>
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
///* If DRESULT is already defined (e.g. by fatfs.h), don't redefine it */
//#ifndef DRESULT
//typedef enum {
//	RES_OK = 0,		/* 0: Successful */
//	RES_ERROR,		/* 1: R/W Error */
//	RES_WRPRT,		/* 2: Write Protected */
//	RES_NOTRDY,		/* 3: Not Ready */
//	RES_PARERR		/* 4: Invalid Parameter */
//} DRESULT;
//#endif

// Khởi tạo thẻ SD
// Đọc/ghi dữ liệu
// Điều khiển và quản lý trạng thái thẻ



/* Generic command (Used by FatFs) */
#define CTRL_SYNC		0	/* Complete pending write process (needed at _FS_READONLY == 0) */
#define GET_SECTOR_COUNT	1	/* Get media size (needed at _USE_MKFS == 1) */
#define GET_SECTOR_SIZE		2	/* Get sector size (needed at _MAX_SS != _MIN_SS) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (needed at _USE_MKFS == 1) */
#define CTRL_TRIM		4	/* Inform device that the data on the block of sectors is no longer used (needed at _USE_TRIM == 1) */


uint8_t xchg_spi (
	uint8_t dat	/* Data to send */
);
void rcvr_spi_multi (
	uint8_t *buff,		/* Pointer to data buffer */
	uint32_t btr		/* Number of bytes to receive (even number) */
);
void xmit_spi_multi (
	const uint8_t *buff,	/* Pointer to the data */
	uint32_t btx			/* Number of bytes to send (even number) */
);
int wait_ready (	/* 1:Ready, 0:Timeout */
	uint32_t wt			/* Timeout [ms] */
);
void despiselect (void);
int spiselect (void);
int rcvr_datablock (	/* 1:OK, 0:Error */
	uint8_t *buff,			/* Data buffer */
	uint32_t btr			/* Data block length (byte) */
);
int xmit_datablock (	/* 1:OK, 0:Failed */
	const uint8_t *buff,	/* Ponter to 512 byte data to be sent */
	uint8_t token			/* Token */
);

uint8_t USER_SPI_initialize (
	uint8_t drv		/* Physical drive number (0) */
);

uint8_t USER_SPI_status (
	uint8_t drv		/* Physical drive number (0) */
);
DRESULT USER_SPI_write (
	uint8_t drv,			/* Physical drive number (0) */
	const uint8_t *buff,	/* Ponter to the data to write */
	uint32_t sector,		/* Start sector number (LBA) */
	uint32_t count			/* Number of sectors to write (1..128) */
);
DRESULT USER_SPI_read (
	uint8_t drv,		/* Physical drive number (0) */
	uint8_t *buff,		/* Pointer to the data buffer to store read data */
	uint32_t sector,	/* Start sector number (LBA) */
	uint32_t count		/* Number of sectors to read (1..128) */
);
DRESULT USER_SPI_ioctl (
	uint8_t drv,		/* Physical drive number (0) */
	uint8_t cmd,		/* Control command code */
	void *buff		/* Pointer to the conrtol data */
);

#ifdef __cplusplus
}
#endif

#endif /* __MMCSD_H */
