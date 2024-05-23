#pragma once
#ifdef _DEBUG
#define _DEBUG_WAS_DEFINED 1
#undef _DEBUG
#endif
#include <Python.h>
#ifdef _DEBUG_WAS_DEFINED
#define _DEBUG 1
#endif
//Python 은 기본적으로 Release 모드에서만 작동 되도록 설정되어 있어서 Debug 시에도 작동 할 수 있도록 바꾸어 주어야 함

#include <stdio.h>

#include <stdlib.h>
#include <iostream>

#include <conio.h>

using namespace std;

void ui();
void python();
void main_thread();
int main();