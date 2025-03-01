#include "kernel/types.h"
#include "user/user.h"

int mian(int argc,char* argv[])
{
    if(argc!=2)
    {
        fprintf(2,"Usage : sleep <time>. \n");
        exit(1);
    }
    sleep(atoi(argv[1]));
    fprintf(1,"Wake After Sleep.\n");
    exit(0);
}