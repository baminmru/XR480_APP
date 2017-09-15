#include "FRTT.H"
#include "CESocket.h"
#include "template.h"
#include "mystring.h"

//const char* dbPath = "\\data\\frtt.db";


char xrDevice::StopMask=0;
CRITICAL_SECTION xrDevice::Critical;
xrDeviceStatus xrDevice::Status ;
CCESocket *xrDevice::devSocket=NULL; 
xrByteStream *xrDevice::xrBS = NULL;
int xrDevice::PacketOK;
HANDLE xrDevice::hNetworkThread;
HANDLE xrDevice::hInventoryThread;
MyDB *xrDevice::db;	
bool xrDevice::HALT=false;
bool xrDevice::StopIt=false;
bool xrDevice::SaveNewTag=false;
char*  xrDevice::NewTag;

bool xrDevice::isPaper( char *TagID){
					   //112233445566778899001122	
	if(!_strnicmp(TagID,"03",2)) return true;

	return false;
}


bool xrDevice::isPlastic( char *TagID){
					   //112233445566778899001122	
	if(!_strnicmp(TagID,"01",2)) return true;
	if(!_strnicmp(TagID,"02",2)) return true;
	return false;
}


bool xrDevice::IsDeviceOFFTag( char *TagID){
					   //112233445566778899001122	
	if(!_strnicmp(TagID,"D0FF000000",10)) return true;

	return false;
}
bool xrDevice::IsDeviceONTag( char *TagID){
					   //112233445566778899001122	
	if(!_strnicmp(TagID,"DADD000000",10)) return true;

	return false;
}


bool xrDevice::IsAntennaOFFTag( char *TagID){
					   //112233445566778899001122	
	if(!_strnicmp(TagID,"A0FF000000",10)) return true;

	return false;
}

bool xrDevice::IsAntennaONTag( char *TagID){
					   //112233445566778899001122	
	if(!_strnicmp(TagID,"AADD000000",10)) return true;
	

	return false;
}


bool xrDevice::IsCheckOFFTag( char *TagID){
		 			   //112233445566778899001122	
	if(!_strnicmp(TagID,"F0FF000000",10)) return true;

	return false;
}

bool xrDevice::IsCheckONTag( char *TagID){
		 			   //112233445566778899001122	
	if(!_strnicmp(TagID,"FADD000000",10)) return true;

	return false;
}



bool xrDevice::IsFilterChangeTag( char *TagID){
					   //112233445566778899001122	
	if(!_strnicmp(TagID,"FF00000000",10)) return true;

	return false;
}

unsigned long xrDevice::ExtractFilter( char *TagID){
	unsigned long f;
	char *buf;
	SetupString(TagID,buf);
	buf[22]=0;
	sscanf(buf+12,"%lx",&f);
	printf("Extract Filter:TAG=%s, Filter=%lx\r\n",TagID,f);
	delete [] buf;
	return f;
}

void xrDevice::PrepareTag( unsigned long Filter, const char * OldTag, char *aNewTag){
	memset(NewTag,0,25);
	strncpy(NewTag,OldTag,12);
	char buf[13];
	char * c24;
	sprintf(buf,"%010lx",Filter);
	strcat(NewTag,buf);
	c24=Calc24(NewTag);
	strcat(NewTag,c24);
	delete c24;

	printf("Prepare Tag: Old=%s, New=%s\r\n",OldTag,aNewTag);
}

char *  xrDevice::Calc24(const char * Tag){

	const char *pstr = Tag;
	char *buf = new  char[3];
	memset(buf,0,3);
	unsigned int x;
	int s=0;

	

	for (int j=0;j<22;j++)
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
	sprintf(buf,"%2X",s);
	return buf;

}

 


bool xrDevice::Error;
char* xrDevice::Event;

void xrDevice::ProcessTag(char * TagID, int Antena, bool PaperOnly){
	printf("\r\nProcess Tag  %s\r\n", TagID);

	Error=false;
	

	
	
	if(Status.xrAntenns[Antena-1].anActive){
		EnterCriticalSection(&Critical);

		printf("PT:Antena is active\r\n");
		
	
		
		// ѕроверка на тип метки
		if(IsDeviceOFFTag(TagID)){
			// это метка останова контрол€  на устройстве?
			Status.xrStopped = 1;
			Status.Changed=true;
			SaveEvent(Antena-1,TagID,"Device Off",false);
			printf("PT:Device Stop Trigger\r\n");
			return;
		}

		// ѕроверка на тип метки
		if(IsDeviceONTag(TagID)){
			// это метка останова контрол€  на устройстве?
			Status.xrStopped = 0;
			Status.Changed=true;
			SaveEvent(Antena-1,TagID,"Device ON",false);
			printf("PT:Device On Trigger\r\n");
			return;
		}

		// ѕроверка на тип метки
		if(IsCheckOFFTag(TagID)){
			// это метка останова контрол€ на антене?
			//SaveEvent
			Status.xrAntenns[Antena-1].anCheckFilter = 0;
			Status.Changed=true;
			SaveCFG(Antena-1,Status.xrAntenns[Antena-1].FilterType, !Status.xrAntenns[Antena-1].anActive ,!Status.xrAntenns[Antena-1].anCheckFilter ,Status.xrAntenns[Antena-1].anType,Status.xrAntenns[Antena-1].StopMask) ;
			SaveEvent(Antena-1,TagID,"Check OFF",false);
			printf("PT:Check Off Trigger\r\n");
			return;
		}
		
			// ѕроверка на тип метки
		if(IsCheckONTag(TagID)){
			// это метка останова контрол€ на антене?
			//SaveEvent
			Status.xrAntenns[Antena-1].anCheckFilter = 1;
			SaveCFG(Antena-1,Status.xrAntenns[Antena-1].FilterType, !Status.xrAntenns[Antena-1].anActive ,!Status.xrAntenns[Antena-1].anCheckFilter ,Status.xrAntenns[Antena-1].anType,Status.xrAntenns[Antena-1].StopMask) ;
			Status.Changed=true;
			SaveEvent(Antena-1,TagID,"Check ON",false);
			printf("PT:Check ON Trigger\r\n");
			return;
		}

		
		// ѕроверка на тип метки
		if(IsAntennaOFFTag(TagID)){
			// это метка отключени€ антенны?
			//SaveEvent
			Status.xrAntenns[Antena-1].anActive =  0;
			Status.Changed=true;
			SaveCFG(Antena-1,Status.xrAntenns[Antena-1].FilterType, !Status.xrAntenns[Antena-1].anActive ,!Status.xrAntenns[Antena-1].anCheckFilter ,Status.xrAntenns[Antena-1].anType,Status.xrAntenns[Antena-1].StopMask) ;
			SaveEvent(Antena-1,TagID,"Antenna OFF",false);
			printf("PT:Antenna Off Trigger\r\n");
			return;
		}
		
		// ѕроверка на тип метки
		if(IsAntennaONTag(TagID)){
			// это метка отключени€ антенны?
			//SaveEvent
			Status.xrAntenns[Antena-1].anActive =  1;
			Status.Changed=true;
			SaveCFG(Antena-1,Status.xrAntenns[Antena-1].FilterType, !Status.xrAntenns[Antena-1].anActive ,!Status.xrAntenns[Antena-1].anCheckFilter ,Status.xrAntenns[Antena-1].anType,Status.xrAntenns[Antena-1].StopMask) ;
			SaveEvent(Antena-1,TagID,"Antenna ON",false);
			printf("PT:Antenna On Trigger\r\n");
			return;
		}


		if(IsFilterChangeTag(TagID)){
			// это метка смены фильтра ?
			DWORD newType;
			newType=ExtractFilter(TagID);
			Status.xrAntenns[Antena-1].FilterType = newType;
			//Status.xrAntenns[Antena-1].anCheckFilter = 1;
			Status.Changed=true;
			SaveCFG(Antena-1,Status.xrAntenns[Antena-1].FilterType, !Status.xrAntenns[Antena-1].anActive ,!Status.xrAntenns[Antena-1].anCheckFilter ,Status.xrAntenns[Antena-1].anType,Status.xrAntenns[Antena-1].StopMask) ;
			SaveEvent(Antena-1,TagID,"Manual filter change",false);
			printf("PT:Change Filter Tag New Filter type=%x\r\n",newType);
			return;
		}

		if(Status.xrAntenns[Antena-1].anCheckFilter ){
		
			printf("PT:Verify Tag\r\n");

			// ѕроверить не обрабатывали ли мы недавно эту метку !!!
			if(CheckThisTag(TagID,Antena)==false){
				
		
				
				if(Status.xrAntenns[Antena-1].anType==loadPoint){
					if((PaperOnly && isPaper(TagID)) || !PaperOnly){

						printf("PT:LoadPoint -> Set Filter Type to %x\r\n",Status.xrAntenns[Antena-1].FilterType);
						// «аписать тип фильтра на этой антене
						

						NewTag = new char[25];

						//printf("PT:Prepare Tag\r\n");
						PrepareTag( Status.xrAntenns[Antena-1].FilterType,TagID,NewTag);

						printf("PT:Writing Tag\r\n");
						if (xrBS->WriteTag(TagID,NewTag,Antena)){
							
							printf("PT:Save Tag\r\n");
							SaveNewTag= true;
						}else{
							long eIdx;
							eIdx =FindErrorIdx (NewTag,Antena);
							if (eIdx==-1){
								DelayedTagError * dte = new DelayedTagError(NewTag,Antena);
								TagErrors->Add(dte);
							}
							printf("PT:Event - Tag writing error %s\r\n", TagID);
							
						}
					}
			
				}

				if(Status.xrAntenns[Antena-1].anType==unloadPoint){
					printf("PT:unload point \r\n");
					unsigned long TagFilter;
					TagFilter =ExtractFilter(TagID);
					
					if(TagFilter == Status.xrAntenns[Antena-1].FilterType ){

						printf("PT:Erase Tag\r\n");
						NewTag = new char[25];

						PrepareTag( 0,TagID,NewTag);
						if(xrBS->WriteTag(TagID,NewTag,Antena)){
							SaveNewTag= true;
						}else{
							long eIdx;
							eIdx =FindErrorIdx (NewTag,Antena);
							if (eIdx==-1){
								DelayedTagError * dte = new DelayedTagError(NewTag,Antena);
								TagErrors->Add(dte);
								printf("PT:Delayed writing error Tag: %s\r\n", NewTag);
							}
							
						}
					
					}else{
						printf("PT:Event - Filter type error error. Required: %x\r\n",Status.xrAntenns[Antena-1].FilterType);
						Event="Filter error"; 
						Error= true;
						StopIt=true;
						StopMask =  Status.xrAntenns[Antena-1].StopMask ;
				
					}
				}
		
			}
		}
		LeaveCriticalSection(&Critical);
	}

	
	

	printf("End Process Tag  %s\r\n", TagID);
	
};


xrDevice::xrDevice(){
	printf("Open xrBS\r\n");
	InitializeCriticalSection(&Critical);
	if (xrBS == NULL){
		xrBS = new xrByteStream (this,"127.0.0.1",3000,"admin","admin");

		BSLoginOK=xrBS->Reconect();
		
		if(!BSLoginOK){
			printf("BS Connect error %d\r\n",xrBS->GetLastError());
			return;
		}
	
	}

	TagErrors = new MyArray<DelayedTagError>();

	printf("Prepare socket for DB Server\r\n");

		// process
	if(devSocket==NULL){
		devSocket = new CCESocket();
	}

	printf("Init configuration\r\n");
	InitDevice();

}


xrDevice::~xrDevice(){
	CloseHandle(hNetworkThread);
	CloseHandle(hInventoryThread);
	db->Close();
	delete db;
	db =NULL;
	delete TagErrors;

	if(devSocket->GetSocketState()==CCESocket::CONNECTED){
		devSocket->Disconnect();
		
	}
	if(devSocket!=NULL){
		delete devSocket;
	}

	devSocket=NULL;
	if(xrBS !=NULL){
		xrBS->Logout();
		xrBS->Disconnect(); 
		delete xrBS;
	}
	xrBS = NULL;
}




void xrDevice::Run(){
	//BOOL bSuccess = FALSE;
	char bChar = 0;
	

	for(int i=0;i<4;i++){
		xrBS->SetGPIOEX(0x3f,0x24);
		Sleep(1000);
		xrBS->SetGPIOEX(0x3f,0x12);
		Sleep(1000);
	}
	
	xrBS->SetGPIOEX(0x3f,0x24);
	

	hNetworkThread = CreateThread(NULL, 0, (unsigned long (__cdecl *)(void *))xrDevice::NetworkThread, (LPVOID)(this) ,0, (unsigned long *)0);

	ResumeThread(hNetworkThread);

	hInventoryThread = CreateThread(NULL, 0, (unsigned long (__cdecl *)(void *))xrDevice::InventoryThread, (LPVOID)(this) ,0, (unsigned long *)0);
	
	ResumeThread(hInventoryThread);



}




// exchange loop thread
void  xrDevice::InventoryThread(void * Param){

	printf("IT:Start\r\n");

	unsigned int EmptyLoopCnt=0;
	int TagCnt;
	TagInfo * tag;
	bool PaperOnly;
	bool WasPlastic;
	bool WasPaper;


	db->DeleteSentTag();
	db->DeleteSentEvent();

	SystemOn();
	while(!HALT){

		//  antenna loop
		for (int a=1;a<=8;a++){

			//printf("IT: Query Ant=%d\r\n",a );
			if(Status.xrAntenns[a-1].anActive){

				//printf("IT: Ant=%d is active %d\r\n",a,Status.xrAntenns[a-1].anActive );

				TagCnt =xrBS->GetTagCount2(a);
				if(TagCnt>0){

					PaperOnly=false;
					WasPlastic=false;
					WasPaper=false;

					// ѕровер€ем ситуацию с бумажным лотком , вложенным в пластиковый лоток дл€ точки загрузки
					if(TagCnt>1  && Status.xrAntenns[a-1].anType==loadPoint){
						//  проверить  нет ли пары бумашный лоток + пластиковый лоток
						for(int i=0;i<TagCnt  ;i++){
							tag = xrBS->GetTagInfo(i);
							if(tag){
								if(isPlastic(tag->TagID))
									WasPlastic = true;
								if(isPaper(tag->TagID))
									WasPaper = true;

							}
						}
						if(WasPlastic && WasPaper)
							PaperOnly = true;
					}


					EmptyLoopCnt=0;
					
					LabelOK();
					

					//scan tags
					for(int i=0;i<TagCnt  ;i++){

							
	
						tag = xrBS->GetTagInfo(i);
						if(tag){
							printf("IT: Tag=%s,Ant=%d, Time=%ld\r\n",tag->TagID , tag->Antenna,GetTickCount() );
							ProcessTag(tag->TagID , tag->Antenna,PaperOnly);
						}else{
							printf("IT:Tag=%d is null,Ant=%d, Time=%ld\r\n",i , a,GetTickCount() );
						} 
						if(SaveNewTag){
							EnterCriticalSection(&Critical);
							SaveLabel(tag->Antenna,NewTag,0);
							LeaveCriticalSection(&Critical);
							delete NewTag;
							SaveNewTag=false;
						}
						
						long eIdx;
						eIdx = FindErrorIdx(tag->TagID,tag->Antenna);
						if(eIdx>=0){
							
							DelayedTagError *dte =TagErrors->Get(eIdx);
							if(dte->StopTick>=GetTickCount()){
								printf("Clean delayed error for tag=%s\r\n",dte->Tag); 	
								TagErrors->Erase(eIdx);
							}
						}
					}
					LabelOff();
				}

				if(StopIt){
					StopIt=false;
					SystemStop(a,"","");
					StopMask=0;
					xrBS->Reconect();
				}
				for(int err=0;err<TagErrors->Count();err++){
					DelayedTagError *dte =TagErrors->Get(err);
					if(dte->StopTick < GetTickCount()){
							printf("Stop from delayed error for tag=%s   %lu <%lu \r\n",dte->Tag,dte->StopTick ,GetTickCount());
							EnterCriticalSection(&Critical);
							
							SaveEvent(dte->Antenna ,dte->Tag ,"Tag writing error",0);
							StopMask = StopMask ^ Status.xrAntenns[dte->Antenna-1].StopMask;
							StopIt= true;
							LeaveCriticalSection(&Critical);

							TagErrors->Erase(err);
							
							 
					}
				}
				if(StopIt){
					StopIt=false;
					SystemStop(a,"","");
					StopMask=0;
					xrBS->Reconect();
				}
			
			}

		}

		EmptyLoopCnt++;
		if(EmptyLoopCnt >10){
			if(EmptyLoopCnt % 100==0){
				printf("IT: %u  Empty loops :-(\r\n",EmptyLoopCnt);
				
			}
			if(EmptyLoopCnt == 110){
				printf("IT: Deleting events\r\n");
				db->DeleteSentEvent();  
			}
			if(EmptyLoopCnt == 120){
				printf("IT: Deleting tags\r\n");
				db->DeleteSentTag();  
			}
			if(EmptyLoopCnt==200){
				printf("IT: Try to turn off Red Light\r\n");
				xrBS->Reconect();
				SystemOff();
				SystemOn();
				EmptyLoopCnt=0;
			}
		}
			
			

		

	}
	SystemOn();
	SystemOff();

}

// exchange loop thread
void  xrDevice::NetworkThread(void * Param){
	
	printf("NT::Network thread start\r\n");
	int count=0;
	while(!HALT){

		printf("NT::Waiting\r\n");
		count =0;

		
		// 5 * 60 second sleep
		while(count<60){
			Sleep(1000);
			count++;
		}
		
		

		printf("NT::Try to open socket\r\n");
		
		bool result =devSocket->Create(SOCK_STREAM);

		// open server socket
		result =devSocket->Connect(Status.xrServerIP ,Status.xrServerPort); 
		
		if (result) {

			Sleep(100);

		    printf("NT::Sending data\r\n");

			if(!HALT){
				if(SendPing()){
			
					// process data exchange
					//if(!HALT) SendCFG();
					if(!HALT) SendLabels();
					if(!HALT) SendEvents();
					if(!HALT) RcvSys();
					if(!HALT) RcvCFG();
					//if(!HALT) RcvLabels();
				}
			}
			
			printf("NT::Disconnecting\r\n");
			devSocket->Disconnect();
		}else{
			printf("NT::Open Error\r\n");
		}

		
		
	}
};


		// init device on startup
void  xrDevice::InitDevice(){

	
	HALT = false;

	printf("Open database\r\n");
	db= new MyDB();
	db->Open();
	
	printf("Verify DB\r\n");
	if(! VerifyDB()){
		printf("Create DB\r\n");
		CreateDB();
		FillDebugDB();
	}

	

	printf("Read SRVADDR\r\n");
	//SRVADDR(SrvIP,Port,MyIP)
	DBReaderConfigRecord *rc = db->GetReaderConfig();

	if(rc)
    {
		strcpy(Status.xrServerIP , rc->ServerIP);
		Status.xrServerPort =rc->Port;
		strcpy(Status.xrIP ,rc->xrIP);
		printf("Server IP %s, Port %d, MyIP %s\r\n", Status.xrServerIP,Status.xrServerPort,Status.xrIP);
		delete rc;
    }		

	ReadCFGFromDB();
	Status.Changed=true;

	DeleteSentEvents();
	DeleteSentLabels(); 

};


void xrDevice::ReadCFGFromDB(){
	DBConfigRecord *ac;
		int idx;
	for(idx=0 ;idx <db->GetConfigCount() ;idx++){
		Status.xrAntenns[idx].anActive=false;
		ac =db->GetPointConfig(idx+1);	
		printf("Load config for antenna %d Filter %x  Type=%d Check=%d Active=%d \r\n", ac->Antena,ac->Filter,ac->PointType ,ac->Check,ac->Active  );
		if(ac){
			Status.xrAntenns[ac->Antena].anNumber =ac->Antena;
			Status.xrAntenns[ac->Antena].FilterType =ac->Filter; 
			Status.xrAntenns[ac->Antena].anActive = ac->Active ;
			Status.xrAntenns[ac->Antena].anCheckFilter  =ac->Check ;
			Status.xrAntenns[ac->Antena].StopMask  =ac->StopMask ;
			Status.xrAntenns[ac->Antena].anType=(xrAntennaType)ac->PointType;
			delete ac;
		}
         
    }
	Status.Changed=true;
}


// Stop system on error
void  xrDevice::SystemStop(int Antena, char * TagID, char * Message){
	xrBS->SetGPIOEX(0x9,StopMask);
	//xrBS->SetGPIOEX(0x24,0); // green off
};

	// Label OK 
void xrDevice::LabelOK( ){
	xrBS->SetGPIOEX(0x12,0x12);
};

	// Label OK 
void xrDevice::LabelOff( ){
	xrBS->SetGPIOEX(0x12,0x0);
};


		
void xrDevice::SystemOn( ){
	xrBS->SetGPIOEX(0x24,0x24); //green on
	StopMask=0;
	xrBS->SetGPIOEX(0x9,StopMask); // Red off

};

void xrDevice::SystemOff( ){
	xrBS->SetGPIOEX(0x24,0);  //Green Off
};


		// Send labels info from database #t - command
bool xrDevice::SendLabels(){

	char  cmd[256];
	int TagCnt;
	
	DBTagRecord *tRec;

	int Ant;
	const char *T;
	const char *LBL;
	

	TagCnt = db->GetTagCount(); 
	printf("SL:Tags in base %d\r\n",TagCnt);
	
	for(int i=1;i<=TagCnt;i++){
	    printf("t");
		tRec =db->GetTag(i);

		if (tRec && !tRec->Sent && !tRec->Deleted ){
   			printf("+\r\n");

			char buf[50];
			sprintf(buf,"%04d-%02d-%02dT%02d:%02d:%02d",tRec->T.wYear,tRec->T.wMonth,tRec->T.wDay ,tRec->T.wHour ,tRec->T.wMinute,tRec->T.wSecond)    ;	
			T=buf;
			Ant =tRec->Ant ;
			LBL=tRec->Tag;	
			

			devSocket->DropData();

			sprintf(cmd,"tdev=%s;ant=%d;dt=%s;tag=%s\r\n",Status.xrIP, Ant,T,LBL);
			devSocket->Send(cmd,strlen(cmd));
			printf("Send To Server: %s\r\n", cmd);

			int l=0;
			int wcnt=1000;
			char *res;
			while(wcnt && (devSocket->GetDataSize()==0)){
				   Sleep(10);
				   wcnt--;
				   printf("*");
			}
			printf("\r\n");
			devSocket->GetPacket(res,&l);
			printf("Receive size=%d string %s\r\n",  l,res);
			if(!strncmp(res,"tsave=ok\r\n",10)){
				
			   tRec->Sent=1;
			   db->UpdateTag(tRec,i); 
			   delete tRec;
			}else{
				delete tRec;
				return false;
			}

		}else{
			delete tRec;
		}
    }

	
	return true;

};




// #p - command
bool xrDevice::SendPing(){
	char  cmd[256];
	devSocket->DropData();

	sprintf(cmd,"pdev=%s\r\n",Status.xrIP);
	devSocket->Send(cmd,strlen(cmd));
	printf("Send To Server: %s\r\n", cmd);

	int l=0;
	int wcnt=1000;
	char *res;
	while(wcnt && (devSocket->GetDataSize()==0)){
		   Sleep(10);
		   wcnt--;
		   printf("*");
	}
	printf("\r\n");
	devSocket->GetPacket(res,&l);
	printf("Receive size=%d string:<%s>\r\n",  l,res);
	if(strncmp(res,"p\r\n",3)){
	   delete res;
	   return false;
	}
		
	delete res;
	return true;
};



 // Send configuration #d - command
bool  xrDevice::SendSys(){

	if(Status.Changed ==true){

		char  cmd[256];
		for(int i=0;i<8;i++){
			 
			devSocket->DropData();

			sprintf(cmd,"ddev=%s;off=%d\r\n",Status.xrIP,Status.xrStopped );
			devSocket->Send(cmd,strlen(cmd));
			printf("Send To Server: %s\r\n", cmd);

			int l=0;
			int wcnt=1000;
			char *res;
			while(wcnt && (devSocket->GetDataSize()==0)){
				   Sleep(10);
				   wcnt--;
				   printf("*");
			}
			printf("\r\n");
			devSocket->GetPacket(res,&l);
			printf("Receive size=%d string %s\r\n",  l,res);
			if(strncmp(res,"dsave=ok\r\n",10)){
			   delete res;	
			   return false;
			}
			delete res;
		}
		Status.Changed=false;
	}
	return true;

}


 // Send configuration #c - comand
bool  xrDevice::SendCFG(){

	if(Status.Changed ==true){

		char  cmd[256];
		for(int i=0;i<8;i++){
			 
			devSocket->DropData();

			sprintf(cmd,"cdev=%s;ant=%d;on=%d;act=%d;fltr=%x\r\n",Status.xrIP, i+1,Status.xrAntenns[i].anCheckFilter,Status.xrAntenns[i].anActive,Status.xrAntenns[i].FilterType);
			devSocket->Send(cmd,strlen(cmd));
			printf("Send To Server: %s\r\n", cmd);

			int l=0;
			int wcnt=1000;
			char *res;
			while(wcnt && (devSocket->GetDataSize()==0)){
				   Sleep(10);
				   wcnt--;
				   printf("*");
			}
			printf("\r\n");
			devSocket->GetPacket(res,&l);
			printf("Receive size=%d string %s\r\n",  l,res);
			if(strncmp(res,"csave=ok\r\n",10)){
			   delete res;	
			   return false;
			}
			delete res;
		}
		Status.Changed=false;
	}
	return true;
};

// Get Configuration from Server #a - command
bool  xrDevice::RcvCFG(){


	char  cmd[256];
	devSocket->DropData();

	sprintf(cmd,"adev=%s\r\n",Status.xrIP);
	devSocket->Send(cmd,strlen(cmd));
	printf("Send To Server: %s\r\n", cmd);

	int l=0;
	int wcnt=1000;
	char *res;
	while(wcnt && (devSocket->GetDataSize()==0)){
		   Sleep(10);
		   wcnt--;
		   printf("*");
	}
	printf("\r\n");
	devSocket->GetPacket(res,&l);
	printf("Receive size=%d string %s\r\n",  l,res);
	if(!strncmp(res,"aget=error\r\n",12)){
	   delete res;
	   return false;
	}
    
	int RCount=0;
    try{
	//parse count from strig
	MyArray<char> *split = SplitString(res+1,';');



	if(split){
	
		printf("vars cnt =%d\r\n",split->Count());
		MyArray<char> * token;
		for(int j=0;j<split->Count();j++){
			char * next;
			next = split->Get(j);
			if(next){
				token=SplitString(next,'=');
				if(token){
					if(token->Count()>1){
						printf("Name=%s, Val=%s\r\n",token->Get(0),token->Get(1));
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

	bool OK = true;
	// get each config line
	if(RCount >0){

		for(int r=1;r<=RCount;r++){
			OK = OK && RcvCFGLine(r);
		}

	}

	if(OK){
		ReadCFGFromDB();
		Status.Changed=true;
	}

	return true;

};

bool xrDevice::RcvCFGLine(int LineNum){

	char  cmd[256];
	devSocket->DropData();

	sprintf(cmd,"idev=%s;idx=%d\r\n",Status.xrIP,LineNum);
	devSocket->Send(cmd,strlen(cmd));
	printf("Send To Server: %s\r\n", cmd);

	int l=0;
	int wcnt=1000;
	char *res;
	while(wcnt && (devSocket->GetDataSize()==0)){
		   Sleep(10);
		   wcnt--;
		   printf("*");
	}
	printf("\r\n");
	devSocket->GetPacket(res,&l);
	printf("Receive size=%d string %s\r\n",  l,res);
	if(!strncmp(res,"iget=error\r\n",12)){
	   delete res;
	   return false;
	}
    

    try{
		//parse count from strig
		MyArray<char> *split = SplitString(res+1,';');

		int ant=0;
		int off=0;
		int check=0;
		long fltr=0;
		int type=0;
		int stopm=0;

		if(split){
		
			printf("vars cnt =%d\r\n",split->Count());
			MyArray<char> * token;
			for(int j=0;j<split->Count();j++){
				char * next;
				next = split->Get(j);
				if(next){
					token=SplitString(next,'=');
					if(token){
						if(token->Count()>1){
							printf("Name=%s, Val=%s\r\n",token->Get(0),token->Get(1));
							if(!_stricmp(token->Get(0),"ant")){
								sscanf(token->Get(1),"%d", &ant);
								printf("ant=%d\r\n",ant);
							}
							if(!_stricmp(token->Get(0),"off")){
								sscanf(token->Get(1),"%d", &off);
								printf("off=%d\r\n",off);
							}
							if(!_stricmp(token->Get(0),"check")){
								sscanf(token->Get(1),"%d", &check);
								printf("check=%d\r\n",check);
							}
							if(!_stricmp(token->Get(0),"type")){
								sscanf(token->Get(1),"%d", &type);
								printf("type=%d\r\n",type);
							}
							if(!_stricmp(token->Get(0),"stop")){
								sscanf(token->Get(1),"%d", &stopm);
								printf("stop=%d\r\n",stopm);
							}
							if(!_stricmp(token->Get(0),"fltr")){
								sscanf(token->Get(1),"%ld", &fltr);
								printf("fltr=%x\r\n",fltr);
							}


						}
						delete token;
					}

				}
			}
			delete split;

		
			//xrAntennaType
			if(type!=0){
				SaveCFG(ant-1,fltr,!off,check,loadPoint,stopm);
			}else{
				SaveCFG(ant-1,fltr,!off,check,unloadPoint,stopm);
			}

			
			
		}

	} catch(...){
	}

	delete res;


	sprintf(cmd,"fdev=%s;idx=%d\r\n",Status.xrIP,LineNum);
	devSocket->Send(cmd,strlen(cmd));
	printf("Send To Server: %s\r\n", cmd);

	Sleep(50);


	return true;
};



// Get Configuration from Server #s - command
bool  xrDevice::RcvSys(){

    char  cmd[256];
	devSocket->DropData();

	sprintf(cmd,"sdev=%s\r\n",Status.xrIP);
	devSocket->Send(cmd,strlen(cmd));
	printf("Send To Server: %s\r\n", cmd);

	int l=0;
	int wcnt=1000;
	char *res;
	while(wcnt && (devSocket->GetDataSize()==0)){
		   Sleep(10);
		   wcnt--;
		   printf("*");
	}
	printf("\r\n");
	devSocket->GetPacket(res,&l);
	printf("Receive size=%d string %s\r\n",  l,res);
	if(!strncmp(res,"sget=error\r\n",12)){
	   delete res;
	   return false;
	}
    
	

	try{
	//parse count from strig
	MyArray<char> *split = SplitString(res+1,';');

	if(split){
	
		printf("vars cnt =%d\r\n",split->Count());
		MyArray<char> * token;
		for(int j=0;j<split->Count();j++){
			char * next;
			next = split->Get(j);
			if(next){
				token=SplitString(next,'=');
				if(token){
					if(token->Count()>1){
						printf("Name=%s, Val=%s\r\n",token->Get(0),token->Get(1));
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
	printf("Send To Server: %s\r\n", cmd);

	int l=0;
	int wcnt=1000;
	char *res;
	while(wcnt && (devSocket->GetDataSize()==0)){
		   Sleep(10);
		   wcnt--;
		   printf("*");
	}
	printf("\r\n");
	devSocket->GetPacket(res,&l);
	printf("Receive size=%d string %s\r\n",  l,res);
	if(!strncmp(res,"bget=error\r\n",12)){
	   	delete res;	
	   return false;
	}
    
    try{
	//parse count from strig
	MyArray<char> *split = SplitString(res+1,';');

	if(split){
	
		printf("vars cnt =%d\r\n",split->Count());
		MyArray<char> * token;
		for(int j=0;j<split->Count();j++){
			char * next;
			next = split->Get(j);
			if(next){
				token=SplitString(next,'=');
				if(token){
					if(token->Count()>1){
						printf("Name=%s, Val=%s\r\n",token->Get(0),token->Get(1));
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
	}
		delete res;
	return true;
};



        // Send event  #e - command
bool xrDevice::SendEvents(){
		
	char  cmd[256];
	int evtCnt;
	
    evtCnt = db->GetEventCount();
	printf("SL:Events in base %d\r\n",evtCnt);
	
	int Ant;
	const char *T;
	const char *LBL;
	const char *INFO;
	
	DBEventRecord *eRec;
	
	for(int i=1;i<=evtCnt;i++)
    {
		printf("e");
		eRec =db->GetEvent(i);

		if(eRec && eRec->Sent==0 && !eRec->Deleted ){
			printf("+\r\n");


		char buf[50];
		sprintf(buf,"%04d-%02d-%02dT%02d:%02d:%02d",eRec->T.wYear,eRec->T.wMonth,eRec->T.wDay ,eRec->T.wHour ,eRec->T.wMinute,eRec->T.wSecond)    ;	
		T=buf;
		//T =eRec->T;	
		Ant =eRec->Ant;
		LBL=eRec->Tag;	
		INFO=eRec->Message ;	
		
         
   
		devSocket->DropData();

		sprintf(cmd,"edev=%s;ant=%d;dt=%s;tag=%s;msg=%s\r\n",Status.xrIP, Ant,T,LBL,INFO);
		devSocket->Send(cmd,strlen(cmd));
		printf("Send To Server: %s\r\n", cmd);

		int l=0;
		int wcnt=1000;
		char *res;
		while(wcnt && (devSocket->GetDataSize()==0)){
			   Sleep(10);
			   wcnt--;
			   printf("*");
		}
		printf("\r\n");
		devSocket->GetPacket(res,&l);
        printf("Receive size=%d string %s\r\n",  l,res);
		if(!strncmp(res,"esave=ok\r\n",10)){
		   delete res;
		   eRec->Sent=1;
		   db->UpdateEvent(eRec,i);
		   
		}else{
			delete eRec;
			return false;
		}

		}else{
			delete eRec;
		}
    }

	
	return true;
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
	DeleteFile(TEXT("\\Data\\CFG.DB"));
	DeleteFile(TEXT("\\Data\\RCFG.DB"));
    db->CreateDB();
}


void xrDevice::SaveLabel( int Ant, char * LBL, bool Sent){

	DBTagRecord tr;
	strcpy(tr.Tag,LBL);
	tr.Ant=Ant;
	tr.Sent=Sent;

	db->AddTag(&tr);

};

void xrDevice::UpdateLabel( int Ant, char * LBL, char * NewLBL){
	
}

void xrDevice::SaveBox(  char * Tag){
//BOXES(TAG 

};

void xrDevice::SaveEvent(int Ant, char* LBL, char * Message,bool Sent){
	DBEventRecord er;
	strcpy(er.Tag,LBL);
	er.Ant=Ant;
	er.Sent=Sent;
	strcpy(er.Message,Message);

	db->AddEvent(&er);
	
};

void xrDevice::SaveCFG( int Ant, long FT, int Active, int Check, int AType, int StopMask){
	bool found;
	int cnt;
	int idx;
	DBConfigRecord *crOld;
	cnt =db->GetConfigCount();
	printf("SaveCFG Records=%d\r\n",cnt);

	found=false;
	for(int i=1;i<=cnt;i++){
		crOld = db->GetPointConfig(i);
		if(crOld){
			if (crOld->Antena==Ant){
				found =true;
				idx=i;
				delete crOld;
				printf("Found record for ant=%d idx=%d\r\n",Ant,i);
				break;
			}
			delete crOld;
		}
	}
	


	DBConfigRecord cr;
	cr.Antena =Ant;
	cr.Active =Active;
	cr.Check =Check;
	cr.PointType =AType;
	cr.Filter=FT;
	cr.StopMask=StopMask;

	if(found){
		printf("Updating config for ant=%d\r\n",Ant);
		db->UpdateConfig(&cr,idx);
	}else{
		printf("Add config for ant=%d\r\n",Ant);
		db->SaveConfig(&cr);
	}
};

void xrDevice::FillDebugDB(){
	if (db !=NULL){

		DBReaderConfigRecord *rc = new DBReaderConfigRecord();
		strcpy(rc->ServerIP,"192.168.127.1");
		rc->Port=3200;
		strcpy(rc->xrIP,"192.168.127.254");

		db->SaveReaderConfig(rc);
		delete rc;

		
		SaveCFG(0,1,1,0,1,1);
		SaveCFG(1,2,0,0,1,8);
		SaveCFG(2,3,0,0,1,1);
		SaveCFG(3,4,0,0,1,8);
	}
	
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


MyArray<DelayedTagError> * xrDevice::TagErrors;

long xrDevice::FindErrorIdx(const char * TagID,int Antenna){
	for(long i=0;i<TagErrors->Count();i++){
		if(!strcmp(TagID,TagErrors->Get(i)->Tag)){
			if(TagErrors->Get(i)->Antenna==Antenna){
				return i;
			}
		}
	}
	return -1;

};