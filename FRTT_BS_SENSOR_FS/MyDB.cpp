#include "frtt.h"
#include <winbase.h>

extern char msg[1024];

bool  operator<(class TagPos const &a,class TagPos const &b){
	if(a.rec->Ant<b.rec->Ant)
		return true;
	else
		if(_stricmp(a.rec->Tag,b.rec->Tag)<0)  
			return true;
		else
			return false;
};

bool  operator==(class TagPos const &a,class TagPos const &b){
	if(a.rec->Ant==b.rec->Ant && !_stricmp(a.rec->Tag,b.rec->Tag))
		return true;
	else
		return false;
};

//// 16.01
//bool  operator<(class TagSubstPos const &a,class TagSubstPos const &b){
//		if(_strnicmp(a.rec->Tag,b.rec->Tag,12)<0)  
//			return true;
//		else
//			return false;
//};
//// 16.01
//bool  operator==(class TagSubstPos const &a,class TagSubstPos const &b){
//	if( !_strnicmp(a.rec->Tag,b.rec->Tag,12))
//		return true;
//	else
//		return false;
//};
//
//bool  operator<(class TagRewritePos const &a,class TagRewritePos const &b){
//		if(_strnicmp(a.rec->Tag,b.rec->Tag,12)<0)  
//			return true;
//		else
//			return false;
//};
//
//bool  operator==(class TagRewritePos const &a,class TagRewritePos const &b){
//	if( !_strnicmp(a.rec->Tag,b.rec->Tag,12))
//		return true;
//	else
//		return false;
//};

DWORD MyDB::MinutesLeft(SYSTEMTIME T){
	SYSTEMTIME Now;
	GetSystemTime(&Now);
	DWORD res;
	res = (Now.wYear - T.wYear)* 12 * 31 * 24 * 60 +  (Now.wMonth-T.wMonth) * 31 * 24 * 60 + (Now.wDay-T.wDay) * 24 *60 + (Now.wHour-T.wHour)  *60 + Now.wMinute -T.wMinute ; 
	return res;	
}

MyDB::MyDB(){
	Cash = new MySortedArray<TagPos>();
	/*CashSubst = new MySortedArray<TagSubstPos>();
	CashRewrite = new MySortedArray<TagRewritePos>();*/
	DB_CS = new CRITICAL_SECTION();
	InitializeCriticalSection(DB_CS); 

};

MyDB::~MyDB(){
	delete Cash;
	//delete CashSubst;
	DeleteCriticalSection(DB_CS);
	delete DB_CS;
};

// 16.01
bool MyDB::RebuildSubstDB(){
	 EnterCriticalSection(DB_CS);
	 HANDLE hAppend;
	 printf ( "Recreate SUBST.DB\r\n");
	
	 hAppend = CreateFile (TEXT("\\Data\\SUBST.DB"),      
					GENERIC_WRITE,          // Open for writing
					0,                      // Do not share
					NULL,                   // No security
					CREATE_ALWAYS,            // Open or create
					FILE_ATTRIBUTE_NORMAL,  // Normal file
					NULL);                  // No template file
	
	if (hAppend == INVALID_HANDLE_VALUE)
	{
	printf ( "Could not open SUBST.DB\r\n");
		LeaveCriticalSection(DB_CS);
	return false;
	}
	CloseHandle (hAppend);
	LeaveCriticalSection(DB_CS);

	return true;
}

bool MyDB::RebuildRewriteDB(){
	 EnterCriticalSection(DB_CS);
	 HANDLE hAppend;
	 printf ( "Recreate Rewrite.DB\r\n");
	
	 hAppend = CreateFile (TEXT("\\Data\\Rewrite.DB"),      
					GENERIC_WRITE,          // Open for writing
					0,                      // Do not share
					NULL,                   // No security
					CREATE_ALWAYS,            // Open or create
					FILE_ATTRIBUTE_NORMAL,  // Normal file
					NULL);                  // No template file
	
	if (hAppend == INVALID_HANDLE_VALUE)
	{
	printf ( "Could not open Rewrite.DB\r\n");
		LeaveCriticalSection(DB_CS);
	return false;
	}
	CloseHandle (hAppend);
	LeaveCriticalSection(DB_CS);

	return true;
}

bool MyDB::CreateDB(){
  EnterCriticalSection(DB_CS);
  HANDLE hAppend;
  hAppend = CreateFile (TEXT("\\Data\\TAG.DB"),      
					GENERIC_WRITE,          // Open for writing
					0,                      // Do not share
					NULL,                   // No security
					OPEN_ALWAYS,            // Open or create
					FILE_ATTRIBUTE_NORMAL,  // Normal file
					NULL);                  // No template file

	if (hAppend == INVALID_HANDLE_VALUE)
	{
	printf ( "Could not open TAG.DB\r\n");
	LeaveCriticalSection(DB_CS);
	return false;
	}
	CloseHandle (hAppend);


	hAppend = CreateFile (TEXT("\\Data\\EVENT.DB"),      
					GENERIC_WRITE,          // Open for writing
					0,                      // Do not share
					NULL,                   // No security
					OPEN_ALWAYS,            // Open or create
					FILE_ATTRIBUTE_NORMAL,  // Normal file
					NULL);                  // No template file

	if (hAppend == INVALID_HANDLE_VALUE)
	{
	printf ( "Could not open EVENT.DB\r\n");
		LeaveCriticalSection(DB_CS);
	return false;
	}
	CloseHandle (hAppend);

	hAppend = CreateFile (TEXT("\\Data\\CFG2.DB"),      
					GENERIC_WRITE,          // Open for writing
					0,                      // Do not share
					NULL,                   // No security
					OPEN_ALWAYS,            // Open or create
					FILE_ATTRIBUTE_NORMAL,  // Normal file
					NULL);                  // No template file

	if (hAppend == INVALID_HANDLE_VALUE)
	{
	printf ( "Could not open CFG2.DB\r\n");
		LeaveCriticalSection(DB_CS);
	return false;
	}
	CloseHandle (hAppend);

	hAppend = CreateFile (TEXT("\\Data\\RCFG.DB"),      
					GENERIC_WRITE,          // Open for writing
					0,                      // Do not share
					NULL,                   // No security
					OPEN_ALWAYS,            // Open or create
					FILE_ATTRIBUTE_NORMAL,  // Normal file
					NULL);                  // No template file

	if (hAppend == INVALID_HANDLE_VALUE)
	{
	printf ( "Could not open RCFG.DB\r\n");
		LeaveCriticalSection(DB_CS);
	return false;
	}
	CloseHandle (hAppend);

    // 16.01
	hAppend = CreateFile (TEXT("\\Data\\SUBST.DB"),      
					GENERIC_WRITE,          // Open for writing
					0,                      // Do not share
					NULL,                   // No security
					OPEN_ALWAYS,            // Open or create
					FILE_ATTRIBUTE_NORMAL,  // Normal file
					NULL);                  // No template file

	if (hAppend == INVALID_HANDLE_VALUE)
	{
	printf ( "Could not open SUBST.DB\r\n");
		LeaveCriticalSection(DB_CS);
	return false;
	}
	CloseHandle (hAppend);


	//hAppend = CreateFile (TEXT("\\Data\\SUBST1.DB"),      
	//				GENERIC_WRITE,          // Open for writing
	//				0,                      // Do not share
	//				NULL,                   // No security
	//				OPEN_ALWAYS,            // Open or create
	//				FILE_ATTRIBUTE_NORMAL,  // Normal file
	//				NULL);                  // No template file

	//if (hAppend == INVALID_HANDLE_VALUE)
	//{
	//printf ( "Could not open SUBST1.DB\r\n");
	//	LeaveCriticalSection(DB_CS);
	//return false;
	//}
	//CloseHandle (hAppend);
	LeaveCriticalSection(DB_CS);
 return true;
};

bool MyDB::Open(){
	return true;
};


void MyDB::Close(){

};

/*
bool MyDB::SaveConfig(DBConfigRecord * rec){
	EnterCriticalSection(DB_CS);
	HANDLE hAppend;
	DWORD wBytes;
	hAppend = CreateFile (TEXT("\\Data\\CFG.DB"),      
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        OPEN_ALWAYS,            // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);                  // No template file

	  if (hAppend == INVALID_HANDLE_VALUE)
	  {
		printf ( "Could not open CFG.DB\r\n");
		LeaveCriticalSection(DB_CS);
		return false;
	  }

	  DWORD dwPos;
	  // Append the first file to the end of the second file.
	  dwPos = SetFilePointer (hAppend, 0, NULL, FILE_END);
	  WriteFile (hAppend, rec, sizeof(DBConfigRecord), &wBytes, NULL);
	  CloseHandle (hAppend);
	  
	  LeaveCriticalSection(DB_CS);
	  return true;
};
*/


bool MyDB::SaveConfig2(DBConfigRecord2 * rec){
	EnterCriticalSection(DB_CS);
	HANDLE hAppend;
	DWORD wBytes;
	hAppend = CreateFile (TEXT("\\Data\\CFG2.DB"),      
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        OPEN_ALWAYS,            // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);                  // No template file

	  if (hAppend == INVALID_HANDLE_VALUE)
	  {
		printf ( "Could not open CFG2.DB\r\n");
		LeaveCriticalSection(DB_CS);
		return false;
	  }

	  DWORD dwPos;
	  // Append the first file to the end of the second file.
	  dwPos = SetFilePointer (hAppend, 0, NULL, FILE_END);
	  WriteFile (hAppend, rec, sizeof(DBConfigRecord2), &wBytes, NULL);
	  CloseHandle (hAppend);
	  
	  LeaveCriticalSection(DB_CS);
	  return true;
};

/*
bool MyDB::UpdateConfig(DBConfigRecord * rec, int i){
	EnterCriticalSection(DB_CS);
	HANDLE hAppend;
	DWORD wBytes;
	hAppend = CreateFile (TEXT("\\Data\\CFG.DB"),      
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        OPEN_ALWAYS,            // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);                  // No template file

	  if (hAppend == INVALID_HANDLE_VALUE)
	  {
		printf ( "Could not open CFG.DB\r\n");
		LeaveCriticalSection(DB_CS);
		return false;
	  }

	  DWORD dwPos;
	  // Append the first file to the end of the second file.
	  dwPos = SetFilePointer (hAppend, (i-1) *sizeof(DBConfigRecord) , NULL, FILE_BEGIN);
	  WriteFile (hAppend, rec, sizeof(DBConfigRecord), &wBytes, NULL);
	  CloseHandle (hAppend);
	  
	  LeaveCriticalSection(DB_CS);
	  return true;
};
*/

bool MyDB::UpdateConfig2(DBConfigRecord2 * rec, int i){
	EnterCriticalSection(DB_CS);
	HANDLE hAppend;
	DWORD wBytes;
	hAppend = CreateFile (TEXT("\\Data\\CFG2.DB"),      
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        OPEN_ALWAYS,            // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);                  // No template file

	  if (hAppend == INVALID_HANDLE_VALUE)
	  {
		printf ( "Could not open CFG2.DB\r\n");
		LeaveCriticalSection(DB_CS);
		return false;
	  }

	  DWORD dwPos;
	  // Append the first file to the end of the second file.
	  dwPos = SetFilePointer (hAppend, (i-1) *sizeof(DBConfigRecord2) , NULL, FILE_BEGIN);
	  WriteFile (hAppend, rec, sizeof(DBConfigRecord2), &wBytes, NULL);
	  CloseHandle (hAppend);
	  LeaveCriticalSection(DB_CS);
	  return true;
};

bool MyDB::SaveReaderConfig(DBReaderConfigRecord * rec){
    HANDLE hAppend;
	EnterCriticalSection(DB_CS);
	hAppend = CreateFile (TEXT("\\Data\\RCFG.DB"),      
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        OPEN_ALWAYS,            // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);                  // No template file

	  if (hAppend == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		printf ( "Could not open RCFG.DB\r\n");
		return false;
	  }

	  DWORD dwPos,wBytes;
	  // Append the first file to the end of the second file.
	  dwPos = SetFilePointer (hAppend, 0, NULL, FILE_BEGIN);
	  WriteFile (hAppend, rec, sizeof(DBReaderConfigRecord), &wBytes, NULL);
	  CloseHandle (hAppend);
	  LeaveCriticalSection(DB_CS);
	  return true;
};


bool MyDB::CheckTag(DBTagRecord *rec){
	DWORD pos=0;
	return CheckTag(rec,&pos);
};

// 16.01
bool MyDB::CheckTagSubst(DBTagSubst *rec){
	DWORD pos=0;
	return CheckTagSubst(rec,&pos);
};

bool MyDB::CheckTagRewrite(DBTagRewrite *rec){
	DWORD pos=0;
	return CheckTagRewrite(rec,&pos);
};

bool MyDB::CheckTag(DBTagRecord *rec,DWORD * pos){

	TagPos * tp = new TagPos(0,rec);
	long p;
	p =Cash->Find(tp);
	if(p>=0){
		delete tp;
		tp=Cash->Get(p);
		*pos=tp->Pos;
		//printf("Tag in cash p=%ld Cash Size=%ld \r\n", p,Cash->Count());
		return true;
	}
	delete tp;
	
	if(Cash->Count()>1000){
		Cash->Clear();
	}
	EnterCriticalSection(DB_CS);
	HANDLE hFile;
	hFile = CreateFile (TEXT("\\Data\\TAG.DB"), 
					  GENERIC_READ,           // Open for reading
					  0,                      // Do not share
					  NULL,                   // No security
					  OPEN_EXISTING,          // Existing file only
					  FILE_ATTRIBUTE_NORMAL,  // Normal file
					  NULL);                  // No template file

	if (hFile == INVALID_HANDLE_VALUE)
	{
		*pos=0;
		LeaveCriticalSection(DB_CS);
		return false;
	}
	DBTagRecord * tr = new DBTagRecord ();
    DWORD dwPos;
	DWORD dwBytesRead;  
	dwPos = SetFilePointer (hFile, 0, NULL,FILE_BEGIN);
	*pos=0;
	do{
		(*pos)++;
		ReadFile (hFile, tr, sizeof(DBTagRecord), &dwBytesRead, NULL);

		if(!tr->Deleted && tr->Ant==rec->Ant && ! _strnicmp(tr->Tag,rec->Tag,12)){
			delete tr;
			CloseHandle(hFile);
			Cash->Add(new TagPos(*pos,rec));
				LeaveCriticalSection(DB_CS);
			return true;
		}
	}
	while(dwBytesRead==sizeof(DBTagRecord));

	CloseHandle(hFile);
	*pos=0;
	delete tr;
	LeaveCriticalSection(DB_CS);
	return false;
};

// 16.01
bool MyDB::SubstThisTag(const char * Tag, char * TagReal, int PaperMode){
	DBTagSubst * rec = new DBTagSubst ();
	memcpy(rec->Tag,Tag,25);
	DWORD pos=0;
	if (CheckTagSubst(rec,&pos)){
		delete rec;
		rec = GetTagSubst(pos);
		if(!rec->Passed ){
			memcpy(TagReal,rec->Tag,25);
			if(PaperMode==0){
				rec->Passed=true;
				//GetSystemTime(&rec->T);
				UpdateTagSubst(rec,pos);
			}

			sprintf(msg,"TAG SUBST from %s to %s\r\n",Tag ,TagReal );
			SaveLog(msg);

			delete rec;
			return true;
		}else{
			memcpy(TagReal,Tag,25);
			///////////////////123456789012
			memcpy(TagReal+12,"0000000000AB",12);
			delete rec;
			return true;
		}
	}
	memcpy(TagReal,Tag,25);
	///////////////////123456789012
	memcpy(TagReal+12,"0000000000AB",12);
	return false;
}
// 16.01
bool MyDB::CheckTagSubst(DBTagSubst *rec,DWORD * pos){

	
	/*TagSubstPos * tp = new TagSubstPos(0,rec);
	long p;
	p =CashSubst->Find(tp);
	if(p>=0){
		delete tp;
		tp=CashSubst->Get(p);
		*pos=tp->Pos;
		printf("Tag subst in cash p=%ld Cash Size=%ld \r\n", p,CashSubst->Count());
		return true;
	}
	delete tp;
	
	if(CashSubst->Count()>5000){
		CashSubst->Clear();
	}*/
	EnterCriticalSection(DB_CS);
	HANDLE hFile;
	
	hFile = CreateFile (TEXT("\\Data\\SUBST.DB"), 
					  GENERIC_READ,           // Open for reading
					  0,                      // Do not share
					  NULL,                   // No security
					  OPEN_EXISTING,          // Existing file only
					  FILE_ATTRIBUTE_NORMAL,  // Normal file
					  NULL);                  // No template file
	
	if (hFile == INVALID_HANDLE_VALUE)
	{
		*pos=0;
		LeaveCriticalSection(DB_CS);
		sprintf(msg,"Error: INVALID_HANDLE !!! while open SUBST.DB for tag %s \r\n",rec->Tag );
		SaveLog(msg);
		return false;
	}
	DBTagSubst * tr = new DBTagSubst ();
    DWORD dwPos;
	DWORD dwBytesRead;  
	dwPos = SetFilePointer (hFile, 0, NULL,FILE_BEGIN);
	*pos=0;
	do{
		(*pos)++;
		ReadFile (hFile, tr, sizeof(DBTagSubst), &dwBytesRead, NULL);

		if( ! _strnicmp(tr->Tag,rec->Tag,12)){
			delete tr;
			CloseHandle(hFile);
			//CashSubst->Add(new TagSubstPos(*pos,rec));
			LeaveCriticalSection(DB_CS);
			return true;
		}
	}
	while(dwBytesRead==sizeof(DBTagSubst));

	


	CloseHandle(hFile);
	*pos=0;
	delete tr;
	LeaveCriticalSection(DB_CS);
	if (dwBytesRead!=0){
		sprintf(msg,"Error: while read SUBST.DB for tag %s last pos:%d \r\n",rec->Tag,*pos );
		SaveLog(msg);
	}
	*pos=0;
	return false;
};


bool MyDB::CheckTagRewrite(DBTagRewrite *rec,DWORD * pos){

	/*TagRewritePos * tp = new TagRewritePos(0,rec);
	long p;
	p =CashRewrite->Find(tp);
	if(p>=0){
		delete tp;
		tp=CashRewrite->Get(p);
		*pos=tp->Pos;
		printf("Tag Rewrite in cash p=%ld Cash Size=%ld \r\n", p,CashRewrite->Count());
		return true;
	}
	delete tp;
	
	if(CashRewrite->Count()>5000){
		CashRewrite->Clear();
	}*/
	EnterCriticalSection(DB_CS);
	HANDLE hFile;
	
	hFile = CreateFile (TEXT("\\Data\\Rewrite.DB"), 
					  GENERIC_READ,           // Open for reading
					  0,                      // Do not share
					  NULL,                   // No security
					  OPEN_EXISTING,          // Existing file only
					  FILE_ATTRIBUTE_NORMAL,  // Normal file
					  NULL);                  // No template file
	
	if (hFile == INVALID_HANDLE_VALUE)
	{
	*pos=0;
		LeaveCriticalSection(DB_CS);
	return false;
	}
	DBTagRewrite * tr = new DBTagRewrite ();
    DWORD dwPos;
	DWORD dwBytesRead;  
	dwPos = SetFilePointer (hFile, 0, NULL,FILE_BEGIN);
	do{
		(*pos)++;
		ReadFile (hFile, tr, sizeof(DBTagRewrite), &dwBytesRead, NULL);

		if(  ! _strnicmp(tr->Tag,rec->Tag,12)){
			delete tr;
			CloseHandle(hFile);
			//CashRewrite->Add(new TagRewritePos(*pos,rec));
				LeaveCriticalSection(DB_CS);
			return true;
		}
	}
	while(dwBytesRead==sizeof(DBTagRewrite));

	CloseHandle(hFile);
	*pos=0;
	delete tr;
	LeaveCriticalSection(DB_CS);
	return false;
};

bool MyDB::DeleteSentTag(){
	EnterCriticalSection(DB_CS);
	//printf("Make backup for tag\r\n");
	CopyFile(  TEXT("\\Data\\TAG.DB"), TEXT("\\Data\\TAG.BAK"),  false);

  HANDLE hFile, hAppend;
  DWORD dwBytesRead, dwBytesWritten, dwPos;
  DBTagRecord *tr = new DBTagRecord(); 

  // Open the existing file.

  //printf("Open backup for tag\r\n");
  hFile = CreateFile (TEXT("\\Data\\TAG.BAK"), 
                      GENERIC_READ,           // Open for reading
                      0,                      // Do not share
                      NULL,                   // No security
                      OPEN_EXISTING,          // Existing file only
                      FILE_ATTRIBUTE_NORMAL,  // Normal file
                      NULL);                  // No template file

  if (hFile == INVALID_HANDLE_VALUE)
  {

    LeaveCriticalSection(DB_CS);
    return false;
  }

  // Open the existing file, or, if the file does not exist,
  // create a new file.

  //printf("Create new tag base\r\n");
  hAppend = CreateFile (TEXT("\\Data\\TAG.DB"),      
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        CREATE_ALWAYS,          // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);                  // No template file

  if (hAppend == INVALID_HANDLE_VALUE)
  {
   
    CloseHandle (hFile);            // Close the first file.
	LeaveCriticalSection(DB_CS);
	//printf("File Error\r\n");
    return false;
  }

  // Append the first file to the end of the second file.

  dwPos = SetFilePointer (hAppend, 0, NULL, FILE_END);
  do
  {
	//  printf("Read tag\r\n");
    if (ReadFile (hFile, tr, sizeof(DBTagRecord), &dwBytesRead, NULL))
    {
		if(dwBytesRead==sizeof(DBTagRecord)){
			//printf("Check tag\r\n");


			if(!tr->Deleted ){
					WriteFile (hAppend, tr, sizeof(DBTagRecord),
						 &dwBytesWritten, NULL);
					sprintf(msg,"Keep tag %s in base ",tr->Tag );
					printf(msg);
				

			}else{
				sprintf(msg,"Tag deleted %s",tr->Tag );
				SaveLog(msg);
			}
		}
    }
  }
  while (dwBytesRead == sizeof(DBTagRecord));

  // Close both files.
  //printf("Close files\r\n");
  CloseHandle (hFile);
  CloseHandle (hAppend);
  
  //printf("Drop cash\r\n");
  Cash->Clear();
  
  LeaveCriticalSection(DB_CS);
  return true;
};



bool MyDB::DeletePassedTag(){
	EnterCriticalSection(DB_CS);
	//printf("Make backup for subst\r\n");
	CopyFile(  TEXT("\\Data\\SUBST.DB"), TEXT("\\Data\\SUBST.BAK"),  false);
	


  HANDLE hFile, hAppend;
  DWORD dwBytesRead, dwBytesWritten, dwPos;
  DBTagSubst *tr = new DBTagSubst(); 

  // Open the existing file.

  //printf("Open backup for tag\r\n");
  hFile = CreateFile (TEXT("\\Data\\SUBST.BAK"), 
                      GENERIC_READ,           // Open for reading
                      0,                      // Do not share
                      NULL,                   // No security
                      OPEN_EXISTING,          // Existing file only
                      FILE_ATTRIBUTE_NORMAL,  // Normal file
                      NULL);                  // No template file

  if (hFile == INVALID_HANDLE_VALUE)
  {

    LeaveCriticalSection(DB_CS);
	//printf("File Error\r\n");
    return false;
  }

  

 // printf("Create new tag base\r\n");
  hAppend = CreateFile (TEXT("\\Data\\SUBST.DB"),      
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        CREATE_ALWAYS,          // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);                  // No template file

  if (hAppend == INVALID_HANDLE_VALUE)
  {
   
    CloseHandle (hFile);            // Close the first file.
	LeaveCriticalSection(DB_CS);
	//printf("File Error\r\n");
    return false;
  }

  // Append the first file to the end of the second file.

  dwPos = SetFilePointer (hAppend, 0, NULL, FILE_END);
  do
  {
	  //printf("Read subst\r\n");
    if (ReadFile (hFile, tr, sizeof(DBTagSubst), &dwBytesRead, NULL))
    {
		if(dwBytesRead==sizeof(DBTagSubst)){
			//printf("Check tag\r\n");
			if(!tr->Passed){ 
				//printf("Write subst\r\n");
				WriteFile (hAppend, tr, sizeof(DBTagSubst),
					 &dwBytesWritten, NULL);
			}else{
				sprintf(msg,"Erase passed SUBST %s\r\n",tr->Tag );
				SaveLog(msg);
			}
		} 
    }
  }
  while (dwBytesRead == sizeof(DBTagSubst));

  // Close both files.
 // printf("Close files\r\n");
  CloseHandle (hFile);
  CloseHandle (hAppend);
  
 // printf("Drop subst cash\r\n");
  //CashSubst->Clear();
  
  LeaveCriticalSection(DB_CS);
  return true;
};





bool MyDB::DeleteSentEvent(){
  EnterCriticalSection(DB_CS);
  //printf("Make backup for event\r\n");
  CopyFile(  TEXT("\\Data\\EVENT.DB"), TEXT("\\Data\\EVENT.BAK"),  false);
	


  HANDLE hFile, hAppend;
  DWORD dwBytesRead, dwBytesWritten, dwPos;
  DBEventRecord *tr = new DBEventRecord(); 

  // Open the existing file.

  hFile = CreateFile (TEXT("\\Data\\EVENT.BAK"), 
                      GENERIC_READ,           // Open for reading
                      0,                      // Do not share
                      NULL,                   // No security
                      OPEN_EXISTING,          // Existing file only
                      FILE_ATTRIBUTE_NORMAL,  // Normal file
                      NULL);                  // No template file

  //printf("Open backup for event\r\n");
  if (hFile == INVALID_HANDLE_VALUE)
  {
    
	//printf("File Error\r\n");
    LeaveCriticalSection(DB_CS);
    return false;
  }

  // Open the existing file, or, if the file does not exist,
  // create a new file.

  //printf("Create main file for event\r\n");
  hAppend = CreateFile (TEXT("\\Data\\EVENT.DB"),      
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        CREATE_ALWAYS,          // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);                  // No template file

  if (hAppend == INVALID_HANDLE_VALUE)
  {
  
	//printf("File Error\r\n");  
    CloseHandle (hFile);            // Close the first file.
	LeaveCriticalSection(DB_CS);
    return false;
  }

  // Append the first file to the end of the second file.

  dwPos = SetFilePointer (hAppend, 0, NULL, FILE_BEGIN);
  do
  {
	//printf("Read record\r\n");
    if (ReadFile (hFile, tr, sizeof(DBEventRecord), &dwBytesRead, NULL))
    {
		if(dwBytesRead==sizeof(DBEventRecord)){
			//printf("Check record\r\n");
			if(!tr->Sent ){
				//printf("Write record\r\n");
				WriteFile (hAppend, tr, sizeof(DBEventRecord),
					 &dwBytesWritten, NULL);
			}
		}
    }
  }
  while (dwBytesRead == sizeof(DBEventRecord));

  // Close both files.

  //printf("Close files\r\n");
  CloseHandle (hFile);
  CloseHandle (hAppend);
    
  LeaveCriticalSection(DB_CS);
  return true;
};


DBReaderConfigRecord * MyDB::GetReaderConfig(){
	EnterCriticalSection(DB_CS);
	DBReaderConfigRecord * rc = new DBReaderConfigRecord();
	HANDLE hFile;
    hFile = CreateFile (TEXT("\\Data\\RCFG.DB"), 
						  GENERIC_READ,           // Open for reading
						  0,                      // Do not share
						  NULL,                   // No security
						  OPEN_EXISTING,          // Existing file only
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);                  // No template file

	  if (hFile == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		return 0;
	  }
	memset(rc,0,sizeof(DBReaderConfigRecord));
    DWORD dwPos;
	DWORD dwBytesRead;  
	dwPos = SetFilePointer (hFile, 0, NULL,FILE_BEGIN);
	ReadFile (hFile, rc, sizeof(DBReaderConfigRecord), &dwBytesRead, NULL);
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	return rc;
};

/*
DBConfigRecord * MyDB::GetPointConfig(int i){
	
	EnterCriticalSection(DB_CS);
	DBConfigRecord* rc = new DBConfigRecord();
	HANDLE hFile;
    hFile = CreateFile (TEXT("\\Data\\CFG.DB"), 
						  GENERIC_READ,           // Open for reading
						  0,                      // Do not share
						  NULL,                   // No security
						  OPEN_EXISTING,          // Existing file only
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);                  // No template file

	  if (hFile == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		delete rc;
		return 0;
	  }
    DWORD dwPos;
	DWORD dwBytesRead;  
	dwPos = SetFilePointer (hFile, (i-1) * sizeof(DBConfigRecord), NULL,FILE_BEGIN);
	ReadFile (hFile, rc, sizeof(DBConfigRecord), &dwBytesRead, NULL);
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	return rc;
};
*/

DBConfigRecord2 * MyDB::GetPointConfig2(int i){
	
	EnterCriticalSection(DB_CS);
	DBConfigRecord2* rc = new DBConfigRecord2();
	HANDLE hFile;
    hFile = CreateFile (TEXT("\\Data\\CFG2.DB"), 
						  GENERIC_READ,           // Open for reading
						  0,                      // Do not share
						  NULL,                   // No security
						  OPEN_EXISTING,          // Existing file only
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);                  // No template file

	  if (hFile == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		delete rc;
		return 0;
	  }
    DWORD dwPos;
	DWORD dwBytesRead;  
	dwPos = SetFilePointer (hFile, (i-1) * sizeof(DBConfigRecord2), NULL,FILE_BEGIN);
	ReadFile (hFile, rc, sizeof(DBConfigRecord2), &dwBytesRead, NULL);
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	return rc;
};

bool MyDB::AddTag( DBTagRecord * rec){


	TagPos * tp = new TagPos(0,rec);
	long p;
	p =Cash->Find(tp);
	if(p>=0){
		delete tp;
		//printf("Tag in-cash %ld  cash size =%ld\r\n",p,Cash->Count()); 
		return true;
	}
	delete tp;

	
	HANDLE hAppend;
	DWORD wBytes;
	       // No template file


	//if (!CheckTagDBSize()){
	//	EnterCriticalSection(DB_CS);
	//	hAppend = CreateFile (TEXT("\\Data\\TAG.DB"),      
 //                       GENERIC_WRITE,          // Open for writing
 //                       0,                      // Do not share
 //                       NULL,                   // No security
 //                       CREATE_ALWAYS,            // Open or create
 //                       FILE_ATTRIBUTE_NORMAL,  // Normal file
 //                       NULL);           
	//}else{
		EnterCriticalSection(DB_CS);
		hAppend = CreateFile (TEXT("\\Data\\TAG.DB"),      
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        OPEN_ALWAYS,            // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);           
	//}

	
	  if (hAppend == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		printf ( "Could not open TAG.DB\r\n");
		return false;
	  }

	  DWORD dwPos;
	  // Append the first file to the end of the second file.
	  GetSystemTime(&(rec->T));
	  GetSystemTime(&(rec->T2));

		DWORD FileSz;
		FileSz=GetFileSize( hAppend,NULL);
		if (FileSz==0xFFFFFFFF){
			CloseHandle(hAppend);
			LeaveCriticalSection(DB_CS);	
			return false;
		}
		long i=0;
		if (FileSz<sizeof(DBTagRecord)){
			i=0;
		}else{
			i=((long)FileSz) / sizeof(DBTagRecord);
		}

		if (i * sizeof(DBTagRecord)<=FileSz){
			dwPos = SetFilePointer (hAppend, (i) * sizeof(DBTagRecord), NULL,FILE_BEGIN);
			WriteFile (hAppend, rec, sizeof(DBTagRecord), &wBytes, NULL);
		}
	
	 /* dwPos = SetFilePointer (hAppend, 0, NULL, FILE_END);
	  WriteFile (hAppend, rec, sizeof(DBTagRecord), &wBytes, NULL);*/
	  CloseHandle (hAppend);

	  //printf("Tag saved to DB %s\r\n",rec->Tag); 

	  Cash->Add(new TagPos(dwPos/sizeof(DBTagRecord),rec));
	  LeaveCriticalSection(DB_CS);
	  return true;
};


bool MyDB::AddTagSubst( DBTagSubst * rec){


	//TagSubstPos * tp = new TagSubstPos(0,rec);
	//long p;
	//p =CashSubst->Find(tp);
	//if(p>=0){
	//	delete tp;
	//	printf("Tag in-cash %ld  cash size =%ld\r\n",p,CashSubst->Count()); 
	//	return true;
	//}
	//delete tp;

	HANDLE hAppend;
	DWORD wBytes=sizeof(DBTagSubst);

	if(CheckSubstDBSize()) {
		EnterCriticalSection(DB_CS);
		hAppend = CreateFile (TEXT("\\Data\\SUBST.DB"),      
							GENERIC_WRITE,          // Open for writing
							0,                      // Do not share
							NULL,                   // No security
							OPEN_ALWAYS,            // Open or create
							FILE_ATTRIBUTE_NORMAL,  // Normal file
							NULL);  
	}else{
		EnterCriticalSection(DB_CS);
		hAppend = CreateFile (TEXT("\\Data\\SUBST.DB"),      
							GENERIC_WRITE,          // Open for writing
							0,                      // Do not share
							NULL,                   // No security
							CREATE_ALWAYS,            // Open or create
							FILE_ATTRIBUTE_NORMAL,  // Normal file
							NULL);  
	}
	
	  if (hAppend == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		printf ( "Could not open SUBST.DB\r\n");
		return false;
	  }

	  DWORD dwPos;
	  // Append the first file to the end of the second file.
	  //GetSystemTime(&(rec->T));
	DWORD FileSz;

	 FileSz=GetFileSize( hAppend,NULL);
	
	 int reccount= (int)((long)FileSz / sizeof(DBTagSubst));
     /*int rem =(int)((long)FileSz % sizeof(DBTagSubst));
	 if (rem!=0){
		 sprintf(msg,"subst file size wrong extra %d bytes ignored",rem);
		 SaveLog(msg);
	 }*/
	  dwPos = SetFilePointer (hAppend, reccount * sizeof(DBTagSubst), NULL, FILE_BEGIN);
	  WriteFile (hAppend, rec, sizeof(DBTagSubst), &wBytes, NULL);
	  CloseHandle (hAppend);

	  //printf("Tag saved to DB %s\r\n",rec->Tag); 

	  //CashSubst->Add(new TagSubstPos(dwPos/sizeof(DBTagSubst),rec));
	  LeaveCriticalSection(DB_CS);
	  return true;
};

bool MyDB::AddTagRewrite( DBTagRewrite * rec){


	/*TagRewritePos * tp = new TagRewritePos(0,rec);
	long p;
	p =CashRewrite->Find(tp);
	if(p>=0){
		delete tp;
		printf("Tag in-cash %ld  cash size =%ld\r\n",p,CashRewrite->Count()); 
		return true;
	}
	delete tp;*/

	EnterCriticalSection(DB_CS);
	HANDLE hAppend;
	DWORD wBytes;
	
	hAppend = CreateFile (TEXT("\\Data\\Rewrite.DB"),      
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        OPEN_ALWAYS,            // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);                  // No template file
	
	  if (hAppend == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		printf ( "Could not open Rewrite.DB\r\n");
		return false;
	  }

	  DWORD dwPos;
	
	  dwPos = SetFilePointer (hAppend, 0, NULL, FILE_END);
	  WriteFile (hAppend, rec, sizeof(DBTagRewrite), &wBytes, NULL);
	  CloseHandle (hAppend);

	  //printf("Tag saved to DB %s\r\n",rec->Tag); 

	  //CashRewrite->Add(new TagRewritePos(dwPos/sizeof(DBTagRewrite),rec));
	  LeaveCriticalSection(DB_CS);
	  return true;
};


bool MyDB::AddEvent(DBEventRecord * rec){

	HANDLE hAppend;
	DWORD wBytes;
	if(CheckEventDBSize()){
		EnterCriticalSection(DB_CS);
		hAppend = CreateFile (TEXT("\\Data\\EVENT.DB"),      
							GENERIC_WRITE,          // Open for writing
							0,                      // Do not share
							NULL,                   // No security
							OPEN_ALWAYS,            // Open or create
							FILE_ATTRIBUTE_NORMAL,  // Normal file
							NULL);                  // No template file
	}else{
		EnterCriticalSection(DB_CS);
		hAppend = CreateFile (TEXT("\\Data\\EVENT.DB"),      
							GENERIC_WRITE,          // Open for writing
							0,                      // Do not share
							NULL,                   // No security
							CREATE_ALWAYS,            // Open or create
							FILE_ATTRIBUTE_NORMAL,  // Normal file
							NULL); 
	}

	  if (hAppend == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		printf ( "Could not open Event.DB\r\n");
		return false;
	  }

	  DWORD dwPos;
	  // Append the first file to the end of the second file.
	  GetSystemTime(&(rec->T));

		DWORD FileSz;
		FileSz=GetFileSize( hAppend,NULL);
		if (FileSz==0xFFFFFFFF){
			CloseHandle(hAppend);
			LeaveCriticalSection(DB_CS);	
			return false;
		}
		long i=0;
		if (FileSz<sizeof(DBEventRecord)){
			i=0;
		}else{
			i=((long)FileSz) / sizeof(DBEventRecord);
		}

		if (i * sizeof(DBEventRecord)<=FileSz){
			dwPos = SetFilePointer (hAppend, (i) * sizeof(DBEventRecord), NULL,FILE_BEGIN);
			WriteFile (hAppend, rec, sizeof(DBEventRecord), &wBytes, NULL);
		}


	/*  dwPos = SetFilePointer (hAppend, 0, NULL, FILE_END);
	  WriteFile (hAppend, rec, sizeof(DBEventRecord), &wBytes, NULL);*/
	  CloseHandle (hAppend);
	  LeaveCriticalSection(DB_CS);
	  return true;
};


int MyDB::GetEventCount(){
	EnterCriticalSection(DB_CS);
	DWORD FileSz;
	HANDLE hFile;
    hFile = CreateFile (TEXT("\\Data\\EVENT.DB"), 
						  GENERIC_READ,           // Open for reading
						  0,                      // Do not share
						  NULL,                   // No security
						  OPEN_EXISTING,          // Existing file only
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);                  // No template file

	  if (hFile == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		return 0;
	  }
	FileSz=GetFileSize( hFile,NULL);
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	if (FileSz==0xFFFFFFFF){
		return 0;
	}
	//printf("Events in base %ld\r\n",FileSz /sizeof(DBEventRecord));

	if (FileSz<sizeof(DBEventRecord)){
		//printf("No Events in base \r\n");
		return 0;
	}else{
		//printf("Events in base %ld\r\n",FileSz /sizeof(DBEventRecord));
		return ((long)FileSz) / sizeof(DBEventRecord);
	}
	
};




int MyDB::GetBoxDelay(){
	EnterCriticalSection(DB_CS);
	DWORD FileSz;
	HANDLE hFile;
    hFile = CreateFile (TEXT("\\Data\\DELAY.TXT"), 
						  GENERIC_READ,           // Open for reading
						  0,                      // Do not share
						  NULL,                   // No security
						  OPEN_EXISTING,          // Existing file only
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);                  // No template file

	  if (hFile == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		  return 200;
	  }
	  char buf[81];
	  FileSz=GetFileSize( hFile,NULL);

		DWORD dwSz;
		if(FileSz<80){
			dwSz=FileSz;
		}else{
			dwSz=80;
		}
		StrClear(buf,80,32);

		DWORD dwBytesRead;  
		SetFilePointer (hFile, 0, NULL,FILE_BEGIN);
		ReadFile (hFile, buf, dwSz, &dwBytesRead, NULL);
		CloseHandle(hFile);
		LeaveCriticalSection(DB_CS);
		int bDelay=200;
		sscanf(buf,"%d",&bDelay);
		return bDelay;
};

/*
int MyDB::GetConfigCount(){
	EnterCriticalSection(DB_CS);
	DWORD FileSz;
	HANDLE hFile;
    hFile = CreateFile (TEXT("\\Data\\CFG.DB"), 
						  GENERIC_READ,           // Open for reading
						  0,                      // Do not share
						  NULL,                   // No security
						  OPEN_EXISTING,          // Existing file only
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);                  // No template file

	  if (hFile == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		  return 0;
	  }
	FileSz=GetFileSize( hFile,NULL);
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	if (FileSz==0xFFFFFFFF){
		return 0;
	}
	//printf("Cfg in base %ld FileSize %ld\r\n",FileSz /sizeof(DBConfigRecord),FileSz);
	return (FileSz /sizeof(DBConfigRecord));
};
*/

int MyDB::GetConfigCount2(){
	EnterCriticalSection(DB_CS);
	DWORD FileSz;
	HANDLE hFile;
    hFile = CreateFile (TEXT("\\Data\\CFG2.DB"), 
						  GENERIC_READ,           // Open for reading
						  0,                      // Do not share
						  NULL,                   // No security
						  OPEN_EXISTING,          // Existing file only
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);                  // No template file

	  if (hFile == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		  return 0;
	  }
	FileSz=GetFileSize( hFile,NULL);
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	if (FileSz==0xFFFFFFFF){
		return 0;
	}
	//printf("Cfg in base %ld FileSize %ld\r\n",FileSz /sizeof(DBConfigRecord),FileSz);
	return (FileSz /sizeof(DBConfigRecord2));
};

int MyDB::GetTagCount(){
	EnterCriticalSection(DB_CS);
	DWORD FileSz;
	HANDLE hFile;
    hFile = CreateFile (TEXT("\\Data\\TAG.DB"), 
						  GENERIC_READ,           // Open for reading
						  0,                      // Do not share
						  NULL,                   // No security
						  OPEN_EXISTING,          // Existing file only
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);                  // No template file

	  if (hFile == INVALID_HANDLE_VALUE)
	  {
			LeaveCriticalSection(DB_CS);
		return 0;
	  }
	FileSz=GetFileSize( hFile,NULL);
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	if (FileSz==0xFFFFFFFF){
		return 0;
	}
		if (FileSz<sizeof(DBTagRecord)){
			//printf("No Tags in base \r\n");
			return 0;
		}else{
			//printf("Tags in base %ld\r\n",FileSz /sizeof(DBTagRecord));
			return ((long)FileSz) / sizeof(DBTagRecord);
		}
};

bool MyDB::CheckSubstDBSize(){
	

	return true;  // ignore checking file size

	EnterCriticalSection(DB_CS);
	DWORD FileSz;
	HANDLE hFile;
	
	hFile = CreateFile (TEXT("\\Data\\SUBST.DB"), 
						  GENERIC_READ,           // Open for reading
						  0,                      // Do not share
						  NULL,                   // No security
						  OPEN_EXISTING,          // Existing file only
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);                  // No template file
	
	  if (hFile == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		return false;
	  }
	FileSz=GetFileSize( hFile,NULL);
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	if (FileSz==0xFFFFFFFF){
		return false;
	}

	if (FileSz == ((long)((long) FileSz / sizeof(DBTagSubst))) * sizeof(DBTagSubst)) 
		return true;
	else
		return false;
};

bool MyDB::CheckTagDBSize(){
	EnterCriticalSection(DB_CS);
	DWORD FileSz;
	HANDLE hFile;
	
	hFile = CreateFile (TEXT("\\Data\\Tag.DB"), 
						  GENERIC_READ,           // Open for reading
						  0,                      // Do not share
						  NULL,                   // No security
						  OPEN_EXISTING,          // Existing file only
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);                  // No template file
	
	  if (hFile == INVALID_HANDLE_VALUE)
	  {
			LeaveCriticalSection(DB_CS);
		return 0;
	  }
	FileSz=GetFileSize( hFile,NULL);
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	if (FileSz==0xFFFFFFFF){
		return false;
	}

	if (FileSz == ((long)( (long)FileSz / sizeof(DBTagRecord))) * sizeof(DBTagRecord)) 
		return true;
	else
		return false;
};


bool MyDB::CheckEventDBSize(){
	EnterCriticalSection(DB_CS);
	DWORD FileSz;
	HANDLE hFile;
	
	hFile = CreateFile (TEXT("\\Data\\Event.DB"), 
						  GENERIC_READ,           // Open for reading
						  0,                      // Do not share
						  NULL,                   // No security
						  OPEN_EXISTING,          // Existing file only
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);                  // No template file
	
	  if (hFile == INVALID_HANDLE_VALUE)
	  {
			LeaveCriticalSection(DB_CS);
		return false;
	  }
	FileSz=GetFileSize( hFile,NULL);
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	if (FileSz==0xFFFFFFFF){
		return false;
	}

	if (FileSz == ((long)( (long)FileSz / sizeof(DBEventRecord))) * sizeof(DBEventRecord)) 
		return true;
	else
		return false;
};

int MyDB::GetTagSubstCount(){
	EnterCriticalSection(DB_CS);
	DWORD FileSz;
	HANDLE hFile;
	
	hFile = CreateFile (TEXT("\\Data\\SUBST.DB"), 
						  GENERIC_READ,           // Open for reading
						  0,                      // Do not share
						  NULL,                   // No security
						  OPEN_EXISTING,          // Existing file only
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);                  // No template file
	
	  if (hFile == INVALID_HANDLE_VALUE)
	  {
			LeaveCriticalSection(DB_CS);
		return 0;
	  }
	FileSz=GetFileSize( hFile,NULL);
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	if (FileSz==0xFFFFFFFF){
		return 0;
	}
	
	//printf("Tag subst in base %ld\r\n",FileSz /sizeof(DBTagSubst));
	return (int)((long)FileSz /sizeof(DBTagSubst));
};

int MyDB::GetTagRewriteCount(){
	EnterCriticalSection(DB_CS);
	DWORD FileSz;
	HANDLE hFile;
	
	hFile = CreateFile (TEXT("\\Data\\Rewrite.DB"), 
						  GENERIC_READ,           // Open for reading
						  0,                      // Do not share
						  NULL,                   // No security
						  OPEN_EXISTING,          // Existing file only
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);                  // No template file
	
	  if (hFile == INVALID_HANDLE_VALUE)
	  {
			LeaveCriticalSection(DB_CS);
		return 0;
	  }
	FileSz=GetFileSize( hFile,NULL);
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	if (FileSz==0xFFFFFFFF){
		return 0;
	}
	//printf("Tag Rewrite in base %ld\r\n",FileSz /sizeof(DBTagRewrite));
	return FileSz /sizeof(DBTagRewrite);
};

DBTagRecord * MyDB::GetTag(int i){
	EnterCriticalSection(DB_CS);
	DBTagRecord * tr = new DBTagRecord ();
	HANDLE hFile;
    hFile = CreateFile (TEXT("\\Data\\TAG.DB"), 
						  GENERIC_READ,           // Open for reading
						  0,                      // Do not share
						  NULL,                   // No security
						  OPEN_EXISTING,          // Existing file only
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);                  // No template file

	  if (hFile == INVALID_HANDLE_VALUE)
	  {
			LeaveCriticalSection(DB_CS);
			delete tr;
		return 0;
	  }
    DWORD dwPos;
	DWORD dwBytesRead; 
	DWORD FileSz;
	FileSz=GetFileSize( hFile,NULL);
	if (FileSz==0xFFFFFFFF){
		CloseHandle(hFile);
		LeaveCriticalSection(DB_CS);	
		return tr;
	}
	if ((i-1) * sizeof(DBTagRecord)<FileSz){
		dwPos = SetFilePointer (hFile, (i-1) * sizeof(DBTagRecord), NULL,FILE_BEGIN);
		ReadFile (hFile, tr, sizeof(DBTagRecord), &dwBytesRead, NULL);
		tr->Tag[24]=0; 
		tr->TagSubst[24]=0; 
	}
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	
	//printf("GetTag(%d)=%s S=%d D=%d\r\n",i,tr->Tag,tr->Sent ,tr->Deleted );
	
	return tr;
};

DBTagSubst * MyDB::GetTagSubst(int i){
	EnterCriticalSection(DB_CS);
	DBTagSubst * tr = new DBTagSubst ();
	HANDLE hFile;
	
    hFile = CreateFile (TEXT("\\Data\\SUBST.DB"), 
						  GENERIC_READ,           // Open for reading
						  0,                      // Do not share
						  NULL,                   // No security
						  OPEN_EXISTING,          // Existing file only
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);                  // No template file
	
	  if (hFile == INVALID_HANDLE_VALUE)
	  {
			
			LeaveCriticalSection(DB_CS);
			delete tr;
		return 0;
	  }
    DWORD dwPos;
	DWORD dwBytesRead;  
	DWORD FileSz;
	
	FileSz=GetFileSize( hFile,NULL);
	if (FileSz==0xFFFFFFFF){
		CloseHandle(hFile);
		LeaveCriticalSection(DB_CS);
		return tr;
	}
	if ((i-1) * sizeof(DBTagSubst)<FileSz){
		dwPos = SetFilePointer (hFile, (i-1) * sizeof(DBTagSubst), NULL,FILE_BEGIN);
		ReadFile (hFile, tr, sizeof(DBTagSubst), &dwBytesRead, NULL);
	}

	
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	//printf("GetTagSubst(%d)=%s P=%d \r\n",i,tr->Tag,tr->Passed  );
	return tr;
};



DBTagRewrite * MyDB::GetTagRewrite(int i){
	EnterCriticalSection(DB_CS);
	DBTagRewrite * tr = new DBTagRewrite ();
	HANDLE hFile;
	
    hFile = CreateFile (TEXT("\\Data\\Rewrite.DB"), 
						  GENERIC_READ,           // Open for reading
						  0,                      // Do not share
						  NULL,                   // No security
						  OPEN_EXISTING,          // Existing file only
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);                  // No template file
	
	  if (hFile == INVALID_HANDLE_VALUE)
	  {
			LeaveCriticalSection(DB_CS);
			delete tr;
		return 0;
	  }
    DWORD dwPos;
	DWORD dwBytesRead;  
	dwPos = SetFilePointer (hFile, (i-1) * sizeof(DBTagRewrite), NULL,FILE_BEGIN);
	ReadFile (hFile, tr, sizeof(DBTagRewrite), &dwBytesRead, NULL);
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	//printf("GetTagRewrite(%d)=%s\r\n",i,tr->Tag );
	return tr;
};

DBEventRecord * MyDB::GetEvent(int i){
	EnterCriticalSection(DB_CS);
	DBEventRecord * er = new DBEventRecord ();
	HANDLE hFile;
    hFile = CreateFile (TEXT("\\Data\\EVENT.DB"), 
						  GENERIC_READ,           // Open for reading
						  0,                      // Do not share
						  NULL,                   // No security
						  OPEN_EXISTING,          // Existing file only
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);                  // No template file

	  if (hFile == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		delete er;
		return 0;
	  }
    DWORD dwPos;
	DWORD dwBytesRead;  
	DWORD FileSz;
	FileSz=GetFileSize( hFile,NULL);
	if (FileSz==0xFFFFFFFF){
		CloseHandle(hFile);
		LeaveCriticalSection(DB_CS);
		return er;
	}
	if ((i-1) * sizeof(DBEventRecord)<FileSz){
		dwPos = SetFilePointer (hFile, (i-1) * sizeof(DBEventRecord), NULL,FILE_BEGIN);
		ReadFile (hFile, er, sizeof(DBEventRecord), &dwBytesRead, NULL);
	}

	
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	//printf("GetEvent(%d)=%s S=%d D=%d\r\n",i,er->Message, er->Sent,er->Deleted);
	return er;
};

bool MyDB::UpdateTag( DBTagRecord * rec, DWORD i){
	EnterCriticalSection(DB_CS);
	HANDLE hAppend;
	DWORD wBytes;
	hAppend = CreateFile (TEXT("\\Data\\TAG.DB"),      
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        OPEN_ALWAYS,            // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);                  // No template file

	  if (hAppend == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		sprintf (msg, "Could not open TAG.DB\r\n");
		SaveLog(msg);
		return false;
	  }

	  DWORD dwPos;
	  // Append the first file to the end of the second file.
	  //GetSystemTime(&(rec->T));
	  	DWORD FileSz;
		FileSz=GetFileSize( hAppend,NULL);
		if (FileSz==0xFFFFFFFF){
			CloseHandle(hAppend);
			LeaveCriticalSection(DB_CS);	
			sprintf (msg, "File size = maxint TAG.DB\r\n");
			SaveLog(msg);
			return false;
		}
		if ((DWORD)((DWORD)(i-1) * (DWORD)sizeof(DBTagRecord)) < FileSz ){
			dwPos = SetFilePointer (hAppend, (DWORD)((DWORD)(i-1) * (DWORD)(sizeof(DBTagRecord))), NULL,FILE_BEGIN);
			WriteFile (hAppend, rec, sizeof(DBTagRecord), &wBytes, NULL);
		}else{ 
			sprintf (msg, "File size error   TAG.DB exp=%lu  size=%lu\r\n",(DWORD)((DWORD)(i-1) * (DWORD)(sizeof(DBTagRecord))),FileSz);
			SaveLog(msg);
			CloseHandle (hAppend);
			LeaveCriticalSection(DB_CS);
			return false;
		}
	  CloseHandle (hAppend);
	  LeaveCriticalSection(DB_CS);
	  return true;
};

bool MyDB::UpdateTagSubst( DBTagSubst * rec, int i){
	EnterCriticalSection(DB_CS);
	HANDLE hAppend;
	DWORD wBytes;
	
	hAppend = CreateFile (TEXT("\\Data\\SUBST.DB"),      
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        OPEN_ALWAYS,            // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);                  // No template file
	
	  if (hAppend == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		printf ( "Could not open SUBST.DB\r\n");
		return false;
	  }

	  DWORD dwPos;
	  // Append the first file to the end of the second file.
	  //GetSystemTime(&(rec->T));
	  dwPos = SetFilePointer (hAppend, (i-1)*sizeof(DBTagSubst), NULL, FILE_BEGIN);
	  WriteFile (hAppend, rec, sizeof(DBTagSubst), &wBytes, NULL);
	  CloseHandle (hAppend);
	  LeaveCriticalSection(DB_CS);
	  return true;
};

bool MyDB::UpdateEvent( DBEventRecord * rec, int i){
	EnterCriticalSection(DB_CS);
	HANDLE hAppend;
	DWORD wBytes;
	hAppend = CreateFile (TEXT("\\Data\\EVENT.DB"),      
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        OPEN_ALWAYS,            // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);                  // No template file

	  if (hAppend == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		printf ( "Could not open EVENT.DB\r\n");
		return false;
	  }

	  DWORD dwPos;
 	DWORD FileSz;
	FileSz=GetFileSize( hAppend,NULL);
	if (FileSz==0xFFFFFFFF){
		CloseHandle(hAppend);
		LeaveCriticalSection(DB_CS);	
		return false;
	}
	if ((i-1) * sizeof(DBEventRecord)<FileSz){
		dwPos = SetFilePointer (hAppend, (i-1) * sizeof(DBEventRecord), NULL,FILE_BEGIN);
		 WriteFile (hAppend, rec, sizeof(DBEventRecord), &wBytes, NULL);
	}

	/*  dwPos = SetFilePointer (hAppend, (i-1) * sizeof(DBEventRecord), NULL, FILE_BEGIN);*/
	  //WriteFile (hAppend, rec, sizeof(DBEventRecord), &wBytes, NULL);
	  CloseHandle (hAppend);
	  LeaveCriticalSection(DB_CS);
	  return true;
};



bool MyDB::VerifyDB(){
  EnterCriticalSection(DB_CS);
  HANDLE hFile;

  hFile = CreateFile (TEXT("\\Data\\TAG.DB"), GENERIC_READ, FILE_SHARE_READ,  NULL,// No security
                      OPEN_EXISTING,          // Existing file only
                      FILE_ATTRIBUTE_NORMAL,  // Normal file
                      NULL);                  // No template file

  if (hFile == INVALID_HANDLE_VALUE)
  {
   	 LeaveCriticalSection(DB_CS);
     return false;
  }
  //DWORD FileSz;

  CloseHandle(hFile);

	hFile = CreateFile (TEXT("\\Data\\EVENT.DB"), GENERIC_READ, FILE_SHARE_READ,  NULL,// No security
					  OPEN_EXISTING,          // Existing file only
					  FILE_ATTRIBUTE_NORMAL,  // Normal file
					  NULL);                  // No template file

	if (hFile == INVALID_HANDLE_VALUE)
	{
			LeaveCriticalSection(DB_CS);
	 return false;
	}
	CloseHandle(hFile);
  hFile = CreateFile (TEXT("\\Data\\CFG2.DB"), GENERIC_READ, FILE_SHARE_READ,  NULL,// No security
                      OPEN_EXISTING,          // Existing file only
                      FILE_ATTRIBUTE_NORMAL,  // Normal file
                      NULL);                  // No template file

  if (hFile == INVALID_HANDLE_VALUE)
  {
	  	LeaveCriticalSection(DB_CS);
     return false;
  }
  CloseHandle(hFile);
  hFile = CreateFile (TEXT("\\Data\\RCFG.DB"), GENERIC_READ, FILE_SHARE_READ,  NULL,// No security
                      OPEN_EXISTING,          // Existing file only
                      FILE_ATTRIBUTE_NORMAL,  // Normal file
                      NULL);                  // No template file

  if (hFile == INVALID_HANDLE_VALUE)
  {
	  	LeaveCriticalSection(DB_CS);
     return false;
  }
  CloseHandle(hFile);


    hFile = CreateFile (TEXT("\\Data\\SUBST.DB"), GENERIC_READ, FILE_SHARE_READ,  NULL,// No security
                      OPEN_EXISTING,          // Existing file only
                      FILE_ATTRIBUTE_NORMAL,  // Normal file
                      NULL);                  // No template file

  if (hFile == INVALID_HANDLE_VALUE)
  {
	  hFile = CreateFile (TEXT("\\Data\\SUBST.DB"),      
					GENERIC_WRITE,          // Open for writing
					0,                      // Do not share
					NULL,                   // No security
					OPEN_ALWAYS,            // Open or create
					FILE_ATTRIBUTE_NORMAL,  // Normal file
					NULL);                  // No template file

	  if (hFile == INVALID_HANDLE_VALUE)
	  {

	  	LeaveCriticalSection(DB_CS);
		return false;
	  }
  }
  CloseHandle(hFile);


//hFile = CreateFile (TEXT("\\Data\\SUBST1.DB"), GENERIC_READ, FILE_SHARE_READ,  NULL,// No security
//                      OPEN_EXISTING,          // Existing file only
//                      FILE_ATTRIBUTE_NORMAL,  // Normal file
//                      NULL);                  // No template file
//
//  if (hFile == INVALID_HANDLE_VALUE)
//  {
//	  hFile = CreateFile (TEXT("\\Data\\SUBST1.DB"),      
//					GENERIC_WRITE,          // Open for writing
//					0,                      // Do not share
//					NULL,                   // No security
//					OPEN_ALWAYS,            // Open or create
//					FILE_ATTRIBUTE_NORMAL,  // Normal file
//					NULL);                  // No template file
//
//	  if (hFile == INVALID_HANDLE_VALUE)
//	  {
//
//	  	LeaveCriticalSection(DB_CS);
//		return false;
//	  }
//  }
//  CloseHandle(hFile);


	LeaveCriticalSection(DB_CS);
  return true;
}
