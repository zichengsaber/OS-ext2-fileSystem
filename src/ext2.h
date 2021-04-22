#ifndef _FAT_H_
#define _FAT_H_

#include "common.h"
#include "disk.h"
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>

#define SHELL_BUFFER_SIZE       13 // shell 缓冲区的大小

#define TOTAL_DISK_SIZE         (268435456 + MAX_BLOCK_SIZE) // 需要加一个Boot block 的位置
#define MAX_SECTOR_SIZE         512 //扇区是最小的物理存储单元,每个扇区为512byte
#define MAX_BLOCK_SIZE          (MAX_SECTOR_SIZE*2) // 1024 ext2 文件系统支持的 Block 的大小有 1024 字节、2048 字节和 4096 字节，如果要存储的文件大多数是几 KB 的小文件，那么选择 1 KB 的块尺寸能提高磁盘存储空间的利用率

#define MAX_NAME_LENGTH         256
#define MAX_ENTRY_NAME_LENGTH   11

// 两种不同类型的BLOCKS
#define EXT2_D_BLOCKS			12 // 直接寻址得到
#define EXT2_N_BLOCKS         	15

#define MAX_BLOCK_GROUP_SIZE	(8 * MAX_BLOCK_SIZE * MAX_BLOCK_SIZE) // 定义block group 的最大大小

#define NUMBER_OF_SECTORS       (TOTAL_DISK_SIZE/MAX_SECTOR_SIZE) // 扇区的总数
#define NUMBER_OF_GROUPS        ((TOTAL_DISK_SIZE-MAX_BLOCK_SIZE)/MAX_BLOCK_GROUP_SIZE) /* 块组的个数,注意计算的时候要减去boot block */
#define NUMBER_OF_BLOCK         (TOTAL_DISK_SIZE/MAX_BLOCK_SIZE) // 块的个数
#define NUMBER_OF_INODES        (NUMBER_OF_BLOCK/2) //一般block数量不小于inode个数，一般为1，2，4倍

#define INODE_SIZE              128 //每个 inode 大小均固定为 128 bytes

// 组块中的成员个数
#define BLOCKS_PER_GROUP        MAX_BLOCK_GROUP_SIZE / MAX_BLOCK_SIZE // 每个块组中的块的数量
#define INODES_PER_GROUP        (NUMBER_OF_INODES/NUMBER_OF_GROUPS)
// 块中的成员个数
#define SECTORS_PER_BLOCK       (MAX_BLOCK_SIZE/MAX_SECTOR_SIZE) 
#define INODES_PER_BLOCK        (MAX_BLOCK_SIZE/INODE_SIZE)
#define ENTRY_PER_BLOCK         (MAX_BLOCK_SIZE/sizeof(EXT2_DIR_ENTRY)) 

#define GROUP_SIZE              (BLOCKS_PER_GROUP*MAX_BLOCK_SIZE) //块组的的大小


#define USER_PERMITION          0x1FF
#define DIRECTORY_TYPE          0x4000
#define FILE_TYPE               0x2000

#define VOLUME_LABLE            "EXT2 BY zicheng" // shell 中的显示名称


// 文件的属性
#define ATTR_READ_ONLY          0x01
#define ATTR_HIDDEN             0x02
#define ATTR_SYSTEM             0x04
#define ATTR_VOLUME_ID          0x08
#define ATTR_DIRECTORY          0x10
#define ATTR_ARCHIVE            0x20
#define ATTR_LONG_NAME          ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID //按位与为 0xf

// 对应的ascii
#define DIR_ENTRY_FREE          0x01  // start of heading
#define DIR_ENTRY_NO_MORE       0x00 // null
#define DIR_ENTRY_OVERWRITE     1

#define RESERVED_INODE_NUM      11
#define NUM_OF_ROOT_INODE       2

#define SPACE_CHAR              0x20 // 空格

// 组块各个分段的基址(块号的偏移)
#define BOOT_BLOCK_BASE         0 // boot_block 的基址
#define SUPER_BLOCK_BASE        (BOOT_BLOCK_BASE+1)
#define GDT_BASE                (SUPER_BLOCK_BASE+1) // group descriptor table
#define BLOCK_NUM_GDT_PER_GROUP (((32*NUMBER_OF_GROUPS)+(MAX_BLOCK_SIZE-1))/MAX_BLOCK_SIZE) // 我们定义的EXT2_GROUP_DESCRIPTOR 32字节
#define BLOCK_BITMAP_BASE       (GDT_BASE+BLOCK_NUM_GDT_PER_GROUP) // block bitmap 可以知道哪些 block 是空的
#define INODE_BITMAP_BASE		(BLOCK_BITMAP_BASE + 1) //inode bitmap 则记录的是使用与未使用的 inode 号
#define INODE_TABLE_BASE		(INODE_BITMAP_BASE + 1)
#define BLOCK_NUM_IT_PER_GROUP  (((128*INODES_PER_GROUP)+(MAX_BLOCK_SIZE-1))/MAX_BLOCK_SIZE)
#define DATA_BLOCK_BASE         (INODE_TABLE_BASE+BLOCK_NUM_IT_PER_GROUP)

#define GET_INODE_GROUP(x) 		((x)-1)/( INODES_PER_GROUP )
#define GET_DATA_GROUP(x)		((x)-1)/(BLOCKS_PER_GROUP)
#define SQRT(x)  				((x)*(x))
#define TRI_SQRT(x)  			((x)*(x)*(x))
#define WHICH_GROUP_BLONG(x) 	( ( (x) - 1)  / INODES_PER_GROUP)

#define TSQRT(x) 				((x)*(x)*(x))
#define GET_INODE_FROM_NODE(X)  ((x)->entry.inode)

#ifdef _WIN32
#pragma pack(push,fatstructures)
#endif
#pragma pack(1) // 改变c编译器的字节对齐方式


// 超级块
// inode 记录文件属性
// block 记录文件内容
typedef struct{
    UINT32 max_inode_count; //inode count
    UINT32 block_count; // 
    UINT32 reserved_block_count; // 保留没有使用的文件块数
    UINT32 free_block_count; // 可用的文件块数
    UINT32 free_inode_count; 
    UINT32 first_data_block; // 第一个数据块的索引
    UINT32 log_block_size; //数据块的大小
    UINT32 log_fragmentation_size; // 碎片文件大小
    UINT32 block_per_group; //每一组的块数
    UINT32 fragmentation_per_group; // 每一组的碎片数目
    UINT32 inode_per_group; //每一组节点总数
    UINT32 mtime; // 最近被安装到内存的时间
    UINT32 wtime; // 最近被修改的时间
    UINT16 mount_count; // 最近一次文件系统查后被安装的次数
    UINT16 max_mount_count; // 最大可以安装的次数
    UINT16 magic_signature; //  文件系统的标识
    UINT16 state; // 文件系统的状态
    UINT16 errors; /*Behaviour when detecting errors */
    UINT16 minor_version;
	UINT32 last_consistency_check_time;
	UINT32 check_interval; /* max. time between checks */
	UINT32 creator_os;
	UINT16 UID_that_can_use_reserved_blocks;
	UINT16 GID_that_can_use_reserved_blocks;

	UINT32 first_non_reserved_inode;
	UINT16 inode_structure_size;
	UINT16 block_group_number;
	UINT32 compatible_feature_flags;
	UINT32 incompatible_feature_flags;
	UINT32 read_only_feature_flags;
	UINT32 UUID[4];
	UINT32 volume_name[4];
	UINT32 last_mounted_path[16];
	UINT32 algorithm_usage_bitmap;
	UINT8 preallocated_blocks_count;
	UINT8 preallocated_dir_blocks_count;
	BYTE padding[2];
	UINT32 journal_UUID[4];
	UINT32 journal_inode_number;
	UINT32 journal_device;
	UINT32 orphan_inode_list;
	UINT32 hash_seed[4];
	UINT8 defined_hash_version;
	BYTE padding1;
	BYTE padding2[2];
	UINT32 default_mount_option;
	UINT32 first_data_block_each_group;
	BYTE reserved[760];
} EXT2_SUPER_BLOCK;
// 32bytes
typedef struct
{
	UINT32 start_block_of_block_bitmap; //存放 block_bitmap所在block的索引
	UINT32 start_block_of_inode_bitmap; //存放文件节点位图的块的索引
	UINT32 start_block_of_inode_table; //文件节点表在外存中的第一块的索引
	UINT16 free_blocks_count; // 可用的文件块数
	UINT16 free_inodes_count; // 可用的文件节点数
	UINT16 directories_count; // 使用中的目录数
	BYTE padding[2]; // 用于32-bit地址的对齐
	BYTE reserved[12];
} EXT2_GROUP_DESCRIPTOR;


typedef struct
{
	UINT16  mode; // 文件类型
	UINT16  uid; // Low 16 bits of Owner Uid
	UINT32  size; // size in bytes
	UINT32  atime; // 访问时间
	UINT32  ctime; //创建时间
	UINT32  mtime; //修改时间
	UINT32  dtime; // 删除时间
	UINT16  gid; //Low 16 bits of Group id
	UINT16  links_count; // Links count
	UINT32  blocks; // 文件所占用的块
	UINT32  flags; // File flags
	UINT32  i_reserved1;
	UINT32  block[EXT2_N_BLOCKS]; // 存放的文件可能占有多个block
	UINT32  generation; // File version
	UINT32  file_acl; // File ACL
	UINT32  dir_acl; // Directory ACL
	UINT32  faddr; // Fragment address
	UINT32  i_reserved2[3];
} INODE;

// 文件目录 32byte
typedef struct{
	UINT32 inode; // inode number 4byte 记录下inode的编号，供在Inode Table中查找，通过该项可以获得文件信息
	char name[MAX_ENTRY_NAME_LENGTH]; //File name 11 byte
	UINT32 name_len; // 4byte
	BYTE pad[13]; // 13 byte 填充对齐
} EXT2_DIR_ENTRY;

// 组描述表
typedef struct{
	EXT2_GROUP_DESCRIPTOR group_descriptor[NUMBER_OF_GROUPS];
}EXT2_GROUP_DESCRIPTOR_TABLE;

// Inode table for each group
typedef struct{
	INODE inode[INODES_PER_GROUP];
}EXT2_INODE_TABLE;


// inode_bitmap
typedef struct{
	BYTE block_bitmap[MAX_BLOCK_SIZE]; // block块位使用
}EXT2_BLOCK_BITMAP;

typedef struct
{
	BYTE inode_bitmap[MAX_BLOCK_SIZE]; 
}EXT2_INODE_BITMAP;

//ext2 文件系统
typedef struct{
	EXT2_SUPER_BLOCK 	 	sb; // 超级块
	EXT2_GROUP_DESCRIPTOR 	gd; // 组描述符
	DISK_OPERATIONS *		disk; // 物理磁盘
}EXT2_FILESYSTEM;

// 文件的定位
typedef struct{
	UINT32 group; //所在组块号
	UINT32 block; //所在块号
	UINT32 offset; //块内偏移
} EXT2_DIR_ENTRY_LOCATION;

typedef struct{
	EXT2_FILESYSTEM * fs; // 记录了实际内容
	EXT2_DIR_ENTRY entry; // 记录了文件目录信息
	EXT2_DIR_ENTRY_LOCATION location;
} EXT2_NODE;

typedef struct
{
    int bit_num; // bit 位数
    int index_num; // 字节数
	int aa_num;
}EXT2_BITSET;


#ifdef _WIN32
#pragma pack(pop, fatstructures)
#else
#pragma pack()
#endif

// block group 的相对位置
#define SUPER_BLOCK 0
#define GROUP_DES 1
#define BLOCK_BITMAP 2
#define INODE_BITMAP 3
#define INODE_TABLE(x) (4+x)

#define FILE_TYPE_FIFO               0x1000
#define FILE_TYPE_CHARACTERDEVICE    0x2000
#define FILE_TYPE_DIR				 0x4000
#define FILE_TYPE_BLOCKDEVICE        0x6000
#define FILE_TYPE_FILE				 0x8000

int ext2_format(DISK_OPERATIONS* disk);
int ext2_create(EXT2_NODE* parent, char* entryName, EXT2_NODE* retEntry);
int ext2_lookup(EXT2_NODE* parent, char* entryName, EXT2_NODE* retEntry);

int fill_super_block(EXT2_SUPER_BLOCK * sb);
int fill_descriptor_table(EXT2_GROUP_DESCRIPTOR_TABLE * gd, EXT2_SUPER_BLOCK * sb);

int create_root(DISK_OPERATIONS* disk, EXT2_SUPER_BLOCK * sb);

typedef int(*EXT2_NODE_ADD)(void*, EXT2_NODE*);

int block_write(DISK_OPERATIONS* this, SECTOR sector, void* data);
int block_read(DISK_OPERATIONS* this, SECTOR sector, void* data);

UINT32 get_free_inode_number(EXT2_FILESYSTEM* fs);
UINT32 get_free_block_number(EXT2_FILESYSTEM* fs);

int init_super_block(DISK_OPERATIONS* disk, EXT2_SUPER_BLOCK* sb, UINT32 group_number);
int init_gdt(DISK_OPERATIONS* disk, EXT2_GROUP_DESCRIPTOR_TABLE * gdt, UINT32 group_number);
int init_block_bitmap(DISK_OPERATIONS* disk, UINT32 group_number);
int init_inode_bitmap(DISK_OPERATIONS* disk, UINT32 group_number);

int set_bit(SECTOR number, BYTE* bitmap);
int dump_memory(DISK_OPERATIONS* disk, int sector);
int validate_sb(void* block);
int get_inode(EXT2_FILESYSTEM* fs, UINT32 inode_num, INODE *inodeBuffer);
int read_root_block(EXT2_FILESYSTEM* fs, BYTE* block);
int read_superblock(EXT2_FILESYSTEM* fs, EXT2_NODE* root);
int format_name(EXT2_FILESYSTEM* fs, char* name);
int lookup_entry(EXT2_FILESYSTEM* fs, int inode, char* name, EXT2_NODE* retEntry);
int find_entry_at_block(BYTE* sector, char* formattedName, UINT32 begin, UINT32 last, UINT32* number);

int set_inode_bitmap(EXT2_FILESYSTEM* fs, EXT2_INODE_BITMAP* i_bitmap, EXT2_BITSET bitset);
int set_block_bitmap(EXT2_FILESYSTEM* fs, EXT2_BLOCK_BITMAP* b_bitmap, EXT2_BITSET bitset);
int insert_inode_table(EXT2_NODE* parent, INODE* inode_entry, int free_inode);

int set_sb_free_block_count(EXT2_NODE* NODE, int num);
int	set_sb_free_inode_count(EXT2_NODE* NODE, int num);

int	set_gd_free_block_count(EXT2_NODE* NODE,UINT32 free_data_block,int num);
int	set_gd_free_inode_count(EXT2_NODE* NODE,UINT32 free_inode_block,int num);

int set_entry( EXT2_FILESYSTEM* fs,EXT2_DIR_ENTRY_LOCATION* location, EXT2_DIR_ENTRY* value );
int insert_entry(EXT2_NODE *, EXT2_NODE *, UINT16 );

#endif

