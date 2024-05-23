// Kinect_opengl.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "main.h"
#include "Kinect.h"
#include "Serial.h"
#include "Client.h"
#include "Data.h"

void ui()
{
	while (true)
	{
		system("cls");
		cout << "실행 동작을 골라주세요." << endl;
		cout << "===============================" << endl;
		cout << "[1] Kinect 연결" << endl;
		cout << "[2] Viewer" << endl;
		cout << "[3] Kinect 연결 해제" << endl;
		cout << "[4] imu 연결" << endl;
		cout << "[5] 동시 실행" << endl;
		cout << "[6] 종료합니다." << endl;
		cout << "[7] 서버 연결." << endl;
		cout << "===============================" << endl;

		switch (_getch())
		{
		case'1': connet();				break;
		case'2': thread_viewer();		break;
		case'3': connect_cut();			break;
		case'4': sensor_thread();		break;
		case'5': main_thread();			break;
		case'6': return;				//모든 센서 out
		case'7': start_client();		break;
		case'8': python();		break;
		}

		system("pause");
	}
}

void python()
{
	PyObject* pName, * pModule, * pFunc, * pValue;
	Py_Initialize();
	pName = PyUnicode_FromString("testFile"); // testFile.py를 PyObject로 생성한다.
	pModule = PyImport_Import(pName); // 생성한 PyObject pName을 import한다.
	pFunc = PyObject_GetAttrString(pModule, "print_"); // 실행할 함수인 test_func을 PyObject에 전달한다.
	pValue = PyObject_CallObject(pFunc, NULL); // pFunc에 매개변수를 전달해서 실행한다. 현재 매개변수가 NULL인 경우이다.
	Py_Finalize();
	system("pause");

}

void main_thread() 
{
	thread t1(color_viewer);
	thread t2(body_viewer);
	thread t3(imu_start);
	start_client();

	t1.join();
	t2.join();
	t3.join();
}

int main()
{
	ui();

	return 0;

}