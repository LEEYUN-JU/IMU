#pragma once
#ifdef _DEBUG
#define _DEBUG_WAS_DEFINED 1
#undef _DEBUG
#endif
#include <Python.h>
#ifdef _DEBUG_WAS_DEFINED
#define _DEBUG 1
#endif
//Python �� �⺻������ Release ��忡���� �۵� �ǵ��� �����Ǿ� �־ Debug �ÿ��� �۵� �� �� �ֵ��� �ٲپ� �־�� ��

#include <stdio.h>

#include <stdlib.h>
#include <iostream>

#include <conio.h>

using namespace std;

void ui();
void python();
void main_thread();
int main();