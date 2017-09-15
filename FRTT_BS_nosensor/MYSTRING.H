/****************************************************
 *     MYSTRING.H                                   *
 *  Copyright (C) Михаил М. Баранов                 *
 *  Модификация без предварительного согласия автора*
 *  будет рассматривается как нарушение авторского  *
 *  и имущественного права                          *
 *  создан: 21 Сентября 1995, Четверг               *
 ****************************************************/
#ifndef MYSTRING
#define MYSTRING

void  StrClear(char *s, int l, char f=' ');
#define STDDELIMITERS " \t\r\n"
char* StrTrim (char *s, char *buf=0, char Filler=' ', char * Delimiters=STDDELIMITERS);
char* StrUpper(char *s, char *buf=0);
/*
  mode =  0 - Cut
  mode =  1 - Wrap
  mode = 'C' - Fill with 'C'
*/
char* StrL(char *s,char *buf, unsigned int l, int mode=0);
char* StrR(char *s,char *buf, unsigned int l, int mode=0);
char* StrC(char *s,char *buf, unsigned int l, int mode=0);
char* StrJ(char *s,char *buf, unsigned int l, int mode=0);

int   StrEmpty(char *s);
int   StrWrap (char *s, unsigned int l);
int   StrRead (char *s ,char *f, ...);
int   StrForm (char *s, char *buf[], unsigned int l,int array);

const char *MakeTemp(const char *path=0, const char *ext="TMP");
char* StrClearAllChar(const char * s  ,char CharToErase=' ');
char* StrReplaceChar(const char * s  ,char OldChar='\r', char NewChar=0);

#endif
