// FRTT XR device structures

#include <time.h>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <Winbase.h>
#include "macro.h"
#include "mystring.h"
#include "mystream.h"
#include "template.h"

#include "CESocket.h"
#include "MyDB.h"
#include "bs.h"

#define HISTORY_CLEAR_TIME 300
#define HISTORY_CLEAR_TIME_LOAD 100
//#define PAPER_BOX_LAG 7
#define NEWBOXTYPE  0x02


void ShiftLog();
void SaveLog(const char * message);

enum xrAntennaType{
	loadPoint=1,
	unloadPoint=0
};



class TagTime{
public:
	TagTime(const char* pTag,int aAnt){
		StrClear(Tag,13,0);
		strncpy(Tag,pTag,12);
		StrClear(SubstTag,25,0);
		strncpy(SubstTag,pTag,24);
		Ant= aAnt;
		RegisterTick = GetTickCount();
		LastTick =RegisterTick;
		Saved=0;
		ErrorSend=0;
		DTEOK=0;
	}
	TagTime(TagTime *val){
		StrClear(Tag,13,0);
		StrClear(SubstTag,25,0);
		strncpy(Tag,val->Tag,12);
		strncpy(SubstTag,val->SubstTag,24);
		Ant= val->Ant;
		RegisterTick = val->RegisterTick;
		LastTick =val->LastTick;
		Saved= val->Saved;
		ErrorSend=val->ErrorSend;
		DTEOK= val->DTEOK;
	};
	TagTime(TagTime &val){
		StrClear(Tag,13,0);
		StrClear(SubstTag,25,0);
		strncpy(Tag,val.Tag,12);
		strncpy(SubstTag,val.SubstTag,24);
		Ant= val.Ant;
		RegisterTick = val.RegisterTick;
		LastTick =val.LastTick;
		Saved = val.Saved;
		ErrorSend=val.ErrorSend;
		DTEOK= val.DTEOK;
	};

	char Tag[13];
	char SubstTag[25];
	int Ant;
	DWORD RegisterTick;
	DWORD LastTick;
	Sortable(TagTime);
	int Saved;
	int ErrorSend;
	int DTEOK;
};

struct xrAntenna{
	short int anNumber;		  // Antenna number
	xrAntennaType anType;     // Processor Type
	bool anActive;			  // Is Active mode	
	char StopMask;			  	
	bool anCheckFilter;       // Check filter type or simple pass thru
	unsigned long FilterType;		  // Filter Type for this antenna	
	bool anStopWhenLostLabel; // stop system when bad label go out from antenna
	long anMiliSecondToStop;  // delay befor stop for load point 
	char LastSpecialTag[25];
	int Paper;
	int NoStops;
};

 struct xrDeviceStatus{

	char xrIP[64];			// device IP
	char xrServerIP[64];	// FRTT Database Server IP
	UINT xrServerPort; // FRTT Database Server port
    bool xrActive;
	char xrFactoryDeviceName[64];
	xrAntenna xrAntenns[8];
	bool xrStopped;
	time_t xrStopTime;
	time_t xrLastExchangeTime;
	bool Changed;
	long NextTag;
	int  PrevNoStops[8];
};

 

 class Stopper{
 public:
	 DWORD StopUntil;
	 char StopMask;
	 Stopper(char aStopMask, int Sec){
		 StopMask =aStopMask;
		 StopUntil=GetTickCount()+Sec*1000;
	 }
 };


 class UnknownTag{
 public:
	 DWORD LastReg;
	 DWORD RegestredTick;
	 char BadTag[25];;
	 char NewTag[25];
	 int RegisterCount;
	 int Ant;
	 bool EventSent;
	 UnknownTag(int aAnt,const char *  aBadTag){
		 StrClear(BadTag,25,0);
		 StrClear(NewTag,25,0);
		 Ant=aAnt;
		 strcpy(BadTag,aBadTag);
		 LastReg=GetTickCount();
		 RegestredTick=GetTickCount();
		 EventSent=false;
		 RegisterCount=1;
		 
	 }

 };

class xrDevice{

	public:
	static int RewriteCounter;
	static bool BrandChange;
	static bool SubstLoaded;
	static bool DBLocked;
	static bool ClearSubstFlag;

	bool ResetOnExit;
	static bool NetworkError;
	static long NetworkErrorTick;
	
	bool BSLoginOK;
	static MyDB *db;
	static xrDeviceStatus Status;
	static xrByteStream *xrBS;


	// network processor data
	static CCESocket *devSocket;
	static int PacketOK;
	static long NetworkThreadLoopCount;
	static long InventoryThreadLoopCount;
	static HANDLE hNetworkThread;
	static HANDLE hInventoryThread;
	static char*  Event;
	static bool   SaveNewTag;
	static bool   NoDTE;
	static char  NewTag[25];
	

	static bool HALT;
	static void ReadCFGFromDB();
	static int BoxDelay;
	
	
    

	// start device work
	void Run();
		// start device work
	void RunIT();
		// start device work
	void RunNT();

	// XR network  thread
	static void NetworkThread(void * Param);


	
	// Inventory Tag Thread
	static void  InventoryThread(void * Param);

	// Icoming TAG processor
	static void ProcessTag(const char * TagID, int Antena, bool WriteThisTag, int isNew,TagTime * TT);

	static bool isPaper( const char * TagID);

	static bool isPlastic(const  char * TagID);

	
	xrDevice();

	~xrDevice();
	static void FillDebugDB();

	private:

		static char StopMask;

		// calculate 23,24 char for changed Tag
		static char *  Calc24(const char * Tag);

		static char *  Calc12(const char * Tag);

		// проверка контрольнй суммы на метке
		static bool VerifyTag(const char *Tag, bool UseRewrite );

		// проверка контрольной суммы первой половины метки
		static bool VerifyTagHalf(const char *Tag, bool UseRewrite);

		static char* BuildNewTag(int BoxType,unsigned long Filter);

		// database support functions

		// verify for tables
		static bool VerifyDB();

		// create tables
		static void CreateDB();

		// erase all data
		static void CleanDB();

		// save info
		static void SaveLabel( int Ant, const char * LBL,const char * LBLSubst, bool Sent, long Filter);
		static void SaveEvent(int Ant, const char* LBL, char * Message,bool Sent=false);
		static void SaveEvent2(int Ant, const char* LBL, char * Message,bool Sent=false);  // без проверки на качество TagID
		//static void SaveCFG( int Ant, long FT, int Stop, int NoWrite, int AType, int StopMask);
		static void SaveCFG2( int Ant, long FT, int Stop, int NoWrite, int AType, int StopMask,int Paper,int NoStops);
		static void SaveRCFG();
	
		static bool CheckThisTag(char * Tag, int Ant);  // Tag exists in database

		static void DeleteBoxes();
		static void DeleteSentLabels();
		static void DeleteSentEvents();

	


		static CRITICAL_SECTION Critical;


		static bool isGoodTagType( const char *TagID);
	
		
		static bool IsAntennaOFFTag( const char *TagID);
		static bool IsAntennaONTag( const char *TagID);

		static bool IsCheckOFFTag( const char *TagID);
		static bool IsCheckONTag( const char *TagID);

		static bool IsFilterChangeTag( const char *TagID);

		static unsigned long ExtractFilter( const char *TagID);

		static void PrepareTag( unsigned long Filter, const char * OldTag, char *NewTag);



		// init device on startup
		static void InitDevice();

		// Stop system on error
		static void SystemStop( int Antena, char * TagID, char * Message);

		// Label OK 
		static void LabelOK( char Mask);
		static void LabelOff( char Mask);


		// Label OK 
		static void SystemOn(  char Mask);
		static void SystemOn( );
		static void SystemOff( );


    	// Send labels info from database
		static bool Ping();


		// Send labels info from database
		static bool SendLabels();

		// Send device status
		static bool  SendSys();

		// Send configuration
		static bool SendCFG();

		// Get Configuration from Server
		static bool RcvCFG();
		static bool RcvCFGLine(int LineNum);

		// Get System Configuration from Server
		static bool RcvSys();

		// Get Labels From Server
		static bool RcvLabels();

		// Get Subst count from Server
		static bool RcvSubstIDX(bool delta);
		static bool RcvSubst( int Idx, bool delta);

			// Get Rewrite count from Server
		static bool RcvRewriteIDX();
		static bool RcvRewrite( int Idx);

		static void CheckClearSubst();


        // Send labels info from database
		static bool SendEvents();

		// Ping database server
		static bool SendPing();

		static bool StopIt;

		
		static MyArray<char>* SplitString( const char * str, char Splitter);
	
		//static MyArray<DelayedTagError> *TagErrors;
		static MyArray<Stopper> * Stoppers;

		//static long FindErrorIdx(const char * TagID, int Antenna);
		static long FindUtagIdx(const char * TagID, int Antenna);

		static MySortedArray<TagTime> *History;

		static MySortedArray<UnknownTag> *UTags;

		static void EraseTagHistory();

		static void DumpInfo();
		
		
		static bool BadTagProcessing(TagInfo* ti,bool isLoadPoint);
	
};
