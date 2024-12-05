/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "sdcard.h"
#include "cli.h"
#include "oshal.h"
#include "flash.h"
#include "easyflash.h"
#include "hal_flash.h"
#include "hal_rtc.h"

#if defined(QL_OCPU_SUPPORT) || defined(QL_AT_SUPPORT)
#define FLASH_FS_START_ADDR			0x231000//0x20000 //128k for bin file
//#define FLASH_FS_SIZE				0x32000//0X4000 //last 16k for factory info
#define FLASH_FS_SIZE				0x100000//0X4000 //last 16k for factory info

#endif
/* Definitions of physical drive number for each drive */

#if defined(QL_OCPU_SUPPORT) || defined(QL_AT_SUPPORT)
#define DEV_MMC		0	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		1	/* Example: Map USB MSD to physical drive 2 */
#define DEV_FLASH   2
#else
#define DEV_RAM		1	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		0	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */
#endif

#if defined(QL_OCPU_SUPPORT) || defined(QL_AT_SUPPORT)
static int is_dev_inited[3] = {0,0,0};
#endif

DWORD get_fattime(void)
{
    return hal_rtc_get_time();
}
/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat=RES_OK;
#if defined(QL_OCPU_SUPPORT) || defined(QL_AT_SUPPORT)	
	int result;
#endif

	switch (pdrv) {
#if !defined(QL_OCPU_SUPPORT) && !defined(QL_AT_SUPPORT)		
	case DEV_RAM :
		//result = RAM_disk_status();

		// translate the reslut code here

		return stat;
#endif
	case DEV_MMC :
		stat = sdcard_status();

		// translate the reslut code here

		return stat;

	case DEV_USB :
		//result = USB_disk_status();

		// translate the reslut code here

		return stat;
#if defined(QL_OCPU_SUPPORT) || defined(QL_AT_SUPPORT)
	case DEV_FLASH:
		result = is_dev_inited[DEV_FLASH];
		// translate the reslut code here
		stat = result ? 0 : STA_NOINIT;
		return stat;
#endif
	}

	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
#if defined(QL_OCPU_SUPPORT) || defined(QL_AT_SUPPORT)
	DSTATUS stat;
#endif
	int result;

	switch (pdrv) {
#if !defined(QL_OCPU_SUPPORT) && !defined(QL_AT_SUPPORT)		
	case DEV_RAM :
		//result = RAM_disk_initialize();

		// translate the reslut code here

		return RES_OK;
#endif
	case DEV_MMC :
		result = sdcard_init();

		// translate the reslut code here
		if(result)
			{
			if(result >0)
				return result;
			else
				return RES_NOTRDY;
			}
		else
			{
			return RES_OK;
			}

	case DEV_USB :
		//result = USB_disk_initialize();

		// translate the reslut code here

		return RES_OK;
#if defined(QL_OCPU_SUPPORT) || defined(QL_AT_SUPPORT)
	case DEV_FLASH:
		result = 0;
		// translate the reslut code here
		if (result) {
			stat = STA_NODISK;
		} else {
			is_dev_inited[DEV_FLASH] = 1;
			stat = 0;
		}
		return stat;
#endif		
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res=RES_OK;
#if defined(QL_OCPU_SUPPORT) || defined(QL_AT_SUPPORT)
	int result;
#endif

	switch (pdrv) {
#if !defined(QL_OCPU_SUPPORT) && !defined(QL_AT_SUPPORT)		
	case DEV_RAM :
		// translate the arguments here

		//result = RAM_disk_read(buff, sector, count);

		// translate the reslut code here
		//res = sdcard_read(buff, sector, count);

		return res;
#endif
	case DEV_MMC :
		// translate the arguments here

		res = sdcard_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
#if defined(QL_OCPU_SUPPORT) || defined(QL_AT_SUPPORT)
	case DEV_FLASH:
	{
		unsigned int offset = sector << FLASH_OFFSET;
		unsigned int read_addr = FLASH_FS_START_ADDR + offset;
		//os_printf(LM_OS, LL_INFO,"%s, flash read sector:%d, count:%d\n",__func__, sector, count);
		result = hal_flash_read(read_addr, buff, count<<FLASH_OFFSET);
		//os_printf(LM_OS, LL_INFO,"%s, flash read ret:%d\n",__func__, result);
		// translate the reslut code here
		if (result == 0) {
			res = RES_OK;
		} else {
			res = RES_ERROR;
		}

		return res;
	}
#endif		
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

static BYTE flash_write_buf[8192 + 4096] = {0};

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res=RES_OK;
	int result;

	switch (pdrv) {
#if !defined(QL_OCPU_SUPPORT) && !defined(QL_AT_SUPPORT)		
	case DEV_RAM :
		// translate the arguments here

		//result = RAM_disk_write(buff, sector, count);

		// translate the reslut code here
		//result = sdcard_write(buff, sector, count);
		//if(result == 0)
		//	res = RES_OK;

		return res;
#endif
	case DEV_MMC :
		// translate the arguments here

		result = sdcard_write(buff, sector, count);
		if(result == 0)
			res = RES_OK;

		// translate the reslut code here

		return res;

	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
#if defined(QL_OCPU_SUPPORT) || defined(QL_AT_SUPPORT)
	case DEV_FLASH:
	{
		//unsigned int cal_offset = 0;
	#if 0
		unsigned int offset = sector<< FLASH_OFFSET;
		unsigned int write_addr = FLASH_FS_START_ADDR + offset;

		//os_printf(LM_OS, LL_INFO,"%s, flash write sector:%d, count:%d\n",__func__, sector, count);
		hal_flash_erase(write_addr, count<< FLASH_OFFSET);
		result = hal_flash_write(write_addr, (unsigned char *)buff, count<< FLASH_OFFSET);
        //os_printf(LM_OS, LL_INFO,"%s, flash write ret:%d\n",__func__, result);
		if (result == 0) {
			res = RES_OK;
		} else {
			//os_printf(LM_OS, LL_INFO, "%s flash write err %d\n", __func__, result);
			res = RES_ERROR;
		}
		// translate the reslut code here
	#else
	    const int erase_cnt = (4096/FF_MAX_SS);
		unsigned int cal_offset = 0;
		unsigned int cal_sector = 0;
		uint8_t read_count = 0;
		if (sector % erase_cnt) {
			cal_sector = sector - (sector % erase_cnt);
			cal_offset = cal_sector << FLASH_OFFSET;
			read_count = erase_cnt;
		} else {
			cal_sector = sector;
			cal_offset = cal_sector << FLASH_OFFSET;
		}

		unsigned int cal_write_addr = FLASH_FS_START_ADDR + cal_offset;		
		unsigned int write_offset = (sector - cal_sector) << FLASH_OFFSET;
		//unsigned int offset = sector << FLASH_OFFSET;
		//unsigned int write_addr = FLASH_FS_START_ADDR + offset;
		memset(flash_write_buf, 0, sizeof(flash_write_buf));
		if (count < erase_cnt) {
			read_count = erase_cnt;
		} else if (count <= erase_cnt*2) {
			read_count += erase_cnt*2;
		} else {
			//os_printf(LM_OS, LL_INFO, "!!!! %s count err %d\n", __func__, count);
			read_count += erase_cnt*2;
		}

		result = hal_flash_read(cal_write_addr, flash_write_buf, read_count<<FLASH_OFFSET);
		memcpy(flash_write_buf + write_offset , buff, count<<FLASH_OFFSET);

		hal_flash_erase(cal_write_addr, read_count<<FLASH_OFFSET);
		result = hal_flash_write(cal_write_addr, flash_write_buf, read_count<<FLASH_OFFSET);

		if (result == 0) {
			res = RES_OK;
		} else {
			//os_printf(LM_OS, LL_INFO,"%s flash write err %d\n", __func__, result);
			res = RES_ERROR;
		}
	#endif
		return res;
	}
#endif
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res=RES_OK;
	//int result;

	switch (pdrv) {
#if !defined(QL_OCPU_SUPPORT) && !defined(QL_AT_SUPPORT)		
	case DEV_RAM :

		// Process of the command for the RAM drive

		return res;
#endif
	case DEV_MMC :

		// Process of the command for the MMC/SD card
		if(CTRL_SYNC==cmd)
			{
			res = 0;
			}
		else if(GET_SECTOR_COUNT==cmd)
			{
			*(unsigned int*)buff = sdcard_get_sector_count();
			res = 0;
			}
		else if(GET_SECTOR_SIZE==cmd)
			{
			res = 0;
			}
		else if(GET_BLOCK_SIZE==cmd)
			{
			*(unsigned int*)buff = sdcard_get_blk_size();
			res = 0;
			}
		else if(CTRL_TRIM==cmd)
			{
			res = 0;
			}
		else
			{
			res = 0;
			}
		
		return res;

	case DEV_USB :

		// Process of the command the USB drive

		return res;
#if defined(QL_OCPU_SUPPORT) || defined(QL_AT_SUPPORT)
	case DEV_FLASH:
		switch (cmd) {
		case CTRL_SYNC:
			res = RES_OK;
			break;
		case GET_SECTOR_COUNT: {
			DWORD flash_size = 0;
			flash_size = FLASH_FS_SIZE;
			*((DWORD *)buff) = flash_size / FF_MAX_SS; // default when not known
			res = RES_OK;
			break;
		}
		case GET_SECTOR_SIZE:
			*((DWORD *)buff) = FF_MAX_SS; // default when not known
			res = RES_OK;
			break;
		case GET_BLOCK_SIZE:
			*((DWORD *)buff) = 1; // default when not known
			res = RES_OK;
			break;
		default:
			res = RES_PARERR;
		break;
		}
		//os_printf(LM_OS, LL_INFO, "%s ret %d\n", __func__, res);
		return res;
#endif
	}

	return RES_PARERR;
}

