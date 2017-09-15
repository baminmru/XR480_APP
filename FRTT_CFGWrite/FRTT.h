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


enum xrAntennaType{
	loadPoint=1,
	unloadPoint=0
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
};

 class DelayedTagError{
 public:
	 DWORD StopTick;
	 char Tag[25];
	 int Antenna;
	 DelayedTagError(const char* pTag,int Ant){
		 strcpy(Tag,pTag);
		 Antenna = Ant;
		 StopTick=GetTickCount()+10000;
		 printf("Scheduled stop at %lu ticks, now %lu \r\n",StopTick,GetTickCount());
	 }

 };

class xrDevice{

	public:
	
	bool BSLoginOK;
	static MyDB *db;
	static xrDeviceStatus Status;
	static xrByteStream *xrBS;


	// network processor data
	static CCESocket *devSocket;
	static PacketOK;
	static HANDLE hNetworkThread;
	static HANDLE hInventoryThread;
	static bool   Error;
	static char*  Event;
	static bool   SaveNewTag;
	static char*  NewTag;

	static bool HALT;
	static void ReadCFGFromDB();
    

	// start device work
	void Run();

	// XR network  thread
	static void NetworkThread(void * Param);


	
	// Inventory Tag Thread
	static void  InventoryThread(void * Param);

	// Icoming TAG processor
	static void ProcessTag(char * TagID, int Antena, bool PaperOnly);

	static bool isPaper( char * TagID);

	static bool isPlastic( char * TagID);

	
	xrDevice();

	~xrDevice();
	static void FillDebugDB();

	private:

		static char StopMask;

		// calculate 23,24 char for changed Tag
		static char *  Calc24(const char * Tag);
		// database support functions

		// verify for tables
		static bool VerifyDB();

		// create tables
		static void CreateDB();

		// erase all data
		static void CleanDB();

		// save info
		static void SaveLabel( int Ant, char * LBL, bool Sent=false);
		static void SaveBox(  char * Tag);
		static void SaveEvent(int Ant, char* LBL, char * Message,bool Sent=false);
		static void SaveCFG( int Ant, long FT, int Stop, int NoWrite, int AType, int StopMask);
		
		static void UpdateLabel( int Ant, char * LBL, char * NewLBL);
		static void UpdateCFG( int Ant, long FT, int Stop, int NoWrite, int AType);

		static bool CheckThisTag(char * Tag, int Ant);  // Tag exists in database

		static void DeleteBoxes();
		static void DeleteSentLabels();
		static void DeleteSentEvents();

	


		static CRITICAL_SECTION Critical;


		static bool IsDeviceOFFTag( char *TagID);
		static bool IsDeviceONTag( char *TagID);
		
		static bool IsAntennaOFFTag( char *TagID);
		static bool IsAntennaONTag( char *TagID);

		static bool IsCheckOFFTag( char *TagID);
		static bool IsCheckONTag( char *TagID);

		static bool IsFilterChangeTag( char *TagID);

		static unsigned long ExtractFilter( char *TagID);

		static void PrepareTag( unsigned long Filter, const char * OldTag, char *NewTag);



		// init device on startup
		static void InitDevice();

		// Stop system on error
		static void SystemStop( int Antena, char * TagID, char * Message);

		// Label OK 
		static void LabelOK( );
		static void LabelOff( );


		// Label OK 
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


        // Send labels info from database
		static bool SendEvents();

		// Ping database server
		static bool SendPing();

		static bool StopIt;

		
		static MyArray<char>* SplitString( const char * str, char Splitter);
	
		static MyArray<DelayedTagError> *TagErrors;

		static long FindErrorIdx(const char * TagID, int Antenna);

	
};
