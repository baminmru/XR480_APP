#ifndef MACRO_H
#define MACRO_H



#define ifdel(s)  \
if ( s) delete s; \
s = 0;

#define ifdela(s)    \
if (s) delete [] s; \
s = 0;

#define SetupString(s,d) 		  \
{										  \
	if(s){                       \
	d = new char[strlen((char*)s)+1];   \
	memset(d,0,strlen((char*)s)+1);    \
	strcpy((char*)d,(char*)s);         \
    } else  d=0; \
}


#define SetupString2(s,d) 		  \
{										  \
	 ifdela(d);                   \
	 if(s){                       \
	d = new char[strlen((char*)s)+1];   \
	strcpy((char*)d,(char*)s);                 \
} else  d=0;}




#define TRACE(x)
//#define TRACE(x) Debug("\n %s %d>>>%s\n",__FILE__,__LINE__,x);



#define Min(x,y) ((x)>(y)?(y):(x))
#define Max(x,y) ((x)>(y)?(x):(y))




#define Sortable(X) \
  friend bool operator == (const X & s, const X& x); \
  friend bool operator < (const X & s,const X& x);


#define Streamable(X)   \
  friend SimpleStream & operator >> (SimpleStream & s, X& x); \
  friend SimpleStream & operator << (SimpleStream & s, X& x);



#endif