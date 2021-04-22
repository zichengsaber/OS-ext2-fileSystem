#ifndef _DISK_H_
#define _DISK_H_

#include "common.h"

typedef struct DISK_OPERATIONS{
    // 声明了两个函数指针
    // 读写扇区
    int (* read_sector) (struct DISK_OPERATIONS *,SECTOR,void *);
    int (* write_sector) (struct DISK_OPERATIONS *,SECTOR,const void *);

    SECTOR numberOfSectors; // 扇区数量
    int bytesPerSector; // 每一个扇区的字节数量
    void *pdata; // 指向heap上的指针

} DISK_OPERATIONS;


#endif

