/*
 * @Author: Yangxl
 * @LastEditors: Please set LastEditors
 * @Description: 测量值计算相关函数及变量声明
 * @Date: 2019-04-10 19:42:53
 * @LastEditTime: 2019-07-22 02:56:45
 */

#ifndef md_wave__h
#define md_wave__h

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

#define SAMB 2     //每个采样点的字节数 2*8bit=16bit
#define SAMC 16    //采样通道总数 16通道
#define YXC  2     //遥信通道总数
#define SAMF 25600 //采样频率 sampling frequency
#define SAMP 512   //每周采样点数 sample Point per cycle
#define SAML 8     //采样周波数
#define RLY_P_LEN (SAMP * SAMC * SAML)  //
#define YXC_P_LEN (YXC * SAMP * SAML)

#define SEM1 1000
#define SEM2 1001
#define SEM_RLY_EMPTY "rlyEmpty"
#define SEM_RLY_FULL "rlyFull"

#define SHM_RLY "shmRly"

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;

typedef enum {
    FALSE = 0,
    TRUE
} FUNSTATUS;
struct tShmRly
{
    int step;
    int cnt;
    char yes_BinFlag; //值为1，生成二进制文件；为0，生成ASCII文件
    char DATCFG_filename[32];  //DAT、CFG文件名
    char startRecoWave[32]; //开始录波时刻时间
    char faultZeroTime[32]; //故障0时刻时间
    uint8 status_data[YXC_P_LEN];  //状态数据量，即摇信
    short data[RLY_P_LEN];  //模拟量采样数据
};

void funSIGINT(int signo);
int initWave();
FUNSTATUS gener_CFGFile(char *startRecTim,char *faultZeroTime,char *filename,char yesBinFlag);
FUNSTATUS gener_BinDATFile(uint16 chanNum,uint16 yx_num,uint16 ever_samTotal,short * samDate,uint8 *staData,char *filename);
FUNSTATUS gerner_ASCIIDATFile(uint16 chanNum,uint16 yx_num,uint16 ever_samTotal,short * samDate,uint8 *staData,char *filename);
#ifdef __cplusplus
}
#endif

#endif