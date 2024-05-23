#include <iostream>
#include <direct.h>		//mkdir
#include <errno.h>		//errno
#include <time.h> // C언어 time
#include <ctime> // C++ time
#include <chrono>
#include <string>
#include <cstring>
#include <fstream> //ofstream 파일 쓰기에 필요한 헤더
#include <direct.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <thread>

using namespace std; using namespace std::chrono;

#include "Data.h"

#pragma region Values
char curDir[1000];
ofstream save_file_kinect;
ofstream save_file_imu;

#pragma endregion


void write_kinect_file() 
{
	string kinect = "/data/Kinect_";
	string printed_time = stamp_time(1);
	string file_format = ".csv";

	_getcwd(curDir, 1000);	//현재 경로

	save_file_kinect.open(curDir + kinect + printed_time + file_format);
	save_file_kinect << "TIME, NECK_x,NECK_y,NECK_z,";
	save_file_kinect << "SHOULDER_LEFT_x, SHOULDER_LEFT_y, SHOULDER_LEFT_z,";
	save_file_kinect << "ELBOW_LEFT_x, ELBOW_LEFT_y, ELBOW_LEFT_z, "; //저장 항목
	save_file_kinect << "WRIST_LEFT_x, WRIST_LEFT_y, WRIST_LEFT_z, ";
	save_file_kinect << "HAND_LEFT_x, HAND_LEFT_y, HAND_LEFT_z,";
	save_file_kinect << "SHOULDER_RIGHT_x, SHOULDER_RIGHT_y, SHOULDER_RIGHT_z,";
	save_file_kinect << "ELBOW_RIGHT_x, ELBOW_RIGHT_y, ELBOW_RIGHT_z, "; //저장 항목
	save_file_kinect << "WRIST_RIGHTT_x, WRIST_RIGHT_y, WRIST_RIGHT_z, ";
	save_file_kinect << "HAND_RIGHT_x, HAND_RIGHT_y, HAND_RIGHT_z";
	save_file_kinect << "PELVIS_x, PELVIS_y, PELVIS_z";
	save_file_kinect << "ANKLE_LEFT_x, ANKLE_LEFT_y, ANKLE_LEFT_z";
	save_file_kinect << "ANKLE_RIGHT_x, ANKLE_RIGHT_y, ANKLE_RIGHT_z\n";
}

void write_imu_file()
{
	string imu = "/data/imu_";
	string printed_time = stamp_time(1);
	string file_format = ".csv";

	_getcwd(curDir, 1000);	//현재 경로

	save_file_imu.open(curDir + imu + printed_time + file_format);
	save_file_imu << "TIME, ID, w, x, y, z, x, y, z, battery\n"; //저장 항목

}


string stamp_time(int date_type) 
{
	time_t timer;
	struct tm today;
	timer = time(NULL); // 현재 시각을 초 단위
	localtime_s(&today, &timer); // 초 단위의 시간을 분리
	char now_time[50];
	string s_time;
	
	chrono::milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	if (date_type == 1) //파일 이름
	{
		sprintf_s(now_time,
			"%02d%02d%02d_%02d%02d%02d", // YYMMDD_HHMMSS
			today.tm_year - 100, // 만약 4자리 연도 쓰고 싶으면 1900 더하면 됨
			today.tm_mon + 1,
			today.tm_mday,
			today.tm_hour,
			today.tm_min,
			today.tm_sec
			);
		string temp_1_time(now_time);
		s_time = temp_1_time;
	}
	if (date_type == 2)	//저장할때 데이터에 사용되는 시간
	{
		sprintf_s(now_time,
			"%02d:%02d:%02d:%d", // YYMMDD_HHMMSS
			today.tm_hour,
			today.tm_min,
			today.tm_sec,
			ms
		);
		string temp_2_time(now_time);
		s_time = temp_2_time;
	}

	return s_time;
}

void write_file(char* data)
{	
	save_file_kinect << data;
}

void write_file2(char* data)
{
	save_file_imu << data;
}

void close_file() 
{
	save_file_kinect.close();
}

void close_file2()
{
	save_file_imu.close();
}

void multi_csv_kinect() 
{
	thread kinect_csv(write_kinect_file);

	kinect_csv.join();
}

void multi_csv_imu()
{
	thread imu_csv(write_imu_file);

	imu_csv.join();
}