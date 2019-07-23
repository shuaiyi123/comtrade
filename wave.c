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
    struct stat buf;;
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

        pshmRly->yes_BinFlag=1;  //生成ASCII文件

        gener_CFGFile(pshmRly->startRecoWave,pshmRly->faultZeroTime,pshmRly->DATCFG_filename,pshmRly->yes_BinFlag);

        if(pshmRly->yes_BinFlag==1)
            gener_BinDATFile(SAMC,YXC,SAMP*SAML,pshmRly->data,pshmRly->status_data,pshmRly->DATCFG_filename);
        else 
            gerner_ASCIIDATFile(SAMC,YXC,SAMP*SAML,pshmRly->data,pshmRly->status_data,pshmRly->DATCFG_filename);
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

    //关闭信号�?
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


/**
 * @description: 生成配置文件
 * @param {startRecTim：startRecTim：开始录波时刻，faultZeroTime：故障0时刻，filename：文件名，yesBinFlag配置成二进制文件 1是/0否} 
 * @return: 
 */
FUNSTATUS gener_CFGFile(char *startRecTim,char *faultZeroTime,char *filename,char yesBinFlag)
{
    FILE *fp;
    CFGFILE_HEADER cfgFile_header;
    if(filename==NULL) return FALSE;

    sprintf(filename+19,".CFG");

    if((fp=fopen(filename, "w"))==NULL){
        printf("open %s flie error",filename);
        return FALSE;
    }
    
    cfgFile_header.station_name    ="环网柜"; //厂站名
    cfgFile_header.dev_id          ="BD622"; //装置编号或名称
    cfgFile_header.rec_year        =2013;    //版本年号
    cfgFile_header.tt              =34;     //通道总数
    cfgFile_header.at              =16;     //模拟量通道总数
    cfgFile_header.dt              =18;     //状态量通道总数
    cfgFile_header.lf              =50;     //电网频率
    cfgFile_header.nrates          =1 ;     //采样率个数
    cfgFile_header.samp            =25600;   //采样率
    cfgFile_header.endsamp         =4096;    //该采样率下的最末采样序号
    cfgFile_header.timemult        =1;       //时标因子
    
    fprintf(fp,"%s,%s,%d\n",cfgFile_header.station_name,cfgFile_header.dev_id,cfgFile_header.rec_year); //厂站名，记录装置标示，版本号
    fprintf(fp,"%d,%dA,%dD\n",cfgFile_header.tt,cfgFile_header.at,cfgFile_header.dt);//通道总数，模拟通道总数，数字通道总数
    //模拟量
    fprintf(fp,"1,U01,,,V,0.000153,0.000000,0,-32768,32767,1,1,S\n");
    fprintf(fp,"2,U02,,,V,0.000153,0.000000,0,-32768,32767,1,1,S\n");
    fprintf(fp,"3,U03,,,V,0.000153,0.000000,0,-32768,32767,1,1,S\n");
    fprintf(fp,"4,U04,,,V,0.000153,0.000000,0,-32768,32767,1,1,S\n");
    fprintf(fp,"5,U05,,,V,0.000153,0.000000,0,-32768,32767,1,1,S\n");
    fprintf(fp,"6,U06,,,V,0.000153,0.000000,0,-32768,32767,1,1,S\n");
    fprintf(fp,"7,U07,,,V,0.000153,0.000000,0,-32768,32767,1,1,S\n");
    fprintf(fp,"8,U08,,,V,0.000153,0.000000,0,-32768,32767,1,1,S\n");
    fprintf(fp,"9,U09,,,V,0.000153,0.000000,0,-32768,32767,1,1,S\n");
    fprintf(fp,"10,U10,,,V,0.000153,0.000000,0,-32768,32767,1,1,S\n");
    fprintf(fp,"11,U11,,,V,0.000153,0.000000,0,-32768,32767,1,1,S\n");
    fprintf(fp,"12,U12,,,V,0.000153,0.000000,0,-32768,32767,1,1,S\n");
    fprintf(fp,"13,U13,,,V,0.000153,0.000000,0,-32768,32767,1,1,S\n");
    fprintf(fp,"14,U14,,,V,0.000153,0.000000,0,-32768,32767,1,1,S\n");
    fprintf(fp,"15,U15,,,V,0.000153,0.000000,0,-32768,32767,1,1,S\n");
    fprintf(fp,"16,U16,,,V,0.000153,0.000000,0,-32768,32767,1,1,S\n");

    //状态量
    fprintf(fp,"1,start,,,0\n");
    fprintf(fp,"2,end,,,0\n");
    fprintf(fp,"3,start1,,,0\n");
    fprintf(fp,"4,end1,,,0\n");
    fprintf(fp,"5,start2,,,0\n");
    fprintf(fp,"6,end2,,,0\n");
    fprintf(fp,"7,start3,,,0\n");
    fprintf(fp,"8,end3,,,0\n");
    fprintf(fp,"9,start4,,,0\n");
    fprintf(fp,"10,end4,,,0\n");
    fprintf(fp,"11,start5,,,0\n");
    fprintf(fp,"12,end5,,,0\n");
    fprintf(fp,"13,start6,,,0\n");
    fprintf(fp,"14,end6,,,0\n");
    fprintf(fp,"15,start7,,,0\n");
    fprintf(fp,"16,end7,,,0\n");
    fprintf(fp,"17,start8,,,0\n");
    fprintf(fp,"18,end8,,,0\n");

    fprintf(fp,"%d\n%d\n%d,%d\n",cfgFile_header.lf,cfgFile_header.nrates,cfgFile_header.samp,cfgFile_header.endsamp); //电网频率，采样率个数，采样率/HZ，最末采样序号
    fprintf(fp,"%s\n%s\n",startRecTim,faultZeroTime);//开始录波时刻，故障0时刻

    if(yesBinFlag==1)fprintf(fp,"BINARY\n1\n");  //生成二进制文件，时标因子1
    else fprintf(fp,"ASCII\n%d\n",cfgFile_header.timemult); //生成ASCII文件

    fprintf(fp,"0,0h\n");//当地时间与UTC时差为0
    fprintf(fp,"B,3\n");//误差10s以内,时钟源没有闰秒功能

    fclose(fp);

    return TRUE;
}

/**
 * @description: 生成.DAT二进制文件
 * @param {chanNum：模拟量通道总数，yx_num：遥信路数，ever_samTotal：每一通道采集个数，samDate：采样数据，staData：遥信数据，filename：文件名} 
 * @return: 
 */
FUNSTATUS gener_BinDATFile(uint16 chanNum,uint16 yx_num,uint16 ever_samTotal,short * samDate,uint8 *staData,char *filename)
{
    uint8  x=0,y=0;  
    uint16 i,j,k;
    uint16 status_temp=0,status_data=0;//状态量
    uint32 num=1,timing=0;//序号，时标
    FILE *fp;
    if(filename==NULL) return FALSE;

    sprintf(filename+19,".DAT");

    if((fp=fopen(filename, "wb"))==NULL){
        printf("open %s flie error",filename);
        return FALSE;
    }

    for(j=0;j<ever_samTotal;j++){

        fwrite(&num,sizeof(int),1,fp); //序号
        fwrite(&timing,sizeof(int),1,fp); //时标
        num++;
        timing+=39;  //时标=1000000/25600

        for(i=0;i<chanNum;i++){
           fwrite(samDate++,sizeof(short),1,fp);
        }
        
        x=(uint8)yx_num/16;  //保存状态量以两个字节为单位，每一位代表一个状态量，即一个单位可保存16路状态量
        y=(uint8)yx_num%16;
        for(i=0;i<x;i++){
            for(k=0;k<16;k++){
                status_temp=(uint16)(*staData&0x01);  //状态量只有两种情况，0或1
                status_temp<<=k;  //低通道保存在低位
                status_data|=status_temp;
                staData++;
            }
            fwrite(&status_data,sizeof(uint16),1,fp);
        }

        for(i=0;i<y;i++){
            status_temp=(uint16)(*staData&0x01); 
            status_temp<<=i;
            status_data|=status_temp;
            staData++;
        }
        fwrite(&status_data,sizeof(uint16),1,fp);
        status_data=0;
    }
    fclose(fp);
    return TRUE;
}

/**
 * @description: 生成.DAT ASCII文件
 * @param {chanNum：模拟通道总数，yx_num:遥信路数,ever_samTotal：每一通道采集个数，samDate：采样数据，staData:状态量数据,filename：文件名} 
 * @return: 
 */
FUNSTATUS gerner_ASCIIDATFile(uint16 chanNum,uint16 yx_num,uint16 ever_samTotal,short * samDate,uint8 *staData,char *filename)
{
     unsigned short i,j;
    unsigned int num=1,timimg=0;
    FILE *fp;
    if(filename==NULL) return FALSE;

    sprintf(filename+19,".DAT");

    if((fp=fopen(filename, "w"))==NULL){
        printf("open %s flie error",filename);
        return FALSE;
    }
    for(j=0;j<ever_samTotal;j++){  //通道最末序号

        fprintf(fp,"%d,%d",num,timimg);  //序号+时标
        num++;
        timimg+=39;  //时标=1000000/25600

        for(i=0;i<chanNum;i++){  //模拟量
            fprintf(fp,",%d",*samDate++);
        }

        for(i=0;i<yx_num;i++){  //数据量
            fprintf(fp,",%d",*staData++);
        }
        fprintf(fp,"\n");
    }
    fprintf(fp,"1A\n");  //文件结束符
    fclose(fp);
    return TRUE;
}

//  //序号、时标倒叙存储，低字节在前，高字节在后
// unsigned int flashStorage(unsigned int oriDate)
// {
//     unsigned int temp=0;

//     temp |= (oriDate&0x000000ff)<<24;
//     temp |= (oriDate&0x0000ff00)<<8;
//     temp |= (oriDate&0x00ff0000)>>8;
//     temp |= (oriDate&0xff000000)>>24;

//     return temp;
// }
