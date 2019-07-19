#include <semaphore.h>
#include <time.h>
#include "wave.h"
static int do_abort = 0;
int main()
{
    int shmRlyId=-1;
    sem_t *semRlyEmpty, *semRlyFull;
    void *shmRlyAddr = NULL;
    struct tShmRly *pshmRly;
    struct stat buf;

    signal(SIGINT, &funSIGINT);
    initWave();

    //打开shmRly
        shmRlyId = shm_open(SHM_RLY, O_RDWR, 0);  
    if (-1 == shmRlyId)
    {
        perror("shmRlyId shm_open");
        exit(-1);
    }

    //获取shmRly大小
    if (-1 == fstat(shmRlyId, &buf))
    {
        perror("fstat failed:");
        exit(-1);
    }

    //映射shmRly
    shmRlyAddr = (char *)mmap(NULL, buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmRlyId, 0);
    if (MAP_FAILED == shmRlyAddr)
    {
        perror("mmap shmRlyAddr");
        exit(-1);
    }
    printf("shmRlyAddr: %x\n", (int)shmRlyAddr);

    //格式化共享内存
    pshmRly = (struct tShmRly *)shmRlyAddr;

    //创建信号量
    semRlyEmpty = sem_open(SEM_RLY_EMPTY, 0);
    if (semRlyEmpty == SEM_FAILED)
    {
        perror("open semRlyEmpty failed:");
        exit(-1);
    }
    semRlyFull = sem_open(SEM_RLY_FULL, 0);
    if (semRlyFull == SEM_FAILED)
    {
        perror("open semRlyFull failed:");
        exit(-1);
    }

    while (!do_abort)
    {
        sleep(1);
        printf("WAVE_PROCESS is working...\n");

        sem_wait(semRlyFull);

        gener_CFGFile(pshmRly->startRecoWave,pshmRly->faultZeroTime,pshmRly->DATCFG_filename);
        //////WORT BGN///////
        printf("接收到semRlyFull信号量\n");
        //////WORK END///////

        sleep(1);
        sem_post(semRlyEmpty);
        sleep(1);

        break;
    }

    //取消shmRly映射
    if (-1 == munmap(shmRlyAddr, sizeof(struct tShmRly)))
    {
        perror("munmap shmRlyAddr failed:");
        exit(-1);
    }
    // 关闭shmRly
    close(shmRlyId);

    //关闭信号量
    sem_close(semRlyEmpty);
    sem_close(semRlyFull);

    printf("WAVE_PROCESS[%d] exit...\n", getpid());
}

/**
 * @param {int} 
 * @return: void
 * @Description: 收到"ctrl+c"停止
 */
void funSIGINT(int signo)
{
    printf("wave funSIGINT got a signal %d\n", signo);

    do_abort = 1;
}

int initWave()
{
    int st = 0; //执行状态

    return st;
}

void gener_CFGFile(char *startRecTim,char *faultTime,char *filename)
{
    FILE *fp;
    if(filename==NULL) exit(1);
    sprintf(filename+19,".CFG");
    if((fp=fopen(filename, "w"))==NULL){
        printf("open %s flie error",filename);
        exit(1);
    }
    fprintf(fp,"%s\n%s\n","环网柜,BD622,2013","61,23A,38D"); //厂站名，记录装置标示，版本号 ；   通道总数，模拟通道总数，数字通道总数

    fprintf(fp,"1,Ua1,,,V,0.005143,0.000000,0,-13735,13594,10000,100,S\n");
    fprintf(fp,"2,Ub1,,,V,0.005143,0.000000,0,-13705,13593,10000,100,S\n");
    fprintf(fp,"3,Uc1,,,V,0.005143,0.000000,0,-13746,13615,10000,100,S\n");
    fprintf(fp,"4,I母,,,V,0.008730,0.000000,0,-8114,7886,10000,100,S\n");
    fprintf(fp,"5,01#线,,,A,0.000257,0.000000,0,-22241,26601,100,1,S\n");
    fprintf(fp,"6,02#线,,,A,0.000257,0.000000,0,-23313,24237,100,1,S\n");
    fprintf(fp,"7,03#线,,,A,0.000257,0.000000,0,-26169,28051,100,1,S\n");
    fprintf(fp,"8,04#线,,,A,0.000257,0.000000,0,-8321,11303,100,1,S\n");
    fprintf(fp,"9,05#线,,,A,0.000257,0.000000,0,-6049,8117,100,1,S\n");
    fprintf(fp,"10,06#线,,,A,0.000257,0.000000,0,-6008,8175,100,1,S\n");
    fprintf(fp,"11,07#线,,,A,0.000257,0.000000,0,-4678,6201,100,1,S\n");
    fprintf(fp,"12,Ua2,,,V,0.005143,0.000000,0,-103,-81,10000,100,S\n");
    fprintf(fp,"13,Ub2,,,V,0.005143,0.000000,0,-102,-88,10000,100,S\n");
    fprintf(fp,"14,Uc2,,,V,0.005143,0.000000,0,-106,-85,10000,100,S\n");
    fprintf(fp,"15,II母,,,V,0.008730,0.000000,0,-106,-82,10000,100,S\n");
    fprintf(fp,"16,08#线,,,A,0.000257,0.000000,0,-3994,5340,100,1,S\n");
    fprintf(fp,"17,09#线,,,A,0.000257,0.000000,0,-3182,4371,100,1,S\n");
    fprintf(fp,"18,10#线,,,A,0.000257,0.000000,0,-3275,4770,100,1,S\n");
    fprintf(fp,"19,11#线,,,A,0.000257,0.000000,0,-2606,3898,100,1,S\n");
    fprintf(fp,"20,12#线,,,A,0.000257,0.000000,0,-2392,3659,100,1,S\n");
    fprintf(fp,"21,13#线,,,A,0.000257,0.000000,0,-1802,3462,100,1,S\n");
    fprintf(fp,"22,14#线,,,A,0.000257,0.000000,0,-1466,2694,100,1,S\n");
    fprintf(fp,"23,15#线,,,A,0.000257,0.000000,0,-1155,2350,100,1,S\n");
    fprintf(fp,"1,选线启动,,,0\n");
    fprintf(fp,"2,选线跳闸,,,0\n");
    fprintf(fp,"3,后加速跳闸,,,0\n");
    fprintf(fp,"4,启动轮切,,,0\n");
    fprintf(fp,"5,分段1-TWJ,,,0\n");
    fprintf(fp,"6,分段2-TWJ,,,0\n");
    fprintf(fp,"7,备用,,,0\n");
    fprintf(fp,"8,备用,,,0\n");
    fprintf(fp,"9,01#线_跳闸,,,0\n");
    fprintf(fp,"10,01#线_TWJ,,,0\n");
    fprintf(fp,"11,02#线_跳闸,,,0\n");
    fprintf(fp,"12,02#线_TWJ,,,0\n");
    fprintf(fp,"13,03#线_跳闸,,,0\n");
    fprintf(fp,"14,03#线_TWJ,,,0\n");
    fprintf(fp,"15,04#线_跳闸,,,0\n");
    fprintf(fp,"16,04#线_TWJ,,,0\n17,05#线_跳闸,,,0\n18,05#线_TWJ,,,0\n19,06#线_跳闸,,,0\n20,06#线_TWJ,,,0\n21,07#线_跳闸,,,0\n22,07#线_TWJ,,,0\n23,08#线_跳闸,,,0\n24,08#线_TWJ,,,0\n25,09#线_跳闸,,,0\n26,09#线_TWJ,,,0\n27,10#线_跳闸,,,0\n28,10#线_TWJ,,,0\n29,11#线_跳闸,,,0\n30,11#线_TWJ,,,0\n31,12#线_跳闸,,,0\n32,12#线_TWJ,,,0\n33,13#线_跳闸,,,0\n34,13#线_TWJ,,,0\n35,14#线_跳闸,,,0\n36,14#线_TWJ,,,0\n37,15#线_跳闸,,,0\n38,15#线_TWJ,,,0\n");

    fprintf(fp,"%s\n%s\n%s\n","50","1","25600,4096"); //电网频率，采样率个数，采样率/HZ，最末采样序号
    fprintf(fp,"%s\n%s\n",faultTime,startRecTim);//开始录波时刻，故障0时刻
    fprintf(fp,"BINARY\n1\n");  //二进制文件，时标因子为1
    fclose(fp);
}
