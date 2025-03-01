#include "kernel/types.h"
#include "user/user.h"


#define RD 0
#define WR 1

int main(int argc,char* argv[])
{
    char buffer='P';//创建缓冲

    int child_pipe[2];//创建管道
    int parent_pipe[2];
    pipe(child_pipe);
    pipe(parent_pipe);

    int pid=fork();//创建fork
    int exit_status=0;//退出状态码

    if(pid==0)//子进程
    {
        close(child_pipe[RD]);//写pipe，关闭读
        close(parent_pipe[WR]);//读pipe，关闭写

        if(read(parent_pipe[RD],&buffer,sizeof(char))!= sizeof(char))//判断读是否出错
        {
            fprintf(2,"child read() error!\n");
            exit_status = 1; //标记出错
        }
        else{
            fprintf(1,"%d: received ping.\n",getpid());
        }

        if(write(child_pipe[WR],&buffer,sizeof(char))!=sizeof(char))
        {
            fprintf(2,"%d:child write() error!\n",getpid());
            exit_status = 1; //标记出错
        }
        close(child_pipe[WR]);
        close(parent_pipe[RD]);
        exit(exit_status);

    }
    else if(pid>0)//父进程
    {
        close(parent_pipe[RD]);//写pipe，关闭读
        close(child_pipe[WR]);//读pipe，关闭写

        if(write(parent_pipe[WR],&buffer,sizeof(char))!=sizeof(char))
        {
            fprintf(2,"%d:parent write() error!\n",getpid());
            exit_status = 1; //标记出错
        }


        if(read(child_pipe[RD],&buffer,sizeof(char))!= sizeof(char))//判断读是否出错
        {
            fprintf(2,"parent read() error!\n");
            exit_status = 1; //标记出错
        }
        else{
            fprintf(1,"%d: received pong.\n",getpid());
        }

        
        close(parent_pipe[WR]);
        close(child_pipe[RD]);
        exit(exit_status);
    }
    else//fork失败
    {
        fprintf(2,"fork() failed!\n");
        close(child_pipe[RD]);
        close(child_pipe[WR]);
        close(parent_pipe[RD]);
        close(parent_pipe[WR]);
        exit(1);
    }
    
}


