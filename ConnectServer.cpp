// ConnectServer.cpp: 구현 파일
//

#include "pch.h"
#include "MFC_opengl_client.h"
#include "ConnectServer.h"
#include "MFC_opengl_clientDlg.h"

char r_h_data[80];


// ConnectServer

ConnectServer::ConnectServer()
{
}

ConnectServer::~ConnectServer()
{
}


// ConnectServer 멤버 함수
void ConnectServer::OnClose(int nErrorCode)
{
	ShutDown();
	Close();

	CSocket::OnClose(nErrorCode);

	AfxMessageBox(_T("ERROR:Disconnected from server!"));
	::PostQuitMessage(0);
}

CString ConnectServer::return_value()
{
	CString temp = tempcs;
	return temp;
}

void ConnectServer::OnReceive(int nErrorCode)
{
	CString strTmp = _T(""), strIPAddress = _T("");
	TCHAR szBuffer[1024];
	char temp[1024];
	//::ZeroMemory(szBuffer, sizeof(szBuffer));

	if (Receive(&temp, sizeof(temp)) > 0) {
		CMFCopenglclientDlg* pMain = (CMFCopenglclientDlg*)AfxGetMainWnd();

		strTmp = temp;
		pMain->m_List.InsertString(-1, strTmp);
		pMain->m_List.AddString(L"\n");
		pMain->m_List.SetCurSel(pMain->m_List.GetCount() - 1);
		tempcs = strTmp;
	}
	CSocket::OnReceive(nErrorCode);
}


