#pragma once
#include <afxsock.h>
class Client_listen : public CAsyncSocket
{
public:
	Client_listen();
	virtual ~Client_listen();

	CPtrList m_ptrClientSocketList;
	void OnAccept(int nErrorCode);
	void CloseClientSocket(CSocket* pClient);
	void SendAllMessage(char* pszMessage);
};

