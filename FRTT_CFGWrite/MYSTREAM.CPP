#include "mystream.h"


//#include "math.h"
//#include "LZss.h"
#define PORTION_SIZE 250
#define IN_PORTION_SIZE 255
// SimpleStream
// >> and << for standart type
///////////////////////////////////////////////////////////////////////////////
SimpleStream &operator << (SimpleStream&out_data,int&P){
     out_data.Write(sizeof(int),&P);
	return out_data;
};

SimpleStream &operator << (SimpleStream&out_data,unsigned short&P){
    out_data.Write(sizeof(unsigned short),&P);
	return out_data;
};

SimpleStream &operator >> (SimpleStream&out_data,unsigned short&P){
    out_data.Read(sizeof(unsigned short),&P);
	return out_data;
};

SimpleStream &operator << (SimpleStream&out_data,bool&P){
   out_data.Write(sizeof(bool),&P);
	return out_data;
};

SimpleStream &operator << (SimpleStream&out_data,long&P){
    out_data.Write(sizeof(long),&P);
	return out_data;
};

SimpleStream &operator << (SimpleStream&out_data,unsigned long&P){
   
   out_data.Write(sizeof(unsigned long),&P);
	return out_data;
};

SimpleStream &operator >> (SimpleStream&out_data,unsigned long&P){
   
   out_data.Read(sizeof(unsigned long),&P);
	return out_data;
};
SimpleStream &operator << (SimpleStream&out_data,float&P){
   out_data.Write(sizeof(float),&P);
	return out_data;
};
SimpleStream &operator << (SimpleStream&out_data,double&P){
   out_data.Write(sizeof(double),&P);
	return out_data;
};
SimpleStream &operator << (SimpleStream&out_data,long double&P){
   out_data.Write(sizeof(long double),&P);
	return out_data;
};
SimpleStream &operator << (SimpleStream&out_data,char &P){
   out_data.Write(sizeof(long double),&P);
	return out_data;
};

SimpleStream &operator << (SimpleStream&out_data,unsigned char &P){
   out_data.Write(sizeof(unsigned char),&P);
	return out_data;
};

SimpleStream &operator << (SimpleStream & out_data, const char *P){
  
   long z=0;
   if(!P){
     out_data.Write(sizeof(long),&z);
   }else{
   	long l = strlen(P)+1;
	   long parts=l/PORTION_SIZE;
	   if (parts*PORTION_SIZE<l ) parts++;
	   out_data.Write(sizeof(long),&parts);
	   const char *ptr=P;
	   while(l){
   	   z=(l>PORTION_SIZE)?PORTION_SIZE:l;
			out_data.Write(z,(void*)ptr);
	      ptr+=z;
   	   l-=z;
	   }
   }
	return out_data;
};




SimpleStream &operator >> (SimpleStream& out_data, char * & P){
    char * buf=0;
    out_data >> &buf;
    P=buf;
    delete buf;
    return out_data;
};

SimpleStream &operator >> (SimpleStream&out_data,int&P){
   P=0;
   out_data.Read(sizeof(int),&P);
	return out_data;
};
SimpleStream &operator >> (SimpleStream&out_data,bool&P){
   P=false;
   out_data.Read(sizeof(bool),&P);
	return out_data;
};

SimpleStream &operator >> (SimpleStream&out_data,long&P){
 P=0;
 out_data.Read(sizeof(long),&P);
	return out_data;
};
SimpleStream &operator >> (SimpleStream&out_data,float&P){
 P=0.0;
 out_data.Read(sizeof(float),&P);
	return out_data;
};
SimpleStream &operator >> (SimpleStream&out_data,double&P){
  P=0.0;
   out_data.Read(sizeof(double),&P);
	return out_data;
};
SimpleStream &operator >> (SimpleStream&out_data,long double&P){
  P=0.0;
  out_data.Read(sizeof(long double),&P);
	 return out_data;
};
SimpleStream &operator >> (SimpleStream&out_data,char &P){
  P=0;
  out_data.Read(sizeof(char),&P);
  return out_data;
};
SimpleStream &operator >> (SimpleStream&out_data,unsigned char &P){
  P=0;
  out_data.Read(sizeof(unsigned char),&P);
	 return out_data;
};

SimpleStream &operator >> (SimpleStream&out_data,char **P){
   ifdela(*P);
   long parts=0;
   unsigned char sz;
   out_data.Read(sizeof(long),&parts);
   if(parts>0){
	   char  *buf =new char[IN_PORTION_SIZE*(parts+1)];
   	   memset(buf,0,IN_PORTION_SIZE*(parts+1));
	   char *ptr=buf;
   	while(parts){
       sz=out_data.Read(IN_PORTION_SIZE,ptr);
	      ptr+=sz;
   	  parts--;
	   };
   	SetupString(buf,*P);
	   delete [] buf;
   }
   return out_data;
};

////////////////////////////////////////////////////////////////////////////////



/*
        j=i*i;
        l1=j&0xF;
        l2=(j&0xF0) >> 4;
        l3=(j&0xF00) >> 8;
        l4=(j&0xF000) >> 12;
        j= (l1<<12)|(l2)|(l3<<4)|(l4<<8);

        // from
        l1=j&0xF;
        l2=(j&0xF0) >> 4;
        l3=(j&0xF00) >> 8;
        l4=(j&0xF000) >> 12;
        j= (l1<<4)|(l2<<8)|(l3<<12)|(l4);
        k=sqrt(j);

*/




 void MyMemory::LoadFromFile(const  wchar_t * fname){
	 
    Clear();
    HANDLE h=INVALID_HANDLE_VALUE;
    h=CreateFile(fname,  GENERIC_READ,           // Open for reading
						  0,                      // Do not share
						  NULL,                   // No security
						  OPEN_EXISTING,  
						  FILE_ATTRIBUTE_NORMAL,  // Normal file
						  NULL);
    //h = open(fname,acRead,noPermission);
    if (h==INVALID_HANDLE_VALUE) return;
    unsigned long l,s,wBytes;
    l=GetFileSize(h,&l);
    unsigned char * ptr;
    Buf = new unsigned char[l];
    BufSize=l;
    Total=l;
    ptr=Buf;
    while(l){
     if(l>16384) s=16384; else s=l;
     l-=s;
     ReadFile(h,ptr,s,&wBytes,NULL);
     ptr+=s;
    }
    CloseHandle(h);
	
 };

 void MyMemory::LoadBuffer(unsigned char * Buffer,long Length){
	Clear();
	Buf= new unsigned char[Length];
  memcpy(Buf, Buffer,Length);
	BufSize=Length;
	Total=Length;
 };
 void MyMemory::SaveToFile(const wchar_t * fname){
    HANDLE h;
    //h = open(fname,acWrite,noPermission);

	h = CreateFile (fname ,      // Open Two.txt.
					GENERIC_WRITE,          // Open for writing
					0,                      // Do not share
					NULL,                   // No security
					OPEN_ALWAYS,            // Open or create
					FILE_ATTRIBUTE_NORMAL,  // Normal file
					NULL);                  // No template file

    if (h==INVALID_HANDLE_VALUE) return;
    long l=BufSize,s;
    unsigned char *ptr;
    ptr=Buf;
	DWORD wBytes;
    while(l){
     if(l>16384) s=16384; else s=l;
     l-=s;

     //write(h,ptr,s);
	  WriteFile (h, ptr, s, &wBytes, NULL);
      ptr+=s;
    }
    //close(h);
	CloseHandle(h);
 };



unsigned char MyMemory::Write(unsigned char Len, void * buf){
  if(BufSize+Len+1>Total){
	  while(BufSize+Len+1>Total){
	  	Total+=Delta;
	  }
   
     unsigned char *Buf2 = new unsigned char[Total];
     memset(Buf2,0,Total);
     if(BufSize) memcpy(Buf2,Buf,BufSize);
     ifdela(Buf);
     Buf=Buf2;
  }
  Buf[WritePtr++]=Len;
  memcpy(Buf+WritePtr,buf,Len);
  WritePtr+=Len;
  BufSize+=Len+1;

  return Len;
};

unsigned char MyMemory::Read(unsigned char Len, void * buf){
   if(Len ){
      unsigned char sz,ss;
      memset(buf,0,Len);
      if(BufSize>ReadPtr)
      	sz=Buf[ReadPtr++];
      else
      {
        return 0;
      }
	  if(ReadPtr + sz  > BufSize) sz = BufSize-ReadPtr;
      if(sz>Len) ss=Len; else ss=sz;
      memcpy(buf,Buf+ReadPtr,ss);
      ReadPtr+=sz;
      return ss;
    }
    return 0;
 };



unsigned char MyFile::Write(unsigned char Len, void * buf){
  if(h!=INVALID_HANDLE_VALUE ){
	  DWORD wBytes;
      //write(h,&Len,1);
	  WriteFile (h, &Len, 1, &wBytes, NULL);
      //write(h,buf,Len);
	  WriteFile (h, buf, Len, &wBytes, NULL);
  }
  return Len;
};

unsigned char MyFile::Read(unsigned char Len, void * buf){
   unsigned long i=0;
   if(h!=INVALID_HANDLE_VALUE && Len ){
	   DWORD wBytes;
      unsigned char sz=0;
	  ReadFile (h, &sz, 1, &wBytes, NULL);
      if(wBytes!=1){
        return 0;
      }
      i=(Len>sz)? sz:Len;
	  ReadFile (h, &buf, i, &wBytes, NULL);
      //i=read(h,buf,i);
	  i = wBytes;
      sz-=i;
      if(sz){
        //lseek(h, sz,SEEK_CUR);
		  SetFilePointer (h, sz, NULL,FILE_CURRENT);
	  }
    }
    return i;
 };


SimpleStream &operator >> (SimpleStream& out_data, SYSTEMTIME  &P){
	
	out_data >> P.wYear ;
	out_data >> P.wMonth  ;
	out_data >> P.wDay   ;
	out_data >> P.wHour   ;
	out_data >> P.wMinute   ;
	out_data >> P.wSecond  ;
	out_data >> P.wMilliseconds  ;
	out_data >> P.wDayOfWeek   ;


	return out_data;
};
SimpleStream &operator << (SimpleStream& out_data, SYSTEMTIME &P){
	out_data<<P.wYear ;
	out_data<<P.wMonth  ;
	out_data<<P.wDay   ;
	out_data<<P.wHour   ;
	out_data<<P.wMinute   ;
	out_data<<P.wSecond  ;
	out_data<<P.wMilliseconds  ;
	out_data<<P.wDayOfWeek   ;
	return out_data;
};

/*

bool StreamSavedFile::PathOnly=true;

SimpleStream &operator << (SimpleStream& stream, StreamSavedFile& F){
    int h=-1;
    long l,s,L=-1;
    unsigned char u;
    char ptr[PORTION_SIZE];
    stream << F.Path; // path

    if(F.PathOnly){
     stream << L; // -1!
     return stream;
    }

	 
    h = open(F.Path.c_str(),acRead,noPermission);
    if (h==-1)
     l=0;
    else
     l=filelength(h);
    L=l;
    stream << L;

    while(l){
     if(l>PORTION_SIZE) u=PORTION_SIZE; else u=l;
     l-=u;
     if(h>=0) read(h,ptr,u);
     stream.Write(u,ptr);
    }

    if(h>=0)close(h);
    return stream;
 };


SimpleStream &operator >> (SimpleStream& stream, StreamSavedFile& F){
    int L;
    long l;
    unsigned char u;
    char  ptr[IN_PORTION_SIZE];
    int h=-1;

    stream >> F.Path; // path

    stream >> L;

    if(!F.PathOnly && L >0) h = open(F.Path.c_str(),acWrite,noPermission);
    l=L;
    if(L<0) l=0;

    while(l){
     if(l>IN_PORTION_SIZE) u=IN_PORTION_SIZE; else u=l & 0xFF;
     u=stream.Read(u,ptr);
     l-=u;
     if(h>=0) write(h,ptr,u);
    }
    if(h>=0) close(h);

    return stream;
};

*/

SimpleStream &operator << (SimpleStream& stream, DataBuffer& F){

    long l;
    unsigned char u,*bptr;
    bptr = F.Buffer;
    l=F.BufSize;
    stream << l;

    while(l){
     if(l>PORTION_SIZE) u=PORTION_SIZE; else u=l;
     stream.Write(u,bptr);
     l-=u;
     bptr+=u;
    }

    return stream;
 };


SimpleStream &operator >> (SimpleStream& stream, DataBuffer& F){
    long l;
    unsigned char u;
    char  ptr[IN_PORTION_SIZE];
    unsigned char *bptr;

    stream >> l;

    F.SetBuffer(l,0);
    bptr=F.Buffer;

    while(l){
     if(l>IN_PORTION_SIZE) u=IN_PORTION_SIZE; else u=l;
     u=stream.Read(u,ptr);
     memcpy(bptr,ptr,u);
     bptr+=u;
     l-=u;

    }

    return stream;
};

