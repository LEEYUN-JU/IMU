
// MFC_opengl_clientDlg.cpp: 구현 파일
//
#include "pch.h"
#include "framework.h"
#include "MFC_opengl_client.h"
#include "MFC_opengl_clientDlg.h"
#include "afxdialogex.h"

#include <stdio.h>
#include <Windows.h>
#include <math.h>
#include <string>
#include <iostream>
#include <cmath>

#include <GL/glut.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#include <Eigen/Dense>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "ConnectServer.h"

using namespace std;
using namespace Eigen;

#pragma region kinect_datas
GLfloat nect_x;				GLfloat nect_y;				GLfloat nect_z;

GLfloat left_sholder_x;		GLfloat left_sholder_y;		GLfloat left_sholder_z;
GLfloat elbow_left_x;		GLfloat elbow_left_y;		GLfloat elbow_left_z;
GLfloat left_wrist_x;		GLfloat left_wrist_y;		GLfloat left_wrist_z;
GLfloat left_hand_x;		GLfloat left_hand_y;		GLfloat left_hand_z;

GLfloat right_sholder_x;	GLfloat right_sholder_y;	GLfloat right_sholder_z;
GLfloat elbow_right_x;		GLfloat elbow_right_y;		GLfloat elbow_right_z;
GLfloat right_wrist_x;		GLfloat right_wrist_y;		GLfloat right_wrist_z;
GLfloat right_hand_x;		GLfloat right_hand_y;		GLfloat right_hand_z;

GLfloat pelvis_x;			GLfloat pelvis_y;			GLfloat pelvis_z;

GLfloat left_ankle_x;		GLfloat left_ankle_y;		GLfloat left_ankle_z;
GLfloat right_ankle_x;		GLfloat right_ankle_y;		GLfloat right_ankle_z;

GLfloat kinect_data[36] = { nect_x , nect_y , nect_z ,
							left_sholder_x , left_sholder_y , left_sholder_z ,
							elbow_left_x,  elbow_left_y , elbow_left_z,
							left_wrist_x , left_wrist_y , left_wrist_z ,
							left_hand_x , left_hand_y , left_hand_z,

							right_sholder_x , right_sholder_y , right_sholder_z ,
							elbow_right_x,  elbow_right_y , elbow_right_z,
							right_wrist_x , right_wrist_y , right_wrist_z ,
							right_hand_x , right_hand_y , right_hand_z,

							pelvis_x,  pelvis_y , pelvis_z,
							left_ankle_x,	left_ankle_y,	left_ankle_z,
							right_ankle_x,	right_ankle_y,	right_ankle_z};

float m_Result[12][2];
float m_Result_eigen[12][2];

#pragma endregion

#pragma region imu_data
GLfloat imu_0_q_w;		GLfloat imu_0_q_x;		GLfloat imu_0_q_y;		GLfloat imu_0_q_z;
GLfloat imu_0_data[4] = { imu_0_q_w , imu_0_q_x , imu_0_q_y , imu_0_q_z };

GLfloat imu_1_q_w;		GLfloat imu_1_q_x;		GLfloat imu_1_q_y;		GLfloat imu_1_q_z;
GLfloat imu_1_data[4] = { imu_1_q_w , imu_1_q_x , imu_1_q_y , imu_1_q_z };

GLfloat imu_0_x;		GLfloat imu_0_y;		GLfloat imu_0_z;
GLfloat imu_0_a[3] = { imu_0_x,		imu_0_y,		imu_0_z};

GLfloat imu_1_x;		GLfloat imu_1_y;		GLfloat imu_1_z;
GLfloat imu_1_a[3] = { imu_1_x,		imu_1_y,		imu_1_z };

#pragma endregion

// CMFCopenglclientDlg 대화 상자

CMFCopenglclientDlg::CMFCopenglclientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_OPENGL_CLIENT_DIALOG, pParent)
	, kinect_view(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); 
}

void CMFCopenglclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_Port_num, port_num);
	DDX_Radio(pDX, IDC_RADIO1, kinect_view);
}

BEGIN_MESSAGE_MAP(CMFCopenglclientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_COMMAND(IDD_MFC_OPENGL_CLIENT_DIALOG, &CMFCopenglclientDlg::GLRenderScene)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCopenglclientDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCopenglclientDlg::OnBnClickedButton2)
	ON_WM_TIMER()
	ON_WM_MOUSEHWHEEL()
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCopenglclientDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CMFCopenglclientDlg 메시지 처리기

BOOL CMFCopenglclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	//kinect skeleton viewer
	GetDlgItem(IDC_BODY)->GetClientRect(sizerect);
	RW = sizerect.right;
	RH = sizerect.bottom;
	GLinit();
	SetTimer(1, 20, 0);

	//kinect skeleton viewer_2d
	GetDlgItem(IDC_BODY_2D)->GetClientRect(sizerect_2d);
	RW_2d = sizerect_2d.right;
	RH_2d = sizerect_2d.bottom;
	GLinit_2d();
	SetTimer(2, 20, 0);

	//imu_0 
	GetDlgItem(IDC_IMU0)->GetClientRect(sizerect_imu0);
	RW_imu0 = sizerect_imu0.right;
	RH_imu0 = sizerect_imu0.bottom;
	GLinit_imu0();
	SetTimer(3, 20, 0);

	//imu_1 
	GetDlgItem(IDC_IMU1)->GetClientRect(sizerect_imu1);
	RW_imu1 = sizerect_imu1.right;
	RH_imu1 = sizerect_imu1.bottom;
	GLinit_imu1();
	SetTimer(4, 20, 0);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCopenglclientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();

		/*wglMakeCurrent(m_hDC, m_hRC);
		GLRenderScene();
		SwapBuffers(m_hDC);
		wglMakeCurrent(m_hDC, NULL);*/
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCopenglclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCopenglclientDlg::GLinit()
{
	int nPixelFormat;
	m_cDC = new CClientDC(GetDlgItem(IDC_BODY));

	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0,0,0,0,0,0,
		0,0,
		0,0,0,0,0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,0,0
	};

	nPixelFormat = ChoosePixelFormat(m_cDC->GetSafeHdc(), &pfd);
	VERIFY(SetPixelFormat(m_cDC->GetSafeHdc(), nPixelFormat, &pfd));
	m_hRC = wglCreateContext(m_cDC->GetSafeHdc());
	VERIFY(wglMakeCurrent(m_cDC->GetSafeHdc(), m_hRC));
}

void CMFCopenglclientDlg::GLinit_2d()
{
	int nPixelFormat;
	m_cDC_2d = new CClientDC(GetDlgItem(IDC_BODY_2D));

	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0,0,0,0,0,0,
		0,0,
		0,0,0,0,0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,0,0
	};

	nPixelFormat = ChoosePixelFormat(m_cDC_2d->GetSafeHdc(), &pfd);
	VERIFY(SetPixelFormat(m_cDC_2d->GetSafeHdc(), nPixelFormat, &pfd));
	m_hRC_2d = wglCreateContext(m_cDC_2d->GetSafeHdc());
	VERIFY(wglMakeCurrent(m_cDC_2d->GetSafeHdc(), m_hRC_2d));
}

void CMFCopenglclientDlg::GLinit_imu0()
{
	int nPixelFormat;
	m_cDC_imu0 = new CClientDC(GetDlgItem(IDC_IMU0));

	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0,0,0,0,0,0,
		0,0,
		0,0,0,0,0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,0,0
	};

	nPixelFormat = ChoosePixelFormat(m_cDC_imu0->GetSafeHdc(), &pfd);
	VERIFY(SetPixelFormat(m_cDC_imu0->GetSafeHdc(), nPixelFormat, &pfd));
	m_hRC_imu0 = wglCreateContext(m_cDC_imu0->GetSafeHdc());
	VERIFY(wglMakeCurrent(m_cDC_imu0->GetSafeHdc(), m_hRC_imu0));
}

void CMFCopenglclientDlg::GLinit_imu1()
{
	int nPixelFormat;
	m_cDC_imu1 = new CClientDC(GetDlgItem(IDC_IMU1));

	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0,0,0,0,0,0,
		0,0,
		0,0,0,0,0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,0,0
	};

	nPixelFormat = ChoosePixelFormat(m_cDC_imu1->GetSafeHdc(), &pfd);
	VERIFY(SetPixelFormat(m_cDC_imu1->GetSafeHdc(), nPixelFormat, &pfd));
	m_hRC_imu1 = wglCreateContext(m_cDC_imu1->GetSafeHdc());
	VERIFY(wglMakeCurrent(m_cDC_imu1->GetSafeHdc(), m_hRC_imu1));
}

int CMFCopenglclientDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	

	return 0;
}


void CMFCopenglclientDlg::GLResize(int cx, int cy)
{
	GLfloat fAspect;

	if (cy == 0) cy = 1;

	glViewport(0, 0, cx, cy);

	fAspect = (GLfloat)cx / (GLfloat)cy;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(90.0f, fAspect, 1.0f, -100.0f); //첫번째 인자로 높이 조절

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void CMFCopenglclientDlg::GLRenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
	//glClearColor(0.0f, 0.5f, 1.0f, 0.9f); //baby blue
	//glClearColor(0.6f, 0.5f, 1.0f, 0.9f); //Lilac
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);//white
	//glClearColor(0.0f, 1.0f, 1.0f, 1.0f);//light blue
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);//black

	glMatrixMode(GL_MODELVIEW); //모델 좌표계와 시점 좌표계의 공간을 앞으로 계산하겠다는 뜻

	glEnable(GL_DEPTH_TEST);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//glShadeModel(GL_FLAT);

	glLoadIdentity();
	gluPerspective(90, float(RH / RW), 200.0, 200.0);

	gluLookAt(0, distance, 0, Levitation, 0, 0, 1, 0, 0);

	glRotatef(vertical, 0, 0, 1);

	glTranslatef(-0.5, 0, 0);

	glRotatef(horizontal, 0, 1, 0);

	/// /////////////////////////////////// 
#pragma region draw_x_y_z
	//x축 8
	glPushMatrix();
	glBegin(GL_LINES);//바로 위에서 설정한 좌표 기준선 표시하기
	glColor3f(0.0f, 0.5f, 1.0f);	//baby blue
	glLineWidth(5.0f);
	glVertex3f(-1000.0, 0.0, 0.0); //길이를 말하는 건가 봄, 위치와 길이
	glVertex3f(1000.0, 0.0, 0.0);
	glEnd();
	glPopMatrix();

	//y축
	glPushMatrix();
	glBegin(GL_LINES);//바로 위에서 설정한 좌표 기준선 표시하기
	glColor3f(0.6f, 0.5f, 1.0f);	//Lilac
	glLineWidth(10.0f);
	glVertex3f(0.0, -1000.0, 0.0);
	glVertex3f(0.0, 1000.0, 0.0);
	glEnd();
	glPopMatrix();

	//z축
	glPushMatrix();
	glBegin(GL_LINES);//바로 위에서 설정한 좌표 기준선 표시하기
	glColor3f(1.0f, 0.5f, 0.0f);	//orange
	glLineWidth(50.0f);
	glVertex3f(0.0, 0.0, -1000.0);
	glVertex3f(0.0, 0.0, 1000.0);
	glEnd();
	glPopMatrix();
#pragma endregion

#pragma region draw_sphere
	glPushMatrix();
	glColor3f(0.0f, 0.5f, 1.0f);	//blue
	draw_sphere(0); //구 그리기 neck 0~2
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.192157f,	0.309804f,	0.309804f);	//blue
	draw_sphere(27); //구 그리기 pelvis 27~29
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.498039f, 1.0f, 0.831373f);	//Aqua marine
	draw_sphere(3); //구 그리기 left_shoulder 3~5
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.933333f, 0.866667f,	0.509804f);	//Light Goldenrod
	draw_sphere(6); //구 그리기 elbow left 6~8
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.270588f, 0.0f);	//Orange Red
	draw_sphere(9); //구 그리기 wrist left 9~11
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.388235f, 0.278431f);	//Tomato
	draw_sphere(12); //구 그리기 hand left 12~14
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f,	0.894118f,	0.882353f);	//Misty Rose
	draw_sphere(30); //구 그리기 ankle left 33~35
	glEnd();
	glPopMatrix();

	/// ////////////////////////////////////////////////////////////////

	glPushMatrix();
	glColor3f(0.498039f, 1.0f, 0.831373f);	//Aqua marine
	draw_sphere(15); //구 그리기 right_shoulder 15~17
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.933333f, 0.866667f, 0.509804f);	//Light Goldenrod
	draw_sphere(18); //구 그리기 elbow right 18~20
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.270588f, 0.0f);	//Orange Red
	draw_sphere(21); //구 그리기 wrist right 21~23
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.388235f, 0.278431f);	//Tomato
	draw_sphere(24); //구 그리기 hand right 24~26
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.894118f, 0.882353f);	//Misty Rose
	draw_sphere(33); //구 그리기 ankle right 33~35
	glEnd();
	glPopMatrix();
#pragma endregion

	glFlush();
}

void CMFCopenglclientDlg::GLRenderScene_2d(void) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
	//glClearColor(0.0f, 0.5f, 1.0f, 0.9f); //baby blue
	//glClearColor(0.6f, 0.5f, 1.0f, 0.9f); //Lilac
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);//white
	//glClearColor(0.0f, 1.0f, 1.0f, 1.0f);//light blue
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);//black

	glMatrixMode(GL_PROJECTION); //모델 좌표계와 시점 좌표계의 공간을 앞으로 계산하겠다는 뜻

	glLoadIdentity();
	gluPerspective(90, float(RH / RW), 200.0, 200.0);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0, 0, 0);
	glRotatef(horizontal_2d, 1, 0, 0);
	glRotatef(vertical_2d, 0, 1, 0);
	gluOrtho2D(-distance_2d, distance_2d, -distance_2d, distance_2d);

	//glEnable(GL_DEPTH_TEST);
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	////glShadeModel(GL_FLAT);
	
	//gluLookAt(0, distance_2d, 0, Levitation_2d, 0, 0, 1, 0, 0);

	//glRotatef(vertical, 0, 0, 1);

	//glTranslatef(-0.5, 0, 0);

	////gluOrtho2D(-0.5, 0.5, -0.5, 0.5);

	//glRotatef(horizontal, 0, 1, 0);
	
#pragma region draw_x_y_z
	//x축 8
	glPushMatrix();
	glBegin(GL_LINES);//바로 위에서 설정한 좌표 기준선 표시하기
	glColor3f(0.0f, 0.5f, 1.0f);	//baby blue
	glLineWidth(5.0f);
	glVertex3f(-1000.0, 0.0, 0.0); //길이를 말하는 건가 봄, 위치와 길이
	glVertex3f(1000.0, 0.0, 0.0);
	glEnd();
	glPopMatrix();

	//y축
	glPushMatrix();
	glBegin(GL_LINES);//바로 위에서 설정한 좌표 기준선 표시하기
	glColor3f(0.6f, 0.5f, 1.0f);	//Lilac
	glLineWidth(10.0f);
	glVertex3f(0.0, -1000.0, 0.0);
	glVertex3f(0.0, 1000.0, 0.0);
	glEnd();
	glPopMatrix();

#pragma endregion

#pragma region draw_x_y
	glPushMatrix();
	glColor3f(0.0f, 0.5f, 1.0f);	//blue
	draw_2d_sphere(0); //구 그리기 neck 0~2
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.192157f, 0.309804f, 0.309804f);	//blue
	draw_2d_sphere(9); //구 그리기 pelvis 27~29
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.498039f, 1.0f, 0.831373f);	//Aqua marine
	draw_2d_sphere(1); //구 그리기 left_shoulder 3~5
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.933333f, 0.866667f, 0.509804f);	//Light Goldenrod
	draw_2d_sphere(2); //구 그리기 elbow left 6~8
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.270588f, 0.0f);	//Orange Red
	draw_2d_sphere(3); //구 그리기 wrist left 9~11
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.388235f, 0.278431f);	//Tomato
	draw_2d_sphere(4); //구 그리기 hand left 12~14
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.894118f, 0.882353f);	//Misty Rose
	draw_2d_sphere(10); //구 그리기 발목
	glEnd();
	glPopMatrix();

	/// ////////////////////////////////////////////////////////////////

	glPushMatrix();
	glColor3f(0.498039f, 1.0f, 0.831373f);	//Aqua marine
	draw_2d_sphere(5); //구 그리기 right_shoulder 15~17
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.933333f, 0.866667f, 0.509804f);	//Light Goldenrod
	draw_2d_sphere(6); //구 그리기 elbow right 18~20
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.270588f, 0.0f);	//Orange Red
	draw_2d_sphere(7); //구 그리기 wrist right 21~23
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.388235f, 0.278431f);	//Tomato
	draw_2d_sphere(8); //구 그리기 hand right 24~26
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0f, 0.894118f, 0.882353f);	//Misty Rose
	draw_2d_sphere(11); //구 그리기 발목
	glEnd();
	glPopMatrix();
	

#pragma endregion

	glFlush();
}

void CMFCopenglclientDlg::GLRenderScene_imu0(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
	//glClearColor(0.0f, 0.5f, 1.0f, 0.9f); //baby blue
	//glClearColor(0.6f, 0.5f, 1.0f, 0.9f); //Lilac
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);//white
	//glClearColor(0.0f, 1.0f, 1.0f, 1.0f);//light blue
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);//black

	glMatrixMode(GL_MODELVIEW); //모델 좌표계와 시점 좌표계의 공간을 앞으로 계산하겠다는 뜻

	glEnable(GL_DEPTH_TEST);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glLoadIdentity();
	gluPerspective(90, float(RH_imu0 / RW_imu0), 200.0, 200.0);

	gluLookAt(0, distance_imu_0, 0, Levitation_imu_0, 0, 0, 1, 0, 0);

	glRotatef(vertical_imu_0, 0, 0, 1);

	glTranslatef(-0.5, 0, 0);

	glRotatef(horizontal_imu_0, 0, 1, 0);

	glLineWidth(5.0f);

	/// /////////////////////////////////// 
//#pragma region draw_x_y_z_0
//	//x축 8
//	glPushMatrix();
//	glBegin(GL_LINES);//바로 위에서 설정한 좌표 기준선 표시하기
//	glColor3f(0.0f, 0.5f, 1.0f);	//baby blue	
//	glVertex3f(-100.0, 0.0, 0.0); //길이를 말하는 건가 봄, 위치와 길이
//	glVertex3f(100.0, 0.0, 0.0);
//	glEnd();
//	glPopMatrix();
//
//	//y축
//	glPushMatrix();
//	glBegin(GL_LINES);//바로 위에서 설정한 좌표 기준선 표시하기
//	glColor3f(0.6f, 0.5f, 1.0f);	//Lilac	
//	glVertex3f(0.0, -100.0, 0.0);
//	glVertex3f(0.0, 100.0, 0.0);
//	glEnd();
//	glPopMatrix();
//
//	//z축
//	glPushMatrix();
//	glBegin(GL_LINES);//바로 위에서 설정한 좌표 기준선 표시하기
//	glColor3f(1.0f, 0.5f, 0.0f);	//orange	
//	glVertex3f(0.0, 0.0, -100.0);
//	glVertex3f(0.0, 0.0, 100.0);
//	glEnd();
//	glPopMatrix();
//#pragma endregion

#pragma region draw_imu_0

	glRotatef(imu_0_a[0] * 100.0f, 1.0, 0.0, 0.0);
	glRotatef(imu_0_a[1] * 100.0f, 0.0, 1.0, 0.0);
	glRotatef(imu_0_a[2] * 100.0f, 0.0, 0.0, 1.0);

	//roll
	glPushMatrix();	
	glBegin(GL_LINES);//바로 위에서 설정한 좌표 기준선 표시하기
	glColor3f(1.0f,	0.752941f,	0.796078f);	//pink		
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(5.0, 0.0, 0.0);	
	glEnd();
	glBegin(GL_TRIANGLES);//삼각형 그리기
	glColor3f(1.0f, 0.752941f, 0.796078f);	//pink	
	glVertex3f(7.0, 0.0, 0.0);
	glVertex3f(5.0, 0.0, -1.0);
	glVertex3f(5.0, 0.0, 1.0);
	glEnd();
	glPopMatrix();

	//yaw
	glPushMatrix();
	glBegin(GL_LINES);//바로 위에서 설정한 좌표 기준선 표시하기
	glColor3f(0.180392f, 0.545098f, 0.341176f);	//sea green	
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 5.0, 0.0);	
	glEnd();
	glBegin(GL_TRIANGLES);//삼각형 그리기
	glColor3f(0.180392f, 0.545098f, 0.341176f);	//sea green	
	glVertex3f(0.0, 7.0, 0.0);
	glVertex3f(-1.0, 5.0, 0.0);
	glVertex3f(1.0, 5.0, 0.0);
	glEnd();
	glPopMatrix();

	//pitch
	glPushMatrix();
	glBegin(GL_LINES);//바로 위에서 설정한 좌표 기준선 표시하기
	glColor3f(0.282353f, 0.819608f, 0.8f);	//black	
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 5.0);	
	glEnd();
	//삼각형
	glBegin(GL_TRIANGLES);//삼각형 그리기
	glColor3f(0.282353f, 0.819608f, 0.8f);	//Medium Turquoise
	glVertex3f(0.0, 0.0, 7.0);
	glVertex3f(-1.0, 0.0, 5.0);
	glVertex3f(1.0, 0.0, 5.0);
	glEnd();
	glPopMatrix();

#pragma endregion

	glFlush();
}

void CMFCopenglclientDlg::GLRenderScene_imu1(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);//white

	glMatrixMode(GL_MODELVIEW); //모델 좌표계와 시점 좌표계의 공간을 앞으로 계산하겠다는 뜻

	glEnable(GL_DEPTH_TEST);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//glShadeModel(GL_FLAT);

	glLoadIdentity();
	gluPerspective(90, float(RH_imu1 / RW_imu1), 200.0, 200.0);

	gluLookAt(0, distance_imu_1, 0, Levitation_imu_1, 0, 0, 1, 0, 0);

	glRotatef(vertical_imu_1, 0, 0, 1);

	glTranslatef(-0.5, 0, 0);

	glRotatef(horizontal_imu_1, 0, 1, 0);

	glLineWidth(5.0f);
	/// /////////////////////////////////// 
//#pragma region draw_x_y_z_1
//	//x축 8
//	glPushMatrix();
//	glBegin(GL_LINES);//바로 위에서 설정한 좌표 기준선 표시하기
//	glColor3f(0.0f, 0.5f, 1.0f);	//baby blue
//	glVertex3f(-10000.0, 0.0, 0.0); //길이를 말하는 건가 봄, 위치와 길이
//	glVertex3f(10000.0, 0.0, 0.0);
//	glEnd();
//	glPopMatrix();
//
//	//y축
//	glPushMatrix();
//	glBegin(GL_LINES);//바로 위에서 설정한 좌표 기준선 표시하기
//	glColor3f(0.6f, 0.5f, 1.0f);	//Lilac
//	glVertex3f(0.0, -10000.0, 0.0);
//	glVertex3f(0.0, 10000.0, 0.0);
//	glEnd();
//	glPopMatrix();
//
//	//z축
//	glPushMatrix();
//	glBegin(GL_LINES);//바로 위에서 설정한 좌표 기준선 표시하기
//	glColor3f(1.0f, 0.5f, 0.0f);	//orange
//	glVertex3f(0.0, 0.0, -10000.0);	//선 시작점
//	glVertex3f(0.0, 0.0, 10000.0);	//선 끝점
//	glEnd();
//	glPopMatrix();
//#pragma endregion
	
#pragma region draw_imu_1
	glRotatef(imu_1_a[0] * 100.0f, 1.0, 0.0, 0.0);
	glRotatef(imu_1_a[1] * 100.0f, 0.0, 1.0, 0.0);
	glRotatef(imu_1_a[2] * 100.0f, 0.0, 0.0, 1.0);

	//roll
	glPushMatrix();
	glBegin(GL_LINES);//바로 위에서 설정한 좌표 기준선 표시하기	
	glColor3f(1.0f, 0.752941f, 0.796078f);	//pink	
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(5.0, 0.0, 0.0);
	glEnd();
	glBegin(GL_TRIANGLES);//삼각형 그리기
	glColor3f(1.0f, 0.752941f, 0.796078f);	//pink	
	glVertex3f(7.0, 0.0, 0.0);
	glVertex3f(5.0, 0.0, -1.0);
	glVertex3f(5.0, 0.0, 1.0);
	glEnd();
	glPopMatrix();

	//yaw
	glPushMatrix();
	glBegin(GL_LINES);//바로 위에서 설정한 좌표 기준선 표시하기
	glColor3f(0.180392f, 0.545098f, 0.341176f);	//sea green	
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 5.0, 0.0);
	glEnd();
	glBegin(GL_TRIANGLES);//삼각형 그리기
	glColor3f(0.180392f, 0.545098f, 0.341176f);	//sea green	
	glVertex3f(0.0, 7.0, 0.0);
	glVertex3f(-1.0, 5.0, 0.0);
	glVertex3f(1.0, 5.0, 0.0);
	glEnd();
	glPopMatrix();

	//pitch
	glPushMatrix();

	glPushMatrix();
	glBegin(GL_LINES);//바로 위에서 설정한 좌표 기준선 표시하기
	glColor3f(0.282353f,0.819608f, 0.8f);	//Medium Turquoise
	glVertex3f(0.0, 0.0, 0.0);	
	glVertex3f(0.0, 0.0, 5.0);
	glEnd();
	//삼각형
	glBegin(GL_TRIANGLES);//삼각형 그리기
	glColor3f(0.282353f, 0.819608f, 0.8f);	//Medium Turquoise
	glVertex3f(0.0, 0.0, 7.0);
	glVertex3f(-1.0, 0.0, 5.0);
	glVertex3f(1.0, 0.0, 5.0);
	glEnd();
	glPopMatrix();

	glPopMatrix();

#pragma endregion

	glFlush();
}

void CMFCopenglclientDlg::draw_2d_sphere(int data_index)
{
	//const float PI = 3.141592f;
	//GLfloat x, y, z, alpha, beta; // Storage for coordinates and angles        
	//GLfloat radius = 15.0f;
	//int gradation = 20;

	//for (alpha = 0.0; alpha < PI; alpha += PI / gradation)
	//{
	//	glBegin(GL_TRIANGLE_STRIP);
	//	for (beta = 0.0; beta < 2.01 * PI; beta += PI / gradation)
	//	{
	//		//구의 길이
	//		x = radius * cos(beta) * sin(alpha);
	//		y = radius * sin(beta) * sin(alpha);
	//		z = radius * cos(alpha);
	//		glVertex3f(x + m_Result[data_index][0], y + m_Result[data_index][1], z);

	//		//렌더링?
	//		x = radius * cos(beta) * sin(alpha + PI / gradation);
	//		y = radius * sin(beta) * sin(alpha + PI / gradation);
	//		z = radius * cos(alpha + PI / gradation);
	//		glVertex3f(x + m_Result[data_index][0], y + m_Result[data_index][1], z);
	//	}
	//	glEnd();
	//}
	glBegin(GL_POLYGON);
	for (int i = 0; i < 360; i++)
	{
		double angle = i * 3.141592 / 180;
		double x = 50 * cos(angle);
		double y = 50 * sin(angle);
		glVertex2f(x + m_Result_eigen [data_index][0], y + m_Result_eigen[data_index][1]);
	}
	glEnd();

	
}

void CMFCopenglclientDlg::draw_sphere(int data_index)
{
	const float PI = 3.141592f;
	GLfloat x, y, z, alpha, beta; // Storage for coordinates and angles        
	GLfloat radius = 10.0f;
	int gradation = 20;

	for (alpha = 0.0; alpha < PI; alpha += PI / gradation)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (beta = 0.0; beta < 2.01 * PI; beta += PI / gradation)
		{
			//구의 길이
			x = radius * cos(beta) * sin(alpha);
			y = radius * sin(beta) * sin(alpha);
			z = radius * cos(alpha);
			glVertex3f(x + kinect_data[data_index], y + kinect_data[data_index + 1], z + kinect_data[data_index + 2]);

			//렌더링?
			x = radius * cos(beta) * sin(alpha + PI / gradation);
			y = radius * sin(beta) * sin(alpha + PI / gradation);
			z = radius * cos(alpha + PI / gradation);
			glVertex3f(x + kinect_data[data_index], y + kinect_data[data_index + 1], z + kinect_data[data_index + 2]);
		}
		glEnd();
	}
}

//데이터 가져오기
void CMFCopenglclientDlg::convert_data() 
{	
	CString temp_original;
	temp_original = m_Socket.tempcs;	//데이터를 임시 변수에 옮김
	
	CString temp_data;
	AfxExtractSubString(temp_data, temp_original, 0, _T('\n'));	//token 으로 데이터 분리 시키는 함수

	CString temp_data1;
	AfxExtractSubString(temp_data1, temp_original, 1, _T('\n'));	//token 으로 데이터 분리 시키는 함수

	CString temp_classify;
	AfxExtractSubString(temp_classify, temp_data, 0, _T(','));	//token 으로 데이터 분리 시키는 함수

	int index_kinect = temp_classify.Compare(_T("kinect"));
	int index_imu = temp_classify.Compare(_T("imu"));

	if (index_kinect == 0) 
	{
		for (int i = 1; i < 37; i++) 
		{
			CString temp_kinect;
			AfxExtractSubString(temp_kinect, temp_original, i, _T(','));	//token 으로 데이터 분리 시키는 함수
			kinect_data[i-1] = _tstof(temp_kinect);		//string to float
		}
		index_kinect = -1;
	}
	if (index_imu == 0)
	{
		CString temp_imu_id;
		AfxExtractSubString(temp_imu_id, temp_original, 1, _T(','));	//token 으로 데이터 분리 시키는 함수
		int imu_num_0 = temp_imu_id.Compare(_T("100-0"));
		int imu_num_1 = temp_imu_id.Compare(_T("100-1"));

		if (imu_num_0 == 0) 
		{
			for (int i = 2; i < 6; i++)
			{
				CString temp_imu_0;
				AfxExtractSubString(temp_imu_0, temp_original, i, _T(','));	//token 으로 데이터 분리 시키는 함수
				imu_0_data[i - 2] = _tstof(temp_imu_0);		//string to float
			}
		}
		if (imu_num_1 == 0)
		{
			for (int i = 2; i < 6; i++)
			{
				CString temp_imu_1;
				AfxExtractSubString(temp_imu_1, temp_original, i, _T(','));	//token 으로 데이터 분리 시키는 함수
				imu_1_data[i - 2] = _tstof(temp_imu_1);		//string to float
			}
		}
		imu_num_0 = -1;
		imu_num_1 = -1;
		index_imu = -1;

	}
	
}

void CMFCopenglclientDlg::quaternion_to_angle(float *quaternion, float *angler)
{
	//쿼터니안을 오일러 앵글 각으로 변환 시켜 준다. 자이로 센서 값을 변환, w,x,y,z 를 이용하여 변환
	// roll (x-axis rotation)
	double sinr_cosp = 2 * (quaternion[0] * quaternion[1] + quaternion[2] * quaternion[3]);
	double cosr_cosp = 1 - 2 * (quaternion[1] * quaternion[1] + quaternion[2] * quaternion[2]);
	angler[0] = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = 2 * (quaternion[0] * quaternion[2] - quaternion[3] * quaternion[1]);
	if (std::abs(sinp) >= 1)
		angler[1] = std::copysign(pi / 2, sinp); // use 90 degrees if out of range
	else
		angler[1] = std::asin(sinp);

	// yaw (z-axis rotation)
	double siny_cosp = 2 * (quaternion[0] * quaternion[3] + quaternion[1] * quaternion[2]);
	double cosy_cosp = 1 - 2 * (quaternion[2] * quaternion[2] + quaternion[3] * quaternion[3]);
	angler[2] = std::atan2(siny_cosp, cosy_cosp);
}

void CMFCopenglclientDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	//핸들 해제
	wglDeleteContext(m_hRC);
	::ReleaseDC(m_hWnd, m_hDC);

	wglDeleteContext(m_hRC_imu0);
	::ReleaseDC(m_hWnd, m_hDC_imu0);

	wglDeleteContext(m_hRC_imu1);
	::ReleaseDC(m_hWnd, m_hDC_imu1);

}


void CMFCopenglclientDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

}


void CMFCopenglclientDlg::OnBnClickedButton1()
{
	port = GetDlgItemInt(IDC_Port_num);	

	if (port == NULL) 
	{
		AfxMessageBox(_T("Please Enter port number"));
		return;
	}
	else 
	{
		m_Socket.Create();
		if (m_Socket.Connect(_T("127.0.0.1"), port) == FALSE)
		{
			AfxMessageBox(_T("ERROR : Failed to connect Server"));
			PostQuitMessage(0);
		}
	}
	
}


void CMFCopenglclientDlg::OnBnClickedButton2()
{
	int iResult = shutdown(m_Socket, SD_BOTH);
	//shutdown 연결이 종료됨을 알린다

	shutdown(m_Socket, SD_BOTH);

	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(m_Socket);
		WSACleanup();
		return;
	}

	m_Socket.Close();

	AfxMessageBox(_T("Closed"));

	return ;
}


void CMFCopenglclientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case 1:
		{
		wglMakeCurrent(m_cDC->GetSafeHdc(), m_hRC);/*
		GLSetupLight();
		Axischange();*/
		GLRenderScene();
		GLResize(RW, RH);
		SwapBuffers(m_cDC->GetSafeHdc());   //Active      //첫번째 월드
		}
	case 2:		//skeleton 2d
	{
		wglMakeCurrent(m_cDC_2d->GetSafeHdc(), m_hRC_2d);/*
		GLSetupLight();
		Axischange();*/
		GLRenderScene_2d();
		GLResize(RW_2d, RH_2d);
		SwapBuffers(m_cDC_2d->GetSafeHdc());   //Active      //두번째 월드
	}
	case 3:		//imu0
		{
		wglMakeCurrent(m_cDC_imu0->GetSafeHdc(), m_hRC_imu0);/*
		GLSetupLight();
		Axischange();*/
		GLRenderScene_imu0();
		GLResize(RW_imu0, RH_imu0);
		SwapBuffers(m_cDC_imu0->GetSafeHdc());   //Active      //두번째 월드
		}
	case 4:		//imu1
	{
		wglMakeCurrent(m_cDC_imu1->GetSafeHdc(), m_hRC_imu1);/*
		GLSetupLight();
		Axischange();*/
		GLRenderScene_imu1();
		GLResize(RW_imu1, RH_imu1);
		SwapBuffers(m_cDC_imu1->GetSafeHdc());   //Active      //세번째 월드
	}
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CMFCopenglclientDlg::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta < 0) { distance += 0.05; }
	if (zDelta > 0) { distance -= 0.05; }

	CDialogEx::OnMouseHWheel(nFlags, zDelta, pt);
}


BOOL CMFCopenglclientDlg::PreTranslateMessage(MSG* pMsg)
{
	//다이얼로그나 기본 뷰 클래스를 폼뷰로 설정하면 WM_KEYDOWN 메세지 핸들러 처리가 안되기 때문에
	//PretranslateMessage를 이용한다

	float temp_vertical = 0, temp_horizontal = 0, temp_Levitation = 0, temp_distance = 0;
		
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_UP) { temp_vertical -= 10; }//키 입력값은 message 의 wparam에 저장된다.		
		if (pMsg->wParam == VK_DOWN) { temp_vertical += 10; }
		if (pMsg->wParam == VK_LEFT || pMsg->wParam == 'A') { temp_horizontal -= 10; }//이동
		if (pMsg->wParam == VK_RIGHT || pMsg->wParam == 'D') { temp_horizontal += 10; }
		if (pMsg->wParam == 'Q') { temp_Levitation += 10.0; }//키 입력값은 message 의 wparam에 저장된다.		
		if (pMsg->wParam == 'E') { temp_Levitation -= 10.0; }
		if (pMsg->wParam == 'W') { temp_distance += 10.0; }
		if (pMsg->wParam == 'S') { temp_distance -= 10.0; }
		
		if ((((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck()) == TRUE)
		{
			vertical += temp_vertical;
			horizontal += temp_horizontal;
			Levitation += temp_Levitation;
			distance += temp_distance;
		}

		if ((((CButton*)GetDlgItem(IDC_RADIO2))->GetCheck()) == TRUE)
		{
			vertical_imu_0 += temp_vertical;
			horizontal_imu_0 += temp_horizontal;
			Levitation_imu_0 += temp_Levitation;
			distance_imu_0 += temp_distance;
		}

		if ((((CButton*)GetDlgItem(IDC_RADIO3))->GetCheck()) == TRUE)
		{
			vertical_imu_1 += temp_vertical;
			horizontal_imu_1 += temp_horizontal;
			Levitation_imu_1 += temp_Levitation;
			distance_imu_1 += temp_distance;
		}

		if ((((CButton*)GetDlgItem(IDC_RADIO4))->GetCheck()) == TRUE)
		{
			distance_2d += temp_distance;
			horizontal_2d += temp_horizontal;
			vertical_2d += temp_vertical;
		}

		if (VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam)
		{
			return TRUE;
		}
	}



	convert_data(); //센서에서 보낸 데이터 상시 업데이트 해주는 함수
	quaternion_to_angle(imu_0_data, imu_0_a);
	quaternion_to_angle(imu_1_data, imu_1_a);
	standardization(kinect_data);
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CMFCopenglclientDlg::standardization(float *origin_data) 
{
	float standardizated_data[27];
	double STD[9];									//표준편차
	double variance[9];								//표준편차를구하기위해분산을계산
	double AVG[9];
	int j = 0;
	int p = 0;
	int k = 0;

	///////////////////////////////////////표준화///////////////////////////////////////////
	for (int i = 1; i < 10; i++) 
	{
		double sum = 0;

		for (j; j < 3 * i; j++)
		{
			sum += origin_data[j];
		}

		AVG[i-1] = sum / 3;								// 평균계산

		sum = 0;

		for (p; p < 3 * i; p++)
		{
			sum += pow(origin_data[p] - AVG[i - 1], 2);	// pow 함수를이용해서제곱계산
		}

		variance[i-1] = sum / 3;                      //  분산계산

		STD[i-1] = sqrt(variance[i-1]);                 // 분산에루트를씌우면표준편차

		for (k; k < 3 * i; k++) 
		{
			standardizated_data[k] = (origin_data[k] - AVG[i - 1]) / STD[i - 1];
		}
	}

	///////////////////////////////////////공분산///////////////////////////////////////////
	float covariance_temp[36]; //공분산
	int l = 0;
	int data_index = 12; //클래스가 9개

	for (int i = 1; i < 10; i++)
	{
		for (l = 0; l < 3 * i; l++) 
		{
			covariance_temp[l] = ((origin_data[l] - AVG[i - 1]) * (origin_data[l] - AVG[i - 1])) / float(data_index - 1);
		}		
	}

	///////////////////////////////////////고유벡터 및 고유값 계산, 정렬///////////////////////////////////////////
	int c_index = 0;

	float covariance[12][12];
	//9*9 형태로 변환
	for (int i = 0; i < 12; i++) 
	{
		for (int j = 0; j < 3; j++) 
		{
			//covariance[i][j] = covariance_temp[c_index];
			if (j > 3) { covariance[i][j] = 0.0f; }
			if (j < 3) { covariance[i][j] = covariance_temp[c_index]; }			
			c_index++;
		}
	}

	float** eigen_buffer;
	Matrix<float, 12, 12> A;

	/*A << 1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f;*/

	//A << covariance[0][0], covariance[0][1], covariance[0][2];
	for (int i = 0; i < joint_class; i++)
	{
		for (int j = 0; j < 9; j++) 
		{
			float temp = covariance[i][j];
			A(i, j) = temp;
		}
	}

#pragma region

	//eigen lib이용해서 해 구하기//////////////////////////////

	Matrix<float, 1, 36> origin_vector;

	for (int i = 0; i < 36; i++) 
	{
		float temp = origin_data[i];
		origin_vector(0, i) = temp;
	}

	Map<MatrixXf> origin2(origin_vector.data(), 12, 3);

	MatrixXf centered = origin2.rowwise() - origin2.colwise().mean();
	MatrixXf cov = centered.adjoint() * centered;

	SelfAdjointEigenSolver<MatrixXf> eigen(cov);

	VectorXf normalizedEigenValues = eigen.eigenvalues() / eigen.eigenvalues().sum();

	Matrix<float, 3, 3> eigen_vector_T = eigen.eigenvectors().transpose(); //U_t 

#pragma endregion


	SelfAdjointEigenSolver<MatrixXf> eig(A);

	Matrix<float, 12, 12> e_vector;

	e_vector = eig.eigenvectors();

	Matrix<float, 12, 1> e_value;	//열벡터 반환

	e_value = eig.eigenvalues();

	Matrix<float, 12, 12> e_vector_T = e_vector.transpose(); //U_t 

	float origin[12][3];
	int index2 = 0;

	for (int i = 0; i < joint_class; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			origin[i][j] = origin_data[index2];
			index2++;
		}
	}

	//차원 축소를 실행하는 부분
	for (int i = 0; i < joint_class; i++)
	{
		for (int j = 0; j < 2; j++) 
		{
			m_Result_eigen[i][j] = 0.0f;
			for (int k = 0; k < 3; k++) 
			{
				m_Result_eigen[i][j] += (origin[i][k] * eigen_vector_T(k, j));
			}
		}
	}

	for (int i = 0; i < joint_class; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			m_Result[i][j] = 0.0f;
			for (int k = 0; k < 3; k++)
			{
				m_Result[i][j] += origin[i][k] * e_vector(k, j);
			}
		}
	}

}


void CMFCopenglclientDlg::OnBnClickedButton3()
{
	CString temp;
	temp.Format(_T("%f, %f\n %f, %f\n %f, %f\n %f, %f\n %f, %f\n %f, %f\n %f, %f\n %f, %f\n %f, %f\n"), 
		m_Result_eigen[0][0], m_Result_eigen[0][1], m_Result_eigen[1][0], m_Result_eigen[1][1],
		m_Result_eigen[2][0], m_Result_eigen[2][1], m_Result_eigen[3][0], m_Result_eigen[3][1],
		m_Result_eigen[4][0], m_Result_eigen[4][1], m_Result_eigen[5][0], m_Result_eigen[5][1],
		m_Result_eigen[6][0], m_Result_eigen[6][1], m_Result_eigen[7][0], m_Result_eigen[7][1],
		m_Result_eigen[8][0], m_Result_eigen[8][1], m_Result_eigen[9][0], m_Result_eigen[9][1]);

	AfxMessageBox(temp);
}

