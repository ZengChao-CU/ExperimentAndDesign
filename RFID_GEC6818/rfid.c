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
	int   uart1_fd= open("/dev/ttySAC1", O_RDWR);//�򿪴���1�豸�ļ�
	if (uart1_fd == -1)
	{
		perror("open error:");
		return -1;
	}
	struct termios myserial;
	//��սṹ��
	memset(&myserial, 0, sizeof (myserial));
	//O_RDWR               
	myserial.c_cflag |= (CLOCAL | CREAD);
	//���ÿ���ģʽ״̬���������ӣ�����ʹ��
	//���� ����λ
	myserial.c_cflag &= ~CSIZE;   //�������λ
	myserial.c_cflag &= ~CRTSCTS; //��Ӳ��������
	myserial.c_cflag |= CS8;      //����λ:8

	myserial.c_cflag &= ~CSTOPB;//   //1λֹͣλ
	myserial.c_cflag &= ~PARENB;  //��ҪУ��

	cfsetospeed(&myserial, B9600);  //���ò�����,B9600�Ƕ���ĺ�
	cfsetispeed(&myserial, B9600);

	/* ˢ���������,��������ܵ����� */
	tcflush(uart1_fd, TCIFLUSH);

	/* �ı����� */
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
				unsigned int id=0;//�״�㣺���id����ʼ������id��ʼֵ��ȷ��������id�������
				for(i=3;i<11;i++)
				{
					id <<= 4; //����4Ϊ���൱�ڳ���16
					id |= ((buf[i]>='0'&&buf[i]<='9')?(buf[i]-'0'):(buf[i]-'A'+10));	
					//����:��Ϊʮ���������Ե�buf[i]ΪA-Fʱ��ת��Ϊ������Ҫ ��+10��
					//ʮ��������A -> 10   B->11  C->12 ...
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
	
