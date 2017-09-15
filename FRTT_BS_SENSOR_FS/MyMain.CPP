#include "FRTT.h"
#include <stdio.h>
#include <Pm.h>


//HANDLE hReader;


int WINAPI WinMain(  HINSTANCE hInstance,   HINSTANCE hPrevInstance,   LPWSTR lpCmdLine,   int nShowCmd )
{
	xrDevice *xrD = new xrDevice(); 
	if( xrD->BSLoginOK) {

		xrD->Run();

		long prevNT = xrD->NetworkThreadLoopCount ;
		long prevIT= xrD->InventoryThreadLoopCount ;
		while (true){
			Sleep(60000);
			 
			if(prevNT>=xrD->NetworkThreadLoopCount){
				xrD->RunNT();
				prevNT=0;
				printf("Restart Network thread\r\n");
			}

			if(prevIT>=xrD->InventoryThreadLoopCount){
				xrD->RunIT();
				prevIT=0;
				printf("Restart Inventory thread\r\n");
			}
			
			prevNT = xrD->NetworkThreadLoopCount ;
			prevIT= xrD->InventoryThreadLoopCount ;

			Sleep(60000);

		}
	}
};