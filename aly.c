#include "aly.h"
#include <strings.h>
int main(int argc, char **argv)
{
    bool status;
    short *ptr_ana;
    char  *ptr_degi;
    char *str,cfgName[32];
    CFGFILE_HEADER *cfgFile_Header;
    int cfgfp;
    void *cfgAddr=NULL;
    if(argc!=2){
        perror("Please enter filename!");
        exit(-1);
    }
    str=argv[1];
    strcpy(cfgName,str);//获取文件名
    
    //创建一个新文件
    cfgfp=open(MY_MMAP, O_CREAT | O_RDWR, 0666);
    if (-1 == cfgfp) //错误检查是一个很好的习惯 
    { 
        perror("open cfgFileHeader.CFG failed:");
        exit(-1); 
    } 
    //设置MY_MMAP大小
    if (-1 == ftruncate(cfgfp, sizeof(CFGFILE_HEADER)))
    {
        perror("cfgFileHeader.CFG ftruncate failed:");
        exit(-1);
    }
    //映射CFGFILE_HEADER
    cfgAddr=(char*)mmap(NULL,sizeof(CFGFILE_HEADER),PROT_READ | PROT_WRITE, MAP_SHARED, cfgfp, 0);
    if (MAP_FAILED == cfgAddr)
    {
        perror("MY_MMAP mmap failed:");
        exit(-1);
    }
    //格式化cfgAddr
    cfgFile_Header=(CFGFILE_HEADER*)cfgAddr;
   
    //分配内存
    //cfgFile_Header=(CFGFILE_HEADER*)malloc(sizeof(CFGFILE_HEADER));
     
    //解析CFGFILE文件
    status=analy_CfgFile(cfgName,cfgFile_Header);
    if(status==false){
        perror("Analyse CFGFile error!");
        exit(1);
    } 
  
    //分配模拟量数据所需的内存
    ptr_ana= (short*)malloc(sizeof(short)*cfgFile_Header->at*cfgFile_Header->endsamp); 
    if(ptr_ana==NULL){
        perror("malloc memory for analog data error!");
        exit(1);
    }
     
     //分配状态量数据所需的内存
    ptr_degi=(char*)malloc(sizeof(char)*cfgFile_Header->dt*cfgFile_Header->endsamp); 
    if(ptr_degi==NULL){
        perror("malloc memory for degital data error!");
        exit(1);
    }
    //解析DAT文件
    status=analy_DATile(cfgName,cfgFile_Header,ptr_ana,ptr_degi);
    if(status==false){
        printf("Analyse DATFile error!");
        exit(1);
    } 
    
    //生成.CSV电子表格
    status=saveToCSV("scmp.csv",cfgFile_Header,ptr_ana,ptr_degi);
    if(status==false){
        printf("scmp.CSV error!");
        exit(1);
    }
    printf("Generate scmp.scv success\n");
        //取消shmRly映射
    if (-1 == munmap(cfgAddr, sizeof(CFGFILE_HEADER)))
    {
        perror("munmap shmRlyAddr failed:");
        exit(-1);
    }
      // 关闭cfgfp
    close(cfgfp);
    //删除MY_MMAP
    if(-1==unlink(MY_MMAP)){
        perror("shm_unlink failed:");
        exit(-1);
    }
    //释放内存
    //free(cfgFile_Header);
    free(ptr_ana);
    free(ptr_degi);
    
}

/**
 * @description: 解析.CFG文件
 * @param {filename：文件名，cfgFile_Header：配置文件句柄} 
 * @return: false:打开文件错误，true：解析成功
 */
bool analy_CfgFile(char *filename, CFGFILE_HEADER *cfgFile_Header)
{
    FILE *fp;
    uint16 i,cnt=0;
    int endSamp;
    if((fp=fopen(filename,"r"))==NULL){
        printf("open %s file error!",filename);
        return false;
    }    
    fseek(fp,0L,SEEK_SET);//定义到文件开头
    fgets(cfgFile_Header->line,127,fp);//读取第一行
    cnt++;
    //fscanf(fp,"%d,%s,%s",cfgFile_Header->tt,anaChar,degiChar); //读取通道总数,模拟通道总数，数字通道总数
    fgets(cfgFile_Header->line,127,fp);//读取第二行
    cnt++;
    sscanf(cfgFile_Header->line,"%d,%dA,%d",&cfgFile_Header->tt,&cfgFile_Header->at,&cfgFile_Header->dt);
    printf("%d,%d,%d\n",cfgFile_Header->tt,cfgFile_Header->at,cfgFile_Header->dt);

    for(i=0;i<cfgFile_Header->at;i++){  //处理模拟通道
        fgets(cfgFile_Header->line,127,fp);//读取整一行
        sscanf(cfgFile_Header->line,"%d,%[^,],,,%[^,],%f,%f,%*d,%*d,%*d,%f,%f,%c",&cfgFile_Header->aNum,cfgFile_Header->aChaName[i],cfgFile_Header->unit[i],&cfgFile_Header->a[i],&cfgFile_Header->b[i],&cfgFile_Header->prim[i],&cfgFile_Header->second[i],&cfgFile_Header->ps[i]);
        //printf("%f,%f,%c\n",cfgFile_Header->a[i],cfgFile_Header->b[i],cfgFile_Header->ps[i]);
        cnt++;
    }
    
    if((cnt-2)!=cfgFile_Header->aNum) return false;
    for(i=0;i<cfgFile_Header->dt;i++){ //处理数字通道
        fgets(cfgFile_Header->line,127,fp);
        sscanf(cfgFile_Header->line,"%d,%[^,],,,%d",&cfgFile_Header->dNum,cfgFile_Header->dChaName[i],&cfgFile_Header->norm[i]);
        //printf("%d\n",cfgFile_Header->norm[i]);
        cnt++;
    }
    if((cnt-2-cfgFile_Header->aNum)!=cfgFile_Header->dNum)return false;
    
    for(i=0;i<2;i++){ //获取采样率个数
        fgets(cfgFile_Header->line,127,fp);
        cnt++;
    }
    sscanf(cfgFile_Header->line,"%d",&cfgFile_Header->nrates);

    //获取通道最末采样数
    cfgFile_Header->endsamp=0;
    for(i=0;i<cfgFile_Header->nrates;i++){
        fgets(cfgFile_Header->line,127,fp);
        sscanf(cfgFile_Header->line,"%*[^,],%d",&endSamp);
        cfgFile_Header->endsamp+=endSamp;
    }
    //printf("%d\n",cfgFile_Header->endsamp);

    for(i=0;i<3;i++){  //获取DAT文件格式
        fgets(cfgFile_Header->line,127,fp);
        cnt++;
    }
    sscanf(cfgFile_Header->line,"%s",cfgFile_Header->fileFmt);

    fclose(fp);
    return true;
}

/**
 * @description: 解析.DAT文件
 * @param {fileName：文件名，cfgFile_Header：配置文件句柄，anaData：模拟量数据，degiData：状态量数据} 
 * @return: false:打开文件错误，true：解析成功
 */
bool analy_DATile(char *fileName,CFGFILE_HEADER *cfgFile_Header,short *anaData,char *degiData)
{
    FILE *fp;
    uint8 i,j,x,y;
    uint16 dData;
    uint32 num,time;
    int cmp,cnt,dNum=0;
    char datFileName[32];

    cnt=strlen(fileName);
    strncpy(datFileName,fileName,cnt-4);
    sprintf(datFileName+cnt-4,".dat");

    //DAT文件为ASCII码
    cmp=strcasecmp(cfgFile_Header->fileFmt,"ASCII");  //忽略两个字符串中的大小写比较字符串
    //printf("DATFILE name is %s,%d\n",datFileName,cmp);
    if(cmp==0){
        if((fp=fopen(datFileName,"r"))==NULL){
            printf("open DATFile error!\n");
            return false;
        }   
        fseek(fp,0L,SEEK_SET);//定义到文件开头
        for(cnt=0;cnt<cfgFile_Header->endsamp;cnt++){ //通道的最末采样数
            fscanf(fp,"%d,%d",&num,&time); //去掉序号，时标
            for (i= 0; i < cfgFile_Header->at; i++){ //读取模拟量
               fscanf(fp,",%d",anaData++);
            }
            for(i=0;i<cfgFile_Header->dt;i++){ //读取数字量
                fscanf(fp,",%d",degiData++);
            }
        }
    }
    //DAT文件为BINARY码
    cmp=strcasecmp(cfgFile_Header->fileFmt,"BINARY");  //忽略两个字符串中的大小写比较字符串
    //printf("DATFILE name is %s,%d\n",datFileName,cmp);
    if(cmp==0){
        if((fp=fopen(datFileName,"rb"))==NULL)
        {
            printf("open DATFile error!\n");
            return false;
        }
            
        fseek(fp,0L,SEEK_SET);//定义到文件开头
        for(cnt=0;cnt<cfgFile_Header->endsamp;cnt++){

            fread(&num,sizeof(uint32),1,fp); //序号
            fread(&time,sizeof(uint32),1,fp); //时标

            for(i=0;i<cfgFile_Header->at;i++){//模拟量
                fread(anaData++,sizeof(short),1,fp); 
                //printf("%d, ",anaData[i]);
            }

            x=cfgFile_Header->dt/16; //数字量
            y=cfgFile_Header->dt%16;
            for(i=0;i<x;i++){ //遥信量大于16路
                fread(&dData,sizeof(uint16),1,fp); 
                for(j=0;j<16;j++){
                    degiData[dNum++] = (char)dData&0x0001;
                    dData >>=1;
                }
            }
            if(y>0)
                fread(&dData,sizeof(uint16),1,fp);
            for(i=0;i<y;i++){ 
                degiData[dNum++] = (char)dData&0x0001;
                dData >>=1;
            }
        }
    }
    //关闭文件
    fclose(fp);
    return true;
}
    
/**
 * @description: 将采样数据生成.CSV文件
 * @param {filename：文件名，cfgFilHeader：配置文件句柄，anaData：模拟量数据，degiData：状态量数据} 
 * @return: fause:打开文件错误，true：成功生成.CSV文件
 */
bool saveToCSV(char *filename,CFGFILE_HEADER *cfgFilHeader,short *anaData,char *degiData)
{
    FILE *fp;
    uint8 i;
    int cnt;
    int anum=0,dnum=0;
    float value;

    if((fp=fopen(filename,"w"))==NULL){//创建电子表格文件
        printf("open %s error\n",filename);
        return false;
    } 
        
    //模拟量通道名/单位
    for(i=0;i<cfgFilHeader->at-1;i++){
        fprintf(fp,"%s/%s, ",cfgFilHeader->aChaName[i],cfgFilHeader->unit[i]); 
    }
    fprintf(fp,"%s/%s",cfgFilHeader->aChaName[i],cfgFilHeader->unit[i]); 

    //数字量通道名
    for(i=0;i<cfgFilHeader->dt;i++){
        fprintf(fp," ,%s",cfgFilHeader->dChaName[i]); 
    }  
    fprintf(fp,"\n"); 

    //写数据
    for(cnt=0;cnt<cfgFilHeader->endsamp;cnt++){ 
        for(i=0;i<cfgFilHeader->at-1;i++){ //模拟量  
            if(cfgFilHeader->ps[i]=='S') {
                //二次测的值=ax+b
                value=anaData[anum++]*cfgFilHeader->a[i]+cfgFilHeader->b[i];
                fprintf(fp,"%.4f, ",value);
            } 
            else{
                //一次测的值=(ax+b)*二次变比/一次变比
                value=(anaData[anum++]*cfgFilHeader->a[i]+cfgFilHeader->b[i])*cfgFilHeader->prim[i]/cfgFilHeader->second[i];
                fprintf(fp,"%.4f, ",value);
            }
        }
        if(cfgFilHeader->ps[i]=='S'){
            value=anaData[anum++]*cfgFilHeader->a[i]+cfgFilHeader->b[i];
            fprintf(fp,"%.4f",value);
        }
        else{
            //一次测的值=(ax+b)*二次变比/一次变比
            value=(anaData[anum++]*cfgFilHeader->a[i]+cfgFilHeader->b[i])*cfgFilHeader->prim[i]/cfgFilHeader->second[i];
            fprintf(fp,"%.4f",value);
        }

        for(i=0;i<cfgFilHeader->dt;i++){
            fprintf(fp," ,%d",degiData[dnum++]);
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
    return true;
}