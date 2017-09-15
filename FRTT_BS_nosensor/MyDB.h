#include "MACRO.H"
#include "TEMPLATE.H"

class DBTagRecord{
public :
	DBTagRecord(){
		memset(Tag,0,25);
		memset(TagSubst,0,25);
		Sent = false;
		Deleted = false;
		Ant=0;
	};
	char Tag[25];
	SYSTEMTIME T;
	int Ant;
	long Filter;
	bool Sent;
	bool Deleted;
	char TagSubst[25]; 
	SYSTEMTIME T2; /* только чтобы не сбивался размер записи */
	int Ant2;      /* только чтобы не сбивался размер записи */
	long Filter2;  /* только чтобы не сбивался размер записи */
	bool Sent2;    /* только чтобы не сбивался размер записи */
	bool Deleted2; /* только чтобы не сбивался размер записи */
};



class DBTagSubst{
public :
	DBTagSubst(){
		memset(Tag,0,25);
		Passed = false;

	};
	char Tag[25];
	bool Passed;	// tag pass my antenna !
	
};


class DBTagRewrite{
	public :
	char Tag[25];
	DBTagRewrite(){
		memset(Tag,0,25);
	}
};

class DBEventRecord{
public :
	DBEventRecord(){
		memset(Message,0,100);
		memset(Tag,0,25);
		Sent = false;
		Deleted = false;
		Ant=0;
	};
	SYSTEMTIME T;
	int Ant;
	char Tag[25] ;
	char Message[100];
	bool Sent;
	bool Deleted;
	unsigned long Filter;
};

struct DBConfigRecord{
	int Antena;
	bool Active;
	bool Check;
	int PointType;
	long Filter;
	int StopMask;
};

struct DBReaderConfigRecord{
	char ServerIP[20];
	unsigned int Port;
	char xrIP[20];
	unsigned long NextTag;
};


class TagPos{
public :
	TagPos(DWORD aPos, DBTagRecord *val){
		rec = new DBTagRecord();
		memcpy(rec,val,sizeof(DBTagRecord));
		Pos=aPos;
	};

	~TagPos(){
		delete rec;
	}
	DWORD Pos;
	DBTagRecord *rec;
	Sortable(TagPos);
};

// 16.01
//class TagSubstPos{
//public :
//	TagSubstPos(DWORD aPos, DBTagSubst *val){
//		rec = new DBTagSubst();
//		memcpy(rec,val,sizeof(DBTagSubst));
//		Pos=aPos;
//	};
//
//	~TagSubstPos(){
//		delete rec;
//	}
//	DWORD Pos;
//	DBTagSubst *rec;
//	Sortable(TagSubstPos);
//};
//
//class TagRewritePos{
//public :
//	TagRewritePos(DWORD aPos, DBTagRewrite *val){
//		rec = new DBTagRewrite();
//		memcpy(rec,val,sizeof(DBTagRewrite));
//		Pos=aPos;
//	};
//
//	~TagRewritePos(){
//		delete rec;
//	}
//	DWORD Pos;
//	DBTagRewrite *rec;
//	Sortable(TagRewritePos);
//};



class MyDB{
	CRITICAL_SECTION *DB_CS;

public:
	

	MyDB();
	~MyDB();

	bool VerifyDB();
	bool CreateDB();

	// 16.01
	bool RebuildSubstDB( ); 
	DBTagSubst * GetTagSubst(int i);
	bool CheckTagSubst(DBTagSubst *rec, DWORD *pos);
	bool CheckTagSubst(DBTagSubst *rec);
	bool AddTagSubst( DBTagSubst * rec);
	int GetTagSubstCount();
	bool UpdateTagSubst( DBTagSubst * rec, int i);
	bool SubstThisTag(const char * Tag, char * TagReal);
	bool CheckSubstDBSize();
	bool CheckTagDBSize();
	bool CheckEventDBSize();




	//10.02.2012
	bool RebuildRewriteDB( ); 
	DBTagRewrite * GetTagRewrite(int i);
	bool CheckTagRewrite(DBTagRewrite *rec, DWORD *pos);
	bool CheckTagRewrite(DBTagRewrite *rec);
	bool AddTagRewrite( DBTagRewrite * rec);
	int GetTagRewriteCount();

	bool Open();
	void Close();

	DWORD MinutesLeft(SYSTEMTIME T);
	
	bool SaveConfig(DBConfigRecord * rec);
	bool UpdateConfig(DBConfigRecord * rec, int i);
	
	bool SaveReaderConfig(DBReaderConfigRecord * rec);

	bool CheckTag(DBTagRecord *rec, DWORD *pos);
	bool CheckTag(DBTagRecord *rec);


	
	bool DeleteSentTag();
	bool DeleteSentEvent();
	bool DeletePassedTag();

	DBReaderConfigRecord * GetReaderConfig();
	DBConfigRecord * GetPointConfig(int i);

	bool AddTag( DBTagRecord * rec);

	


	bool AddEvent(DBEventRecord * rec);
	int GetEventCount();
	int GetTagCount();
	int GetConfigCount();
	int GetBoxDelay();




	DBTagRecord * GetTag(int i);
	DBEventRecord * GetEvent(int i);


	bool UpdateTag( DBTagRecord * rec, DWORD i);
	bool UpdateEvent( DBEventRecord * rec, int i);

	/*void ClearSubstCash(){
		CashSubst->Clear();
	};


	void ClearRewriteCash(){
		CashRewrite->Clear();
	};*/

private:

	//void AddToCach()
	MySortedArray<TagPos> *Cash; 

	//16.01
	/*MySortedArray<TagSubstPos> *CashSubst; 
	MySortedArray<TagRewritePos> *CashRewrite; */
};