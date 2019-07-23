/*
 * @Author: Yangxl
 * @LastEditors: Please set LastEditors
 * @Description: 测量值计算相关函数及变量声明
 * @Date: 2019-04-10 19:42:53
 * @LastEditTime: 2019-07-22 18:24:08
 */

#ifndef md_sl__h
#define md_sl__h

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
#define YXC  18     //遥信通道总数
#define SAMF 25600 //采样频率 sampling frequency
#define SAMP 512   //每周采样点数 sample Point per cycle
#define SAML 8     //采样周波数
#define RLY_P_LEN (SAMP * SAMC * SAML)
#define YXC_P_LEN (YXC * SAMP * SAML)

#define SEM1 1000
#define SEM2 1001
#define SEM_RLY_EMPTY "rlyEmpty"
#define SEM_RLY_FULL "rlyFull"

#define SHM_RLY "shmRly"

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;

struct tShmRly
{
    int step;
    int cnt;
    char yes_BinFlag; //值为1，生成二进制文件；为0，生成ASCII码
    char DATCFG_filename[32];  //DAT、CFG文件名
    char startRecoWave[32]; //开始录波时刻
    char faultZeroTime[32]; //故障0时刻
    uint8 status_data[YXC_P_LEN];  //状态数据量，即摇信
    short data[RLY_P_LEN];
};

    int initSL();
    void funSIGINT(int signo);
    void samDataMonitor(short *pdata, int samf, int samc, int saml, int delt);
    void statusDataMonitor(uint16 yxNnum, uint16 yxTotal, uint8 *status_data);
    int saveToCsv(short *buffer, int frame_w, int frame_h, char *path_out);
    static int get_SysTime(char *filename,char *timeBuff);
    
#ifdef __cplusplus
}
#endif

#endif