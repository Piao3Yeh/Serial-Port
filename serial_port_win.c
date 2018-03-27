/**
 * Name:Serial Port Send example
 * Date:3/27/2018
 * Auth:Blackbamboo
 * Description:
 *          Open 'COM10' and send from 1,
 *      each time increasing by 1, until sending one hundred thousand bytes.
 *   Press the button 'g' at the time of sending, and press other keys to stop sending
 *
 *   Just a simple case.Maybe helpful to someone.
 * Environment(what i used):
 *      windows10, CodeBlocks 17.12，
 *
*/


#include "stdio.h"
#include "Windows.h"
#include "string.h"
#include "stdlib.h"


/*
 * ws2_32.lib文件，提供了对以下网络相关API的支持，
 * 若使用其中的API，则应该将ws2_32.lib加入工程
 * （否则需要动态载入ws2_32.dll）
*/
#pragma comment(lib,"ws2_32.lib")

int main(void)
{
	HANDLE hCom;//申请句柄
	//打开串口
	//串口号大于等于10时串口前需要加'\\\\.\\',否则不加
	hCom = CreateFile(TEXT("\\\\.\\COM10"),//com号
		GENERIC_READ | GENERIC_WRITE,      //允许读写
		0,              //独占模式
		NULL,           //安全属性
		OPEN_EXISTING,  //方式为打开而不是创建
		0,              //文件属性
		NULL);
	if (hCom == INVALID_HANDLE_VALUE) {
		printf("wrong");
		exit(EXIT_FAILURE);
	}
	else {
		printf("OK\n");
	}
	SetupComm(hCom, 256, 256);//设置输入输出缓冲区

	COMMTIMEOUTS Timeouts;

	//初始化读超时结构
	Timeouts.ReadIntervalTimeout = 1000;        //读两个字符间最大延时
	Timeouts.ReadTotalTimeoutMultiplier = 500;  //读每个字符的超时
	Timeouts.ReadTotalTimeoutConstant = 5000;   //一次读取操作的超时
	/*一次读取串口数据的固定超时。所以在一次读取串口的操作中，
	其超时为ReadTotalTimeoutMultiplier乘以读取的字节数再加上
	ReadTotalTimeoutConstant。将ReadIntervalTimeout设置为MAXDWORD，
	并将ReadTotalTimeoutMultiplier 和ReadTotalTimeoutConstant设置为0，
	表示读取操作将立即返回存放在输入缓冲区的字符。
	*/



	//初始化写超时结构
	Timeouts.WriteTotalTimeoutMultiplier = 500; //写入每个字符的间的超时
	Timeouts.WriteTotalTimeoutConstant = 2000;  //一次写操作的超时
	SetCommState(hCom, &Timeouts);              //用结构体设置超时
	DCB dcb;                    //DCB为设置串口的一个结构体
	dcb.BaudRate = 9600;        //波特率9600
	dcb.ByteSize = 8;           //8位数据位
	dcb.Parity = NOPARITY;      //无校验位
	dcb.StopBits = ONE5STOPBITS;//1位停止位
	SetCommState(hCom, &dcb);   //设置串口参数

	DWORD wCount=0;
	int wSend = 0,sendCt=0;
	BOOL bStat;
	char strNum[20] = { 0 };


	while (1) {
        printf("input g to run ,others will stop\n");
        while(1){
            if(getch() == 'g')break;
        }
		for (wSend = 100000; wCount <= 200000; wSend++) {
            //清空输入输出缓冲区
			PurgeComm(hCom, PURGE_RXCLEAR | PURGE_TXCLEAR);
			sprintf_s(strNum,20, "%d", wSend);//将数字转为字符
			printf("%s,%d\t", strNum,strlen(strNum));
            //检验是否有按键按下，为'g'则继续运行，否则停止直到按'g'
			while((kbhit())){
			    while(1){
                    if(getch() == 'g') break;
                }
			}
			//发送字符
			bStat = WriteFile(hCom,//句柄
				strNum,         //发送的字符串地址
				strlen(strNum), //要发送的字符个数
				&sendCt,        //实际发送的字节数返回给该值
				NULL            //不使用模板
				);
            //验证是否发送成功
			if (!bStat){
				printf("写串口失败!");
				return FALSE;
			}

			/*
			串口接收函数
             bStat = ReadFile(hCom,     //句柄
                              strNum,   //接收字符串的缓冲区地址
                              20,       //要接收的字符个数
                              &sendCt,  //实际接收的字节数返回给该值
                              NULL      //不使用模板
                             );
            */

			//计数所发送的字节
            wCount = wCount + sendCt;
			Sleep(20);//ms
			printf("count:%ld\n", wCount);

		}
		//暂停，然后清空计数
		system("pause");
		wCount = 0;
	}
	//释放句柄
    CloseHandle( hCom );
}
