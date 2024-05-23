// 소켓을 사용하기 위해서 라이브러리 참조해야 한다.
#pragma comment(lib, "ws2_32")
// inet_ntoa가 deprecated가 되었는데.. 사용하려면 아래 설정을 해야 한다.
#pragma warning(disable:4996)
#include <stdio.h>
#include <iostream>
#include <vector>
#include <thread>
#include <conio.h>
// 소켓을 사용하기 위한 라이브러리
#include <WinSock2.h>
#include <Windows.h>

// 수신 버퍼 사이즈
#define BUFFERSIZE 1024
using namespace std;

float datas[8];
char origin_data[1024];	//imu data
char kinect_data[1024];	//kinect data

#include "Client.h"

// 콘솔에 메시지를 출력하는 함수
void print(vector<char>* str)
{
	// 포인트 위치
	int p = 0;
	// 버퍼 설정. +1은 \0를 넣기 위한 크기
	char out[BUFFERSIZE + 1];
	// 콘솔 출력
	cout << "From server message : ";
	for (int n = 0; n < (str->size() / BUFFERSIZE) + 1; n++)
	{
		// 버퍼 사이즈 설정
		int size = str->size();
		// 수신 데이터가 버퍼 사이즈를 넘었을 경우.
		if (size > BUFFERSIZE) {
			if (str->size() < (n + 1) * BUFFERSIZE)
			{
				size = str->size() % BUFFERSIZE;
			}
			else
			{
				size = BUFFERSIZE;
			}
		}
		// echo 메시지와 콘솔 메시지를 작성한다.
		for (int i = 0; i < size; i++, p++)
		{
			out[i] = *(str->begin() + p);
		}
		// 콘솔 메시지 콘솔 출력.
		cout << out;
	}
}

void receive_data(char* data)
{	
	//for (int i = 0; i < 1024; i++) { origin_data[i] = {}; }
	//데이터 전송하기 전에 문자열 초기화 시켜주기
	copy(data, data + 700, origin_data);
}

void receive_k_data(char* data)
{
	//for (int i = 0; i < 1024; i++) { kinect_data[i] = {}; }
	copy(data, data + 700, kinect_data);
}

// 실행 함수
int start_client()
{
	// 소켓 정보 데이터 설정
	WSADATA wsaData;
	// 소켓 실행.
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 1;
	}
	// Internet의 Stream 방식으로 소켓 생성
	SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);
	// 소켓 주소 설정
	SOCKADDR_IN addr;
	// 구조체 초기화
	memset(&addr, 0, sizeof(addr));
	// 소켓은 Internet 타입
	addr.sin_family = AF_INET;
	// 127.0.0.1(localhost)로 접속하기
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	int port_num = 21000;

	printf("enter port_num ");
	//system("pause");
	//cin >> port_num;

	addr.sin_port = htons(port_num);

	// 접속
	if (connect(sock, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		// 에러 콘솔 출력
		cout << "error" << endl;
		return 1;
	}
	else
	{	
		char input[BUFFERSIZE];
		char k;

		if (recv(sock, NULL, NULL, 0) == SOCKET_ERROR)
		{
			// 에러 콘솔 출력
			cout << "error" << endl;
		}			

		if (recv(sock, NULL, NULL, 0) != SOCKET_ERROR)
		{
			while (1) 
			{
#pragma region user_data
				//if (sizeof(datas) <= 0)
				//{
				//	// 유저로 부터 입력 받기
				//	cout << "enter msg" << endl;
				//	cin >> input;

				//	// 입력받은 길이를 받는다.
				//	int size = strlen(input); 
				//	// 개행을 넣는다.
				//	*(input + size + 1) = '\n';
				//	*(input + size + 2) = '\n';

				//	//000을 입력하면 종료
				//	if (strcmp(input, "000") == 0)
				//	{
				//		cout << "bye" << endl;
				//		break;
				//	}

				//	send(sock, input, size + 3, 0);

				//	// 데이터를 받는다. 에러가 발생하면 멈춘다.
				//	//0x0000 : 이전에 누른 적이 없고 호출 시점에도 눌려있지 않은 상태
				//	//0x0001 : 이전에 누른 적이 있고 호출 시점에는 눌려있지 않은 상태
				//	//0x8000 : 이전에 누른 적이 없고 호출 시점에는 눌려있는 상태							 
				//	//0x8001 : 이전에 누른 적이 있고 호출 시점에도 눌려있는 상태
				//}
#pragma endregion 

				if (strcmp(kinect_data, "\0") != 0)
				{
					send(sock, kinect_data, sizeof(kinect_data), 0);
					cout << kinect_data;
				}

				if (strcmp(origin_data, "\0") != 0)
				{					
					send(sock, origin_data, sizeof(origin_data), 0);
					cout << origin_data;
					
				}			
				Sleep(1); //Sleep 안시키면 에러남 1 milliseconds = 0.001 Seconds
			}
			
		}
	}

	// 서버 소켓 종료
	closesocket(sock);
	// 소켓 종료
	WSACleanup();
	return 0;
}

