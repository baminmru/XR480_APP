#include "frtt.h"
#include <winbase.h>



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

MyDB::MyDB(){
	Cash = new MySortedArray<TagPos>();
	DB_CS = new CRITICAL_SECTION();
	InitializeCriticalSection(DB_CS); 

};

MyDB::~MyDB(){
	delete Cash;
	DeleteCriticalSection(DB_CS);
	delete DB_CS;
};

bool MyDB::CreateDB(){
  EnterCriticalSection(DB_CS);
  HANDLE hAppend;
  hAppend = CreateFile (TEXT("\\Data\\TAG.DB"),      // Open Two.txt.
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


	hAppend = CreateFile (TEXT("\\Data\\EVENT.DB"),      // Open Two.txt.
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

	hAppend = CreateFile (TEXT("\\Data\\CFG.DB"),      // Open Two.txt.
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
	CloseHandle (hAppend);

	hAppend = CreateFile (TEXT("\\Data\\RCFG.DB"),      // Open Two.txt.
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
	LeaveCriticalSection(DB_CS);
 return true;
};

bool MyDB::Open(){
	return true;
};





void MyDB::Close(){

};


bool MyDB::SaveConfig(DBConfigRecord * rec){
	EnterCriticalSection(DB_CS);
	HANDLE hAppend;
	DWORD wBytes;
	hAppend = CreateFile (TEXT("\\Data\\CFG.DB"),      // Open Two.txt.
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


bool MyDB::UpdateConfig(DBConfigRecord * rec, int i){
	EnterCriticalSection(DB_CS);
	HANDLE hAppend;
	DWORD wBytes;
	hAppend = CreateFile (TEXT("\\Data\\CFG.DB"),      // Open Two.txt.
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


bool MyDB::SaveReaderConfig(DBReaderConfigRecord * rec){
    HANDLE hAppend;
	EnterCriticalSection(DB_CS);
	hAppend = CreateFile (TEXT("\\Data\\RCFG.DB"),      // Open Two.txt.
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

bool MyDB::CheckTag(DBTagRecord *rec,DWORD * pos){

	TagPos * tp = new TagPos(0,rec);
	long p;
	p =Cash->Find(tp);
	if(p>=0){
		delete tp;
		tp=Cash->Get(p);
		*pos=tp->Pos;
		printf("Tag in cash p=%ld Cash Size=%ld \r\n", p,Cash->Count());
		return true;
	}
	delete tp;
	
	if(Cash->Count()>1000){
		Cash->Clear();
	}
	EnterCriticalSection(DB_CS);
	HANDLE hFile;
	hFile = CreateFile (TEXT("\\Data\\TAG.DB"),      // Open One.txt
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
	do{
		(*pos)++;
		ReadFile (hFile, tr, sizeof(DBTagRecord), &dwBytesRead, NULL);

		if(!tr->Deleted && tr->Ant==rec->Ant && ! _stricmp(tr->Tag,rec->Tag)){
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


bool MyDB::DeleteSentTag(){
	EnterCriticalSection(DB_CS);
	printf("Make backup for tag\r\n");
	CopyFile(  TEXT("\\Data\\TAG.DB"), TEXT("\\Data\\TAG.BAK"),  false);
	


  HANDLE hFile, hAppend;
  DWORD dwBytesRead, dwBytesWritten, dwPos;
  DBTagRecord *tr = new DBTagRecord(); 

  // Open the existing file.

  printf("Open backup for tag\r\n");
  hFile = CreateFile (TEXT("\\Data\\TAG.BAK"),      // Open One.txt
                      GENERIC_READ,           // Open for reading
                      0,                      // Do not share
                      NULL,                   // No security
                      OPEN_EXISTING,          // Existing file only
                      FILE_ATTRIBUTE_NORMAL,  // Normal file
                      NULL);                  // No template file

  if (hFile == INVALID_HANDLE_VALUE)
  {

    LeaveCriticalSection(DB_CS);
	printf("File Error\r\n");
    return false;
  }

  // Open the existing file, or, if the file does not exist,
  // create a new file.

  printf("Create new tag base\r\n");
  hAppend = CreateFile (TEXT("\\Data\\TAG.DB"),      // Open Two.txt.
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
	printf("File Error\r\n");
    return false;
  }

  // Append the first file to the end of the second file.

  dwPos = SetFilePointer (hAppend, 0, NULL, FILE_END);
  do
  {
	  printf("Read tag\r\n");
    if (ReadFile (hFile, tr, sizeof(DBTagRecord), &dwBytesRead, NULL))
    {
		if(dwBytesRead==sizeof(DBTagRecord)){
			printf("Check tag\r\n");
			if(!tr->Sent ){ 
				printf("Write tag\r\n");
				WriteFile (hAppend, tr, sizeof(DBTagRecord),
					 &dwBytesWritten, NULL);
			}
		}
    }
  }
  while (dwBytesRead == sizeof(DBTagRecord));

  // Close both files.
  printf("Close files\r\n");
  CloseHandle (hFile);
  CloseHandle (hAppend);
  
  printf("Drop cash\r\n");
  Cash->Clear();
  
  LeaveCriticalSection(DB_CS);
  return true;
};

bool MyDB::DeleteSentEvent(){
  EnterCriticalSection(DB_CS);
  printf("Make backup for event\r\n");
  CopyFile(  TEXT("\\Data\\EVENT.DB"), TEXT("\\Data\\EVENT.BAK"),  false);
	


  HANDLE hFile, hAppend;
  DWORD dwBytesRead, dwBytesWritten, dwPos;
  DBEventRecord *tr = new DBEventRecord(); 

  // Open the existing file.

  hFile = CreateFile (TEXT("\\Data\\EVENT.BAK"),      // Open One.txt
                      GENERIC_READ,           // Open for reading
                      0,                      // Do not share
                      NULL,                   // No security
                      OPEN_EXISTING,          // Existing file only
                      FILE_ATTRIBUTE_NORMAL,  // Normal file
                      NULL);                  // No template file

  printf("Open backup for event\r\n");
  if (hFile == INVALID_HANDLE_VALUE)
  {
    
	printf("File Error\r\n");
    LeaveCriticalSection(DB_CS);
    return false;
  }

  // Open the existing file, or, if the file does not exist,
  // create a new file.

  printf("Create main file for event\r\n");
  hAppend = CreateFile (TEXT("\\Data\\EVENT.DB"),      // Open Two.txt.
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        CREATE_ALWAYS,          // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);                  // No template file

  if (hAppend == INVALID_HANDLE_VALUE)
  {
  
	printf("File Error\r\n");  
    CloseHandle (hFile);            // Close the first file.
	LeaveCriticalSection(DB_CS);
    return false;
  }

  // Append the first file to the end of the second file.

  dwPos = SetFilePointer (hAppend, 0, NULL, FILE_BEGIN);
  do
  {
	printf("Read record\r\n");
    if (ReadFile (hFile, tr, sizeof(DBEventRecord), &dwBytesRead, NULL))
    {
		if(dwBytesRead==sizeof(DBEventRecord)){
			printf("Check record\r\n");
			if(!tr->Sent ){
				printf("Write record\r\n");
				WriteFile (hAppend, tr, sizeof(DBEventRecord),
					 &dwBytesWritten, NULL);
			}
		}
    }
  }
  while (dwBytesRead == sizeof(DBEventRecord));

  // Close both files.

  printf("Close files\r\n");
  CloseHandle (hFile);
  CloseHandle (hAppend);
    
  LeaveCriticalSection(DB_CS);
  return true;
};


DBReaderConfigRecord * MyDB::GetReaderConfig(){
	EnterCriticalSection(DB_CS);
	DBReaderConfigRecord * rc = new DBReaderConfigRecord();
	HANDLE hFile;
    hFile = CreateFile (TEXT("\\Data\\RCFG.DB"),      // Open One.txt
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
    DWORD dwPos;
	DWORD dwBytesRead;  
	dwPos = SetFilePointer (hFile, 0, NULL,FILE_BEGIN);
	ReadFile (hFile, rc, sizeof(DBReaderConfigRecord), &dwBytesRead, NULL);
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	return rc;
};


DBConfigRecord * MyDB::GetPointConfig(int i){
	
	EnterCriticalSection(DB_CS);
	DBConfigRecord* rc = new DBConfigRecord();
	HANDLE hFile;
    hFile = CreateFile (TEXT("\\Data\\CFG.DB"),      // Open One.txt
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

bool MyDB::AddTag( DBTagRecord * rec){


	TagPos * tp = new TagPos(0,rec);
	long p;
	p =Cash->Find(tp);
	if(p>=0){
		delete tp;
		printf("Tag in-cash %ld  cash size =%ld\r\n",p,Cash->Count()); 
		return true;
	}
	delete tp;

	EnterCriticalSection(DB_CS);
	HANDLE hAppend;
	DWORD wBytes;
	hAppend = CreateFile (TEXT("\\Data\\TAG.DB"),      // Open Two.txt.
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        OPEN_ALWAYS,            // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);                  // No template file

	  if (hAppend == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		printf ( "Could not open CFG.DB\r\n");
		return false;
	  }

	  DWORD dwPos;
	  // Append the first file to the end of the second file.
	  GetSystemTime(&(rec->T));
	  dwPos = SetFilePointer (hAppend, 0, NULL, FILE_END);
	  WriteFile (hAppend, rec, sizeof(DBTagRecord), &wBytes, NULL);
	  CloseHandle (hAppend);

	  printf("Tag saved to DB %s\r\n",rec->Tag); 

	  Cash->Add(new TagPos(dwPos/sizeof(DBTagRecord),rec));
	  LeaveCriticalSection(DB_CS);
	  return true;
};

bool MyDB::AddEvent(DBEventRecord * rec){
	EnterCriticalSection(DB_CS);
	HANDLE hAppend;
	DWORD wBytes;
	hAppend = CreateFile (TEXT("\\Data\\EVENT.DB"),      // Open Two.txt.
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        OPEN_ALWAYS,            // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);                  // No template file

	  if (hAppend == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		printf ( "Could not open CFG.DB\r\n");
		return false;
	  }

	  DWORD dwPos;
	  // Append the first file to the end of the second file.
	  GetSystemTime(&(rec->T));

	  dwPos = SetFilePointer (hAppend, 0, NULL, FILE_END);
	  WriteFile (hAppend, rec, sizeof(DBEventRecord), &wBytes, NULL);
	  CloseHandle (hAppend);
	  LeaveCriticalSection(DB_CS);
	  return true;
};


int MyDB::GetEventCount(){
	EnterCriticalSection(DB_CS);
	DWORD FileSz;
	HANDLE hFile;
    hFile = CreateFile (TEXT("\\Data\\EVENT.DB"),      // Open One.txt
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
	FileSz=GetFileSize( hFile, &FileSz);
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	printf("Events in base %ld\r\n",FileSz /sizeof(DBEventRecord));
	return FileSz /sizeof(DBEventRecord);
};


int MyDB::GetConfigCount(){
	EnterCriticalSection(DB_CS);
	DWORD FileSz;
	HANDLE hFile;
    hFile = CreateFile (TEXT("\\Data\\CFG.DB"),      // Open One.txt
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
	FileSz=GetFileSize( hFile,&FileSz);
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	//printf("Cfg in base %ld FileSize %ld\r\n",FileSz /sizeof(DBConfigRecord),FileSz);
	return (FileSz /sizeof(DBConfigRecord));
};

int MyDB::GetTagCount(){
	EnterCriticalSection(DB_CS);
	DWORD FileSz;
	HANDLE hFile;
    hFile = CreateFile (TEXT("\\Data\\TAG.DB"),      // Open One.txt
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
	FileSz=GetFileSize( hFile,&FileSz);
	CloseHandle(hFile);
		LeaveCriticalSection(DB_CS);
	printf("TAgs in base %ld\r\n",FileSz /sizeof(DBTagRecord));
	return FileSz /sizeof(DBTagRecord);
};

DBTagRecord * MyDB::GetTag(int i){
	EnterCriticalSection(DB_CS);
	DBTagRecord * tr = new DBTagRecord ();
	HANDLE hFile;
    hFile = CreateFile (TEXT("\\Data\\TAG.DB"),      // Open One.txt
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
	dwPos = SetFilePointer (hFile, (i-1) * sizeof(DBTagRecord), NULL,FILE_BEGIN);
	ReadFile (hFile, tr, sizeof(DBTagRecord), &dwBytesRead, NULL);
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	printf("GetTag(%d)=%s S=%d D=%d\r\n",i,tr->Tag,tr->Sent ,tr->Deleted );
	return tr;
};
DBEventRecord * MyDB::GetEvent(int i){
	EnterCriticalSection(DB_CS);
	DBEventRecord * er = new DBEventRecord ();
	HANDLE hFile;
    hFile = CreateFile (TEXT("\\Data\\EVENT.DB"),      // Open One.txt
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
	dwPos = SetFilePointer (hFile, (i-1) * sizeof(DBEventRecord), NULL,FILE_BEGIN);
	ReadFile (hFile, er, sizeof(DBEventRecord), &dwBytesRead, NULL);
	CloseHandle(hFile);
	LeaveCriticalSection(DB_CS);
	printf("GetEvent(%d)=%s S=%d D=%d\r\n",i,er->Message, er->Sent,er->Deleted);
	return er;
};
bool MyDB::UpdateTag( DBTagRecord * rec, int i){
	EnterCriticalSection(DB_CS);
	HANDLE hAppend;
	DWORD wBytes;
	hAppend = CreateFile (TEXT("\\Data\\TAG.DB"),      // Open Two.txt.
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        OPEN_ALWAYS,            // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);                  // No template file

	  if (hAppend == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		printf ( "Could not open CFG.DB\r\n");
		return false;
	  }

	  DWORD dwPos;
	  // Append the first file to the end of the second file.
	  //GetSystemTime(&(rec->T));
	  dwPos = SetFilePointer (hAppend, (i-1)*sizeof(DBTagRecord), NULL, FILE_BEGIN);
	  WriteFile (hAppend, rec, sizeof(DBTagRecord), &wBytes, NULL);
	  CloseHandle (hAppend);
	  LeaveCriticalSection(DB_CS);
	  return true;
};
bool MyDB::UpdateEvent( DBEventRecord * rec, int i){
	EnterCriticalSection(DB_CS);
	HANDLE hAppend;
	DWORD wBytes;
	hAppend = CreateFile (TEXT("\\Data\\EVENT.DB"),      // Open Two.txt.
                        GENERIC_WRITE,          // Open for writing
                        0,                      // Do not share
                        NULL,                   // No security
                        OPEN_ALWAYS,            // Open or create
                        FILE_ATTRIBUTE_NORMAL,  // Normal file
                        NULL);                  // No template file

	  if (hAppend == INVALID_HANDLE_VALUE)
	  {
		LeaveCriticalSection(DB_CS);
		printf ( "Could not open CFG.DB\r\n");
		return false;
	  }

	  DWORD dwPos;
	  // Append the first file to the end of the second file.
	  //GetSystemTime(&(rec->T));

	  dwPos = SetFilePointer (hAppend, (i-1) * sizeof(DBEventRecord), NULL, FILE_BEGIN);
	  WriteFile (hAppend, rec, sizeof(DBEventRecord), &wBytes, NULL);
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
  hFile = CreateFile (TEXT("\\Data\\CFG.DB"), GENERIC_READ, FILE_SHARE_READ,  NULL,// No security
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


	LeaveCriticalSection(DB_CS);
  return true;
}
