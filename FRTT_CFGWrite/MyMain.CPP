#include "MyDB.h"
#include <stdio.h>



HANDLE hReader;
MyDB * db;


void SaveCFG( int Ant, long FT, int Active, int Check, int AType, int StopMask){
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

int WINAPI WinMain(  HINSTANCE hInstance,   HINSTANCE hPrevInstance,   LPWSTR command_line,   int nShowCmd )
{

	LPWSTR *argv;
	int argc;


    LPWSTR  arg;
    int    index;


    // count the arguments
    
    argc = 1;
    arg  = command_line;
    
    while (arg[0] != 0) {

        while (arg[0] != 0 && arg[0] == L' ') {
            arg++;
        }

        if (arg[0] != 0) {
        
            argc++;
        
            while (arg[0] != 0 && arg[0] != L' ') {
                arg++;
            }
        
        }
    
    }    
    
    // tokenize the arguments

    argv = (unsigned short**)malloc(argc * sizeof(unsigned short*));

    arg = command_line;
    index = 1;

    while (arg[0] != 0) {

        while (arg[0] != 0 && arg[0] == L' ') {
            arg++;
        }

        if (arg[0] != 0) {
        
            argv[index] = arg;
            index++;
        
            while (arg[0] != 0 && arg[0] != L' ') {
                arg++;
            }
        
            if (arg[0] != 0) {
                arg[0] = 0;    
                arg++;
            }
        
        }
    
    } 
	    
	
	
	printf("FRTT ulit Usage: WriteCFG  xrIP  ServerIP ServerPort\r\n");
	// create device
	
	if(argc==4){

		db = new MyDB();
		printf("Write CFG create DB files\r\n");

		db->CreateDB();

		printf("Clear antenna config\r\n");
		SaveCFG(0,0,0,0,1,9);
		SaveCFG(1,0,0,0,1,9);
		SaveCFG(2,0,0,0,1,9);
		SaveCFG(3,0,0,0,1,9);
		SaveCFG(4,0,0,0,1,9);
		SaveCFG(5,0,0,0,1,9);
		SaveCFG(6,0,0,0,1,9);
		SaveCFG(7,0,0,0,1,9);
	
		printf("Write Reader CFG files with params: %S %S %S\r\n",argv[1],argv[2],argv[3]);

		
		
		DBReaderConfigRecord *rc = new DBReaderConfigRecord();
		
		char buf1[30];
		char buf2[30];
		char buf3[20];

		sprintf(buf1,"%S",argv[1]);
		strcpy(rc->xrIP, buf1);
		sprintf(buf2,"%S",argv[2]);
		strcpy(rc->ServerIP, buf2);
		sprintf(buf3,"%S",argv[3]);
		sscanf(buf3,"%u", & (rc->Port) );
		printf("Saving info: %s %s %u \r\n",rc->xrIP,rc->ServerIP,rc->Port);

		db->SaveReaderConfig(rc);
		delete rc;	
		delete db;


		
	}
	return(0);
};

