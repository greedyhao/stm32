#ifndef __LOG_H
#define __LOG_H

#define LOG_ERROR(error_code) do{ \
    switch(error_code) \
    { \
      case FR_OK:				/* (0) Succeeded */  \
        printf("FR_OK\n");break; \
      case FR_DISK_ERR:			/* (1) A hard error occurred in the low level disk I/O layer */  \
        printf("FR_DISK_ERR\n");break; \
      case FR_INT_ERR:				/* (2) Assertion failed */  \
        printf("FR_INT_ERR\n");break; \
      case FR_NOT_READY:			/* (3) The physical drive cannot work */  \
        printf("FR_NOT_READY\n");break; \
      case FR_NO_FILE:				/* (4) Could not find the file */  \
        printf("FR_NO_FILE\n");break; \
      case FR_NO_PATH:				/* (5) Could not find the path */  \
        printf("FR_NO_PATH\n");break; \
      case FR_INVALID_NAME:		/* (6) The path name format is invalid */  \
        printf("FR_INVALID_NAME\n");break; \
      case FR_DENIED:				/* (7) Access denied due to prohibited access or directory full */  \
        printf("FR_DENIED\n");break; \
      case FR_EXIST:			/* (8) Access denied due to prohibited access */  \
        printf("FR_EXIST£º\n");break; \
      case FR_INVALID_OBJECT:		/* (9) The file/directory object is invalid */  \
        printf("FR_INVALID_OBJECT\n");break; \
      case FR_WRITE_PROTECTED:		/* (10) The physical drive is write protected */  \
        printf("FR_WRITE_PROTECTED\n");break; \
      case FR_INVALID_DRIVE:		/* (11) The logical drive number is invalid */  \
        printf("FR_INVALID_DRIVE\n");break; \
      case FR_NOT_ENABLED:			/* (12) The volume has no work area */  \
        printf("FR_NOT_ENABLED\n");break; \
      case FR_NO_FILESYSTEM:		/* (13) There is no valid FAT volume */  \
        printf("FR_NO_FILESYSTEM\n");break; \
      case FR_MKFS_ABORTED:		/* (14) The f_mkfs() aborted due to any problem */  \
        printf("FR_MKFS_ABORTED\n");break; \
      case FR_TIMEOUT:				/* (15) Could not get a grant to access the volume within defined period */  \
        printf("FR_TIMEOUT\n");break; \
      case FR_LOCKED:				/* (16) The operation is rejected according to the file sharing policy */  \
        printf("FR_LOCKED\n");break; \
      case FR_NOT_ENOUGH_CORE:		/* (17) LFN working buffer could not be allocated */  \
        printf("FR_NOT_ENOUGH_CORE\n");break; \
      case FR_TOO_MANY_OPEN_FILES:	/* (18) Number of open files > _FS_LOCK */  \
        printf("FR_TOO_MANY_OPEN_FILES\n");break; \
      case FR_INVALID_PARAMETER:	/* (19) Given parameter is invalid */  \
        printf("FR_INVALID_PARAMETER\n");break; \
    } \
    printf("<<-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__); \
 }while(0)

#endif
 
 
 