#include "common.h"
#include "shell.h"

#ifndef NULL
#define NULL	( ( void* )0 )
#endif
// 用链表管理父目录下的子目录
int init_entry_list(SHELL_ENTRY_LIST *list){
    memset(list,0,sizeof(SHELL_ENTRY_LIST));
    return 0;
}

int add_entry_list(SHELL_ENTRY_LIST *list,SHELL_ENTRY *entry){
    SHELL_ENTRY_LIST_ITEM *newIem;
    // 新节点的初始化
    newIem=(SHELL_ENTRY_LIST_ITEM *) malloc (sizeof(SHELL_ENTRY_LIST_ITEM));
    newIem->entry=*entry;
    newIem->next=NULL;
    // 将新的节点添加到链表的尾部
    if(list->count==0){
        list->first=list->last=newIem;
    }else{
        list->last->next=newIem;
        list->last=newIem;
    }

    list->count++;
    return 0;
}
// 释放链表
void release_entry_list(SHELL_ENTRY_LIST *list){
    SHELL_ENTRY_LIST_ITEM *currentItem;
    SHELL_ENTRY_LIST_ITEM *nextItem;

    if(list->count==0){
        return ;
    }

    nextItem=list->first;

    do{
        currentItem=nextItem;
        nextItem=currentItem->next;
        free(currentItem);
    } while (nextItem);

    list->count=0;
    list->first=NULL;
    list->last=NULL;
}