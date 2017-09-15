#include "FRTT.H"
#include "CESocket.h"
#include "template.h"
#include "mystring.h"
#include <Pm.h>

#define VERSION "[SENSOR 28.04.2014]" 
#define LAZYREADDELAY 1

int LogShiftFlag=0;
char msg[1024];
CRITICAL_SECTION *LOG_CS;

void CloseLog(){
	DeleteCriticalSection(LOG_CS);
	delete LOG_CS;
}
void ShiftLog(){
	LOG_CS = new CRITICAL_SECTION();
	InitializeCriticalSection(LOG_CS); 
	try{	
		CopyFile(  TEXT("\\Data\\LOG.2"), TEXT("\\Data\\LOG.3"),  false);
	}catch(...){
	}
	try{	
		CopyFile(  TEXT("\\Data\\LOG.1"), TEXT("\\Data\\LOG.2"),  false);
	}catch(...){
	}
	try{	
		CopyFile(  TEXT("\\Data\\LOG.TXT"), TEXT("\\Data\\LOG.1"),  false);
	}catch(...){
	}
	HANDLE hAppend;
	hAppend = CreateFile (TEXT("\\Data\\LOG.TXT"),      
	GENERIC_WRITE,          // Open for writing
	0,                      // Do not share
	NULL,                   // No security
	CREATE_ALWAYS,            // create new
	FILE_ATTRIBUTE_NORMAL,  // Normal file
	NULL);    
	CloseHandle (hAppend);
}

void SaveLog(const char * message){
	
	//EnterCriticalSection(LOG_CS);
	//HANDLE hAppend;
	//DWORD wBytes;
	//
	//	
	//hAppend = CreateFile (TEXT("\\Data\\LOG.TXT"),      
 //                   GENERIC_WRITE,          // Open for writing
 //                   0,                      // Do not share
 //                   NULL,                   // No security
 //                   OPEN_ALWAYS,          // Open 
 //                   FILE_ATTRIBUTE_NORMAL,  // Normal file
 //                   NULL);           
	//

	   printf("%s",message); 

	//  if (hAppend == INVALID_HANDLE_VALUE)
	//  {
	//	 // printf ( "Could not open LOG.TXT\r\n");
	//	 LeaveCriticalSection(LOG_CS);
	//	return ;
	//  }

	//  DWORD dwPos;
	//  // Append the first file to the end of the second file.
	//  SYSTEMTIME T;
	//  GetSystemTime(&T);
	//  char buf[1024];
	//  sprintf(buf,"\r\n%02d.%02d.%04d %02d:%02d.%02d ",T.wDay,T.wMonth,T.wYear,T.wHour,T.wMinute,T.wSecond);

 //     dwPos = SetFilePointer (hAppend, 0, NULL, FILE_END);
	//  WriteFile (hAppend, buf, strlen(buf), &wBytes, NULL);
	//  WriteFile (hAppend, message, strlen(message), &wBytes, NULL);
	//  CloseHandle (hAppend);
	//  LeaveCriticalSection(LOG_CS);
	  
	  
};

void MaintananceRestart(){
	
	SYSTEMTIME T;
	GetSystemTime(&T);
	if(T.wHour==7  || T.wHour ==19 ){
		if(T.wMinute==57 && T.wSecond >10 && LogShiftFlag==false){
			//xrDevice::HALT=true;
			LogShiftFlag=true;
			ShiftLog();
		}

		if(T.wMinute>=58 && LogShiftFlag==true  ){
			LogShiftFlag=false;
		}
	}
	
}



bool ShowMem(){
	SYSTEMTIME T;
	GetSystemTime(&T);
	MEMORYSTATUS *memStatus = new MEMORYSTATUS();
    GlobalMemoryStatus(memStatus);
	bool TimeToHalt=false;
	unsigned long fence =2000000ul;
	printf("V=%s Avail Physical:  %10ld Delta %10ld.  SysTime is: %04d-%02d-%02d %02d:%02d:%02d\r\n" ,VERSION  , memStatus->dwAvailPhys, memStatus->dwAvailPhys - fence, T.wYear,T.wMonth,T.wDay ,T.wHour ,T.wMinute,T.wSecond);
	if(memStatus->dwAvailPhys < fence){
		TimeToHalt=true;
	}
	delete memStatus;
	return TimeToHalt;
}

const char *nibble_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 8; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "0" : "1");
    }

    return b;
}


bool xrDevice::DBLocked=false;
bool xrDevice::SubstLoaded=false;
bool xrDevice::BrandChange=false;
bool xrDevice::NetworkError=false;
long xrDevice::NetworkErrorTick=0;
char xrDevice::StopMask=0;
int xrDevice::RewriteCounter=0;
CRITICAL_SECTION xrDevice::Critical;
xrDeviceStatus xrDevice::Status ;
CCESocket *xrDevice::devSocket=NULL; 
xrByteStream *xrDevice::xrBS = NULL;
int xrDevice::PacketOK;
HANDLE xrDevice::hNetworkThread;
HANDLE xrDevice::hInventoryThread;
MyDB *xrDevice::db;	
bool xrDevice::HALT=false;
int xrDevice::BoxDelay=200;
bool xrDevice::StopIt=false;
bool xrDevice::SaveNewTag=false;
bool xrDevice::NoDTE=false;
char  xrDevice::NewTag[25];
MyArray<Stopper> * xrDevice::Stoppers;
MySortedArray<TagTime> *xrDevice::History;
MySortedArray<UnknownTag> *xrDevice::UTags;
long xrDevice::NetworkThreadLoopCount=0;
long xrDevice::InventoryThreadLoopCount=0;



bool  operator<(class TagTime const &a,class TagTime const &b){
	if(a.Ant<b.Ant)
		return true;

	if(a.Ant>b.Ant)
		return false;

	
	if(a.RegisterTick <b.RegisterTick)
		return true;

	if(a.RegisterTick >b.RegisterTick)
		return false;

	if(_strnicmp(a.Tag,b.Tag,12)<0)  
		return true;
	else
		return false;
		
	
};

bool  operator==(class TagTime const &a,class TagTime const &b){
	if((a.Ant==b.Ant) && !_stricmp(a.Tag,b.Tag))
		return true;
	else
		return false;
};


bool  operator<(class UnknownTag const &a,class UnknownTag const &b){
	if(a.Ant<b.Ant)
		return true;

	if(a.Ant>b.Ant)
		return false;

	


	if(_stricmp(a.BadTag,b.BadTag)<0)  
		return true;
	else
		return false;
		
	
};

bool  operator==(class UnknownTag const &a,class UnknownTag const &b){
	if((a.Ant==b.Ant) && !_stricmp(a.BadTag,b.BadTag))
		return true;
	else
		return false;
};


bool xrDevice::isPaper(const  char *TagID){
					   //112233445566778899001122	
	if(!_strnicmp(TagID,"03",2)) return true;

	return false;
}


bool xrDevice::isPlastic( const char *TagID){
	//112233445566778899001122	
	if(!_strnicmp(TagID,"01",2)) return true;
	if(!_strnicmp(TagID,"02",2)) return true;
	if(!_strnicmp(TagID,"04",2)) return true;
	if(!_strnicmp(TagID,"05",2)) return true;
	/*if(!_strnicmp(TagID,"06",2)) return true;
	if(!_strnicmp(TagID,"07",2)) return true;
	if(!_strnicmp(TagID,"08",2)) return true;
	if(!_strnicmp(TagID,"09",2)) return true;*/
	return false;
}


bool xrDevice::isGoodTagType( const char *TagID){
	
	bool ok=false;
	ok=isPlastic(TagID);
	if(ok) return ok;
	ok=isPaper(TagID);
	if(ok) return ok;
	if(!_strnicmp(TagID,"66",2)) return true;

	return false;
}




bool xrDevice::IsAntennaOFFTag( const char *TagID){
					   //112233445566778899001122	
	if(!_strnicmp(TagID,"66FF000000",10)) return true;

	return false;
}

bool xrDevice::IsAntennaONTag( const char *TagID){
					   //112233445566778899001122	
	if(!_strnicmp(TagID,"66EE000000",10)) return true;
	

	return false;
}


bool xrDevice::IsCheckOFFTag(const  char *TagID){
		 			   //112233445566778899001122	
	if(!_strnicmp(TagID,"66DD000000",10)) return true;

	return false;
}

bool xrDevice::IsCheckONTag(const  char *TagID){
		 			   //112233445566778899001122	
	if(!_strnicmp(TagID,"66CC000000",10)) return true;

	return false;
}



bool xrDevice::IsFilterChangeTag(const  char *TagID){
					   //112233445566778899001122	
	if(!_strnicmp(TagID,"66BB000000",10)) return true;

	return false;
}

unsigned long xrDevice::ExtractFilter(const  char *TagID){
	unsigned long f;
	char *buf;
	SetupString(TagID,buf);
	buf[22]=0;
	sscanf(buf+12,"%lx",&f);
	//printf("Extract Filter:TAG=%s, Filter=%lx\r\n",TagID,f);
	delete [] buf;
	return f;
}

void xrDevice::PrepareTag( unsigned long Filter, const char * OldTag, char *aNewTag){
	memset(aNewTag,0,25);
	strncpy(aNewTag,OldTag,12);
	char buf[13];
	char * c24;
	sprintf(buf,"%010lX",Filter);
	strcat(aNewTag,buf);
	c24=Calc24(aNewTag);
	if (c24[0]==' '){
		c24[0]='0';
	}
	strcat(aNewTag,c24);
	delete c24;

	//printf("Prepare Tag: Old=%s, New=%s\r\n",OldTag,aNewTag);
}


bool xrDevice::VerifyTag(const char *Tag,bool UseRewrite){
	bool ok=false;
	if(UseRewrite){
		DBTagRewrite *rr = new DBTagRewrite();
		strncpy(rr->Tag,Tag,24);
		ok=db->CheckTagRewrite(rr);
		delete rr;
		if(ok) return false;
	}

	if( isGoodTagType(Tag)){
		char *check= Calc24(Tag);
		if(check){

			if (check[0]==Tag[22] && check[1]==Tag[23]){
				delete check;
				
				check=Calc12(Tag);
			
				if (check[0]==Tag[10] && check[1]==Tag[11]){
					delete check;
					return true;
				}else{
					delete check;
					return false;
				}
			}else{
				delete check;
				return false;
			}
		}
	}
	return false;

}

bool xrDevice::VerifyTagHalf(const char *Tag, bool UseRewrite){
	bool ok=false;
	if(UseRewrite){
		DBTagRewrite *rr = new DBTagRewrite();
		strncpy(rr->Tag,Tag,24);
		ok=db->CheckTagRewrite(rr);
		delete rr;
		if(ok) return false;
	}
	if( isGoodTagType(Tag)){
		char *check= Calc12(Tag);
		if(check){
			if (check[0]==Tag[10] && check[1]==Tag[11]){
				delete check;
				return true;
			}else{
				delete check;
				return false;
			}
		}
	}
	return false;

}


char *  xrDevice::Calc12(const char * Tag){

	const char *pstr = Tag;
	char *buf = new  char[3];
	memset(buf,0,3);
	unsigned int x;
	int s=0;

	

	for (int j=0;j<5;j++)
	{
		if(sscanf(pstr, "%2X", &x) == 1)
		{
			s = s + x;
			s =s & 0xFF;
			pstr += 2;
			
		}
	}

	if (s < 0xAA){
            s = 0xAA - s;
	}else{
            s = 0xAA + 256 - s;
	}
    s =s & 0xFF;
	sprintf(buf,"%02X",s);
	return buf;

}

char *  xrDevice::Calc24(const char * Tag){

	const char *pstr = Tag;
	char *buf = new  char[3];
	memset(buf,0,3);
	unsigned int x;
	int s=0;


	for (int j=0;j<11;j++)
	{
		if(sscanf(pstr, "%2X", &x) == 1)
		{
			s = s + x;
			s =s & 0xFF;
			pstr += 2;
			
		}
	}

	if (s < 0x55){
            s = 0x55 - s;
	}else{
            s = 0x55 + 256 - s;
	}
    s =s & 0xFF;
	sprintf(buf,"%02X",s);
	return buf;

}

 
char* xrDevice::Event;

void xrDevice::ProcessTag(const char * TagID, int Antena, bool WriteThisTag, int isNew, TagTime * TT){

	
	if(!VerifyTag(TagID,false)){
		sprintf(msg,"PT:Bad tag format. Tag ignored\r\n");
		SaveLog(msg);
		return;
	}

	

	StrClear(Status.xrAntenns[Antena-1].LastSpecialTag,24,'0');
	strcpy(Status.xrAntenns[Antena-1].LastSpecialTag,TagID);

	long eIdx=0;

	if(Status.xrAntenns[Antena-1].anActive){
		EnterCriticalSection(&Critical);



		/////////////////////////////////////////// конец секции проверки специальных меток
		// Эта антенна будет чего-то делать
		{
			NoDTE=false;
			// работаем с точкой загрузки
			if(Status.xrAntenns[Antena-1].anType==loadPoint){
				//if(WriteThisTag){
					if(!TT->Saved ){
						PrepareTag( Status.xrAntenns[Antena-1].FilterType,TagID,NewTag);
						sprintf(msg,"IT:BOX LOADED LOOPIDX=%d TAG=%s NEW=%s\r\n",Antena,TagID ,NewTag );
						SaveLog(msg);
						SaveNewTag= true;
					}
				//}
			} // loadpoint

			// работаем с точкой выгрузки
			if(Status.xrAntenns[Antena-1].anType==unloadPoint){
				unsigned long TagFilter=0;
				if(Status.xrAntenns[Antena-1].anCheckFilter ){

					//  если не была зафиксирована метка
					if(isNew){
						TagFilter = ExtractFilter(TagID);
						PrepareTag( 0,TagID,NewTag);
						if(TagFilter == Status.xrAntenns[Antena-1].FilterType   ){ // 
							PrepareTag( 0,TagID,NewTag);
							SaveNewTag= true;
							sprintf(msg,"IT:BOX UNLOAD LOOPIDX=%d TAG=%s NEW=%s\r\n",Antena,TagID ,NewTag );
							SaveLog(msg);
						}else{
							
							sprintf(msg,"IT:BOX ERROR LOOPIDX=%d TAG=%s TAGFILTER=%04x ANTFILTER=%04x\r\n",Antena,TagID ,TagFilter,Status.xrAntenns[Antena-1].FilterType );
							SaveLog(msg);

							// one error for tag
							if(!TT->ErrorSend){
								SaveEvent(Antena, TagID ,"Filter error",0);
								TT->ErrorSend=-1;
							}
							
						}
					}else{
						if(GetTickCount()- TT->RegisterTick >2000){ 
							TagFilter = ExtractFilter(TagID);
							if((TagFilter != Status.xrAntenns[Antena-1].FilterType)  ){ // && (TagFilter!=0 )
								if(Status.xrAntenns[Antena-1].NoStops==0){
									StopIt=true;
									StopMask =  Status.xrAntenns[Antena-1].StopMask ;
								}else{
									sprintf(msg,"Stop skipped at ant=%d. NoStop flag set.\r\n",Antena );
									SaveLog(msg);
								}
							}
						}
					}
				}else{ // no check. only save and clear box tag
					//  если не была зафиксирована метка
					if(isNew){
						//PrepareTag( Status.xrAntenns[Antena-1].FilterType,TagID,NewTag);
						PrepareTag( 0,TagID,NewTag);
						sprintf(msg,"IT:BOX UNLOAD (ANT=OFF) LOOPIDX=%d TAG=%s NEW=%s\r\n",Antena,TagID ,NewTag );
						SaveLog(msg);
						SaveNewTag= true;
					}
				}
			} // unload point
		}
		LeaveCriticalSection(&Critical);
	}
};



xrDevice::xrDevice(){
	ResetOnExit =false;
	//printf("Open xrBS\r\n");
	InitializeCriticalSection(&Critical);
	ShiftLog();
	SaveLog(VERSION);
	

	if (xrBS == NULL){
		xrBS = new xrByteStream (this,"127.0.0.1",3000,"admin","admin");

		BSLoginOK=xrBS->Reconect();
		
		if(!BSLoginOK){
			printf("BS Connect error %d\r\n",xrBS->GetLastError());
			return;
		}
	
	}

	Stoppers = new MyArray<Stopper>();
	History = new MySortedArray<TagTime>();
	UTags= new MySortedArray<UnknownTag>;

	//printf("Init configuration\r\n");
	InitDevice();

	
	TagTime * tt;
	DBTagRecord *dbt;
	int cnt =db->GetTagCount(); 
	try{
		//printf("Load %d saved tags to history from disk.\r\n",cnt);
		for(int i=1;i<=cnt;i++){
			
			dbt=db->GetTag(i);
			if(dbt){
				if (!dbt->Deleted && dbt->Ant>0 ){
					tt = new TagTime( dbt->Tag ,dbt->Ant );
					tt->Saved = -1;
					strncpy(tt->SubstTag,dbt->TagSubst,24);
					sprintf(msg,"LOAD saved history %s\r\n",tt->SubstTag);
					SaveLog(msg);

					if(dbt->Sent ){
						tt->DTEOK=-1;
						tt->ErrorSend=-1;
					}
					History->Add(tt);
				}
			}
		}
	}catch(...){
		printf("Exception at Load tags to history from disk.\r\n");
	}

}


xrDevice::~xrDevice(){
	CloseHandle(hNetworkThread);
	CloseHandle(hInventoryThread);
	db->Close();
	delete db;
	db =NULL;
	//delete TagErrors;
	delete Stoppers;
	delete History;


	if(xrBS !=NULL){
		xrBS->Logout();
		xrBS->Disconnect(); 
		delete xrBS;
	}
	xrBS = NULL;
	CloseLog();
}




void xrDevice::Run(){
	//BOOL bSuccess = FALSE;
	char bChar = 2;
	
	xrBS->SetGPIOEX(0x3f,0);

	
	xrBS->SetGPIOEX(0x3f,0x24);
	
	
	hNetworkThread = CreateThread(NULL, 0, (unsigned long (__cdecl *)(void *))xrDevice::NetworkThread, (LPVOID)(this) ,0, (unsigned long *)0);

	ResumeThread(hNetworkThread);

	int count=0;
	while(!xrDevice::SubstLoaded && count <3600){
		Sleep(1000);
		printf("*");
		count++;
	}

	hInventoryThread = CreateThread(NULL, 0, (unsigned long (__cdecl *)(void *))xrDevice::InventoryThread, (LPVOID)(this) ,0, (unsigned long *)0);
	
	ResumeThread(hInventoryThread);



}

void xrDevice::RunIT(){
	try{
		TerminateThread(hInventoryThread,0);
		CloseHandle(hInventoryThread);
	}catch(...){
	}
	hInventoryThread = CreateThread(NULL, 0, (unsigned long (__cdecl *)(void *))xrDevice::InventoryThread, (LPVOID)(this) ,0, (unsigned long *)0);
	
	ResumeThread(hInventoryThread);
	DBLocked =false;

}

void xrDevice::RunNT(){
	
	try{
		TerminateThread(hNetworkThread,0);
		CloseHandle(hNetworkThread);
	}catch(...){
	}

	hNetworkThread = CreateThread(NULL, 0, (unsigned long (__cdecl *)(void *))xrDevice::NetworkThread, (LPVOID)(this) ,0, (unsigned long *)0);

	ResumeThread(hNetworkThread);
	DBLocked =false;
}


void xrDevice::EraseTagHistory(){
// erase TagTime History
	TagTime *ttt;
	DWORD pos;
	DBTagRecord *DBTag=NULL;

	int tt;
	for(tt=0;tt<History->Count();tt++){
		ttt =History->Get(tt);
		if(ttt){
			if( Status.xrAntenns[ttt->Ant-1].anType==loadPoint){

				// delete tag info, if tag not visible more than HISTORY_CLEAR_TIME second
				try{
					if(ttt->LastTick + (long)(HISTORY_CLEAR_TIME_LOAD*1000) < GetTickCount()){
						//printf("IT:History erase for:%s  label lost for %d sec. \r\n",ttt->Tag,HISTORY_CLEAR_TIME_LOAD );
						if(_strnicmp(Status.xrAntenns[ttt->Ant-1].LastSpecialTag ,ttt->Tag,12)) {
							DBTag= new DBTagRecord();
							DBTag->Ant =ttt->Ant ;
							strncpy(DBTag->Tag,ttt->SubstTag,25); 
							pos=0;
							if(db->CheckTag(DBTag,&pos)){
								delete DBTag;
								DBTag=db->GetTag(pos);
								if(DBTag){
									DBTag->Deleted=true;
									if(db->UpdateTag(DBTag,pos)){
									
										sprintf(msg,"History erase tag:%s deleted=1 pos=%lu\r\n",ttt->Tag,pos);
										SaveLog(msg);
									}else{
										sprintf(msg,"Error while set deleted for tag:%s\r\n",ttt->Tag);
										SaveLog(msg);
									}
								}
							}else{
								delete DBTag;
								sprintf(msg,"History erase tag:%s no tag\r\n",ttt->Tag);
								SaveLog(msg);
							}
							
							History->Erase(tt);
						}
					}
				}catch(...)
				{
					printf("IT:Exception at Load point  History erase  \r\n" );
				}
			}else{
				if(ttt->LastTick + (long)(HISTORY_CLEAR_TIME*1000) < GetTickCount()){
					try{
						//printf("IT:History erase for:%s  label lost for %d sec. \r\n",ttt->Tag,HISTORY_CLEAR_TIME );
						if(_strnicmp(Status.xrAntenns[ttt->Ant-1].LastSpecialTag ,ttt->Tag,12)) {
							DBTag= new DBTagRecord();
							DBTag->Ant =ttt->Ant ;
							strncpy(DBTag->Tag,ttt->SubstTag,25); 
							if(db->CheckTag(DBTag,&pos)){
								delete DBTag;
								DBTag=db->GetTag(pos);
								if(DBTag){
									DBTag->Deleted=true;
									db->UpdateTag(DBTag,pos);
									sprintf(msg,"History erase tag:%s deleted=1\r\n",ttt->Tag);
									SaveLog(msg);
								}
								
							}else{
								delete DBTag;
								sprintf(msg,"History erase tag:%s no tag\r\n",ttt->Tag);
								SaveLog(msg);
							}
							
							History->Erase(tt);
						}
					}catch(...){
						printf("IT:Exception at UnLoad point  History erase  \r\n" );
					}
				}
			}
		}else{
			try{
				//printf("IT:History erase null pointer \r\n" );
				History->Erase(tt);
			}catch(...){
				printf("IT:Exception History erase with null pointer  \r\n" );
			}
		}
	}

}

void xrDevice::DumpInfo (){
	TagTime *ttt;
	//DWORD pos;
	DBTagRecord *DBTag=NULL;

	int tt;
	//int err;
	//ShowMem();

	DWORD tickCount = GetTickCount();

	// dump History
	printf("\r\nIT:>>>>History dump: count= %d\r\n",History->Count());
	for(tt=0;tt<History->Count();tt++){
		ttt =History->Get(tt);
		if(ttt){
			printf("IT:>>History box= %s  a=%d (Time Register=%ld, After lost=%ld)\r\n",ttt->Tag,ttt->Ant,(tickCount-ttt->RegisterTick) /1000,(tickCount-ttt->LastTick) /1000  );
		}

	}
	
	/*

	printf("IT:----UT dump: count= %d \r\n",UTags->Count());
	 // dump UTags
	UnknownTag * unTag;
	for(err=0;err<UTags->Count();err++){
		unTag =UTags->Get(err);
		printf("IT:--UT Dump tag= %s  a= %d \r\n",unTag->BadTag,unTag->Ant);

	}
	*/
	printf("IT:----\r\n");
}


bool xrDevice::BadTagProcessing(TagInfo* tag,bool isLoadPoint){
	long pos=-1;

	// проверяем правильно ли записана метка
    if(!VerifyTag(tag->TagID,isLoadPoint)){
		if(_strnicmp(tag->TagID,"000000000000000000000000",24)){

			try{

			int a=tag->Antenna ;
			UnknownTag *unTag;

			// метка не проходит по формату
			if(Status.xrAntenns[a-1].anActive){
				if(Status.xrAntenns[a-1].anType==loadPoint){
					// если неверная первая половина, то перезаписать, иначе просто игнорировать проблему с хвостом
					if(!VerifyTagHalf(tag->TagID,isLoadPoint)){
						// в точке загрузки мы можем инициализировать ее новым кодом, лучше сразу все вычислить и записывать один раз
						unTag = new UnknownTag(a,tag->TagID);
						pos=UTags->Find(unTag);
						if(pos<0){

							// добавили неизвестную метку
							UTags->Add(unTag);
							char *nt;

							// построили новый код метки
							// берем NEWBOXTYPE в качестве типа лотка

							nt=BuildNewTag(NEWBOXTYPE,Status.xrAntenns[a-1].FilterType);
							printf("Build new tag %s\r\n",nt);
							strcpy(unTag->NewTag,nt);
							delete [] nt;

							// пытаемся записать метку
							printf("Write unknown tag %s -> %s\r\n",unTag->BadTag,unTag->NewTag);
							xrBS->WriteTag(unTag->BadTag,unTag->NewTag,a);   

						}else{
							delete unTag;

							// пытаемся записать уже известную неверную метку
							unTag=UTags->Get(pos);
							unTag->RegisterCount ++;

							//  будем пытаться записать метку только если она повторилась на антенне несколько раз
							if (unTag->RegisterCount>=5){
								printf("Write unknown tag %s -> %s\r\n",unTag->BadTag,unTag->NewTag);
								xrBS->WriteTag(unTag->BadTag,unTag->NewTag,a); 


								// регистрируем время появление метки на антене
								UTags->Get(pos)->LastReg=GetTickCount();

							}
						}
						return true;
					}else{
						return false;
					}

					  
				}else{
					// точка выгрузки, плохая метка
					unTag = new UnknownTag(a,tag->TagID);
					pos=UTags->Find(unTag);
					
					if(pos<0){
						UTags->Add(unTag);  // новая плохая метка
					}else{
						delete unTag;
						unTag =UTags->Get(pos);
						unTag->LastReg=GetTickCount();
						unTag->RegisterCount ++;
						if(!unTag->EventSent){
							// пробуем игнорировать помехи
							if (unTag->RegisterCount>=3){
								// на точке выгрузки сразу останавливаемся, так же как с плохим типом фильтра
								SaveEvent2(a ,tag->TagID ,"Wrong label format",0);
								if(Status.xrAntenns[a-1].NoStops==0){
									StopIt=true;
									StopMask=Status.xrAntenns[a-1].StopMask; 
								}else{
									sprintf(msg,"Stop skipped at ant=%d. NoStop flag set.\r\n",a );
									SaveLog(msg);
								}
								unTag->EventSent=true;
							} // это реальная плохая метка, не помеха...
						}
					} // повтор плохой метки
					return true;
				} //выгрузка, плохая метка
			
				}
			}catch(...){
				printf("IT:Exception BadTagProcessing\r\n");
				return false;
			}
		}else{
			return true;   /// просто игнорируем метку из одних нулей. очень похоже , что это помеха
		}
	}else{
			return false;
	}
	return false;
}

// exchange loop thread
void  xrDevice::InventoryThread(void * Param){

	/* Motion detectors support */ 
	int mdmask[4]={1,2,4,8};
	int BoxDetected[4]={0,0,0,0};
	long int lastRead[4]={0,0,0,0};
	int MotionDetectors=0xf;
	//int pNear=-1;
	//int bNear=-1;
	//int pNearRSSI=-30000;
	//int bNearRSSI=-30000;

	//printf("IT:Start\r\n");
	//int tt;
	unsigned int EmptyLoopCnt=0;
	int TagCnt;
	TagInfo * tag;
	/*bool WasPlastic;
	bool WasPaper;*/
	bool WriteThisTag;
	int isNew=0;
	TagTime *ttFind=NULL;
	TagTime *ttScan=NULL;
	long pos=-1;

	SaveEvent2(1 ,"" ,"Device started",false);
	SystemOn();
	bool GreenOn=false;
	int LockCount=0;
	while(!HALT)
	{
		MaintananceRestart();
		Sleep(1);
		
		if(xrDevice::DBLocked){
			printf("!");
			LockCount++;
			if(xrDevice::SubstLoaded){
				if (LockCount==2000){
					xrDevice::DBLocked=false;
					LockCount=0;
					SaveEvent2(1 ,"" ,"Device manual unlock",false);
					sprintf(msg,"!!!Direct database unlock from IT thread!!!");
					SaveLog(msg);
				}
			}else{
				if (LockCount==5000){
					xrDevice::DBLocked=false;
					LockCount=0;
					SaveEvent2(1 ,"" ,"Device manual unlock while loading subst database",false);
					sprintf(msg,"!!!Direct database unlock from IT thread SubstLoaded flag set!!!");
					xrDevice::SubstLoaded=false;
					SaveLog(msg);
				}
			}
		}else{
			printf(".");
			LockCount=0;
		}
		InventoryThreadLoopCount++;

		// Reconnect if we lost bytestream server
		try
		{
			
			if(xrBS->GetSocketState()!=CCESocket::CONNECTED){
				printf("BS Server communication lost\r\nReconnecting...\r\n");
				xrBS->Reconect();
			}
		}
		catch(...)
		{
			printf("IT:Exception while reconnecting BS Server\r\n");
		}

	
		if(!xrDevice::DBLocked)
		{
			//printf("\r\nIT:Lock database\r\n");
			xrDevice::DBLocked=true;
			EraseTagHistory();

			if(xrBS->GetSocketState()==CCESocket::CONNECTED)
			{
				MotionDetectors=xrBS->GetGPIOStatus() ;
							
				bool wastag=false;
				
				for (int a=1;a<=4;a++)
				{

				    if((MotionDetectors & mdmask[a-1])==0)
					{
						sprintf(msg,"IT:sensor on %d\r\n",a);
						printf(msg);

						BoxDetected[a-1]++;
						bool SkipTag=false;
						long eIdx;
						bool isLoadPoint=false;
						StopIt=false;
						TagCnt=0;

						isLoadPoint=false;
						if(Status.xrAntenns[a-1].anActive)
						{
							if(Status.xrAntenns[a-1].anType==loadPoint)
							{
								isLoadPoint=true;
							}
						}

						if(Status.xrAntenns[a-1].anType==loadPoint){
							if (lastRead[a-1]==0  || (lastRead[a-1]>0 && lastRead[a-1] + 1000 * LAZYREADDELAY<=GetTickCount()))
								TagCnt = xrBS->GetTagCount2(a);
						}else{
							TagCnt = xrBS->GetTagCount2(a);
						}

						if(TagCnt>0)
						{
							lastRead[a-1]=GetTickCount();
							wastag=true;
							BoxDetected[a-1]=0;
							EmptyLoopCnt=0;
							//pNear=-1;
							//bNear=-1;
							//pNearRSSI=-30000;
							//bNearRSSI=-30000;

							//scan nearest tags
							/* if(TagCnt >1){
								for(int i=0; i<TagCnt; i++){
									tag = xrBS->GetTagInfo(i);
									//if(isPaper(tag->TagID)){
										if(bNearRSSI<tag->RSSI){
											bNearRSSI=tag->RSSI;
											bNear=i;
										}
									}
									//if(isPlastic(tag->TagID)){
									//	if(pNearRSSI < tag->RSSI){
									//		pNearRSSI=tag->RSSI;
									//		pNear=i;
									//	}
									//}
								}
							} 
					       */

							//printf("Ant:%d, Detected %d tag(s)\r\n",a,TagCnt);
							/*
							for(int i=0;i<TagCnt; i++){
								tag = xrBS->GetTagInfo(i);
								if(tag){
									tag->OK=true;
									//if(isLoadPoint){
									//	tag->OK=true;
									//}else{
										//if(i==bNear  || TagCnt==1){
									//		tag->OK=true;
										//}else{
											//if(isPlastic(tag->TagID) || isPaper(tag->TagID))
										//	tag->OK=false;
										//}
										//printf("Tag:%s, RSSI=%d, OK=%d\r\n" ,tag->TagID ,tag->RSSI,tag->OK);
									//}
								}
							} */
						

							for(int i=0;i<TagCnt; i++){
								tag = xrBS->GetTagInfo(i);
								if(tag){
									//if(true /* tag->OK */)
									//{
										//sprintf(msg,"IT:LOOPIDX=%d TAG=%s ANT=%d\r\n",a ,tag->TagID ,tag->Antenna );
										//printf(msg);

										// зажгли лампочку
										if(Status.xrAntenns[a-1].StopMask!=0)
											LabelOK(Status.xrAntenns[a-1].StopMask);
											
										if(!BadTagProcessing(tag,isLoadPoint))
										{
   										    // метка проходит по формату, так что обрабатываем её
											// это хорошая метка
												
											ttFind = new TagTime(tag->TagID,a);
											long hpos=-1;
											hpos = History->Find(ttFind);
											
											if(hpos>=0){ // метка уже есть в истории
												delete ttFind;
												ttFind = History->Get(hpos);
												sprintf(msg,"IT:TAG FROM HISTORY LOOPIDX=%d TAG=%s SUBST=%s\r\n",a,tag->TagID ,ttFind->SubstTag );
												printf(msg);
												strncpy(tag->TagID, ttFind->SubstTag,24);
												// update last scan time
												ttFind->LastTick = GetTickCount();

												isNew =0;	
											}else{ // делаем подстановку кода для метки

												db->SubstThisTag(tag->TagID, ttFind->SubstTag,Status.xrAntenns[a-1].Paper  );
												sprintf(msg,"IT:NEWTAG LOOPIDX=%d TAG=%s SUBST=%s\r\n",a,tag->TagID ,ttFind->SubstTag );
												SaveLog(msg);
												strncpy(tag->TagID, ttFind->SubstTag,24);
												History->Add(ttFind);
												isNew =-1;
											}
											WriteThisTag = false;
											if(Status.xrAntenns[a-1].anActive){
												//if(Status.xrAntenns[a-1].anType!=loadPoint){
													WriteThisTag =true;
												//}// point type
											} //active

											try{
												ProcessTag(tag->TagID , a ,WriteThisTag,isNew,ttFind);
											}catch(...){
												printf("IT: Exception while ProcessTag \r\n" );
											}
											
											if(SaveNewTag){
												if(!ttFind->Saved ){  
													sprintf(msg,"IT: Save Tag %s \r\n",NewTag );
													SaveLog(msg);

													EnterCriticalSection(&Critical);
													SaveLabel(tag->Antenna,NewTag,ttFind->SubstTag,0,Status.xrAntenns[tag->Antenna-1].FilterType );
													ttFind->Saved =-1;
													LeaveCriticalSection(&Critical);
												}
												SaveNewTag=false;
											}
			
											// если пришла правильная метка надо проверить не осталась ли она в списке плохих
											UnknownTag* unTag = new UnknownTag(a,tag->TagID);
											eIdx=FindUtagIdx(tag->TagID,a); // проверка идет по новому коду
											if(eIdx>=0){
												UTags->Erase(eIdx);
											}
											delete unTag;

											if(Status.xrAntenns[a-1].StopMask!=0)
												LabelOff(Status.xrAntenns[a-1].StopMask);
										} // good tag  
									//} //tagOK

									if(StopIt){
										StopIt=false;
										SystemStop(a,"","");
										StopMask=0;
									}
								} // tag 
							} //for each tag
						} // TagCnt >0
						else
						{ // TagCnt =0
							if(Status.xrAntenns[a-1].anActive && Status.xrAntenns[a-1].anCheckFilter ){
								if(Status.xrAntenns[a-1].anType==loadPoint){  
									if(Status.xrAntenns[a-1].Paper ==0 ){ // ignore 'box has no label error' if use paper box
										if (lastRead[a-1]==0  ||(lastRead[a-1]>0 && lastRead[a-1] + 1000 * LAZYREADDELAY <= GetTickCount())){
											if(BoxDetected[a-1] == BoxDelay){
												sprintf(msg,"Box has no label at ant=%d\r\n" , a );
												SaveLog(msg);

												if(Status.xrAntenns[a-1].NoStops==0){
													StopIt=true;
													StopMask=Status.xrAntenns[a-1].StopMask; 	
													SystemStop(a,"","");
													StopMask=0;
												}else{
													sprintf(msg,"Stop skipped at ant=%d. NoStop flag is set.\r\n" , a );
													SaveLog(msg);
												}
												SaveEvent2(a,"000000000000000000000000","Box has no label.",0);
											}
											if((BoxDetected[a-1] % 50)==0){
												printf("\r\nSensor %d off %d cycles",a,BoxDetected[a-1]);
											}
											if(BoxDetected[a-1] >BoxDelay){
												if((BoxDetected[a-1] % 10)==0 ){
													if(Status.xrAntenns[a-1].NoStops==0){
														StopIt=true;
														StopMask=Status.xrAntenns[a-1].StopMask; 	
														SystemStop(a,"","");
														StopMask=0;
													}else{
														sprintf(msg,"Stop skipped at ant=%d. NoStop flag set.\r\n",a );
														SaveLog(msg);
													}
												}
											}
										}
									}
								}else{
									if(BoxDetected[a-1] == BoxDelay){
										sprintf(msg,"Box has no label at ant=%d\r\n" , a );
										SaveLog(msg);

										if(Status.xrAntenns[a-1].NoStops==0){
											StopIt=true;
											StopMask=Status.xrAntenns[a-1].StopMask; 	
											SystemStop(a,"","");
											StopMask=0;
										}else{
											sprintf(msg,"Stop skipped at ant=%d. NoStop flag is set.\r\n" , a );
											SaveLog(msg);
										}
										SaveEvent2(a,"000000000000000000000000","Box has no label.",0);
									}
									if((BoxDetected[a-1] % 50)==0){
											printf("\r\nSensor %d off %d cycles",a,BoxDetected[a-1]);
									}
									if(BoxDetected[a-1] >BoxDelay){
										if((BoxDetected[a-1] % 10)==0 ){
											if(Status.xrAntenns[a-1].NoStops==0){
												StopIt=true;
												StopMask=Status.xrAntenns[a-1].StopMask; 	
												SystemStop(a,"","");
												StopMask=0;
											}else{
												sprintf(msg,"Stop skipped at ant=%d. NoStop flag set.\r\n",a );
												SaveLog(msg);
											}
										}
									}
								}
							
							}else{
								if(BoxDetected[a-1] == BoxDelay){
									printf("O");
								}
								if((BoxDetected[a-1] % 50)==0){
										printf("\r\nSensor %d off %d cycles",a,BoxDetected[a-1]);
								}
							}
						}
					} // motion detector was active
					else
					{ // motion detector not active
						if(BoxDetected[a-1]!=0)
						{
							BoxDetected[a-1]=0;   
						
							printf("B");
						}
						lastRead[a-1]=0;   // next box 
						Sleep(5);
					} //motion detector

				} // for each antenna

				if(wastag){
					// dump history
					try{
						DumpInfo();
					}catch(...){
						printf("IT:Exception while dump data\r\n");
					}
				}

			}
			else
			{ // BS Server not  OK
				SaveEvent2(1 ,"" ,"Device resetting. Bytestream server lost.",false);
				xrDevice::HALT=true;
			}// xrBS.Connected

			// stopping for UTAG
			try
			{
				UnknownTag * refUT;
				//  останавливаем машину по новому алгоритму 
				for(int ut=0;ut<UTags->Count();ut++)
				{
					refUT=UTags->Get(ut);
					// --LastTick
					//if((UTags->Get(ut))->LastReg + 60000 < GetTickCount()){
					if(refUT->RegestredTick + 40000 < GetTickCount())
					{
					//if((UTags->Get(ut))->LastReg + 60000 < GetTickCount()){

						// останавливаемся, только если метка была замечена более 5 раз, иначе считаем это дело помехой
						if(refUT->RegisterCount  >=5)
						{
							if(!refUT->EventSent)
							{
								SaveEvent2(refUT->Ant ,refUT->BadTag ,"Wrong label format",0);

								sprintf(msg,"Wrong label format at ant=%d.\r\n",refUT->Ant );
								SaveLog(msg);

								if(Status.xrAntenns[refUT->Ant-1].NoStops==0){
									StopIt=true;
									StopMask=Status.xrAntenns[refUT->Ant-1].StopMask; 
									SystemStop(refUT->Ant,"","");
									StopIt=false;
								}else{
									sprintf(msg,"Stop skipped at ant=%d. NoStop flag set.\r\n",refUT->Ant );
									SaveLog(msg);
								}
								refUT->EventSent=true;
							}
						}
					}
				}
			}catch(...){
				printf("IT:Exception while stopping machine\r\n");
			}

			// erase UTAG
			try
			{
				UnknownTag * refUT;
				//  стираем метку из истории по старому признаку
				for(int ut=0;ut<UTags->Count();ut++)
				{
					// --LastTick
					refUT=UTags->Get(ut);
					if(refUT->LastReg + 60000 < GetTickCount())
					{
						UTags->Erase(ut);
						printf("IT:Unknown tag erased\r\n"); 
					}
				}
			}
			catch(...)
			{
				printf("IT:Exception while erase UTags\r\n");
			}

			// Red light control
			try
			{
				xrBS->SetGPIOEX(0x12,0); // Yellow off
				xrBS->SetGPIOEX(0x9,0); // Red off
				if (NetworkError){
					if(GreenOn){
						xrBS->SetGPIOEX(0x24,0); //green off
					}else{
						xrBS->SetGPIOEX(0x24,0x24); //green on
					}
					GreenOn=! GreenOn;
				}else{
					xrBS->SetGPIOEX(0x24,0x24); //green on
				}
			
			}
			catch(...)
			{
				printf("IT:Exception at lamp control\r\n");
		
			}

			// red light
			try
			{
				for(int stp=0;stp<Stoppers->Count();stp++)
				{
					Stopper *sss =Stoppers->Get(stp);
					if(sss->StopUntil > GetTickCount())
					{
						printf("IT:Stop %x until %ld now %ld\r\n",(int)(sss->StopMask),sss->StopUntil,GetTickCount()); 
						xrBS->SetGPIOEX(sss->StopMask,sss->StopMask);
							 
					}
					else
					{
						Stoppers->Erase(stp);
						printf("IT:Stopper erased\r\n"); 
					}
				}
			}catch(...){
				printf("IT:Exception at stopper erase\r\n");
			}

			EmptyLoopCnt++;
			
			// service operation
			try{
				if(EmptyLoopCnt && 0xFF == 0){
					xrBS->Reconect();
					SystemOff();
					SystemOn();
				}

				if(EmptyLoopCnt && 0xFFF==0){
					xrBS->Reconect();
					SystemOff();
					SystemOn();
					EmptyLoopCnt=0;
				}
			
			}catch(...){
				printf("IT:Exception at service operation\r\n");
		
			}
			
			xrDevice::DBLocked=false;
			Sleep(30);
			//printf("\r\nIT:Unlock database\r\n");
		}
		else
		{ // DBLock
			//printf("\r\nIT:Skip one loop\r\n");
			Sleep(5);
		}// DBLock
		
	} //xrDevice::HALT
	
	SetSystemPowerState(NULL,POWER_STATE_RESET,0);
}



static bool firsttime=true;


void xrDevice::CheckClearSubst(){
	
	SYSTEMTIME T;
	GetSystemTime(&T);
	if(T.wHour==8   ){
		if(T.wMinute==30 && T.wSecond >10 ){
			//xrDevice::HALT=true;
			firsttime=true;
			SaveLog("Reload subst database every day at 8:30!");
		}
	}
}

// exchange loop thread
void  xrDevice::NetworkThread(void * Param){
	
	SaveLog("NT::Network thread start\r\n");
	NetworkThreadLoopCount=0;
	RewriteCounter=0;
	int count=0;
	
	
	while(!xrDevice::HALT){
		NetworkThreadLoopCount++;
		CheckClearSubst();
		try{
			//ShowMem();
			//MaintananceRestart();
			if(devSocket==NULL){
				devSocket = new CCESocket();
			}

			//printf("\r\nNT::Open socket %s %d\r\n",Status.xrServerIP ,Status.xrServerPort);
			bool result = devSocket->Create(SOCK_STREAM);

			// open server socket
			result =devSocket->Connect(Status.xrServerIP ,Status.xrServerPort); 
			
			if (result) {
			
				printf("NT::Locking database\r\n");

				count=0;
				
				SendPing();

				while(xrDevice::DBLocked){
					Sleep(1);
					count++;
					if(count==10000){
						SendPing();
						count=0;
						sprintf(msg,"!!!Direct database unlock from NT thread!!!!");
						SaveLog(msg);
						xrDevice::DBLocked=false;
					}
				}
				xrDevice::DBLocked = true;
				printf("NT::Database locked, Sending data\r\n");

				if(NetworkError){
					// 10 minute of server errorrs 
					if(NetworkErrorTick>0){
						if(GetTickCount()- NetworkErrorTick >= (long)(600.0 * 1000.0)){
							NetworkErrorTick=GetTickCount();
							firsttime=true;
							sprintf(msg,"Reload flag! 10 min network error");
							SaveLog(msg);
						}
					}
					// restore PrevNoStops	
					int iiii=0;
					for(iiii=0;iiii<=3;iiii++){
						Status.xrAntenns[iiii].NoStops=Status.PrevNoStops[iiii];
					}
					SaveLog("Network OK. NoStop mode returned to normal state!");
					SaveEvent2(1 ,"" ,"NoStop mode returned to normal state!",0);

				}
				NetworkErrorTick=0;
				NetworkError=false;

				

				if(!xrDevice::HALT){
					if(SendPing()){
						if(!xrDevice::HALT) SendLabels();
						if(!xrDevice::HALT) SendEvents();
						if(!xrDevice::HALT) RcvSys();

						if(!xrDevice::HALT){ 
							if(RcvCFG()){
								if(xrDevice::BrandChange){
									NetworkThreadLoopCount++;
									SaveEvent2(1 ,"" ,"Device BrandChange detected",false);
									firsttime=true;
									SaveLog("Reload flag! Device BrandChange detected");
									
									xrDevice::BrandChange=false;
									UTags->Clear();
								}
							}
						}
						
						if(!xrDevice::HALT){ 
							if(firsttime==true){
								NetworkThreadLoopCount++;
								firsttime=false;
								SaveLog("Clear Reload flag!");
								xrDevice::SubstLoaded = false;
								sprintf(msg,"Rebuild subst database");
								SaveLog(msg);
								db->RebuildSubstDB(); 
								sprintf(msg,"Reload full subst database");
								SaveLog(msg);
								RcvSubstIDX(false); // read full database
								sprintf(msg,"Reload full subst database finished");
								SaveLog(msg);
								xrDevice::SubstLoaded = true;
								RewriteCounter=0;
								firsttime=false;
							}else{
								if(!db->CheckSubstDBSize()){
									NetworkThreadLoopCount++;
									firsttime=false;
									SaveLog("Clear Reload flag!");
									sprintf(msg,"Wrong subst database size");
									SaveLog(msg);
									xrDevice::SubstLoaded = false;
									sprintf(msg,"Rebuild subst database");
									SaveLog(msg);
									db->RebuildSubstDB(); 

									sprintf(msg,"Reload full subst database");
									SaveLog(msg);
									RcvSubstIDX(false); // read full database
									sprintf(msg,"Reload full subst database finished");
									SaveLog(msg);

									xrDevice::SubstLoaded = true;
									firsttime=false;
								}else{

									xrDevice::SubstLoaded = false;
									RcvSubstIDX(true); // read partial
									xrDevice::SubstLoaded = true;
								}
							}
						}
				
					
						NetworkThreadLoopCount++;
						if(!xrDevice::HALT) db->DeleteSentTag();

						if(RewriteCounter %10 == 5){
							NetworkThreadLoopCount++;
							if(!xrDevice::HALT) db->DeletePassedTag();
						}

						if(RewriteCounter%20 == 8){
							NetworkThreadLoopCount++;
							if(!xrDevice::HALT) db->DeleteSentEvent();
						}

						if(RewriteCounter==40){
							RewriteCounter=0;
						
						}
						RewriteCounter++;

					}
				}
				
				printf("NT::Unlock database\r\n");
				xrDevice::DBLocked=false;

				printf("NT::Disconnecting\r\n");
				devSocket->Disconnect();
					
				if(devSocket!=NULL){
					delete devSocket;
				}

				devSocket=NULL;
				
			}else{
				printf("NT::Open Error\r\n");
				devSocket=NULL;
				NetworkError=true;
				
				

				if(NetworkErrorTick==0){
					NetworkErrorTick=GetTickCount();  // register start time of network error status
					int iiii=0;
					for(iiii=0;iiii<=3;iiii++){
						Status.PrevNoStops[iiii]=Status.xrAntenns[iiii].NoStops;
						Status.xrAntenns[iiii].NoStops=1;
					}
				
					SaveLog("Network error. NoStop mode activated!");
					SaveEvent2(1 ,"" ,"Network error. NoStop mode activated!",0);
				}
			}
		}catch(...){
				devSocket=NULL;
				printf("NT::Exception catched\r\n");
		}
		count=0;
		
		while(count < 3 ){   // *3 
			NetworkThreadLoopCount++;
			Sleep(15000);   // 20000
			NetworkThreadLoopCount++;
			try{
			
				if(devSocket==NULL){
					devSocket = new CCESocket();
				}
				//printf("\r\nNT::Try to open socket\r\n");
				bool result =devSocket->Create(SOCK_STREAM);
   				result =devSocket->Connect(Status.xrServerIP ,Status.xrServerPort); 

				if (result) {
					Sleep(100);
					SendPing();
					NetworkErrorTick=0;
					//  restore PrevNoStops
					if(NetworkError==true){
						int iiii=0;
						for(iiii=0;iiii<=3;iiii++){
							Status.xrAntenns[iiii].NoStops=Status.PrevNoStops[iiii];
						}
						SaveLog("Network OK. NoStop mode returned to normal state!");
						SaveEvent2(1 ,"" ,"NoStop mode returned to normal state!",0);
					}
					NetworkError=false;
				}else{
					NetworkError=true;
					if(NetworkErrorTick==0){
						NetworkErrorTick=GetTickCount();  // register start time of network error status
						int iiii=0;
						for(iiii=0;iiii<=3;iiii++){
							Status.PrevNoStops[iiii]=Status.xrAntenns[iiii].NoStops;
							Status.xrAntenns[iiii].NoStops=1;
						}
						SaveLog("Network error on ping. NoStop mode activated!");
						SaveEvent2(1 ,"" ,"Network error. NoStop mode activated!",0);

					}else{
						// 10 minute of server errorrs 
						if(GetTickCount() - NetworkErrorTick >=(long) (600.0 * 1000.0)){
							NetworkErrorTick=GetTickCount() ;
							firsttime=true;
							sprintf(msg,"Reload flag! 10 min network error");
							SaveLog(msg);
							count=4;  // exit from sleep & ping loop 
						}
					}
				}
				
				//printf("\r\nNT::Delete socket\r\n");
				if(devSocket!=NULL){
					devSocket->Disconnect();
					delete devSocket;
					devSocket=NULL;
				}
			}catch(...){
				devSocket=NULL;
				printf("NT::Exception catched\r\n");
			}
			if(xrDevice::HALT) break;
			count++;
		}
		if(!xrDevice::HALT){
			if(ShowMem()){
				SaveEvent2(1 ,"" ,"Reseting device. Memory fence detected.",false);
				xrDevice::HALT=true;
			}
		}else{
			SaveEvent2(1 ,"" ,"Reseting device.",false);
		}
		
	}
	
};


// init device on startup
void  xrDevice::InitDevice(){

	
	xrDevice::HALT = false;

	//printf("Open database\r\n");
	db= new MyDB();
	db->Open();
	
	//printf("Verify DB\r\n");
	if(! VerifyDB()){
		//printf("Create DB\r\n");
		CreateDB();
		//FillDebugDB();
	}

	

	xrDevice::DBLocked = true;
	DBReaderConfigRecord *rc = db->GetReaderConfig();

	if(rc)
    {
		strcpy(Status.xrServerIP , rc->ServerIP);
		Status.xrServerPort =rc->Port;
		strcpy(Status.xrIP ,rc->xrIP);
		Status.NextTag=rc->NextTag; 
		delete rc;
    }		

	sprintf(msg,"Server IP: %s, Port: %d, XR_IP: %s\r\n", Status.xrServerIP,Status.xrServerPort,Status.xrIP);
	SaveLog(msg);

	ReadCFGFromDB();
	Status.Changed=true;

	DeleteSentEvents();
	DeleteSentLabels(); 
	xrDevice::DBLocked = false;
	sprintf(msg,"Load Box Delay. Default=%d\r\n", BoxDelay);
	SaveLog(msg);
	BoxDelay=db->GetBoxDelay();
	sprintf(msg,"Loaded Box Delay=%d\r\n", BoxDelay);
	SaveLog(msg);

};


void xrDevice::ReadCFGFromDB(){
	DBConfigRecord2 *ac;
	int idx;
	for(idx=0 ;idx <db->GetConfigCount2() ;idx++){
		Status.xrAntenns[idx].anActive=false;
	}

	for(idx=0 ;idx < db->GetConfigCount2() ;idx++){
		ac =db->GetPointConfig2(idx+1);	
		if(ac){
			if(ac->Antena>=0 && ac->Antena<=7){
				sprintf(msg,"Load config for antenna %d Filter %04lx  Type=%d Check=%d Active=%d \r\n", ac->Antena,ac->Filter,ac->PointType ,ac->Check,ac->Active  );
				SaveLog(msg);
				Status.xrAntenns[ac->Antena].anNumber =ac->Antena;
				Status.xrAntenns[ac->Antena].FilterType =ac->Filter; 
				Status.xrAntenns[ac->Antena].anActive = ac->Active ;
				Status.xrAntenns[ac->Antena].anCheckFilter  =ac->Check ;
				Status.xrAntenns[ac->Antena].StopMask  =ac->StopMask ;
				Status.xrAntenns[ac->Antena].anType  = (xrAntennaType)ac->PointType;
				Status.xrAntenns[ac->Antena].Paper   = ac->Paper ;
				Status.xrAntenns[ac->Antena].NoStops = ac->NoStops ;
			}
			delete ac;
		}
    }
	Status.Changed=true;
}


// Stop system on error
void  xrDevice::SystemStop(int Antena, char * TagID, char * Message){
	Stopper *sss ;
	for(int i=0;i<Stoppers->Count();i++){
		sss=Stoppers->Get(i);
		if(sss->StopMask==StopMask){
			sss->StopUntil=GetTickCount()+3*1000; 
			return;
		}
	}
	sss = new Stopper(StopMask,3);
	Stoppers->Add(sss);
};

	// Label OK 
void xrDevice::LabelOK( char Mask ){

	//printf("Label ON %d\r\n",(int)Mask);
	if(Mask == 0x01)
		xrBS->SetGPIOEX(0x2,0x2);
	if(Mask == 0x08)
		xrBS->SetGPIOEX(0x10,0x10);
	if(Mask == 0x09)
		xrBS->SetGPIOEX(0x12,0x12);


};

	// Label OFF
void xrDevice::LabelOff(char Mask ){

	//printf("Label OFF %d\r\n",(int)Mask);
	if(Mask == 0x01)
		xrBS->SetGPIOEX(0x2,0x0);
	if(Mask == 0x08)
		xrBS->SetGPIOEX(0x10,0x0);
	if(Mask == 0x09)
		xrBS->SetGPIOEX(0x12,0x0);

};


void xrDevice::SystemOn( char Mask){
	xrBS->SetGPIOEX(0x24,0x24); //green on
	xrBS->SetGPIOEX(0x12,0); // Yellow off
	xrBS->SetGPIOEX(Mask,0); // Red off
};
		
void xrDevice::SystemOn( ){
	xrBS->SetGPIOEX(0x24,0x24); //green on
	StopMask=0;
	xrBS->SetGPIOEX(0x12,0); // Yellow off
	xrBS->SetGPIOEX(0x9,0); // Red off
};

void xrDevice::SystemOff( ){
	xrBS->SetGPIOEX(0x24,0);  //Green Off
	xrBS->SetGPIOEX(0x12,0); // Yellow off
};


		// Send labels info from database #t - command
bool xrDevice::SendLabels(){

	char  cmd[256];
	int TagCnt;
	
	DBTagRecord *tRec;

	int Ant;
	const char *T;
	const char *LBL;
	long Fltr;
	
	try{
		TagCnt = db->GetTagCount(); 
		int SentCnt=0;
		int pingCount=0;
		for(int i=1;i<=TagCnt  && SentCnt<200 ;i++){
			pingCount++;
			if(pingCount==50){
				SendPing();
				pingCount=0;
			}
			tRec =db->GetTag(i);

			if (tRec && !tRec->Sent && !tRec->Deleted ){

				if(VerifyTag(tRec->Tag ,false)){
   					//printf("+\r\n");

					char buf[50];
					sprintf(buf,"%04d-%02d-%02dT%02d:%02d:%02d",tRec->T.wYear,tRec->T.wMonth,tRec->T.wDay ,tRec->T.wHour ,tRec->T.wMinute,tRec->T.wSecond)    ;	
					T=buf;
					Ant =tRec->Ant ;
					LBL=tRec->Tag;
					Fltr = tRec->Filter ;
					

					devSocket->DropData();

					sprintf(cmd,"tdev=%s;ant=%d;dt=%s;tag=%s;fltr=%ld\r\n",Status.xrIP, Ant,T,LBL,Fltr);
					devSocket->Send(cmd,strlen(cmd));
					//printf("Send To Server: %s\r\n", cmd);

					int l=0;
					int wcnt=1000;
					char *res;
					 //Sleep(5);
					while(wcnt && (devSocket->GetDataSize()==0)){
						   Sleep(5);
						   wcnt--;
					}
					printf(" (w%d) ",(1000-wcnt)*5);
					Sleep(5);
					devSocket->GetPacket(res,&l);
					if(l==0){
						return false;
					}
					//printf("Receive size=%d string %s\r\n",  l,res);
					if(!_strnicmp(res,"tsave=ok\r\n",10)){
					   sprintf(msg,"Tag %s sent to server",LBL);
					   SaveLog(msg);
					   tRec->Sent=1;
					   db->UpdateTag(tRec,i); 
					   delete res;				 
					   delete tRec;
					}else{
						delete tRec;
						delete res;				 				
						return false;
					}
					SentCnt++;
				} //  vrrify tag
			}else{
				delete tRec;
			}
		}
	}catch(...){
		return false;
	}
	
	return true;

};




// #p - command
bool xrDevice::SendPing(){
	char  cmd[256];
	try{
		NetworkThreadLoopCount ++;
		devSocket->DropData();

		sprintf(cmd,"pdev=%s;msg=%s\r\n",Status.xrIP,VERSION);
		devSocket->Send(cmd,strlen(cmd));
		printf("<ping>", cmd);

		int l=0;
		int wcnt=1000;
		char *res;
		// Sleep(5);
		while(wcnt && (devSocket->GetDataSize()==0)){
			   Sleep(5);
			   wcnt--;
			   //printf("*");
		}
		printf(" (w%d) ",(1000-wcnt)*5);
		Sleep(5);
		devSocket->GetPacket(res,&l);
		if(l==0){
		   return false;
		}
		//printf("Receive size=%d string:<%s>\r\n",  l,res);
		if(_strnicmp(res,"p\r\n",3)){
		   delete res;
		   return false;
		}
			
		delete res;
		return true;
	}catch(...){
		return false;
	}
	
};



 // Send configuration #d - command
bool  xrDevice::SendSys(){

	try{
		if(Status.Changed ==true){

			char  cmd[256];
			for(int i=0;i<8;i++){
				 
				devSocket->DropData();

				sprintf(cmd,"ddev=%s;off=%d;ntag=%ld\r\n",Status.xrIP,Status.xrStopped,Status.NextTag  );
				devSocket->Send(cmd,strlen(cmd));
				//printf("Send To Server: %s\r\n", cmd);

				int l=0;
				int wcnt=1000;
				char *res;
				// Sleep(5);
				while(wcnt && (devSocket->GetDataSize()==0)){
					   Sleep(5);
					   wcnt--;
					   //printf("*");
				}
				printf(" (w%d) ",(1000-wcnt)*5);
				Sleep(5);
				devSocket->GetPacket(res,&l);
				if(l==0){
					return false;
				}
				//printf("Receive size=%d string %s\r\n",  l,res);
				if(_strnicmp(res,"dsave=ok\r\n",10)){
				   delete res;	
				   return false;
				}
				delete res;
			}
			Status.Changed=false;
		}
		return true;

	}catch(...){
		return false;

	}

}


 // Send configuration #c - comand
bool  xrDevice::SendCFG(){

	try{
		if(Status.Changed ==true){

			char  cmd[256];
			for(int i=0;i<8;i++){
				 
				devSocket->DropData();

				sprintf(cmd,"cdev=%s;ant=%d;on=%d;act=%d;fltr=%x\r\n",Status.xrIP, i+1,Status.xrAntenns[i].anCheckFilter,Status.xrAntenns[i].anActive,Status.xrAntenns[i].FilterType);
				devSocket->Send(cmd,strlen(cmd));
				//printf("Send To Server: %s\r\n", cmd);

				int l=0;
				int wcnt=1000;
				char *res;
				// Sleep(5);
				while(wcnt && (devSocket->GetDataSize()==0)){
					   Sleep(5);
					   wcnt--;
					   //printf("*");
				}
				printf(" (w%d) ",(1000-wcnt)*5);
				Sleep(5);
				devSocket->GetPacket(res,&l);
				if(l==0){
					return false;
				}
				//printf("Receive size=%d string %s\r\n",  l,res);
				if(_strnicmp(res,"csave=ok\r\n",10)){
				   delete res;	
				   return false;
				}
				delete res;
			}
			Status.Changed=false;
		}
		return true;
	}catch(...){
		return false;
	}
};


bool  xrDevice::RcvSubstIDX(bool delta){

	printf ( "RcvSubstIDX\r\n");
	try{
		char  cmd[256];
		devSocket->DropData();
		if (delta)
			sprintf(cmd,"ydev=%s;delta=1\r\n",Status.xrIP);
		else
			sprintf(cmd,"ydev=%s\r\n",Status.xrIP);
		devSocket->Send(cmd,strlen(cmd));
		//printf("Send To Server: %s\r\n", cmd);

		int l=0;
		int wcnt=1000;
		char *res;
		// Sleep(5);
		while(wcnt && (devSocket->GetDataSize()==0)){
			   Sleep(5);
			   wcnt--;
			   //printf("*");
		}
		printf(" (w%d) ",(1000-wcnt)*5);
		Sleep(5);
		devSocket->GetPacket(res,&l);
		if(l==0){
			return false;
		}
		//printf("Receive size=%d string %s\r\n",  l,res);
		if(!_strnicmp(res,"yget=error\r\n",12)){
		   delete res;
		   return false;
		}
    
		int RCount=0;

		try{
		//parse count from strig
		MyArray<char> *split = SplitString(res+1,';');



		if(split){
		
			//printf("vars cnt =%d\r\n",split->Count());
			MyArray<char> * token;
			for(int j=0;j<split->Count();j++){
				char * next;
				next = split->Get(j);
				if(next){
					token=SplitString(next,'=');
					if(token){
						if(token->Count()>1){
							
							if(!_stricmp(token->Get(0) ,"cnt")){

								sscanf(token->Get(1),"%d", &RCount);
							}
						}
						delete token;
					}

				}
			}
			delete split;
		}
		} catch(...){
		}

		delete res;

    	int OK = 0;
		int pingCount=0;
		int maxPingCount=10;
		bool OK1 = false;

		if(!delta) maxPingCount=50;
		// get each config line
		if(RCount >0){
			sprintf(msg,"Load %d tags\r\n",RCount);
			SaveLog(msg);
			for(int r=1;r<=RCount;r++){
				OK1=false;
				for(int onerow=0;onerow<5;onerow++){
					OK1=RcvSubst(r,delta);
					if(OK1) break;
				}
				if(OK1)  
					OK++;
				else
					break;
				pingCount++;
				if(pingCount==maxPingCount){

					// unlock database for scan every 'maxPingCount' tags
					if (delta){
					    xrDevice::DBLocked=false;
						printf("Unlock database for inventory\r\n");
						SendPing();
						
						Sleep(3000);
						int count=0;	
						printf("Try Lock database back for read tag subst\r\n");
						while(xrDevice::DBLocked){
							Sleep(1);
							count++;
							if(count==10000){
								SendPing();
								count=0;
								sprintf(msg,"!!!Direct database unlock from NT thread!!!!");
								SaveLog(msg);
								xrDevice::DBLocked=false;
							}
						}
						xrDevice::DBLocked = true;
					
					}else{
						SendPing();
					}

					pingCount=0;
				}

			}

		}

		if (OK!=RCount){
			sprintf(msg,"Allow %d tags, but load only  %d\r\n",RCount,OK);
			SaveLog(msg);
		}
		return OK==RCount;

	}catch(...){
		
		sprintf(msg,"Exception...  while load subst\r\n");
		SaveLog(msg);
	
		return false;
	}
};



bool  xrDevice::RcvRewriteIDX(){

	//printf ( "RcvRewriteIDX\r\n");
	try{
		char  cmd[256];
		devSocket->DropData();

		sprintf(cmd,"qdev=%s\r\n",Status.xrIP);
		devSocket->Send(cmd,strlen(cmd));
		//printf("Send To Server: %s\r\n", cmd);

		int l=0;
		int wcnt=1000;
		char *res;
		// Sleep(5);
		while(wcnt && (devSocket->GetDataSize()==0)){
			   Sleep(5);
			   wcnt--;
			   //printf("*");
		}
		printf(" (w%d) ",(1000-wcnt)*5);
		Sleep(5);
		devSocket->GetPacket(res,&l);
		if(l==0){
			return false;
		}
		//printf("Receive size=%d string %s\r\n",  l,res);
		if(!_strnicmp(res,"qget=error\r\n",12)){
		   delete res;
		   return false;
		}
    
		int RCount=0;

		try{
		//parse count from strig
		MyArray<char> *split = SplitString(res+1,';');



		if(split){
		
			//printf("vars cnt =%d\r\n",split->Count());
			MyArray<char> * token;
			for(int j=0;j<split->Count();j++){
				char * next;
				next = split->Get(j);
				if(next){
					token=SplitString(next,'=');
					if(token){
						if(token->Count()>1){
							
							if(!_stricmp(token->Get(0) ,"cnt")){

								sscanf(token->Get(1),"%d", &RCount);
							}
						}
						delete token;
					}

				}
			}
			delete split;
		}
		} catch(...){
		}

		delete res;

		//bool OK = true;
		//// get each config line
		//if(RCount >0){

		//	for(int r=1;r<=RCount;r++){
		//		OK = OK && RcvRewrite(r);
		//	}

		//}

		int OK = 0;
		bool OK1 = false;
		// get each config line
		if(RCount >0){

			for(int r=1;r<=RCount;r++){
				OK1=false;
				for(int onerow=0;onerow<5;onerow++){
					OK1=RcvRewrite(r);
					if(OK1) break;
				}
				if(OK1)  OK++;
			}

		}

		return true;

	}catch(...){
		return false;
	}
};

bool xrDevice::RcvSubst(int LineNum, bool delta){

	NetworkThreadLoopCount++;
	char  cmd[256];
    //printf ( "RcvSubst %d\r\n",LineNum);
	devSocket->DropData();

	sprintf(cmd,"zdev=%s;idx=%d\r\n",Status.xrIP,LineNum);
	devSocket->Send(cmd,strlen(cmd));
	//printf("Send To Server: %s\r\n", cmd);

	int l=0;
	int wcnt=1000;
	char *res;
	// Sleep(5);
	while(wcnt && (devSocket->GetDataSize()==0)){
		   Sleep(5);
		   wcnt--;
		   //printf("*");
	}
	printf(" (w%d) ",(1000-wcnt)*5);
	Sleep(5);
	devSocket->GetPacket(res,&l);
	if(l==0){
		return false;
	}
    printf("Receive size=%d string %s\r\n",  l,res);

	if(!_strnicmp(res,"zget=error\r\n",12)){
	   delete res;
	   return false;
	}
    
	bool OK=false;
	bool wastag=false;
    try{
		//parse count from strig
		MyArray<char> *split = SplitString(res+1,';');

	
		char tag[25];
		int passed=0;
		memset (tag,0,25);

		if(split){
		
			//printf("vars cnt =%d\r\n",split->Count());
			MyArray<char> * token;
			passed=0;
			for(int j=0;j<split->Count();j++){
				char * next;
				next = split->Get(j);
				if(next){
					token=SplitString(next,'=');
					if(token){
						if(token->Count()>1){
							//printf("Name=%s, Val=%s\r\n",token->Get(0),token->Get(1));
							if(!_stricmp(token->Get(0),"tag")){
								
								strncpy(tag,token->Get(1),24);
								wastag=true;
							}
							if(!_stricmp(token->Get(0),"passed")){
								sscanf(token->Get(1),"%d",&passed);
							}
							
							
						}
						delete token;
					}

				}
			}
			delete split;


			if(wastag){
				if(VerifyTag(tag,false)){
					DBTagSubst *dbts= new DBTagSubst();
					strncpy(dbts->Tag,tag,24);
					dbts->Passed=(passed!=0);

					/* change received tag in history */
					if(passed == 0 ){
						//sprintf(msg,"Search tag (%s) in history \r\n",tag);
						//SaveLog(msg);
						TagTime *ttFind=0;
						long tIdx;
						int a;
						for (a=1;a<=4;a++){
							ttFind = new TagTime(tag,a);
							tIdx=History->Find(ttFind);
							if (tIdx >=0){
								delete ttFind;
								ttFind=0;
								ttFind=History->Get(tIdx);
								if(ttFind){
									sprintf(msg,"!!!!>>>Change tag %s in HISTORY (REPACKED?)<<<!!!\r\n",tag);
									SaveLog(msg);
									memcpy(ttFind->SubstTag,tag,24);
								}
							}
						}
					}
					
					DWORD pos=0;
					if(delta){
						if(db->CheckTagSubst(dbts,&pos)){
							sprintf(msg,"UPDATE SUBST %s Passed=%d\r\n",tag,passed);
							//SaveLog(msg);
							printf(msg);
							db->UpdateTagSubst(dbts,pos);
							delete dbts;
						}else{
							sprintf(msg,"ADD SUBST %d %s Passed=%d\r\n",LineNum,tag,passed);
							SaveLog(msg);
							db->AddTagSubst(dbts); 
							delete dbts;
						}
					}else{
						sprintf(msg,"ADD SUBST %d %s Passed=%d\r\n",LineNum,tag,passed);
						SaveLog(msg);
						db->AddTagSubst(dbts); 
						delete dbts;
					}
					OK=true;
					
				}else{
					sprintf(msg,"SUBST record %d from server broken tag=%s",LineNum,tag);
					SaveLog(msg);
					OK=true;
				}
			}
		}

	} catch(...){
	}
	if(res){
		try{
			delete res;
		} catch(...){

		}
	}
	
	return OK;
};


bool xrDevice::RcvRewrite(int LineNum){

	char  cmd[256];
    //printf ( "RcvRewrite %d\r\n",LineNum);
	devSocket->DropData();

	sprintf(cmd,"wdev=%s;idx=%d\r\n",Status.xrIP,LineNum);
	devSocket->Send(cmd,strlen(cmd));
	//printf("Send To Server: %s\r\n", cmd);

	int l=0;
	int wcnt=1000;
	char *res;
	// Sleep(5);
	while(wcnt && (devSocket->GetDataSize()==0)){
		  Sleep(5);
		   wcnt--;
		   //printf("*");
	}
	printf(" (w%d) ",(1000-wcnt)*5);
	Sleep(5);
	devSocket->GetPacket(res,&l);
	if(l==0){
		return false;
	}
	//printf("Receive size=%d string %s\r\n",  l,res);
	if(!_strnicmp(res,"wget=error\r\n",12)){
	   delete res;
	   return false;
	}
    
	bool OK = false;
	bool wastag=false;

    try{
		//parse count from strig
		MyArray<char> *split = SplitString(res+1,';');

		int ant=0;
		int off=0;
		int check=0;
		long fltr=0;
		int type=0;
		int stopm=0;
		char tag[25];
		memset (tag,0,25);

		if(split){
		
			//printf("vars cnt =%d\r\n",split->Count());
			MyArray<char> * token;
			for(int j=0;j<split->Count();j++){
				char * next;
				next = split->Get(j);
				if(next){
					token=SplitString(next,'=');
					if(token){
						if(token->Count()>1){
							//printf("Name=%s, Val=%s\r\n",token->Get(0),token->Get(1));
							if(!_stricmp(token->Get(0),"tag")){
								strncpy(tag,token->Get(1),24);
								wastag=true;
							}
							if(wastag){
								if(VerifyTag(tag,false)){
									DBTagRewrite *dbts= new DBTagRewrite();
									strncpy(dbts->Tag,tag,24);
									DWORD pos;
									if(!db->CheckTagRewrite(dbts,&pos)){
										db->AddTagRewrite(dbts); 
									}
									OK=true;
									delete dbts;
								}
							}
						}
						delete token;
					}

				}
			}
			delete split;
		}

	} catch(...){
	}

	delete res;
	
	return OK;
};


// Get Configuration from Server #a - command
bool  xrDevice::RcvCFG(){

	try{
		char  cmd[256];
		devSocket->DropData();

		sprintf(cmd,"adev=%s\r\n",Status.xrIP);
		devSocket->Send(cmd,strlen(cmd));
		//printf("Send To Server: %s\r\n", cmd);

		int l=0;
		int wcnt=1000;
		char *res;
		// Sleep(5);
		while(wcnt && (devSocket->GetDataSize()==0)){
			  Sleep(5);
			   wcnt--;
			   //printf("*");
		}
		printf(" (w%d) ",(1000-wcnt)*5);
		Sleep(5);
		devSocket->GetPacket(res,&l);
		if(l==0){
			return false;
		}
		//printf("Receive size=%d string %s\r\n",  l,res);
		if(!_strnicmp(res,"aget=error\r\n",12)){
		   delete res;
		   return false;
		}
    
		int RCount=0;

		try{
		//parse count from strig
		MyArray<char> *split = SplitString(res+1,';');



		if(split){
		
			//printf("vars cnt =%d\r\n",split->Count());
			MyArray<char> * token;
			for(int j=0;j<split->Count();j++){
				char * next;
				next = split->Get(j);
				if(next){
					token=SplitString(next,'=');
					if(token){
						if(token->Count()>1){
							//printf("Name=%s, Val=%s\r\n",token->Get(0),token->Get(1));
							if(!_stricmp(token->Get(0) ,"cnt")){

								sscanf(token->Get(1),"%d", &RCount);
							}
						}
						delete token;
					}

				}
			}
			delete split;
		}
		} catch(...){
		}

		delete res;

		int OK = 0;
		bool OK1 = false;

		// get each config line
		if(RCount >0){

			for(int r=1;r<=RCount;r++){
				OK1=false;
				for(int onerow=0;onerow<5;onerow++){
					OK1=RcvCFGLine(r);
					if(OK1) break;
				}
				if(OK1)  OK++;
			}

		}

		if(OK>0){
			ReadCFGFromDB();
			Status.Changed=true;
		}

		return true;

	}catch(...){
		return false;
	}
};

bool xrDevice::RcvCFGLine(int LineNum){

	char  cmd[256];
	devSocket->DropData();

	sprintf(cmd,"idev=%s;idx=%d\r\n",Status.xrIP,LineNum);
	devSocket->Send(cmd,strlen(cmd));
	//printf("Send To Server: %s\r\n", cmd);

	int l=0;
	int wcnt=1000;
	char *res;
	// Sleep(5);
	while(wcnt && (devSocket->GetDataSize()==0)){
		  Sleep(5);
		   wcnt--;
		   //printf("*");
	}
    printf(" (w%d) ",(1000-wcnt)*5);
	Sleep(5);
	devSocket->GetPacket(res,&l);
	if(l==0){
		return false;
	}
	//printf("Receive size=%d string %s\r\n",  l,res);
	if(!_strnicmp(res,"iget=error\r\n",12)){
	   delete res;
	   return false;
	}
    
	int OK = 0;
    try{
		//parse count from strig
		MyArray<char> *split = SplitString(res+1,';');

		int ant=0;
		int off=0;
		int check=0;
		unsigned long fltr=0;
		int type=0;
		int stopm=0; /* stop mask */
		int paper=0;
		int nostops=0;

		if(split){
		
			//printf("vars cnt =%d\r\n",split->Count());
			MyArray<char> * token;
			for(int j=0;j<split->Count();j++){
				char * next;
				next = split->Get(j);
				if(next){
					token=SplitString(next,'=');
					if(token){
						if(token->Count()>1){
							//printf("Name=%s, Val=%s\r\n",token->Get(0),token->Get(1));
							if(!_stricmp(token->Get(0),"ant")){
								sscanf(token->Get(1),"%d", &ant);
								//printf("ant=%d\r\n",ant);
								OK++;
							}
							if(!_stricmp(token->Get(0),"off")){
								sscanf(token->Get(1),"%d", &off);
								//printf("off=%d\r\n",off);
								OK++;
							}
							if(!_stricmp(token->Get(0),"check")){
								sscanf(token->Get(1),"%d", &check);
								//printf("check=%d\r\n",check);
								OK++;
							}
							if(!_stricmp(token->Get(0),"type")){
								sscanf(token->Get(1),"%d", &type);
								//printf("type=%d\r\n",type);
								OK++;
							}
							if(!_stricmp(token->Get(0),"stop")){
								sscanf(token->Get(1),"%d", &stopm);
								//printf("stop=%d\r\n",stopm);
								OK++;
							}
							if(!_stricmp(token->Get(0),"fltr")){
								sscanf(token->Get(1),"%lu", &fltr);
								//printf("fltr=%x\r\n",fltr);
								OK++;
							}
							if(!_stricmp(token->Get(0),"paper")){
								sscanf(token->Get(1),"%d", &paper);
								//printf("stop=%d\r\n",stopm);
								OK++;
							}
							if(!_stricmp(token->Get(0),"nostops")){
								sscanf(token->Get(1),"%d", &nostops);
								//printf("fltr=%x\r\n",fltr);
								OK++;
							}


						}
						delete token;
					}

				}
			}
			delete split;
			if (ant>0){
				if (xrDevice::Status.xrAntenns[ant-1].FilterType != fltr){
					xrDevice::BrandChange =true;
				}
				
				

				//xrAntennaType
				if(type!=0){
					SaveCFG2(ant-1,fltr,!off,check,loadPoint,stopm,paper,nostops);
				}else{
					SaveCFG2(ant-1,fltr,!off,check,unloadPoint,stopm,paper,nostops);
				}
			}
			
			
		}

	} catch(...){
	}

	delete res;


	sprintf(cmd,"fdev=%s;idx=%d\r\n",Status.xrIP,LineNum);
	devSocket->Send(cmd,strlen(cmd));
	//printf("Send To Server: %s\r\n", cmd);

	Sleep(50);


	return (OK>=6);
};



// Get Configuration from Server #s - command
bool  xrDevice::RcvSys(){

    char  cmd[256];
	devSocket->DropData();

	sprintf(cmd,"sdev=%s\r\n",Status.xrIP);
	devSocket->Send(cmd,strlen(cmd));
	//printf("Send To Server: %s\r\n", cmd);

	int l=0;
	int wcnt=1000;
	char *res;
	// Sleep(5);
	while(wcnt && (devSocket->GetDataSize()==0)){
		   Sleep(5);
		   wcnt--;
		   //printf("*");
	}
	printf(" (w%d) ",(1000-wcnt)*5);
	Sleep(5);
	devSocket->GetPacket(res,&l);
	if(l==0){
		return false;
	}
	//printf("Receive size=%d string %s\r\n",  l,res);
	if(!_strnicmp(res,"sget=error\r\n",12)){
	   delete res;
	   return false;
	}
    
	

	  try{
		//parse count from strig
		MyArray<char> *split = SplitString(res+1,';');

		SYSTEMTIME T;
		int cnt;
		cnt=0;
		T.wMilliseconds=0;
		if(split){
		
			//printf("vars cnt =%d\r\n",split->Count());
			MyArray<char> * token;
			for(int j=0;j<split->Count();j++){
				char * next;
				next = split->Get(j);
				if(next){
					token=SplitString(next,'=');
					if(token){
						if(token->Count()>1){
							//printf("Name=%s, Val=%s\r\n",token->Get(0),token->Get(1));
							if(!_stricmp(token->Get(0),"year")){
								sscanf(token->Get(1),"%d", &T.wYear);
								cnt++;
							}
							if(!_stricmp(token->Get(0),"month")){
								sscanf(token->Get(1),"%d", &T.wMonth);
								cnt++;
							}
							if(!_stricmp(token->Get(0),"day")){
								sscanf(token->Get(1),"%d", &T.wDay );
								cnt++;
							}
							
							if(!_stricmp(token->Get(0),"hour")){
								sscanf(token->Get(1),"%d", &T.wHour );
								cnt++;
							}
							if(!_stricmp(token->Get(0),"min")){
								sscanf(token->Get(1),"%d", &T.wMinute );
								cnt++;
							}
							
							if(!_stricmp(token->Get(0),"sec")){
								sscanf(token->Get(1),"%d", &T.wSecond);
								cnt++;
							}


						}
						delete token;
					}

				}
			}
			delete split;
			if (cnt==6){
				SetSystemTime(&T);
			}
		}

	} catch(...){
	}
	delete res;

	return true;

};

// Get Labels From Server #b - command
bool  xrDevice::RcvLabels(){
	
    char  cmd[256];
	devSocket->DropData();

	sprintf(cmd,"bdev=%s\r\n",Status.xrIP);
	devSocket->Send(cmd,strlen(cmd));
	//printf("Send To Server: %s\r\n", cmd);

	int l=0;
	int wcnt=1000;
	char *res;
	// Sleep(5);
	while(wcnt && (devSocket->GetDataSize()==0)){
		   Sleep(5);
		   wcnt--;
		   //printf("*");
	}
	printf(" (w%d) ",(1000-wcnt)*5);
	Sleep(5);
	devSocket->GetPacket(res,&l);
	if(l==0){
		return false;
	}
	//printf("Receive size=%d string %s\r\n",  l,res);
	if(!_strnicmp(res,"bget=error\r\n",12)){
	   	delete res;	
	   return false;
	}
    
    try{
	//parse count from strig
	MyArray<char> *split = SplitString(res+1,';');

	if(split){
	
		//printf("vars cnt =%d\r\n",split->Count());
		MyArray<char> * token;
		for(int j=0;j<split->Count();j++){
			char * next;
			next = split->Get(j);
			if(next){
				token=SplitString(next,'=');
				if(token){
					if(token->Count()>1){
						//printf("Name=%s, Val=%s\r\n",token->Get(0),token->Get(1));
					}
					//delete token->Get(0);
					delete token;
				}

			}
		}
		//delete split->Get(0);
		delete split;
	}
	} catch(...){
			delete res;
			return false;
	}
		delete res;
	return true;
};



        // Send event  #e - command
bool xrDevice::SendEvents(){
		
	char  cmd[512];
	int evtCnt;
	
	try{
		evtCnt = db->GetEventCount();
		//printf("SL:Events in base %d\r\n",evtCnt);
		
		int Ant;
		const char *LBL;
		const char *INFO;
		unsigned long FLTR;
		
		DBEventRecord *eRec;
		int SentCnt=0;
		int pingCount=0;
		for(int i=1;i<=evtCnt && SentCnt <=200;i++)
		{
			pingCount++;
			if(pingCount==50){
				SendPing();
				pingCount=0;
			}
			eRec =db->GetEvent(i);

			if(eRec && !eRec->Sent && !eRec->Deleted ){
			char buf[50];
			sprintf(buf,"%04d-%02d-%02dT%02d:%02d:%02d",eRec->T.wYear,eRec->T.wMonth,eRec->T.wDay ,eRec->T.wHour ,eRec->T.wMinute,eRec->T.wSecond)    ;	
			
			Ant =eRec->Ant;
			LBL=eRec->Tag;	
			INFO=eRec->Message ;	
		    FLTR=eRec->Filter ;
			devSocket->DropData();

			sprintf(cmd,"edev=%s;ant=%d;dt=%s;tag=%s;msg=%s;fltr=%ld\r\n",Status.xrIP, Ant,buf,LBL,INFO,FLTR);
			devSocket->Send(cmd,strlen(cmd));
			//printf("Send To Server: %s\r\n", cmd);

			int l=0;
			int wcnt=1000;
			char *res;
			// Sleep(5);
			while(wcnt && (devSocket->GetDataSize()==0)){
				   Sleep(5);
				   wcnt--;
			}
			printf(" (w%d) ",(1000-wcnt)*5);
			Sleep(5);
			devSocket->GetPacket(res,&l);
			if(l==0){
				return false;
			}
			//printf("Receive size=%d string %s\r\n",  l,res);
			if(!_strnicmp(res,"esave=ok\r\n",10)){
			   delete res;
			   eRec->Sent=1;
			   db->UpdateEvent(eRec,i);
			   
			}else{
				delete res;
				delete eRec;
				return false;
			}
			SentCnt++;

			}else{
				delete eRec;
			}
		}
		return true;
	}catch(...){
		return false;
	}
	

};






//////////////////////////////////////////////////
// Database procs



bool xrDevice::VerifyDB(){

 return db->VerifyDB();

}

void xrDevice::CreateDB(){
	
	 db->CreateDB();

}

void xrDevice::CleanDB(){
	DeleteFile(TEXT("\\Data\\TAG.DB"));
	DeleteFile(TEXT("\\Data\\EVENT.DB"));
	DeleteFile(TEXT("\\Data\\CFG2.DB"));
	DeleteFile(TEXT("\\Data\\RCFG.DB"));
	DeleteFile(TEXT("\\Data\\SUBST.DB"));
    db->CreateDB();
}


void xrDevice::SaveLabel( int Ant, const char * LBL, const char *LBLSubst, bool Sent, long Filter){

	if(VerifyTag(LBL,false) && VerifyTag(LBLSubst,false)){
		DBTagRecord tr;
		memset(tr.Tag,0,25);
		strncpy(tr.Tag,LBL,24);
		memset(tr.TagSubst,0,25);
		strncpy(tr.TagSubst,LBLSubst,24);
		tr.Ant=Ant;
		tr.Sent=Sent;
		tr.Filter=Filter;
		db->AddTag(&tr);
	}

};





void xrDevice::SaveEvent(int Ant /* 1..8 */, const char* LBL, char * Message,bool Sent){
	if(VerifyTag(LBL,false)){
		unsigned long TagFilter;
		unsigned long AFilter;
		xrAntennaType atype=Status.xrAntenns[Ant-1].anType;
		AFilter=Status.xrAntenns[Ant-1].FilterType;
		TagFilter = ExtractFilter(LBL);
		if ((TagFilter!=AFilter) && (atype==unloadPoint)){
			DBEventRecord er;
			strcpy(er.Tag,LBL);
			er.Ant=Ant;
			er.Sent=Sent;
			er.Filter=AFilter;
			strncpy(er.Message,Message,100);
			db->AddEvent(&er);
		}
	}
};

void xrDevice::SaveEvent2(int Ant  /* 1..8 */, const char* LBL, char * Message,bool Sent){
	DBEventRecord er;
	strcpy(er.Tag,LBL);
	er.Ant=Ant;
	er.Sent=Sent;
	er.Filter=Status.xrAntenns[Ant-1].FilterType;
	strncpy(er.Message,Message,100);

	db->AddEvent(&er);
	
};

/*
void xrDevice::SaveCFG( int Ant , long FT, int Active, int Check, int AType, int StopMask){
	bool found;
	int cnt;
	int idx;
	DBConfigRecord *crOld;
	cnt =db->GetConfigCount();
	//printf("SaveCFG Records=%d\r\n",cnt);

	found=false;
	for(int i=1;i<=cnt;i++){
		crOld = db->GetPointConfig(i);
		if(crOld){
			if (crOld->Antena==Ant){
				found =true;
				idx=i;
				delete crOld;
				//printf("Found record for ant=%d idx=%d\r\n",Ant,i);
				break;
			}
			delete crOld;
		}
	}
	


	DBConfigRecord cr;
	cr.Antena =Ant;
	cr.Active =(Active!=0);
	cr.Check =(Check!=0);
	cr.PointType =AType;
	cr.Filter=FT;
	cr.StopMask=StopMask;

	if(found){
		sprintf(msg,"Updating config for ant=%d FT=%04lx\r\n",Ant,FT);
		SaveLog(msg);
		db->UpdateConfig(&cr,idx);
	}else{
		sprintf(msg,"add config for ant=%d FT=%04lx\r\n",Ant,FT);
		SaveLog(msg);
		db->SaveConfig(&cr);
	}
};
*/

void xrDevice::SaveCFG2( int Ant /* 0..7 */, long FT, int Active, int Check, int AType, int StopMask, int Paper, int NoStops){
	bool found;
	int cnt;
	int idx;
	DBConfigRecord2 *crOld;
	cnt =db->GetConfigCount2();
	//printf("SaveCFG Records=%d\r\n",cnt);

	found=false;
	for(int i=1;i<=cnt;i++){
		crOld = db->GetPointConfig2(i);
		if(crOld){
			if (crOld->Antena==Ant){
				found =true;
				idx=i;
				delete crOld;
				//printf("Found record for ant=%d idx=%d\r\n",Ant,i);
				break;
			}
			delete crOld;
		}
	}


	DBConfigRecord2 cr;
	cr.Antena =Ant;
	cr.Active =(Active!=0);
	cr.Check =(Check!=0);
	cr.PointType =AType;
	cr.Filter=FT;
	cr.StopMask=StopMask;
	cr.Paper=Paper;
	cr.NoStops =NoStops;

	if(found){
		sprintf(msg,"Updating config for ant=%d FT=%04lx Paper=%d NoStops=%d\r\n",Ant,FT,Paper,NoStops);
		SaveLog(msg);
		db->UpdateConfig2(&cr,idx);
	}else{
		sprintf(msg,"Add config for ant=%d FT=%04lx Paper=%d NoStops=%d\r\n",Ant,FT,Paper,NoStops);
		SaveLog(msg);
		db->SaveConfig2(&cr);
	}
};

/*
void xrDevice::FillDebugDB(){
	if (db !=NULL){

		DBReaderConfigRecord *rc = new DBReaderConfigRecord();
		strcpy(rc->ServerIP,"127.0.0.1");
		rc->Port=3200;
		strcpy(rc->xrIP,"127.0.0.1");

		db->SaveReaderConfig(rc);
		delete rc;

		
		SaveCFG2(0,0,0,0,1,9,0,0);
		SaveCFG2(1,0,0,0,1,9,0,0);
		SaveCFG2(2,0,0,0,1,9,0,0);
		SaveCFG2(3,0,0,0,1,9,0,0);
		SaveCFG2(4,0,0,0,1,9,0,0);
		SaveCFG2(5,0,0,0,1,9,0,0);
		SaveCFG2(6,0,0,0,1,9,0,0);
		SaveCFG2(7,0,0,0,1,9,0,0);

	}
	
}
*/

void xrDevice::SaveRCFG(){
	    DBReaderConfigRecord *rc = new DBReaderConfigRecord();
		strcpy(rc->ServerIP,Status.xrServerIP );
		rc->Port=Status.xrServerPort ;
		strcpy(rc->xrIP,Status.xrIP );
		rc->NextTag=Status.NextTag; 
		db->SaveReaderConfig(rc);
		delete rc;
}

void xrDevice::DeleteBoxes(){
//BOXES(TAG 

};

void xrDevice::DeleteSentLabels(){
	db->DeleteSentTag();
}

void xrDevice::DeleteSentEvents(){
	db->DeleteSentEvent();
}


bool xrDevice::CheckThisTag(char * TagId,int Ant){
	/*if(!_strnicmp(TagId,Status.xrAntenns[Ant-1].LastWrittenTag)){
		return true;
	}*/

	DBTagRecord tr;
	strcpy(tr.Tag ,TagId);
	tr.Ant=Ant;
	return db->CheckTag(&tr);
};


MyArray<char> * xrDevice::SplitString( const char * str, char Splitter){
	MyArray<char> *lChar= new MyArray<char>();
	
	char * buf = new char[strlen(str)+1];
	char * ptr;
	char * s;
	int sz;
	memset(buf,0,strlen(str)+1);
	strncpy(buf,str,strlen(str));
	ptr =StrReplaceChar(buf);
	delete buf;
	buf = ptr;
	ptr=buf;

	for(unsigned int i=0; i <strlen(str);i++){
		if(str[i]==Splitter){
			buf[i]=0;
			sz=strlen(ptr);
			s = new char[sz+1];
			memset(s,0,sz+1);
			strcpy(s,ptr);
			lChar->Add(s);
			ptr =buf+i+1;
		}
	}
	sz=strlen(ptr);
	s = new char[sz+1];
	memset(s,0,sz+1);
	strcpy(s,ptr);
	lChar->Add(s);

	delete buf;
    return lChar;
}


//MyArray<DelayedTagError> * xrDevice::TagErrors;

//long xrDevice::FindErrorIdx(const char * TagID,int Antenna){
//	for(long i=0;i<TagErrors->Count();i++){
//		if(TagErrors->Get(i)->Antenna==Antenna){
//			if(!_strnicmp(TagID,TagErrors->Get(i)->Tag)){
//				return i;
//			}
//		}
//	}
//	return -1;
//
//};

long xrDevice::FindUtagIdx(const char * TagID,int Antenna){
	for(long i=0;i<UTags->Count();i++){
		if((UTags->Get(i))->Ant==Antenna){
			if(!_stricmp(TagID,(UTags->Get(i))->NewTag)){
				return i;
			}
		}
	}
	return -1;

};

// создание новой метки для записи некондиционного кода
char* xrDevice::BuildNewTag(int BoxType,unsigned long Filter){
	
	char *OldTag;
	OldTag= new char[25];
	
	char *NewTag;
	NewTag= new char[25];

	memset(OldTag,'0',25);
	OldTag[24]=0;
	
	int iLen=strlen(Status.xrIP);
	char pnum[3];
	pnum[0]=Status.xrIP[iLen-3];
	pnum[1]=Status.xrIP[iLen-2];
	pnum[2]=Status.xrIP[iLen-1];
	if (pnum[0]=='.') pnum[0]='0';
		
	sprintf(OldTag,"%02d%c%c%c%05lx00000",BoxType,pnum[0],pnum[1],pnum[2],Status.NextTag);
	
	Status.NextTag++;

	char * c12=Calc12(OldTag);
	OldTag[10]=c12[0];
	OldTag[11]=c12[1];
    
	delete [] c12;

	memset(NewTag,0,25);
	strncpy(NewTag,OldTag,12);
	char buf[13];
	char * c24;
	sprintf(buf,"%010lX",Filter);
	strcat(NewTag,buf);
	c24=Calc24(NewTag);
	if (c24[0]==' '){
		c24[0]='0';
	}
	strcat(NewTag,c24);
	delete [] c24;
	delete [] OldTag;
	
	SaveRCFG();

	return NewTag;
}