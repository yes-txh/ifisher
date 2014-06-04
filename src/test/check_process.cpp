#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

int main(int argc, char **argv)
{
	FILE *pstr; char cmd[128],buff[512],*p;
        pid_t pID;
        int pidnum;
	printf("argc:%d", argc);
	if(argc != 2) {
	    printf("yes\n");
	    return -1;
        }
	char *name= argv[1];
	int ret=3;
	memset(cmd,0,sizeof(cmd));

	sprintf(cmd, "ps -ef|grep %s ",name);
	pstr=popen(cmd, "r");
	if(pstr==NULL){ 
	    return 1; 
	}
	memset(buff,0,sizeof(buff));
	fgets(buff,512,pstr);
	p=strtok(buff, " ");
	p=strtok(NULL, " "); //这句是否去掉，取决于当前系统中ps后，进程ID号是否是第一个字段 pclose(pstr);
	if(p==NULL)
	{ return 1; }
	if(strlen(p)==0)
	{ return 1; }
	if((pidnum=atoi(p))==0)
	{ return 1; }
	printf("pidnum: %d\n",pidnum);
	pID=(pid_t)pidnum;
	ret=kill(pID,0);//这里不是要杀死进程，而是验证一下进程是否真的存在，返回0表示真的存在
	printf("ret= %d \n",ret);
	if(0==ret)
	    printf("process: %s exist!\n",name);
	else 
	    printf("process: %s not exist!\n",name);
return 0;
}
