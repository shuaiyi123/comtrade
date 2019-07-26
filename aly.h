/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-07-22 23:07:01
 * @LastEditTime: 2019-07-26 00:45:26
 * @LastEditors: Please set LastEditors
 */

#ifndef md_aly__h
#define md_aly__h

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <assert.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/stat.h>

#ifdef __cplusplus
}
#endif

#define MY_MMAP "cfgFileHeader.CFG"
typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;

typedef struct {
    int tt;  //通道总数
    int at;  //模拟量通道总数
    int dt;  //数字量通道总数 

    int    aNum; //模拟通道序号
    char   aChaName[64][12]; //通道名称，最大32个通道，名称长度16个字节
    char   unit[64][3]; //通道单位
    float  a[64];  //通道增益系数
    float  b[64];  //通道偏移量
    float  prim[64] ;  //一次侧系数
    float  second[64]; //二次侧系数
    char   ps[64];    //为P，还原为一次值；为S，还原为二次值

    int   dNum;  //数字量序号
    char  dChaName[64][12]; //数字量通道名
    char  norm[64]; //正常状态

    int endsamp; //该采样率下最末采样序号
    char fileFmt[10]; //DAT文件格式

    char line[256]; //用来保存文件一行的数据
}CFGFILE_HEADER;

bool analy_CfgFile(char *filename, CFGFILE_HEADER *cfgFile_Header);
bool analy_DATile(char *fileName,CFGFILE_HEADER *cfgFile_Header,short *anaData,char *degiData);
bool saveToCSV(char *filename,CFGFILE_HEADER *cfgFilHeader,short *anaData,char *degiData);
#endif
