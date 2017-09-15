#ifndef MYSTREAM_H
#define MYSTREAM_H

#include "macro.h"

#include <limits.h>
#include <winbase.h>


// SimpleStream
class SimpleStream{
 public :
   virtual unsigned char Write(unsigned char Len, void * buf){return 0;};
   virtual unsigned char Read(unsigned char Len, void * buf){return 0;};
};

// >> and << for standart type
///////////////////////////////////////////////////////////////////////////////
SimpleStream &operator << (SimpleStream& out_data, bool& P);
SimpleStream &operator << (SimpleStream& out_data, int& P);
SimpleStream &operator << (SimpleStream& out_data, unsigned short& P);
SimpleStream &operator << (SimpleStream& out_data, long& P);
SimpleStream &operator << (SimpleStream& out_data, unsigned long& P);
SimpleStream &operator << (SimpleStream& out_data, float& P);
SimpleStream &operator << (SimpleStream& out_data, double& P);
SimpleStream &operator << (SimpleStream& out_data, long double& P);
SimpleStream &operator << (SimpleStream& out_data, char &P);
SimpleStream &operator << (SimpleStream& out_data, unsigned char & P);
SimpleStream &operator << (SimpleStream& out_data, const char *P);
SimpleStream &operator >> (SimpleStream& out_data, bool& P);
SimpleStream &operator >> (SimpleStream& out_data, int& P);
SimpleStream &operator >> (SimpleStream& out_data, unsigned short& P);
SimpleStream &operator >> (SimpleStream& out_data, long& P);
SimpleStream &operator >> (SimpleStream& out_data, unsigned long& P);
SimpleStream &operator >> (SimpleStream& out_data, float& P);
SimpleStream &operator >> (SimpleStream& out_data, double& P);
SimpleStream &operator >> (SimpleStream& out_data, long double& P);
SimpleStream &operator >> (SimpleStream& out_data, char & P);
SimpleStream &operator >> (SimpleStream& out_data, unsigned char & P);
SimpleStream &operator >> (SimpleStream& out_data, char **P);
SimpleStream &operator >> (SimpleStream& out_data, SYSTEMTIME &P);
SimpleStream &operator << (SimpleStream& out_data, SYSTEMTIME &P);



class DataBuffer{
   long BufSize;
   unsigned char *Buffer;
public:
    void Clear(){
        BufSize=0;
        delete Buffer;
        Buffer = 0;
   };
   DataBuffer(long _BufSize=0){
        BufSize=_BufSize;
        Buffer = 0;
        if(BufSize)
                Buffer = new unsigned char[BufSize];
   };
   DataBuffer(long _BufSize,const unsigned char* _Buffer){
        Buffer=0;
        SetBuffer(_BufSize,_Buffer);
   };
   void SetBuffer(long _BufSize,const unsigned char* _Buffer){
        BufSize=_BufSize;
        if(Buffer)
          delete Buffer;
        Buffer = 0;
        if(BufSize){
          Buffer = new unsigned char[BufSize];
          if(_Buffer)
            memcpy(Buffer,_Buffer,BufSize);
        }
   };

   void SetMyBuffer(long _BufSize, unsigned char* _Buffer){
        BufSize=_BufSize;
        if(Buffer)
          delete Buffer;
        Buffer = 0;
        if(BufSize){
          Buffer = _Buffer;
        }
   };

   void Append(long _BufSize, unsigned char* _Buffer){
      unsigned char * ap = new unsigned char[BufSize+_BufSize];
      memcpy(ap,Buffer,BufSize);
      memcpy(ap+BufSize,_Buffer,_BufSize);
      SetMyBuffer(BufSize+_BufSize,ap);
   };


   long Length() const{ return BufSize;} ;
   unsigned char * GetBuffer() const{ return Buffer;} ;

   friend SimpleStream &operator << (SimpleStream& out_data, DataBuffer& F);
   friend SimpleStream &operator >> (SimpleStream& out_data, DataBuffer& F);
};


////////////////////////////////////////////////////////////////////////////////





class MyMemory: public SimpleStream{
	unsigned char * Buf;
	long BufSize;
	long ReadPtr;
	long WritePtr;
	int Delta;
	long Total;
public:
	MyMemory(int _Delta=4096){
	 Buf=0;
	 BufSize=0;
	 ReadPtr=0;
	 WritePtr=0;
	 Total=0;
	 Delta=_Delta;
	 };
	 ~MyMemory(){
		Clear();
	 }
 DataBuffer *MakeDataBuffer(){
  return new DataBuffer(BufSize,(unsigned char*)Buf);
 };
 virtual unsigned char Write(unsigned char Len, void * buf);
 virtual unsigned char Read(unsigned char Len, void * buf);

 void LoadBuffer(unsigned char * Buffer,long Length);
 long BufferSize(){ return BufSize;};
 const unsigned char * GetBuffer(){ return (unsigned char*) Buf;};

 void LoadFromFile(const  wchar_t * fname);
 //void LoadFromFileDecode(char * fname);

 void SaveToFile(const  wchar_t * fname);
 //void SaveToFileEncode(char * fname);

 void SetDelta( int _Delta){Delta=_Delta;};

 void Clear(){
	ifdela(Buf);
	 BufSize=0;
	 ReadPtr=0;
	 WritePtr=0;
	 Total=0;
 };

 void Restart(){
    ReadPtr=0;
 };

};

class MyFile:  public SimpleStream {
 HANDLE h;
 /*
 DWORD a;
 DWORD p;
 DWORD c;
 */

public:

	MyFile(LPCTSTR  name){ //,Access access ,Permission  permission =noPermission){
			h=INVALID_HANDLE_VALUE;
		
			h = CreateFile (name,      
						  GENERIC_READ | GENERIC_WRITE, 
						  0,        // Do not share
						  NULL,              // No security
						  OPEN_ALWAYS,          // Open or Create
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);                  // No template file
		
 };

 ~MyFile( ){
	 if (h!=INVALID_HANDLE_VALUE) CloseHandle(h);
	
 };

 unsigned char GetNextSize(){
			unsigned char sz;
		 if(h!=INVALID_HANDLE_VALUE ){
			DWORD wBytes;
			ReadFile(h,&sz,1,& wBytes,NULL);
			SetFilePointer(h,-1,0, FILE_CURRENT);
			
		 }
		 return sz;
 };

 virtual unsigned char Write(unsigned char Len, void * buf);
 virtual unsigned char Read(unsigned char Len, void * buf);
};

/*
class StreamSavedFile{
public:
   static bool PathOnly;
   TCHAR* Path;
   StreamSavedFile(TCHAR* _Path=""){
    Path=_Path;
   };
   friend SimpleStream &operator << (SimpleStream& out_data, StreamSavedFile& F);
   friend SimpleStream &operator >> (SimpleStream& out_data, StreamSavedFile& F);
};


*/




////////////////////////////////////////////////////////////////////////////////


#endif
