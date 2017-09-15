#include <windows.h>
#include <stdio.h>
#include "CESocket.h"

class xrDevice;


struct TagInfo{
	char TagID[25];
	int Antenna;
};

class xrByteStream : public CCESocket {

	char * IP;
	char * User;
	char * Password;
	unsigned Port;
	bool Loged;

public:

	
	static int Hexify(char *str, unsigned char *hex);
	static int HexifyNib(char *str, unsigned char *hex)	;
	static int MyMakeHexBuffer(char *pBytes, int nBytes, unsigned char *pOutBuf);
	static void MakeHexString(unsigned char *pBytes, int nBytes, char *pOutBuf);

	xrDevice *myDevice;
    xrByteStream(xrDevice *Device, char * ip, unsigned short port, char * user, char* password);
	bool Reconect();
    virtual ~xrByteStream();
    virtual bool OnAccept(SOCKET serviceSocket);
    virtual void OnReceive();
	virtual bool OnReceive(char* buf, int len);
    virtual void OnClose(int closeEvent);

    bool Login(char * UserName, char * Password);
	bool Logout();


	bool WriteTag( char * TagID, char * NewTag, int Antena);
	bool SetGPIO( char Mask, char Value);
	bool SetGPIOEX( char Mask, char Value);

	int GetTagCount(int Antenna);
	int GetTagCount2(int Antenna);
	bool SetFilter( char * TagID, int Antenna);
	bool ClearFilter( int Antenna);

	TagInfo * GetTagInfo(int TagNum);

private :
	static char GPIO;
	static TagInfo * TagBuffer;
	static int TagBufferSize;
	static unsigned short crcTable[256];
	unsigned short calcBlockCRC(long count, unsigned short crc, unsigned char* buffer);
        
};