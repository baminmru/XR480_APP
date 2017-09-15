#include "MACRO.H"
#include "TEMPLATE.H"

class DBTagRecord{
public :
	DBTagRecord(){
		memset(Tag,0,25);
		Sent = false;
		Deleted = false;
		Ant=0;
	};
	char Tag[25] ;
	SYSTEMTIME T;
	int Ant;
	long Filter;
	bool Sent;
	bool Deleted;

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

class MyDB{
	CRITICAL_SECTION *DB_CS;

public:
	MyDB();
	~MyDB();

	bool VerifyDB();
	bool CreateDB();
	bool Open();
	void Close();

	
	bool SaveConfig(DBConfigRecord * rec);
	bool UpdateConfig(DBConfigRecord * rec, int i);
	
	bool SaveReaderConfig(DBReaderConfigRecord * rec);

	bool CheckTag(DBTagRecord *rec, DWORD *pos);
	bool CheckTag(DBTagRecord *rec);

	
	bool DeleteSentTag();
	bool DeleteSentEvent();

	DBReaderConfigRecord * GetReaderConfig();
	DBConfigRecord * GetPointConfig(int i);

	bool AddTag( DBTagRecord * rec);
	bool AddEvent(DBEventRecord * rec);
	int GetEventCount();
	int GetTagCount();
	int GetConfigCount();
	DBTagRecord * GetTag(int i);
	DBEventRecord * GetEvent(int i);
	bool UpdateTag( DBTagRecord * rec, int i);
	bool UpdateEvent( DBEventRecord * rec, int i);

private:

	//void AddToCach()
	MySortedArray<TagPos> *Cash; 
};