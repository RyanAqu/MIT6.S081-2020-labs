#include "kernel/types.h"  //一定要放在最前面

#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"


void find(const char* path,const char* filename)
{
    //1. 打开目录
    //2. 获取文件信息结构体
    //3. 判断是目录还是文件（文件类型）
    //4. 检查路径长度是否超出缓冲区
    //5. 开始查找
    

    //1
    int fd;//文件描述符，用于存储打开的文件目录
    if((fd=open(path,O_RDONLY))<0)//尝试打开path
    {
        fprintf(2,"find: cannot open %s!\n",path);
        return;
    }
    //2
    struct stat st;//文件信息结构体，包括设备号，Inode号，链接数，文件类型，文件大小（bytes）
    if((fstat(fd,&st))<0)//读取文件信息,以便查看文件类型
    {
        fprintf(2,"find: cannot fstat %s!\n",path);
        close(fd);
        return ;
    }
    //3 path必须是目录
    if(st.type!=T_DIR)
    {
        fprintf(2,"Usage : find <directory> <filename> .\n");
        close(fd);
        return ;
    }
    //4
    char buf[512];//存储路径的buffer
    if(strlen(path)+1+DIRSIZ+1>sizeof(buf))//
    {
        fprintf(2, "find: path too long\n");
        close(fd);
        return ;
    }

    //5
    char* p;
    strcpy(buf,path);
    p=buf+strlen(buf);
    *p='/';p++;

    struct dirent de;//文件的目录项结构体，包括文件索引节点号，文件名
    while(read(fd,&de,sizeof(de))==sizeof(de))//开始读取目录下的文件
    {
        if(de.inum==0)continue;

        //这一步把完整路径写出来
        memmove(p,de.name,DIRSIZ);//添加路径名称
        p[DIRSIZ]=0;//添加结束符

        //这一步查询目录下文件的信息，是文件需要比对，如果是目录需要递归查询
        if(stat(buf,&st)<0)
        {
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;//打不开这个文件就跳过
        }

        //假如是目录，就去递归查询；但不递归 . 和 ..
        if(st.type==T_DIR&&strcmp(p,".")!=0&&strcmp(p,".."))
        {
            find(buf,filename);
        }
        else if(strcmp(p,filename)==0)//找到了
        {
            printf("%s\n",buf);
        }

    }
    close(fd);


}

int main(int argc, char* argv[])
{
    if(argc!=3)
    {
        fprintf(2,"Usage : find <directory> <filename> .\n");
        exit(1);
    }
    find(argv[1],argv[2]);
    exit(0);
}

