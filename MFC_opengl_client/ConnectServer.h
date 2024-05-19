#pragma once

// ConnectServer 명령 대상

class ConnectServer : public CSocket
{
public:
	CString tempcs;

	ConnectServer();
	virtual ~ConnectServer();
	virtual void OnClose(int nErrorCode);
	virtual CString return_value();
	virtual void OnReceive(int nErrorCode);
};


