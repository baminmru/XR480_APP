#include "FRTT.H"

//xrDevice xrSocket::myDevice; 

xrSocket::xrSocket(xrDevice * Device) : CCESocket()
{
   	myDevice =Device;
}



xrSocket::~xrSocket()
{
	//CloseHandle(hDataEvent);
}

bool xrSocket::OnAccept(SOCKET serviceSocket)
{
  
        return TRUE;
   
}

void xrSocket::OnReceive()
{
	
}


bool xrSocket::OnReceive(char * buf, int  len)
{
	return FALSE;
}

void xrSocket::OnClose(int closeEvent)
{

}