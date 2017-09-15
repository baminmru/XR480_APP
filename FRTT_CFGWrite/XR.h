#include <windows.h>
#include <stdio.h>
#include "rfiddefs.h"
#include "rfidcapi.h"

const int KILL	= 0;
const int ACCS	= 1;
const int EPC		= 2;
const int TID		= 4;
const int USER	= 8;
const int WRITE	= 0;
const int PERMWRT = 1;
const int PWDWRT  = 2;
const int NOWRITE	= 3;

class xrDevice;


class xrReader{

	public:

// vars
HANDLE hReader;
TCHAR tszReadTagEventName[32];// = L"TagReadEvent";
TCHAR tszIPAddress[32];
const TCHAR* ReaderIP;
int ReaderPort;
HANDLE hReadThread;
HANDLE hStopThreads;
HANDLE hReadTagEventThread;
HANDLE hReadTagEvent;
HANDLE hGPIOEvent;
DWORD dwTotalTags;

static TYPE_TAG *EventTag;
static TYPE_TAG_EX *EventTagEx;
static xrDevice *Dev;


// tag process thread
static DWORD WINAPI ReadTagEventThread(LPVOID pvarg);



// writing tag functions
DWORD SetSelect (char* SelTag);
DWORD ClearSelect();
bool WriteTag(char * TagMask, char* NewTag, int Antenna);


// read
DWORD ReadTags (TAG_LIST_EX *gTagLis);

//GPIO operations
bool SetGPIO(BYTE GPOMask, BYTE GPOValue);


//lock
DWORD LockTagBank( BYTE type, BYTE bank, BYTE TagLen);

// error processing
bool ProcessErrorRead( TYPE_TAG &Tag);
bool ProcessErrorReadEx( TYPE_TAG_EX &Tag);


// configuraton
BOOL ConfigureReader();
DWORD ConfigureAutonomousMode();
DWORD ConfigureOnDemandMode();
DWORD ConfigureReadLoop( BYTE bOuterLoop, BYTE bInnerLoop);
void ConfigureLockCode( TYPE_TAG_EX *pTag, BYTE *cLockCode);
void SetAirProtocolGen2Only();
void SetCombinedAntenna();
DWORD SetOutLoopValue( DWORD value);
void AddAntennaToSequence( BYTE AntennaIndex);
void RemoveAntennaFromSequence( BYTE AntennaIndex);


// initializing
BOOL FindAndOpenReader( const TCHAR *pszIPAddress, DWORD dwPort);
bool IsXR480();
BOOL ConfigureTCPIP( const TCHAR *pszIPAddress, DWORD dwPort);


void DisplayCapabilities();

// tools
static int Hexify(char *str, unsigned char *hex);
static int HexifyNib(char *str, unsigned char *hex)	;
static int MyMakeHexBuffer(char *pBytes, int nBytes, unsigned char *pOutBuf);
static void MakeHexString(unsigned char *pBytes, int nBytes, char *pOutBuf);




xrReader(const TCHAR* IPAddr, int IPPort,xrDevice *Dev);
~xrReader();
};