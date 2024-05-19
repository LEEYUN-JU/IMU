#include "pch.h"
#include "MFC_Server.h"
#include "Clinet_socket.h"
#include "Client_listen.h"
#include "MFC_ServerDlg.h"

Clinet_socket::Clinet_socket()
{
}

Clinet_socket::~Clinet_socket()
{
}

// CClientSocket 멤버 함수
void Clinet_socket::SetListenSocket(CAsyncSocket* pSocket)
{
	m_pListenSocket = pSocket;
}
// CClientSocket 멤버 함수


void Clinet_socket::OnClose(int nErrorCode)
{
	CSocket::OnClose(nErrorCode);

	Client_listen* pServerSocket = (Client_listen*)m_pListenSocket;
	pServerSocket->CloseClientSocket(this);

	Clinet_socket::OnClose(nErrorCode);
}

void Clinet_socket::OnReceive(int nErrorCode)
{
	CString strTmp = _T(""), strIPAddress = _T("");
	UINT uPortNumber = 0;
	char temp[1024];
	

	GetPeerName(strIPAddress, uPortNumber);

	if (Receive(&temp, sizeof(temp)) > 0) 
	{
		CMFCServerDlg* pMain = (CMFCServerDlg*)AfxGetMainWnd();
		
		strTmp = temp;

		pMain->message_list.InsertString(-1, strTmp);
		pMain->message_list.AddString(L"\n");  // 메시지 리스트(메시지창?)에 입력받은 메시지 띄우기
		pMain->message_list.SetCurSel(pMain->message_list.GetCount() - 1);
		//AfxMessageBox(strTmp);

		//send(*m_pListenSocket, temp, sizeof(temp), 0);
		Client_listen* pServerSocket = (Client_listen*)m_pListenSocket;
		//TCHAR* szUniCode = (TCHAR*)(LPCTSTR)strTmp;
		//pServerSocket->SendAllMessage(szUniCode); // 다른 클라이언트들에게 메시지 전달

		pServerSocket->SendAllMessage(temp); // 다른 클라이언트들에게 메시지 전달

	}		
	
	CSocket::OnReceive(nErrorCode);
}
