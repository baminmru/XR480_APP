/****************************************************
 *   MYSTRING.CPP                                   *
 *  Copyright (C) Михаил М. Баранов                 *
 *  Модификация без предварительного согласия автора*
 *  будет рассматривается как нарушение авторского  *
 *  и имущественного права                          *
 *  создан: 21 Сентября 1995, Четверг               *
 ****************************************************/
#include "stdio.h"
#include "macro.h"
#include "mystring.h"


void  StrClear(char * s,int l,char f){
  TRACE("StrClear");
  memset(s,f,l);
  s[l]=0;
};

int    upper(char ch){
  if(ch>='a' && ch <='z') return ch-32;
  if(ch>='а' && ch<='я') return ch-32;
	if(ch=='ё') return 'Ё';
	return ch;
};

char *   StrUpper(char *source,char *dest){
	TRACE("StrUpper");
  unsigned char *s=(unsigned char *)source;
  unsigned char *buf=(unsigned char *)dest;
	if(!buf)
		buf=s;
	for(unsigned int i=0; i<strlen((char*)s);i++)
		buf[i]=upper(s[i]);
	return (char*)buf;
};

char*   StrTrim(char *s,char *buf, char Filler ,char * Delimiters){
	TRACE("StrTrim");
	if(!buf)
    buf=s;
	long i,j,sz;
	i=j=0;
    sz=strlen(s);
	for(j=0;j<sz;j++){

		if(!strchr(Delimiters,s[j]))
			//if(s[j]!=' ' && s[j] !='\t' && s[j] !=10 && s[j] !=13)
			buf[i++]=s[j];
		else
			if(i!=0 && buf[i-1]!=Filler)
				buf[i++]=Filler;
	}

	// подрезка хвоста
	if(i && buf[i-1]==Filler) i--;

	buf[i]=0;

	return buf;
};

int     StrWrap(char *s, unsigned int l){
	
		if(strlen(s)<=l)
			return strlen(s)-1;

		for( int i=l-1 ;i>=0;i--)
			if( s[i]==' ' ||          s[i]=='.' ||
				s[i]==',' ||          s[i]=='+' ||
				s[i]=='-' ||          s[i]=='/' ||
				s[i]=='\\'||          s[i]=='%' ||
				s[i]=='*' ||          s[i]==')' ||
				s[i]==']' ||          s[i]=='&' ||
				s[i]=='!' ||          s[i]=='@' ||
				s[i]=='$' ||          s[i]=='\''||
				s[i]=='|' ||          s[i]=='?' ||
				s[i]=='{' ||          s[i]=='}' ||
				s[i]=='[' ||          s[i]=='(' ||
				s[i]=='#' ||          s[i]=='~' ||
				s[i]=='>' ||          s[i]=='<' ||
				s[i]=='№' ||          s[i]=='`' ||
				s[i]=='^' ||          s[i]==0 )
					return i;

	 return 0;

};


char*   StrL(char *s,char *buf, unsigned int l,int mode){
	TRACE("StrL");
	StrClear(buf,l);
	if(strlen(s)<l){
		strncpy(buf,s,strlen(s));
	}else{
		switch ( mode )
		{
		case 0:
			strncpy(buf,s,l);
			break;

		case 1:
			{
			int g=StrWrap(s,l);
			if(g){
				//g++;
				strncpy(buf,s,g);
			}else
				strncpy(buf,s,l);
			break;
      }
    default:
      StrClear(buf,l,mode);

    }
  }
  return buf;
};

char*   StrR(char *s,char *buf, unsigned int l,int mode){
  TRACE("StrR");

  StrClear(buf,l);
  if(strlen(s)<l){
    strncpy((buf+l-strlen(s)),s,strlen(s));
  }else{
    switch ( mode )
    {
    case 0:
      strncpy(buf,s,l);
      break;

    case 1:
      {
      int g=StrWrap(s,l);
      if(g){
        //g++;
        strncpy(buf+l-g,s,g);
      }else
        strncpy(buf,s,l);
      break;
      }
    default:
      StrClear(buf,l,mode);

    }
  }
  return buf;
};

char*   StrC(char *s,char *buf, unsigned int l,int mode){
  TRACE("StrC");
  StrClear(buf,l);
  if(strlen(s)<l){
    strncpy((buf+(l-strlen(s))/2),s,strlen(s));
  }else{
    switch ( mode )
    {
    case 0:
      strncpy(buf,s,l);
      break;

    case 1:
      {
      int g=StrWrap(s,l);
      if(g){
        //g++;
        strncpy(buf+(l-g)/2,s,g);
      }else
        strncpy(buf,s,l);
      break;
      }
    default:
      StrClear(buf,l,mode);

    }
  }
  return buf;
};
char*   StrJ(char *s,char *buf, unsigned int l,int mode){
  TRACE("StrJ");
  StrClear(buf,l);
  char *c=new char [strlen(s)+1];
  StrTrim(s,c);
  int sp=0;
  int mult=0;
  int mod=0;
  unsigned int i,j=0;

  for(i=0 ;i< strlen(c);i++)
    if(c[i]==' ') sp++;

  if(strlen(c)<l){
    if(sp){
      mult=(l-strlen(c))/sp;
      mod=(l-strlen(c))%sp;
      for(i=0;i<strlen(c);i++){
        if(c[i]==' '){
          if(mod){ buf[j++]=' '; mod--;}
          for(int k=0;k<=mult;k++)
            buf[j++]=' ';
        }else{
          buf[j++]=c[i];
        }
      }
    }else{
      strncpy(buf,c,strlen(c));
    }

  }else{
    switch ( mode )
    {
    case 0:
      strncpy(buf,s,l);
      break;

    case 1:
      {
      int g=StrWrap(c,l);
      if(g){
          //g++;
          sp=0;
          for(int i=0 ;i< g;i++)
            if(c[i]==' ') sp++;
          if(sp){
            mult=(l-g)/sp;
            mod=(l-g)%sp;
            for(i=0;i<g;i++){
              if(c[i]==' '){
                if(mod){ buf[j++]=' '; mod--;}
                for(int k=0;k<=mult;k++)
                  buf[j++]=' ';
              }else
                buf[j++]=c[i];
            }
          }else{
            strncpy(buf,c,g);
          }


      }else
        strncpy(buf,s,l);
      break;
      }
    default:
      StrClear(buf,l,mode);

    }
  }
  delete c;
  return buf;
};





int     StrForm(char *s,char * buf[], int l,unsigned int array){
  TRACE("StrForm");
  unsigned int curl=0,ll,i;
  for(i=0;i<array;i++){
    if(curl<strlen(s)){
      ll=StrWrap(s+curl,l);
      StrClear(buf[i],l);

      if(!ll)
        ll=l;
      else
        ll++;
      strncpy(buf[i],s+curl,ll);
      curl+=ll;
      if(s[curl]==' ')
        curl++;
    }else
      break;
  }
 return i;
};

int     StrEmpty(char *s){
    TRACE("StrEmpty");
    for(unsigned int i=0 ;i<=strlen(s);i++)
      if( !(s[i]==' ' || s[i]==0 || s[i]=='\r' || s[i] =='\t' || s[i]=='\n' ))
            return 0;

   return 1;

};

int   StrRead(char *buf ,char *format, ...){
  TRACE("StrRead");
  int i,p[100],fp[100];
  char **ptr=&format;
  char *b=new char[strlen(buf)+1];
  char *f=new char[strlen(format)+1];
  strcpy(b,buf);
  strcpy(f,format);
  StrTrim(b);
  StrTrim(f);

  int fj=1,j=1,k=strlen(b);
  memset(p,0,sizeof(int)*100);
  memset(fp,0,sizeof(int)*100);

  // разбиваем на строки
  for(i=0;i<k ;i++){
    if(b[i]==' '){
       p[j++]=i+1;
       b[i]=0;
       if(j==100) break;
    }
  }

  k=strlen(f);
  for(i=0;i<k ;i++){
    if(f[i]==' '){
       fp[fj++]=i+1;
       f[i]=0;
       if(fj==100) break;
    }
  }

  int res,out=0;
  for(i=0;i<fj;i++){
    res=0;
    if(f[fp[i]]=='['){
      fp[i]++;
      k=strlen(&(f[fp[i]]));
      f[fp[i]+k]=0;
      for(k=0;k<j;k++){
			 if(res= sscanf(&(b[p[k]]),&(f[fp[i]]),ptr[i+1]) ) break;
      }
    }else
      res=sscanf(&(b[p[i]]),&(f[fp[i]]),ptr[i+1]);
    if(res) out++;
  }
  delete b;
  delete f;
  return out;
};


char*   PackFloat(char *d, char *s,short size){
	StrTrim(s);
	char *point=strchr(s,'.');
	char b[41];
	StrClear(b,40);
	unsigned short idx=39, digs=strlen(s);
	unsigned int i,j;
	// записали то что после точки
	if(point){
		for(i=1;i<=strlen(point);i++)
			b[idx--]=point[strlen(point)-i];
		digs-=strlen(point);
	}

	// теперь переписываем по три цифры и ставим запятую

	for(i=1;i<=digs;i+=3){
		for(j=0;j<3 && i+j <= digs;j++){
			b[idx--]=s[digs-i-j];
		}
		if(j==3)
			b[idx--]=',';
	}

	StrTrim(b);
	if((b[0]=='+' || b[0]=='-') && b[1]==','){
		b[1]=b[0];
		b[0]=' ';
		StrTrim(b);
	}
	if(b[0]==','){
    b[0]=' ';
    StrTrim(b);
  }
  StrR(b,d,size);
  return d;
};





char * StrClearAllChar(const char * s  ,char CharToErase){
  char * buf = new char[strlen(s)];
  memset(buf,0,strlen(s));
  long j=0;
  for(unsigned long i=0;i<strlen(s);i++){
	  if( s[i]!=CharToErase){
		buf[j++]=s[i];
	  }
  }
  return buf;
};

char * StrReplaceChar(const char * s  ,char OldChar, char NewChar){
  char * buf = new char[strlen(s)+1];
  memset(buf,0,strlen(s)+1);
  long j=0;
  for(unsigned long i=0;i<strlen(s);i++){
	  if( s[i]!=OldChar){
		buf[j++]=s[i];
	  }else{
		buf[j++]=NewChar;
	  }
  }
  return buf;
};