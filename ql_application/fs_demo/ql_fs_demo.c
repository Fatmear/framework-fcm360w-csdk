#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ql_type.h"
#include "ql_debug.h"
#include "ql_api_osi.h"
//#include "ql_fs.h"
#include "ff.h"

ql_task_t fs_test_thread_handle = NULL;

void ql_fs_demo_thread(void *param)
{
#if 0
    QFILE fd = 0;
    
    fd = ql_fopen("UFS:1.txt", "w+");
     os_printf(LM_OS, LL_INFO,"test write fd:%d\n",fd);
    if(fd >= 0){
        ql_fwrite("1234567890",10, 1, fd);
        ql_fclose(fd);
    }

    os_printf(LM_OS, LL_INFO,"\"%s\" exist[%d]\n","UFS:1.txt", (ql_file_exist("UFS:1.txt") == 0)?true:false);

    fd = ql_fopen("UFS:1.txt","r");
    os_printf(LM_OS, LL_INFO,"test read fd:%d\n",fd);
    if(fd >= 0){
        char read_buf[10] = {0};
        int size = 0;
        size = ql_fsize(fd);
        os_printf(LM_OS, LL_INFO,"size:%d\n", size);
        ql_fread(read_buf,10, 1, fd);
        os_printf(LM_OS, LL_INFO,"read_buf:%s\n", read_buf);
        ql_fclose(fd);
    }
#else
    static FATFS fs;
    static char fs_buf[FF_MAX_SS] = {0};
    int fs_ret = 0;
    FIL    fd;
    unsigned int  rw_len = 0;
    char read_buf[10] = {0};

    os_printf(LM_OS, LL_INFO, "init fat fs...\n");
    memset((void *)&fs, 0x00, sizeof(FATFS));
    if((fs_ret = f_mount(&fs, "FLASH:/", 1)) != FR_OK){
        if((fs_ret = f_mkfs("FLASH:/",NULL, fs_buf, sizeof(fs_buf))) != FR_OK){
            os_printf(LM_OS, LL_INFO, "fail to init fat fs[%d]...\n", fs_ret);
        }else{
            os_printf(LM_OS, LL_INFO, "make fat fs successful...\n");
            f_mount(&fs, "FLASH:/", 1);
        }
    }else{
        os_printf(LM_OS, LL_INFO, "mount fat fs successful...\n");
    }

    fs_ret = f_open(&fd, "FLASH:/1.txt", FA_READ|FA_WRITE | FA_CREATE_ALWAYS);
	if(fs_ret != FR_OK ){
		goto end;
	}
    fs_ret = f_write(&fd,"1234567890",10,&rw_len);
    f_close(&fd);
    if(fs_ret != FR_OK){
        goto end;
    }
    fs_ret = f_open(&fd, "FLASH:/1.txt", FA_READ  | FA_OPEN_EXISTING);
	if(fs_ret != FR_OK ){
		goto end;
	}
    fs_ret = f_read(&fd,read_buf, 10, &rw_len);
    f_close(&fd);
    if(fs_ret != FR_OK){
        goto end;
    }
    os_printf(LM_OS, LL_INFO,"read_buf:%s\n", read_buf);
end:
    ql_rtos_task_delete(fs_test_thread_handle);
#endif
}


void ql_fs_demo_thread_creat(void)
{
    int ret;
    ret = ql_rtos_task_create(&fs_test_thread_handle,
                              (unsigned short)1024*10,
                              RTOS_HIGHER_PRIORTIY_THAN(3),
                              "fs_test",
                              ql_fs_demo_thread,
                              0);

    if (ret != QL_OSI_SUCCESS)
    {
        os_printf(LM_OS, LL_INFO, "Error: Failed to create fs test thread: %d\r\n", ret);
        goto init_err;
    }

    return;

init_err:
    if (fs_test_thread_handle != NULL)
    {
        ql_rtos_task_delete(fs_test_thread_handle);
    }
}