#pragma once
#include <string>
#include <vector>
using namespace std;

#define Data_size 10000

string stamp_time(int date_type);

void write_file(char* data);
void write_file2(char* data);

void close_file();
void close_file2();

void write_kinect_file();
void write_imu_file();

void multi_csv_kinect();
void multi_csv_imu();