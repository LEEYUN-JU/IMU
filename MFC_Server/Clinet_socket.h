#pragma once

class Clinet_socket : public CSocket
{
public:
	Clinet_socket();
	virtual ~Clinet_socket();

	CAsyncSocket* m_pListenSocket;
	void SetListenSocket(CAsyncSocket* pSocket);
	void OnClose(int nErrorCode);
	void OnReceive(int nErrorCode);

};

