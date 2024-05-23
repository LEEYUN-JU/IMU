// ������ ����ϱ� ���ؼ� ���̺귯�� �����ؾ� �Ѵ�.
#pragma comment(lib, "ws2_32")
// inet_ntoa�� deprecated�� �Ǿ��µ�.. ����Ϸ��� �Ʒ� ������ �ؾ� �Ѵ�.
#pragma warning(disable:4996)
#include <stdio.h>
#include <iostream>
#include <vector>
#include <thread>
#include <conio.h>
// ������ ����ϱ� ���� ���̺귯��
#include <WinSock2.h>
#include <Windows.h>

// ���� ���� ������
#define BUFFERSIZE 1024
using namespace std;

float datas[8];
char origin_data[1024];	//imu data
char kinect_data[1024];	//kinect data

#include "Client.h"

// �ֿܼ� �޽����� ����ϴ� �Լ�
void print(vector<char>* str)
{
	// ����Ʈ ��ġ
	int p = 0;
	// ���� ����. +1�� \0�� �ֱ� ���� ũ��
	char out[BUFFERSIZE + 1];
	// �ܼ� ���
	cout << "From server message : ";
	for (int n = 0; n < (str->size() / BUFFERSIZE) + 1; n++)
	{
		// ���� ������ ����
		int size = str->size();
		// ���� �����Ͱ� ���� ����� �Ѿ��� ���.
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
		// echo �޽����� �ܼ� �޽����� �ۼ��Ѵ�.
		for (int i = 0; i < size; i++, p++)
		{
			out[i] = *(str->begin() + p);
		}
		// �ܼ� �޽��� �ܼ� ���.
		cout << out;
	}
}

void receive_data(char* data)
{	
	//for (int i = 0; i < 1024; i++) { origin_data[i] = {}; }
	//������ �����ϱ� ���� ���ڿ� �ʱ�ȭ �����ֱ�
	copy(data, data + 700, origin_data);
}

void receive_k_data(char* data)
{
	//for (int i = 0; i < 1024; i++) { kinect_data[i] = {}; }
	copy(data, data + 700, kinect_data);
}

// ���� �Լ�
int start_client()
{
	// ���� ���� ������ ����
	WSADATA wsaData;
	// ���� ����.
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return 1;
	}
	// Internet�� Stream ������� ���� ����
	SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);
	// ���� �ּ� ����
	SOCKADDR_IN addr;
	// ����ü �ʱ�ȭ
	memset(&addr, 0, sizeof(addr));
	// ������ Internet Ÿ��
	addr.sin_family = AF_INET;
	// 127.0.0.1(localhost)�� �����ϱ�
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	int port_num = 21000;

	printf("enter port_num ");
	//system("pause");
	//cin >> port_num;

	addr.sin_port = htons(port_num);

	// ����
	if (connect(sock, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		// ���� �ܼ� ���
		cout << "error" << endl;
		return 1;
	}
	else
	{	
		char input[BUFFERSIZE];
		char k;

		if (recv(sock, NULL, NULL, 0) == SOCKET_ERROR)
		{
			// ���� �ܼ� ���
			cout << "error" << endl;
		}			

		if (recv(sock, NULL, NULL, 0) != SOCKET_ERROR)
		{
			while (1) 
			{
#pragma region user_data
				//if (sizeof(datas) <= 0)
				//{
				//	// ������ ���� �Է� �ޱ�
				//	cout << "enter msg" << endl;
				//	cin >> input;

				//	// �Է¹��� ���̸� �޴´�.
				//	int size = strlen(input); 
				//	// ������ �ִ´�.
				//	*(input + size + 1) = '\n';
				//	*(input + size + 2) = '\n';

				//	//000�� �Է��ϸ� ����
				//	if (strcmp(input, "000") == 0)
				//	{
				//		cout << "bye" << endl;
				//		break;
				//	}

				//	send(sock, input, size + 3, 0);

				//	// �����͸� �޴´�. ������ �߻��ϸ� �����.
				//	//0x0000 : ������ ���� ���� ���� ȣ�� �������� �������� ���� ����
				//	//0x0001 : ������ ���� ���� �ְ� ȣ�� �������� �������� ���� ����
				//	//0x8000 : ������ ���� ���� ���� ȣ�� �������� �����ִ� ����							 
				//	//0x8001 : ������ ���� ���� �ְ� ȣ�� �������� �����ִ� ����
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
				Sleep(1); //Sleep �Ƚ�Ű�� ������ 1 milliseconds = 0.001 Seconds
			}
			
		}
	}

	// ���� ���� ����
	closesocket(sock);
	// ���� ����
	WSACleanup();
	return 0;
}

