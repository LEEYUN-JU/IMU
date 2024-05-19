#include "pch.h"
#include "MFC_Server.h"
#include "Client_listen.h"
#include "Clinet_socket.h"
#include "MFC_ServerDlg.h"

Client_listen::Client_listen()
{
}

Client_listen::~Client_listen()
{
}

// CListenSocket ��� �Լ�
void Client_listen::OnAccept(int nErrorCode)
{
	Clinet_socket* pClient = new Clinet_socket;
	CString str;

	if (Accept(*pClient)) { // Ŭ���̾�Ʈ ���� ��û�� ���� ����-Ŭ���̾�Ʈ�� ��������ش�
		pClient->SetListenSocket(this);
		m_ptrClientSocketList.AddTail(pClient);

		CMFCServerDlg* pMain = (CMFCServerDlg*)AfxGetMainWnd(); // CSocketServerDlg�� �ڵ��� ������
		str.Format(_T("Client (%d)"), (int)m_ptrClientSocketList.Find(pClient)); // Ŭ���̾�Ʈ ��ȣ(POSITION ��)
		pMain->clientList->AddString(str); // Ŭ���̾�Ʈ�� �����Ҷ����� ����Ʈ�� Ŭ���̾�Ʈ �̸� �߰�
		
		send(*pClient, "hey", 6, 0);
	}
	else {
		delete pClient;
		AfxMessageBox(_T("ERROR : Failed can't accept new Client!"));
	}

	CAsyncSocket::OnAccept(nErrorCode);
}

// Ŭ���̾�Ʈ ���� �����Լ�
void Client_listen::CloseClientSocket(CSocket* pClient)
{	
	POSITION pos;
	
	pos = m_ptrClientSocketList.Find(pClient);
	
	if (pos != NULL) {
		if (pClient != NULL) {
			// Ŭ���̾�Ʈ ���������� ����
			pClient->ShutDown();
			pClient->Close();
		}

		CMFCServerDlg* pMain = (CMFCServerDlg*)AfxGetMainWnd();
		CString str1, str2;
		UINT indx = 0, posNum = 0;
		pMain->clientList->SetCurSel(0);
		// ���� ����Ǵ� Ŭ���̾�Ʈ ã��
		while (indx < pMain->clientList->GetCount()) {
			posNum = (int)m_ptrClientSocketList.Find(pClient);
			pMain->clientList->GetText(indx, str1);
			str2.Format(_T("%d"), posNum);
			// ���ϸ���Ʈ, Ŭ���̾�Ʈ����Ʈ�� ���ؼ� ���� Ŭ���̾�Ʈ ��ȣ(POSITION ��) ������ ����Ʈ���� ����
			if (str1.Find(str2) != -1) {
				AfxMessageBox(str1 + str2);
				pMain->clientList->DeleteString(indx);
				break;
			}
			indx++;
		}

		m_ptrClientSocketList.RemoveAt(pos);
		delete pClient;
	}
}

void Client_listen::SendAllMessage(char* pszMessage)
{
	POSITION pos;
	pos = m_ptrClientSocketList.GetHeadPosition();
	Clinet_socket* pClient = NULL;

	while (pos != NULL) {
		pClient = (Clinet_socket*)m_ptrClientSocketList.GetNext(pos);
		if (pClient != NULL) {
			// Send�Լ��� �ι�° ���ڴ� �޸��� ũ���ε� �����ڵ带 ����ϰ� �����Ƿ� *2�� �� ũ�Ⱑ �ȴ�.
			// �� �Լ��� ������ �������� ���̸� ��ȯ�Ѵ�.
			send(*pClient, pszMessage, sizeof(pszMessage) + 500, 0);

			//Send(&pClient[2], pszMessage, 0);
			/*int checkLenOfData = pClient->Send(pszMessage, lstrlen(pszMessage) * 2);
			if (checkLenOfData != lstrlen(pszMessage) * 2) {
				AfxMessageBox(_T("�Ϻ� �����Ͱ� �������� ���۵��� ���߽��ϴ�!"));
			}*/
		}
	}
}