#include <stdio.h>
#include <Windows.h>
#include <thread>
#include <string>

#include "Serial.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Client.h"

using namespace std;

char sbuf[SBUF_SIZE];
signed int sbuf_cnt = 0;

char csv_imu_data[1024];
char seding_imu[1024];

unsigned int nTxState = 0, nRxState = 0;  //status bar의 rx tx 전송상태 클리어 시간 카운트
/*
Parity
Parity scheme to be used. This member can be one of the following values. Value Meaning
===================
EVENPARITY	Even
MARKPARITY	Mark
NOPARITY	No parity
ODDPARITY	Odd
SPACEPARITY	Space
===================

StopBits
Number of stop bits to be used. This member can be one of the following values. Value Meaning
============================
ONESTOPBIT		1 stop bit
ONE5STOPBITS	1.5 stop bits
TWOSTOPBITS		2 stop bits
============================
*/
HANDLE hSerialPort[NUMBEROFPORT];
int OpenSerialPort(int nPort, unsigned long nBaudRate, int nParityBit, int nDataBit, int nStopBit)
{
	DCB dcb;
	COMMTIMEOUTS ct;
	LPCWSTR str;

	//wsprintf(str, "\\\\.\\COM%d", nPort);
	if ((hSerialPort[nPort - 1] = CreateFile((L"\\\\.\\COM3"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
		return ERR_CREATEFILE;

	if (!SetupComm(hSerialPort[nPort - 1], RBUF_SIZE, TBUF_SIZE)) 
		return ERR_SETUPCOMM;

	if (!GetCommState(hSerialPort[nPort - 1], &dcb)) 
		return ERR_GETCOMMSTATE;

	dcb.BaudRate = nBaudRate;
	dcb.ByteSize = nDataBit;
	dcb.Parity = nParityBit;
	dcb.StopBits = nStopBit;
	dcb.fNull = FALSE;   //0x00 송수신가능


	 //흐름제어 : 없음
	dcb.fOutxCtsFlow = false;					// Disable CTS monitoring
	dcb.fOutxDsrFlow = false;					// Disable DSR monitoring
	dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR monitoring
	dcb.fOutX = false;							// Disable XON/XOFF for transmission
	dcb.fInX = false;							// Disable XON/XOFF for receiving
	dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
/*
	 // 흐름제어 : 하드웨어(DTR,RTS,CTS,DSR)
		dcb.fOutxCtsFlow = true;					// Enable CTS monitoring
		dcb.fOutxDsrFlow = true;					// Enable DSR monitoring
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;	// Enable DTR handshaking
		dcb.fOutX = false;							// Disable XON/XOFF for transmission
		dcb.fInX = false;							// Disable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;	// Enable RTS handshaking

	 // 흐름제어 : 소프트웨어(Xon,Xoff)
		dcb.fOutxCtsFlow = false;					// Disable CTS (Clear To Send)
		dcb.fOutxDsrFlow = false;					// Disable DSR (Data Set Ready)
		dcb.fDtrControl = DTR_CONTROL_DISABLE;		// Disable DTR (Data Terminal Ready)
		dcb.fOutX = true;							// Enable XON/XOFF for transmission
		dcb.fInX = true;							// Enable XON/XOFF for receiving
		dcb.fRtsControl = RTS_CONTROL_DISABLE;		// Disable RTS (Ready To Send)
*/


	if (!SetCommState(hSerialPort[nPort - 1], &dcb)) return ERR_SETCOMMSTATE;
	if (!GetCommTimeouts(hSerialPort[nPort - 1], &ct)) return ERR_GETCOMMTIMEOUT;

	ct.ReadIntervalTimeout = 100;
	ct.ReadTotalTimeoutMultiplier = 0;
	ct.ReadTotalTimeoutConstant = 10;
	ct.WriteTotalTimeoutMultiplier = 0;
	ct.WriteTotalTimeoutConstant = 10;

	if (!SetCommTimeouts(hSerialPort[nPort - 1], &ct)) return ERR_GETCOMMTIMEOUT;

	multi_csv_imu();	//csv로 저장하기 위해 파일 생성

	return ERR_OK;
}

int CloseSerialPort(int nPort)
{
	if (!CloseHandle(hSerialPort[nPort - 1])) return ERR_CLOSEHANDLE;

	close_file2();

	return ERR_OK;
}

#define RXTXONTIME 10
/*int WriteSerialPort(int nPort,unsigned char *szData,unsigned int nBytesToWrite)
{	unsigned int nBytesWritten;
	nTxState=RXTXONTIME;   // 상태표시만은 위한 코드
	if (!WriteFile(hSerialPort[nPort-1],szData,nBytesToWrite,(unsigned long *)&nBytesWritten,NULL)) return ERR_WRITEFILE;
	return ERR_OK;
}
*/

int WriteSerialPort(int nPort, unsigned char* szData, unsigned int nBytesToWrite)
{
	unsigned int nBytesWritten;
	unsigned int nBytesCompleted = 0;
	nTxState = RXTXONTIME;   // 상태표시만은 위한 코드
	while (nBytesCompleted < nBytesToWrite)
	{
		if (!WriteFile(hSerialPort[nPort - 1], &szData[nBytesCompleted], nBytesToWrite - nBytesCompleted, (unsigned long*)&nBytesWritten, NULL)) return ERR_WRITEFILE;
		nBytesCompleted += nBytesWritten;
	}
	return ERR_OK;
}



int ReadSerialPort(int nPort, SERIALREADDATA* SerialReadData)
{
	if (!ReadFile(hSerialPort[nPort - 1], SerialReadData->szData, READ_SIZE, (unsigned long*)&SerialReadData->nSize, NULL)) 
		return ERR_READFILE;

	if (SerialReadData->nSize != 0) 
		nRxState = RXTXONTIME;  // 상태표시만은 위한 코드

	return ERR_OK;
}

////////////////////////////////////////////////////////////////////////////
char* my_strtok(char* str, char dm, int* result)
{
	int n;

	*result = 0;

	for (n = 0; n < 100; n++)
	{
		if (str[n] == dm) { *result = 1;  break; }
		if (str[n] == NULL) break;
	}

	return &str[n + 1];
}

int EBimuAsciiParser(int* id, float* item, unsigned int number_of_item)
{
	SERIALREADDATA srd;
	unsigned int n, i;
	char* addr;
	int result = 0;
	int ret;

	if (ReadSerialPort(MY_SERIALPORT, &srd) == ERR_OK)
	{
		if (srd.nSize)
		{			
			for (n = 0; n < srd.nSize; n++)
			{
				//////////////////////////////////////////////////////////////////////
				sbuf[sbuf_cnt] = srd.szData[n];
				
				if (sbuf[sbuf_cnt] == '\r')  // 1줄 수신완료				
				{	
					string temp_data2(sbuf);
					int index = temp_data2.find(",");

					string temp_id_num;	//첫 , 앞부분은 100-1, 100-0 으로 이루어져 있어서 센서 번호 가져오기

					for (int i = 0; i < index; i++) 
					{
						temp_id_num = temp_id_num + temp_data2[i];
					}

					{
						addr = my_strtok(sbuf, '-', &ret);						

						if (ret)
						{
							*id = (float)atoi(addr);
							//item[8] = id;
							addr = my_strtok(sbuf, ',', &ret);
							
							for (i = 0; i < number_of_item; i++)
							{
								item[i] = (float)atof(addr);
								addr = my_strtok(addr, ',', &ret);
							}

							result = 1;
						}

					}

					//csv로 데이터를 저장하기 위해 시간 가져오기
					string captured_time = stamp_time(2);
					char temp_time[1024];
					strcpy(temp_time, captured_time.c_str());

					char c_id[20];

					strcpy(c_id, temp_id_num.c_str());
					
					strcat(temp_time, ",");		//strcat 함수: 문자열 붙이기
					strcat(temp_time, c_id);

					sprintf(csv_imu_data, ",%f,%f,%f,%f,%f,%f,%f,%.0f \n", item[0], item[1], item[2], item[3], item[4], item[5], item[6], item[7]);

					strcat(temp_time, csv_imu_data);				

					write_file2(temp_time);

					//cout << temp_time;

					//서버에 데이터 전송하는 부분
					sprintf(seding_imu, "imu,%s,%f,%f,%f,%f,%f,%f,%f,%.0f \n", temp_id_num.c_str(), item[0], item[1], item[2], item[3], item[4], item[5], item[6], item[7]);

					receive_data(seding_imu);

				}
				else if (sbuf[sbuf_cnt] == '\n')
				{
					sbuf_cnt = -1;
					
				}

				sbuf_cnt++;
				if (sbuf_cnt >= SBUF_SIZE) sbuf_cnt = 0;
				///////////////////////////////////////////////////////////////////////
			}
		}
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////

void imu_start()
{
	int id;	
	float item[9]; //한줄에 읽어올 데이터 개수

	if (OpenSerialPort(MY_SERIALPORT, 921600, NOPARITY, 8, ONESTOPBIT) != ERR_OK)

		{
			printf("\n\rSerialport Error...\n");
			Sleep(2000);
			return;
		}
	else 
	{
		printf("Serialport Connected!\n");
	}

	while (1) 
	{
		if (EBimuAsciiParser(&id, item, 8))
		{
			//printf("%f %f %f %f gyrocope : %f %f %f battery : %.0f \n", item[0], item[1], item[2], item[3], item[4], item[5], item[6], item[7]);
			
			//w, x, y, z, 
			//system("pause");
			//receive_data(item);

			if (GetKeyState(VK_LEFT) == TRUE)
			{
				printf("key pressed\n");
				return;
			}
		}
	}
	
};

void sensor_thread()
{
	/*thread t2_soc(start_client);
	thread t1_imu(imu_start);
	
	t2_soc.join();
	t1_imu.join();*/

	imu_start();
	
}