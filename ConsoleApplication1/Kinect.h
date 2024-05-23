#pragma once
#pragma comment(lib, "k4a.lib")

#include <stdio.h>

#include <stdlib.h>
#include <thread>
#include <vector>

		//joint name			parent joint name
#define NECK 3
#define SHOULDER_LEFT 5			//CLAVICLE_LEFT
#define ELBOW_LEFT 6			//SHOULDER_LEFT
#define WRIST_LEFT 7			//ELBOW_LEFT
#define HAND_LEFT 8				//WRIST_LEFT
#define ANKLE_LEFT 20			//ANKLE_LEFT

#define HEAD 26

#define SHOULDER_RIGHT 12
#define ELBOW_RIGHT 13
#define WRIST_RIGHT 14			//ELBOW_LEFT
#define HAND_RIGHT 15			//WRIST_LEFT
#define ANKLE_RIGHT 24			//ANKLE_RIGHT

#define PELVIS 0				//WRIST_LEFT



using namespace std;


void connet();
int color_viewer();

int body_viewer();

void thread_viewer();

void connect_cut();