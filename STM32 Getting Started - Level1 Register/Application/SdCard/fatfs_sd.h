/*
 * fatfs_sd.h
 *
 *  Created on: Jul 2, 2020
 *      Author: meh
 *      Last Modified: MYaqoob, 8-10-2020
 */

#ifndef INC_FATFS_SD_H_
#define INC_FATFS_SD_H_

#ifndef __FATFS_SD_H
#define __FATFS_SD_H

#include "integer.h"
#include "diskio.h"
#include "main.h"

DSTATUS SD_disk_initialize (BYTE pdrv);
DSTATUS SD_disk_status (BYTE pdrv);
DRESULT SD_disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count);
DRESULT SD_disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count);
DRESULT SD_disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);

#endif


#endif /* INC_FATFS_SD_H_ */
