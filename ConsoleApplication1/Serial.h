#pragma once
#include "Data.h"

#define MY_SERIALPORT  3  // ����� �ø��� ��Ʈ��ȣ

#define SBUF_SIZE 2000

#define NUMBEROFPORT 4      //�� �� �ִ� ��Ʈ ��
#define RBUF_SIZE 1024		//���Ź���ũ��
#define TBUF_SIZE 1024      //�۽Ź���ũ��

#define READ_SIZE 10		//�ѹ��� ���� ������
#define   ERR_OK               0
#define   ERR_CREATEFILE      -1
#define   ERR_SETUPCOMM       -2
#define   ERR_GETCOMMSTATE    -3
#define   ERR_SETCOMMSTATE    -4
#define   ERR_GETCOMMTIMEOUT  -5
#define   ERR_WRITEFILE       -6
#define   ERR_READFILE        -7
#define   ERR_CLOSEHANDLE     -8


typedef struct {
	unsigned int  nSize;
	char  szData[READ_SIZE];
} SERIALREADDATA;

int OpenSerialPort(int nPort, unsigned long nBaudRate, int nParityBit, int nDataBit, int nStopBit);
int CloseSerialPort(int nPort);
int WriteSerialPort(int nPort, unsigned char* szData, unsigned int nBytesToWrite);
int ReadSerialPort(int nPort, SERIALREADDATA* SerialReadData);

char* my_strtok(char* str, char dm, int* result);
int EBimuAsciiParser(int* id, float* item, unsigned int number_of_item);

void imu_start();
void sensor_thread();
