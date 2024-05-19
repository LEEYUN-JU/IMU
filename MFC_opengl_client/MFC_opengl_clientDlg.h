
// MFC_opengl_clientDlg.h: 헤더 파일
//

#pragma once

#include "afxwin.h"
#include "ConnectServer.h"


#pragma region skeleton

#pragma endregion

// CMFCopenglclientDlg 대화 상자
class CMFCopenglclientDlg : public CDialogEx
{
// 생성입니다.
public:
	CMFCopenglclientDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	ConnectServer m_Socket;

	int port = 0; //서버 연결을 위한 포트번호

#pragma region opengl_for_kinect
	HGLRC m_hRC;
	HDC m_hDC;
	CDC* m_cDC;

	CRect sizerect;
	int RH;
	int RW;

	float horizontal = 150;		
	float vertical = -250;
	float distance = 2000.0f;	//카메라 거리
	float Levitation = 0;

	float pi = 3.1415926f;
#pragma endregion 

#pragma region opengl_for_kinect_2d
	HGLRC m_hRC_2d;
	HDC m_hDC_2d;
	CDC* m_cDC_2d;

	CRect sizerect_2d;
	int RH_2d;
	int RW_2d;	

	float horizontal_2d = 0;
	float vertical_2d = 0;
	float distance_2d = 2000.0f;	//카메라 거리
#pragma endregion 

#pragma region opengl_for_imu0
	HGLRC m_hRC_imu0;
	HDC m_hDC_imu0;
	CDC* m_cDC_imu0;

	CRect sizerect_imu0;
	int RH_imu0;
	int RW_imu0;

	float horizontal_imu_0 = 100;
	float vertical_imu_0 = 10.0f;
	float distance_imu_0 = 10.0f;	//카메라 거리
	float Levitation_imu_0 = 0;
#pragma endregion 

#pragma region opengl_for_imu1
	HGLRC m_hRC_imu1;
	HDC m_hDC_imu1;
	CDC* m_cDC_imu1;

	CRect sizerect_imu1;
	int RH_imu1;
	int RW_imu1;

	float horizontal_imu_1 = 100;
	float vertical_imu_1 = 10.0f;
	float distance_imu_1 = 10.0f;	//카메라 거리
	float Levitation_imu_1 = 0;

#pragma endregion 

	int joint_class = 12;


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_OPENGL_CLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()	

public:

	void GLinit();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void GLResize(int cx, int cy);
	void GLRenderScene(void);
	void draw_sphere(int data_index);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnBnClickedButton1();
	
	CListBox m_List;

	CEdit port_num;
	afx_msg void OnBnClickedButton2();
	CStatic body_viewer;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void GLinit_2d();
	void GLRenderScene_2d(void);
	void draw_2d_sphere(int data_index);

	void GLinit_imu0();
	void GLRenderScene_imu0(void);

	void GLinit_imu1();
	void GLRenderScene_imu1(void);

	//데이터 수신 후 vector 변환
	virtual void CMFCopenglclientDlg::convert_data();
	BOOL kinect_view;

	virtual void CMFCopenglclientDlg::quaternion_to_angle(float* quaternion, float* angler);

	//PCA를 위한 함수
	virtual void CMFCopenglclientDlg::standardization(float* origin_data);
	afx_msg void OnBnClickedButton3();
//	CListBox temp_list;
//	CListBox temp2;

//	CListBox dddd;
//	CListBox eee;
	CEdit edit1;
};
