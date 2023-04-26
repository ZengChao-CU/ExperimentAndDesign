#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <string.h>


int  uart1_init()
{	
	int   uart1_fd= open("/dev/ttySAC1", O_RDWR);//打开串口1设备文件
	if (uart1_fd == -1)
	{
		perror("open error:");
		return -1;
	}
	struct termios myserial;
	//清空结构体
	memset(&myserial, 0, sizeof (myserial));
	//O_RDWR               
	myserial.c_cflag |= (CLOCAL | CREAD);
	//设置控制模式状态，本地连接，接受使能
	//设置 数据位
	myserial.c_cflag &= ~CSIZE;   //清空数据位
	myserial.c_cflag &= ~CRTSCTS; //无硬件流控制
	myserial.c_cflag |= CS8;      //数据位:8

	myserial.c_cflag &= ~CSTOPB;//   //1位停止位
	myserial.c_cflag &= ~PARENB;  //不要校验

	cfsetospeed(&myserial, B9600);  //设置波特率,B9600是定义的宏
	cfsetispeed(&myserial, B9600);

	/* 刷新输出队列,清楚正接受的数据 */
	tcflush(uart1_fd, TCIFLUSH);

	/* 改变配置 */
	tcsetattr(uart1_fd, TCSANOW, &myserial);
	return   uart1_fd;
}
unsigned char buf[13];
int main()
{	
	int uart1_fd = uart1_init();
	while(1){
		int r=read(uart1_fd,buf,13);
		if(r==13)
		{
			if(buf[0]!=0x02||buf[12]!=0x03){
				printf("rfid read error!\n");
			}
			else{
				printf("rfid read ok\n");
				int i;
				for(i=0;i<13;i++)
				{
					printf("Original ID :%x ",buf[i]);
				}
				printf("\n");
				unsigned int id=0;//易错点：如果id不初始化，则id初始值不确定，导致id输出错误
				for(i=3;i<11;i++)
				{
					id <<= 4; //左移4为，相当于乘以16
					id |= ((buf[i]>='0'&&buf[i]<='9')?(buf[i]-'0'):(buf[i]-'A'+10));	
					//解释:因为十六进制所以当buf[i]为A-F时，转化为整数需要 ‘+10’
					//十六进制中A -> 10   B->11  C->12 ...
				}
				printf("Final ID:%d",id);
				printf(" ");
				unsigned int id1=0;
				for(i=4;i<=6;i++)
				{
					id1 <<= 4;
					id1 |= ((buf[i]>='0'&&buf[i]<='9')?(buf[i]-'0'):(buf[i]-'A'+10));

				}
				printf("%d",id1);
				printf(",");
				unsigned int id2=0;
				for(i=7;i<11;i++)
				{
					id2 <<= 4 ;
					id2 |= ((buf[i]>='0'&&buf[i]<='9')?(buf[i]-'0'):(buf[i]-'A'+10));
					
				}
				printf("%d",id2);
				printf("\n");
			}
			
		}
		close(uart1_fd);
	}
	return 0;
}
	
