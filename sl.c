#include <semaphore.h>
#include <math.h>
#include <time.h>
#include "sl.h"

static int do_abort = 0; 

int main()
{
    int shmRlyId;
    sem_t *semRlyEmpty, *semRlyFull;
    void *shmRlyAddr = NULL;
    struct tShmRly *pshmRly = NULL;
    int loop = 0;
 
    signal(SIGINT, &funSIGINT);

    //创建shmRly
    shmRlyId = shm_open(SHM_RLY, O_CREAT | O_TRUNC | O_RDWR, 0666);
    if (-1 == shmRlyId)
    {
        perror("open shmRly failed:");
        exit(-1);
    }

    //设置shmRly大小
    if (-1 == ftruncate(shmRlyId, sizeof(struct tShmRly)))
    {
        perror("shmRly ftruncate failed:");
        exit(-1);
    }

    //映射shmRly
    shmRlyAddr = (char *)mmap(NULL, sizeof(struct tShmRly), PROT_READ | PROT_WRITE, MAP_SHARED, shmRlyId, 0);
    if (MAP_FAILED == shmRlyAddr)
    {
        perror("shmRly mmap failed:");
        exit(-1);
    }
    printf("shmRlyAddr: %x\n", (int)shmRlyAddr);

    //格式化共享内存
    pshmRly = (struct tShmRly *)shmRlyAddr;

    //创建信号量 semRlyEmpty
    semRlyEmpty = sem_open(SEM_RLY_EMPTY, O_CREAT, 0666, 0);
    if (semRlyEmpty == SEM_FAILED)
    {
        perror("open semRlyEmpty failed:");
        exit(-1);
    }

    //初始化为局部信号量，信号量初始值为1,第二个参数为0时信号量只能在线程间共享
    sem_init(semRlyEmpty, 1, 1);

    //创建信号量 semRlyFull
    semRlyFull = sem_open(SEM_RLY_FULL, O_CREAT, 0666, 0);
    if (semRlyFull == SEM_FAILED)
    {
        perror("open semRlyFull failed:");
        exit(-1);
    }
    //初始化局部信号量，信号量初始值为0
    sem_init(semRlyFull, 1, 0);

    //执行状态
    initSL();

    while (!do_abort)
    {
        sleep(10);
        printf("YC_PROCESS is working...\n");

        // save rlyDate to buffer,recalling success will return 0
        if (!sem_wait(semRlyEmpty))  
        { 
            get_SysTime(pshmRly->DATCFG_filename,pshmRly->startRecoWave);//开始录波时刻系统时间
            samDataMonitor(pshmRly->data, SAMP, SAMC, SAML, 20);
            statusDataMonitor(YXC, SAMP*SAML, pshmRly->status_data);
            get_SysTime(NULL,pshmRly->faultZeroTime); //故障0时刻系统时钟

            printf("sl post semRlyFull\n");
            sem_post(semRlyFull);
            sleep(2);
        }

        if (++loop == 1)
        {
            break;
        }
    }

    // sem_wait(semRlyEmpty);

    // save buffer to file
    printf("write data to file...\n");
    saveToCsv(&pshmRly->data[0], SAMC, SAMP * SAML, "rly.csv");

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
    sem_unlink(SEM_RLY_EMPTY);
    sem_unlink(SEM_RLY_FULL);

    // 删除shmRLy
    if (-1 == shm_unlink(SHM_RLY))
    {
        perror("shm_unlink failed:");
        exit(-1);
    }

    printf("YC_PROCESS[%d] exit...\n", getpid());
}

int initSL()
{
    int st = 0; //执行状态

    return st;
}

/**
 * @param {int} 
 * @return: void
 * @Description: 收到"ctrl+c"停止
 */
void funSIGINT(int signo)
{
    printf("sl funSIGINT got a signal %d\n", signo);

    do_abort = 1;
}

/**
 * @param {type} 
 * @return: 
 * @Description: 模拟生成采样数据
 */
void samDataMonitor(short *pdata, int samp, int samc, int saml, int delt)
{
    int i, chn, len, cnt;
    double dval;

    srand(delt);

    cnt = 0;
    len = samp * saml;
    for (i = 0; i < len; i++)
    {
        for (chn = 0; chn < samc; chn++)
        {
            dval = (500 + chn * 100) * cos((double)i * 2 * 3.1415926 / samp);
            pdata[cnt++] = (short)dval + (short)rand() % 20;
        }
    }

    printf("samDataMonitor Done\n");

#if 0
    saveToCsv(pdata, samc, samf * saml, "sam.csv");
#endif
}

/**
 * @param {type} 
 * @return: 
 * @Description: 模拟生成状态量数据
 */
void statusDataMonitor(uint16 yxNnum, uint16 yxTotal, uint8 *status_data)
{
    uint16 i,chn;
    uint32 cnt=0;
    uint8 flag=0;
    for (i = 0; i < yxTotal; i++)
    {
        if(i>0)   
            if((i%2000)==0)flag=1;
       
        for (chn = 0; chn < yxNnum; chn++)
        {

            if(flag==1)  
                status_data[cnt++]=1;
            else
                status_data[cnt++]=0;
        }

    }

    printf("samDataMonitor Done\n");

}
/**
 * @param {type} 
 * @return: 
 * @Description: ADC输出转换为csv格式
 */
int saveToCsv(short *buffer, int frame_w, int frame_h, char *path_out)
{
    int i, j;

    FILE *fp_out = fopen(path_out, "w");

    // wirte cs title
    fprintf(fp_out, "%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s\n",
            "B1", "A1", "B2", "A2", "B3", "A3", "B4", "A4", "B5", "A5", "B6", "A6", "B7", "A7", "B8", "A8");

    for (i = 0; i < frame_h; i++)
    {
        short *line = buffer + frame_w * i;
#if 0 //输出原始码值
        fprintf(fp_out, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
                line[0], line[1], line[2], line[3], line[4], line[5], line[6], line[7],
                line[8], line[9], line[10], line[11], line[12], line[13], line[14], line[15]);
#endif

#if 1 //输出外部模拟量
        float values[frame_w];
        for (j = 0; j < frame_w; j++)
        {

            values[j] = line[j] * 5.0 / 32768;
        }

        fprintf(fp_out, "%.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f\n",
                values[0], values[1], values[2], values[3], values[4], values[5], values[6], values[7],
                values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[15]);
#endif
    }

    fclose(fp_out);

    printf("saveToCsv %s OK\n", path_out);

    return 0;
}

 //getting system date
static int get_SysTime(char *filename,char *timeBuff)
{
    struct timeval microtime;  //精确到微秒
    time_t now; //实例化time_t结构    
    struct tm timenow; //实例化tm结构指针 

    time(&now);  //得到时间秒数 
    localtime_r(&now,&timenow);  //线程安全,将秒数转化为日历，并存储在timenow结构体
    gettimeofday(&microtime,NULL);//获取微秒

    if(timeBuff!=NULL){
        strftime(timeBuff,32, "%d/%m/%Y,%H:%M:%S",&timenow);//将时间转化为自己需要的时间格式
        sprintf(timeBuff+19,".%ld",microtime.tv_usec); //将微秒追加到时间后面
    }
    else return 0;  

    if(filename!=NULL){
       strftime(filename,32,"%Y-%m-%d %H-%M-%S",&timenow);
    }
    else return 0;
    
    return 1;
}