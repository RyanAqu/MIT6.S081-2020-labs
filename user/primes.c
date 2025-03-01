#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

void sieve(int left_pipe[2])
{
    close(left_pipe[WR]);//关闭写，从左邻居管道读第一个数字
    int prime;
    if(read(left_pipe[RD],&prime,sizeof(prime))!=sizeof(prime))
    {
        fprintf(2,"failed to read prime!\n");
        exit(1);
    }


    printf("prime %d\n", prime);//根据规则，读取的第一个数必是素数，直接打印


    int num;
    int right_pipe[2];
    pipe(right_pipe);  // 创建右侧管道

    if(fork()==0)
    {
        //子进程递归
        close(right_pipe[WR]);
        sieve(right_pipe);
    }
    else
    {
        // 当前父进程筛选数字，读取全部左管道，除以当前数（从2开始）不整除则写入右管道
        close(right_pipe[RD]);
        while (read(left_pipe[RD], &num, sizeof(num)) == sizeof(num)) {
            if (num % prime != 0) {
                write(right_pipe[WR], &num, sizeof(num));
            }
        }
        //读完左管道读就没用了，直接回收，右管道写也没用了，直接关闭
        close(left_pipe[RD]);
        close(right_pipe[WR]);

        wait(0);//等待自己的子进程
    }

    

}

int main(int argc, char* argv[])
{
    int pipefd[2];
    if(pipe(pipefd)==-1)//创建管道
    {
        fprintf(2,"pipe() failed!\n");
        return 1;
    }


    if(fork()==0)//子进程进入递归函数
    {
        sieve(pipefd);
    }
    else
    {//父进程写入2-35
        close(pipefd[RD]);
        for(int i=2;i<=35;++i)
        {
            write(pipefd[WR],&i,sizeof(i));
        }
        close(pipefd[WR]);
        wait(0);
    }
    

    exit(0);
}




