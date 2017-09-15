#include "FRTT.h"

#define APP_VERSION	"3.10"
#define MAXLEN 1024
#define USE_TYPE_TAG_EX

//TYPE_TAG *xrReader::EventTag;
//TYPE_TAG_EX *xrReader::EventTagEx;

xrDevice *xrReader::Dev=NULL;


// converts a byte array into a hex null terminated string
 void xrReader::MakeHexString(unsigned char *pBytes, int nBytes, char *pOutBuf)
{
	int i;
	char *psz = pOutBuf;

	for(i = 0; i < nBytes; i++)
	{
		psz += sprintf(psz, "%2.2X", (int)*pBytes);
		pBytes++;
	};
};






void xrReader::ConfigureLockCode( TYPE_TAG_EX *pTag, BYTE *cLockCode )
{
	DWORD dwNumItems;
	char szLockCode[16];

	// Set Lock Code
	if(pTag->dwType == RFID_TAG_TYPE_EPC_CLASS1)
	{
		dwNumItems = 1;
		MakeHexString(cLockCode, dwNumItems, szLockCode);
		// Use first byte of lock code
		RFID_SetCapCurrValue(hReader, RFID_TAGCAP_LOCKCODE, &dwNumItems, 1, (DWORD *)cLockCode);
	}
	else if(pTag->dwType == RFID_TAG_TYPE_EPC_CLASS0 || pTag->dwType == RFID_TAG_TYPE_EPC_CLASS0PLUS)
	{
		dwNumItems = 3;
		MakeHexString(cLockCode, dwNumItems, szLockCode);
		RFID_SetCapCurrValue(hReader, RFID_TAGCAP_LOCKCODE, &dwNumItems, 3, (DWORD *)cLockCode);
	};

	printf("Using lock code: %s\n", szLockCode);
};

DWORD xrReader::ConfigureAutonomousMode()
{
	DWORD dwParamValue = RFID_READCAP_READMODE_AUTONOMOUS;
	DWORD dwNumItems = 1;
	DWORD rtn;

	// Now tell API to configure the read mode
	rtn = RFID_SetCapCurrValue(hReader, RFID_READCAP_READMODE, &dwNumItems, sizeof(dwParamValue), &dwParamValue);
    //	dwReadMode = RFID_READCAP_READMODE_AUTONOMOUS;
	return (rtn);
}


DWORD xrReader::ConfigureOnDemandMode()
{
	DWORD dwParamValue = RFID_READCAP_READMODE_ONDEMAND;
	DWORD dwNumItems = 1;
	DWORD rtn;


	// Now tell API to configure the read mode
	rtn = RFID_SetCapCurrValue(hReader, RFID_READCAP_READMODE, &dwNumItems, sizeof(dwParamValue), &dwParamValue);

	
	// any tag events that are still coming are Autonomous mode events...
//	dwReadMode = RFID_READCAP_READMODE_ONDEMAND;
	return(rtn);
};

BOOL xrReader::ConfigureReader()
{
	BOOL bSuccess = FALSE;
	BYTE bSupportedTagTypesParam[5];
	BYTE bEnabledTagTypesParam[5];
	DWORD dwNumItems;
	DWORD rtn;
	WORD  offset=2;

	BYTE bank,type;
	DWORD  nItems=1;
	DWORD  accpswd=1;


	RFID_CAPS Caps;

	ZeroMemory(bEnabledTagTypesParam, sizeof(bEnabledTagTypesParam));

	dwNumItems = sizeof(bSupportedTagTypesParam);
	
	rtn = RFID_GetCapCurrValue(hReader, RFID_TAGCAP_SUPPORTED_TYPES, &dwNumItems, sizeof(bSupportedTagTypesParam), bSupportedTagTypesParam);

	if(rtn == RFID_SUCCESS)
	{
		if(bSupportedTagTypesParam[RFID_TAG_TYPE_EPC_CLASS0])
		{
			bEnabledTagTypesParam[RFID_TAG_TYPE_EPC_CLASS0] = 1;		// enable class 0 tag reads
		};

		if(bSupportedTagTypesParam[RFID_TAG_TYPE_EPC_CLASS1])
		{
			bEnabledTagTypesParam[RFID_TAG_TYPE_EPC_CLASS1] = 1;		// enable class 1 tag reads
		};

		if(bSupportedTagTypesParam[RFID_TAG_TYPE_EPC_CLASSG2])
		{
			bEnabledTagTypesParam[RFID_TAG_TYPE_EPC_CLASSG2] = 1;		// enable class 1 Gen 2 tag reads
		};

		if(bSupportedTagTypesParam[RFID_TAG_TYPE_ISO18000_6B])
		{
			bEnabledTagTypesParam[RFID_TAG_TYPE_ISO18000_6B] = 1;		// enable class 1 Gen 2 tag reads
		};

		// dwNumItems is now set to the number of possibly supported tag types.  This list is the same length as the enabled list...
		if(RFID_SetCapCurrValue(hReader, RFID_TAGCAP_ENABLED_TYPES, &dwNumItems, sizeof(bEnabledTagTypesParam), bEnabledTagTypesParam) == RFID_SUCCESS)
		{
			bSuccess = TRUE;
		};
	};
	
	BYTE EnableTypes[5]= {0,1,0,0,1};	//C0, C1, G2
	DWORD items = sizeof (EnableTypes);
	rtn = RFID_SetCapCurrValue(hReader, RFID_TAGCAP_ENABLED_TYPES, &items, sizeof(EnableTypes), EnableTypes);


	RFID_GetCaps(hReader, &Caps);

	dwNumItems = 1;


	offset =2;
	bank=1;
	accpswd =0;

	

	nItems = 1;
	rtn    = RFID_SetCapCurrValue(hReader, RFID_TAGCAP_G2_ACCESS_PASSWORD, &nItems, sizeof(DWORD), &accpswd);

	nItems = 1;
	type	 = RFID_TAG_TYPE_EPC_CLASSG2;						// set tag type to be processed
	rtn		 = RFID_SetCapCurrValue(hReader, RFID_WRITECAP_TAGTYPE, &nItems, sizeof(type), &type);
										
	nItems = 1;																		// setup the G2 mem bank to write
	rtn    = RFID_SetCapCurrValue(hReader, RFID_TAGCAP_G2_MEM_BANK, &nItems, sizeof(bank), &bank);

	nItems = 1;		//set offset =2 to write to start of EPC and not CRC & PCBits
	rtn		 = RFID_SetCapCurrValue(hReader, RFID_TAGCAP_G2_WORD_POINTER, &nItems, sizeof(offset), &offset);

/*
	DWORD dwParamValue = 1;
	rtn = RFID_SetCapCurrValue(hReader,RFID_READCAP_EVENT_ALLTAGS,&nItems,sizeof(dwParamValue), &dwParamValue);
	

	DWORD dwSize;
	
	nItems = wcslen(tszReadTagEventName) + 1;		// include the null character
	RFID_SetCapCurrValue(hReader, RFID_READCAP_EVENTNAME, &nItems, nItems * sizeof(tszReadTagEventName[0]), tszReadTagEventName);

	EventTag = new TYPE_TAG();
	memset(EventTag,0,sizeof(TYPE_TAG));
	
	TYPE_TAG *pTag;
	nItems = 1;		
	dwSize = sizeof(EventTag);// Size of pointer to a tag
	pTag = EventTag;
	
	rtn		 =RFID_SetCapCurrValue(hReader, RFID_READCAP_EVENTTAGPTR, &nItems, dwSize, &pTag);

	
	EventTagEx = new TYPE_TAG_EX();
	memset(EventTagEx,0,sizeof(TYPE_TAG_EX));

	TYPE_TAG_EX *pTagEx;
	nItems = 1;		
	dwSize = sizeof(EventTagEx);// Size of pointer to a tag
	pTagEx = EventTagEx;
	
	rtn		 =RFID_SetCapCurrValue(hReader, RFID_READCAP_EVENTTAGPTR_EX, &nItems, dwSize, &pTagEx);

	 
	
	nItems = 1;	
	dwSize=700;
	rtn		 = RFID_SetCapCurrValue(hReader, RFID_READCAP_DUTYCYCLE_ONTIME, &nItems, sizeof(dwSize), &dwSize);
	nItems = 1;	
	dwSize=0;
	rtn		 = RFID_SetCapCurrValue(hReader, RFID_READCAP_DUTYCYCLE_OFFTIME_PCNT, &nItems, sizeof(dwSize), &dwSize);

*/   

	// erase all masks
	ClearSelect();


	
	BYTE bMask = 0x3F;
	BYTE bInterval = 1; //minimum detection interval
	RFID_SetGPIDetection(hReader, hGPIOEvent, bMask, bInterval);

	return(bSuccess);
};

BOOL xrReader::ConfigureTCPIP(const TCHAR *pszIPAddress, DWORD dwPort)
{
	BOOL bSuccess = FALSE;
	DWORD dwItems;
	DWORD dwStatus;

	dwItems = 1;

	dwStatus = RFID_SetCapCurrValue(hReader, RFID_DEVCAP_IP_PORT, &dwItems, sizeof(dwPort), &dwPort);
	if(dwStatus == RFID_SUCCESS)
	{
		dwItems = wcslen(pszIPAddress) + 1;		// + 1 for null

		dwStatus = RFID_SetCapCurrValue(hReader, RFID_DEVCAP_IP_NAME, &dwItems, dwItems * sizeof(pszIPAddress[0]),(void*) pszIPAddress);
		if(dwStatus == RFID_SUCCESS)
		{
			bSuccess = TRUE;
		};
	};

	return(bSuccess);
};


void xrReader::RemoveAntennaFromSequence( BYTE AntennaIndex)
{
	BYTE NewSequence[RFID_ANTENNA_SEQUENCE_LENGTH];
	BYTE CapSequence[RFID_ANTENNA_SEQUENCE_LENGTH];
	memset(NewSequence, 0, sizeof(NewSequence));
	memset(CapSequence, 0, sizeof(CapSequence));

	DWORD dwNumItems = sizeof(CapSequence);

	DWORD dwStatus = RFID_GetCapCurrValue(hReader, RFID_DEVCAP_ANTENNA_SEQUENCE, &dwNumItems, dwNumItems, CapSequence);

	int index = 0;
	if (dwStatus == RFID_SUCCESS) 
	{
		for (int i=0; i<sizeof(NewSequence); i++)
		{
			if (CapSequence[i] != AntennaIndex)
			{
				NewSequence[index++] = CapSequence[i];
			}
			if (CapSequence[i] == 0xFF)
			{
				break;
			}
		}
		dwNumItems = index;
		RFID_SetCapCurrValue(hReader, RFID_DEVCAP_ANTENNA_SEQUENCE, &dwNumItems, dwNumItems, NewSequence);
	}
}

void xrReader::AddAntennaToSequence( BYTE AntennaIndex)
{
	BYTE NewSequence[RFID_ANTENNA_SEQUENCE_LENGTH];
	BYTE CapSequence[RFID_ANTENNA_SEQUENCE_LENGTH];
	memset(NewSequence, 0, sizeof(NewSequence));
	memset(CapSequence, 0, sizeof(CapSequence));

	DWORD dwNumItems = sizeof(CapSequence);

	DWORD dwStatus = RFID_GetCapCurrValue(hReader, RFID_DEVCAP_ANTENNA_SEQUENCE, &dwNumItems, dwNumItems, CapSequence);

	int index = 0;
	if (dwStatus == RFID_SUCCESS) 
	{
		for (index=0; index<sizeof(NewSequence)-1; index++)
		{
			if (CapSequence[index] == 0xFF)
			{
				NewSequence[index++] = AntennaIndex;
				break;
			}
		}
		NewSequence[index] = 0xFF;

		dwNumItems = index;
		RFID_SetCapCurrValue(hReader, RFID_DEVCAP_ANTENNA_SEQUENCE, &dwNumItems, dwNumItems, NewSequence);
	}
}

int _httoi(const char *value)
{
  struct CHexMap
  {
    char chr;
    int value;
  };
  const int HexMapL = 16;
  CHexMap HexMap[HexMapL] =
  {
    {'0', 0}, {'1', 1},
    {'2', 2}, {'3', 3},
    {'4', 4}, {'5', 5},
    {'6', 6}, {'7', 7},
    {'8', 8}, {'9', 9},
    {'A', 10}, {'B', 11},
    {'C', 12}, {'D', 13},
    {'E', 14}, {'F', 15}
  };
  char *mstr =  _strupr(_strdup(value)); //duplicate and upper
  char *s = mstr;
  int result = 0;
  if (*s == '0' && *(s + 1) == 'X') s += 2;
  bool firsttime = true;
  while (*s != '\0')
  {
    bool found = false;
    for (int i = 0; i < HexMapL; i++)
    {
      if (*s == HexMap[i].chr)
      {
        if (!firsttime) result <<= 4;
        result |= HexMap[i].value;
        found = true;
        break;
      }
    }
    if (!found) break;
    s++;
    firsttime = false;
  }
  free(mstr);
  return result;
}

bool xrReader::SetGPIO(BYTE GPOMask, BYTE GPOValue)
{

	bool ret;
	
	DWORD dwStatus = RFID_SetGPOValue(hReader, GPOMask, GPOValue);
	if (dwStatus == RFID_SUCCESS) 
	{
		ret = true;
		printf("Set GPIO output mask == 0x%2x, output value == 0x%2x\n", GPOMask, GPOValue);
	} else {
		ret = false;
		printf("Failed to set GPIO output. error = %u\n", dwStatus);
	}

	dwStatus = RFID_SUCCESS;
	BYTE InputValue = 0;
	BYTE OutputValue = 0;
	dwStatus = RFID_GetGPIOValue(hReader, &InputValue, &OutputValue);
	if (dwStatus == RFID_SUCCESS) 
	{
		printf("GPIO input == 0x%x, output == 0x%x\n\n", InputValue, OutputValue);
		
	} else {
		printf("Failed to get GPIO input. error = %u\n", dwStatus);
		ret= false;
	}
	return ret;
}

/// Processes the error from a reader's tag reads
/// @param[in] hReader	The handle of the reader
/// @param[in] Tag		The tag structure which holds the status information
bool xrReader::ProcessErrorRead( TYPE_TAG &Tag)
{
	DWORD dwStatus = Tag.status;
	if (dwStatus == RFID_SUCCESS)
	{
		return false;
	}

	printf("Tag read Error %S, antenna: %2u\n", 
		RFID_GetCommandStatusText(hReader, Tag.status),
		Tag.antennaNum);
	
	switch (dwStatus)
	{
	case RFID_CMD_UNKVAL:
	case RFID_CMD_HWERR: 
		//RemoveAntennaFromSequence( Tag.antennaNum);
		break;

	case RFID_PORT_OPEN_ERROR:
	case RFID_PORT_WRITE_ERROR:
	case RFID_COMMAND_TIMEOUT:
	case RFID_PORT_NOT_OPEN:
	case RFID_UNKNOWN_COMM_TYPE:
		break;

	case RFID_CMD_UNKLEN:		//0xF1:	// insufficient data
	case RFID_CMD_UNKCMD:		//0xF2:	// Command not supported
	case RFID_UNKNOWN_ERROR:	//0xFF:
	default:
		break;
	}

	Sleep(0);
	//Sleep(100);
	return true;

}

/// Processes the error from a reader's tag reads
/// @param[in] hReader	The handle of the reader
/// @param[in] Tag		The tag structure which holds the status information
bool xrReader::ProcessErrorReadEx( TYPE_TAG_EX &Tag)
{
	DWORD dwStatus = Tag.dwOperationStatus;
	if (dwStatus == RFID_SUCCESS)
	{
		return false;
	}

	printf("Tag Read EX Error %S, reader: %8X, detail: %2x, antenna: %2u\n", 
	RFID_GetCommandStatusText(hReader, dwStatus),
	hReader,
	Tag.wStatusDetail, Tag.antennaNum);
	
	switch (dwStatus)
	{
	case RFID_CMD_UNKVAL:
		{
			switch (Tag.wStatusDetail)
			{
			case 0xF0:		// invalid command params
				//RemoveAntennaFromSequence( Tag.antennaNum);
				break;
			default:
				break;
			}
		}
		break;

	case RFID_CMD_HWERR: 
		{
			switch (Tag.wStatusDetail)
			{
			case 0xF3:		// Antenna fault
				//RemoveAntennaFromSequence( Tag.antennaNum);
				break;
			case 0xF4:		// DSP Timeout
			case 0xF5:		// DSP Error
			case 0xF6:		// DSP Idle
			case 0xF7:		// Zero Power  (invalid RF power specified)
			default:
				break;
			}
		}
		break;

	case RFID_PORT_OPEN_ERROR:
	case RFID_PORT_WRITE_ERROR:
	case RFID_COMMAND_TIMEOUT:
	case RFID_PORT_NOT_OPEN:
	case RFID_UNKNOWN_COMM_TYPE:
		break;

	case RFID_CMD_UNKLEN:		//0xF1:	// insufficient data
	case RFID_CMD_UNKCMD:		//0xF2:	// Command not supported
	case RFID_UNKNOWN_ERROR:	//0xFF:
	default:
		//RemoveAntennaFromSequence( Tag.antennaNum);
		break;
	}

	Sleep(0);
	//Sleep(100);
	return true;

}




DWORD WINAPI xrReader::ReadTagEventThread(LPVOID pvarg)
{
	HANDLE hEvents[3];
	DWORD dwEvent;
	char szTagID[128];
	DWORD dwCounter = 0;
	char *szProtocol;
	DWORD dwStartTick = 0;
	DWORD dwStatus;
	DWORD rtn;

	xrReader *myReader = (xrReader *)pvarg;

	
	int numEvents = 1;
	hEvents[0] = myReader->hStopThreads;

	int i=0;
	hEvents[numEvents++] = myReader->hReadTagEvent;
	//hEvents[numEvents++] = myReader->hGPIOEvent;
	
	while(1)
	{
		dwEvent = WaitForMultipleObjects(numEvents, hEvents, FALSE, 1000);

		if(dwEvent == WAIT_TIMEOUT){
			printf("RT:*\r\n");
			continue;
		}

		// stop threads??
		if(dwEvent == WAIT_OBJECT_0)
		{
			break;
		}

		if(dwEvent > WAIT_OBJECT_0)
		{
			printf("RT:Tag\r\n");
			//if(RFID_GetTagID(myReader->hReader, EventTag) == RFID_SUCCESS)
			{
				
				dwCounter++;
				/*
				memset (szTagID,0,sizeof(szTagID));
				

				MakeHexString(EventTag->tagID, EventTag->dataLength, szTagID);

				printf("Tag %8.8u:%25.25s Ant:%2.2u %3.3u.%3.3u \n",
				dwCounter,
				szTagID, 
				EventTag->antennaNum, 
				EventTag->lastSeen.wSecond, 
				EventTag->lastSeen.wMilliseconds);
					memset (szTagID,0,sizeof(szTagID));
				

				MakeHexString(EventTagEx->tagID, EventTagEx->dataLength, szTagID);

				printf("TagEx %8.8u:%25.25s Ant:%2.2u %3.3u.%3.3u \n",
				dwCounter,
				szTagID, 
				EventTagEx->antennaNum, 
				EventTagEx->lastSeen.wSecond, 
				EventTagEx->lastSeen.wMilliseconds);

				if(Dev){
				//	Dev->ProcessTag(szTagID, EventTag->antennaNum, EventTag);
				}

				//memset(myReader->EventTag,0,sizeof(TYPE_TAG));
				*/
			
			
			}//if rtn

			ResetEvent(myReader->hReadTagEvent);
		}
	
		
	};

	return(0);
};




DWORD xrReader::SetOutLoopValue( DWORD value)
{
	DWORD dwNumItems = 1;
	return RFID_SetCapCurrValue(hReader, RFID_READCAP_OUTLOOP, &dwNumItems, 1, &value);
}


void xrReader::SetCombinedAntenna() {

	return;
	BYTE bAntennaS[MAX_NUMBER_ANTENNA] = {0,1,2,3, 0,0,0,0};

	DWORD dwNumItems = MAX_NUMBER_ANTENNA;
	if(RFID_SetCapCurrValue(hReader, RFID_DEVCAP_ANTENNA_SEQUENCE, &dwNumItems, sizeof(bAntennaS), bAntennaS) == RFID_SUCCESS)
	{
	};


	
	//return;

	//BYTE bAntennaGroup[MAX_NUMBER_ANTENNA] = {1,2,1,2, 0,0,0,0};
	BYTE bAntennaGroup[MAX_NUMBER_ANTENNA] = {1,2,3,4, 0,0,0,0};

	dwNumItems = MAX_NUMBER_ANTENNA;
	if(RFID_SetCapCurrValue(hReader, RFID_DEVCAP_ANTENNA_GROUP, &dwNumItems, sizeof(bAntennaGroup), bAntennaGroup) == RFID_SUCCESS)
	{
	};
	return;

//RFID_SetCapCurrValue(hReader, RFID_DEVCAP_ANTENNA_SEQUENCE, &dwNumItems, sizeof(cAntennaSequence[0]) * 2, cAntennaSequence);

}

bool xrReader::IsXR480()
{
	// buffer to hold the capability value
	BYTE bValueBuffer[MAX_NUMBER_ANTENNA];
	// the size of the buffer holding the value of this capability
	DWORD dwBufferSize = sizeof(bValueBuffer);
	// number of items for this capability
	DWORD dwNumItems = dwBufferSize/sizeof(BYTE);

	// retrieve the capability value for currently enabled tag types from the API
	if(RFID_GetCapCurrValue(hReader, 
		RFID_DEVCAP_VALID_ANTENNA_LIST, 
		&dwNumItems, 
		dwBufferSize, 
		bValueBuffer) == RFID_SUCCESS)
	{	
		if (dwNumItems == 8)
		{
			for (int i=0; i<dwNumItems; i++)
			{
				if (bValueBuffer[i] == 0xFF)
				{
					return false;
				}
			}
			return true;
		}
	}
	return false;
}


void xrReader::SetAirProtocolGen2Only() {

	// buffer to hold the capability value
	BYTE bValueBuffer[RFID_TAG_TYPE_MAXINDEX];
	// the size of the buffer holding the value of this capability
	DWORD dwBufferSize = sizeof(bValueBuffer);
	// number of items for this capability
	DWORD dwNumItems = dwBufferSize/sizeof(BYTE);

	// retrieve the capability value for currently enabled tag types from the API
	if(RFID_GetCapCurrValue(hReader, RFID_TAGCAP_ENABLED_TYPES, &dwNumItems, 
		dwBufferSize, bValueBuffer) == RFID_SUCCESS)
	{
		// detect currently enabled tag types
		if (bValueBuffer[RFID_TAG_TYPE_EPC_CLASS0]) {
			// EPC Gen 1 Class 0 is enabled for reading
		}
	
		if (bValueBuffer[RFID_TAG_TYPE_EPC_CLASS0PLUS]) {
			// EPC Gen 1 Class 0+ is enabled for reading
		}

		if (bValueBuffer[RFID_TAG_TYPE_EPC_CLASS1]) {
			// EPC Gen 1 Class 1 is enabled for reading
		}

		if (bValueBuffer[RFID_TAG_TYPE_EPC_CLASSG2]) {
			// EPC Gen 2 is enabled for reading
		}

		if (bValueBuffer[RFID_TAG_TYPE_ISO18000_6B]) {
			// EPC Gen 2 is enabled for reading
		}

		// change currently enabled tag types, reading Gen 2 only
		// disable class 0 for reading	
		bValueBuffer[RFID_TAG_TYPE_EPC_CLASS0] = 0;		
		// disable class 0+ for reading
		bValueBuffer[RFID_TAG_TYPE_EPC_CLASS0PLUS] = 0;	
		// disable class 1 for reading
		bValueBuffer[RFID_TAG_TYPE_EPC_CLASS1] = 0;
		// disbale ISO
		bValueBuffer[RFID_TAG_TYPE_ISO18000_6B] = 0;
		// enable Gen 2 for reading
		bValueBuffer[RFID_TAG_TYPE_EPC_CLASSG2] = 1; 

		if(RFID_SetCapCurrValue(hReader, RFID_TAGCAP_ENABLED_TYPES, &dwNumItems, 
			dwBufferSize, bValueBuffer) == RFID_SUCCESS) 
		{
			// successfully change enabled tag types for reading
		}
	}


}



DWORD xrReader::ReadTags (TAG_LIST_EX *gTagList)
{
	DWORD rtn;

	rtn = RFID_ReadTagInventoryEX(hReader, gTagList, TRUE);
	return rtn;
}



bool xrReader::WriteTag(char * OldTagMask, char* NewTag, int Antenna)
{
	static int nSequence = 0;
	DWORD accpswd=0,rtn;
	WORD  offset=0x10;

	BYTE ant;
	DWORD  nItems=1;

	char  cstr[100];
	BYTE  xstr[100], xMstr[100];
	BYTE len,wlen;
	
	bool OK;


	strcpy(cstr,OldTagMask);
	
	len = strlen(cstr);
	
	memset (xMstr,0,sizeof(xMstr));


	printf("WT:Set Antenna\r\n");
	// Setup Antenna for writing
	ant = Antenna;
	nItems = 1;		//set antenna for writing
	rtn		 = RFID_SetCapCurrValue(hReader, RFID_WRITECAP_ANTENNA, &nItems, sizeof(ant), &ant);

	
	rtn = SetSelect (cstr);											// apply filter for EPC given

	if (rtn != RFID_SUCCESS)
	{
		//ClearSelect();
		printf("Error set mask, return\r\n");
		return false;
	}


	memset (cstr,0,sizeof(cstr));

	strcpy(cstr,NewTag);

	wlen = strlen(cstr);

	wlen = (wlen > 104)? 104:wlen;	// each char = 1/2 byte

	memset (xstr,0,sizeof(xstr));

	wlen = Hexify(cstr, xstr);		//returns number of bytes


	
	int wCnt=0;
	OK = false;
	while(wCnt<=9){
		wCnt++;
		printf("WT:Programming pass %d\r\n",wCnt);
		rtn = RFID_ProgramTags(hReader, xstr, wlen,1, 2, 3);

		if(rtn == RFID_SUCCESS)
		{
			printf("Tag Programmed\n");
			OK = true;
			break;

		}	
		else
		{
			printf("%S\n", RFID_GetCommandStatusText(hReader, rtn));
		
		};
	}


	rtn = ClearSelect ();											// erase filter

	if (rtn != RFID_SUCCESS)
	{
		
		printf("ClearSelect Error: %S\n", RFID_GetCommandStatusText(hReader, rtn));
	}



	return OK;

}


DWORD xrReader::ConfigureReadLoop( BYTE bOuterLoop, BYTE bInnerLoop)
{
	DWORD items;
	DWORD rtn;

	items = 1;												// setup the outer read loop

	rtn = RFID_SetCapCurrValue (hReader, RFID_READCAP_OUTLOOP, &items, sizeof(bOuterLoop), &bOuterLoop);

	if(rtn != RFID_SUCCESS)
	{
		//...
	}
	items = 1;												// setup the outer read loop

	rtn |= RFID_SetCapCurrValue (hReader, RFID_READCAP_INLOOP, &items, sizeof(bInnerLoop), &bInnerLoop);

	if(rtn != RFID_SUCCESS)
	{
		//...
	}
	return(rtn);
}






int xrReader::MyMakeHexBuffer(char *pBytes, int nBytes, unsigned char *pOutBuf)
{
	int nBytesConverted = 0;
	char *pBuf = pBytes;
	unsigned char *pOutPtr = pOutBuf;
	unsigned int temp;

	for (INT j=0;j<nBytes;j++)
	{
		if(sscanf(pBuf, "%2X", &temp) == 1)
		{
			*pOutPtr++ = (unsigned char)temp;
			pBuf += 2;
			nBytesConverted++;
		};
	};
	return(nBytesConverted);
}

int xrReader::HexifyNib(char *str, unsigned char *hex)		// returns nibbles
{
	int byte=0, cnt = 0;
	int nibbles=0;
	char *pstr = str;
	unsigned char *phex = hex;
	unsigned int x;

	cnt = strlen(str)/2;

	for (int j=0;j<cnt;j++)
	{
		if(sscanf(pstr, "%2X", &x) == 1)
		{
			*phex++ = (unsigned char)x;
			pstr += 2;
			nibbles+=2;
		}
	}

	if (cnt % 2)
	{
		if (sscanf(pstr, "%X", &x)==1)
		{
			x<<=4;
			*phex++ = x;
			nibbles++;
		}
	}
	return (nibbles);
}


// формирует шестнадцатиричное значение маски из строки  
int xrReader::Hexify(char *str, unsigned char *hex)		// returns bytes
{
	int bytes=0, cnt = 0;
	char *pstr = str;
	unsigned char *phex = hex;
	unsigned int x;

	cnt = strlen(str)/2;

	for (int j=0;j<cnt;j++)
	{
		if(sscanf(pstr, "%2X", &x) == 1)
		{
			*phex++ = (unsigned char)x;
			pstr += 2;
			++bytes;
		}
	}

	if (strlen(str) % 2)										// if odd nibbles in string pickup odd nibb and pad zero into Least sig nibble in byte
	{
		if (sscanf(pstr, "%X", &x)==1)
		{
			x<<=4;
			*phex++ = x;
			++bytes;
		}
	}

	return(bytes);
};


// set mask for writing tag
DWORD xrReader::SetSelect (char* SelTag)
{
	DWORD rtn;
	BYTE  len;
	DWORD nItems;

/*

	TAG_MASK tagMask;
	memset(&tagMask, 0, sizeof(tagMask));
	len =  Hexify(SelTag, tagMask.cTagMask );
	tagMask.cBitLen= 6*8;
	tagMask.cBitStartOffset = 32;
	rtn = RFID_SetTagMask(hReader, &tagMask);

  
	char szTagID[25];
	memset (szTagID,0,sizeof(szTagID));
	MakeHexString(tagMask.cTagMask, len, szTagID);

	printf("SS:Set mask %s \n",szTagID);

*/
	
	TAG_MASK_EX tagMask;


	TAG_MASK_EX1_INIT(tagMask);


	len =  Hexify(SelTag, tagMask.cTagMask); //returns number of bytes
		
	tagMask.nBitLen= (len >= 2) ? (len*8) : 16;// need a minimum of 16 bits for the bit length regardless of the size of the actual mask len

	tagMask.nBitStartOffset = 0x20;						 //#### EPC ONLY--no PC bits


	tagMask.dwAntMask = 0x03030303;					 // new =		 a0=0x0001, b0=0x0100, c0=0x010000, d0=01000000,
											 		 // 0x03030303;; a1=0x0002, b1=0x0200, c1=0x020000, d1=02000000
	memset (tagMask.cActBitMsk, 0xff, len);			 // assign active bit mask...this permits strange filtering patterns

	tagMask.nMemBank = RFID_GEN2_MEMORY_BANK_EPC;	// EPC memory bank filter support ONLY!

	tagMask.nBitLenEPCID = 0;						// not used

	rtn	= ConfigureReadLoop( 1, 1);

	rtn = RFID_SetTagMaskEX(hReader, &tagMask);		// set the filter mask
	
	char szTagID[25];
	memset (szTagID,0,sizeof(szTagID));
	MakeHexString(tagMask.cTagMask, len, szTagID);
	printf("SS:Set mask %s \n",szTagID);


  
	return(rtn);
}


// erase mask after writing
DWORD xrReader::ClearSelect ()
{
	DWORD rtn;
//	BYTE  len;
	DWORD nItems;



	TAG_MASK tagMask1;
	memset(&tagMask1, 0, sizeof(tagMask1));
	rtn = RFID_SetTagMask(hReader, &tagMask1);
	printf("CS:Set  zero mask \n");

	
	TAG_MASK_EX tagMask;
	TAG_MASK_EX1_INIT(tagMask);
	rtn = RFID_SetTagMaskEX(hReader, &tagMask);
	printf("CS:Set  zero mask \n");


	for(int idx=0;idx<4;idx++){
		nItems = 1;		//set antenna for writing
		rtn		 = RFID_SetCapCurrValue(hReader, RFID_TAGCAP_G2_REMOVE_SELECT_RECORD_INDEX, &nItems, sizeof(idx), &idx);
		rtn = RFID_RemoveTagMaskEX(hReader);
		if(rtn == RFID_SUCCESS){
				printf("Erase mask %d\n",idx);
		}else{
			printf("Error erase mask %d\n",idx);
		}
			
	}

	return(rtn);
}





///////////////////////////////////////////////////// HOW TO LOCK //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Password Write...//...Permanent Write...// Description............................................................
//			0					 //					0						// 0-Region is writeable with or without an Access Password.
//			0					 //					1						// 1-Region is permanently writeable with or without an Access Password.
//			1					 //					0						// 2-Access Password required to write to region.
//			1					 //					1						// 3-Writing to region is permanently disabled.
// note...mask and action in the constant names can be interchanged in the code below, if done in entirety
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD xrReader::LockTagBank( BYTE type, BYTE bank, BYTE TagLen)
{
	DWORD rtn=0;
	DWORD items;

	WORD action, mask;

	BYTE a=true;

	action = mask = 0;


	if (bank & KILL)
	{
		switch (type)
		{
			case WRITE:							// 0-Region is writeable with or without an Access Password.
					
						action |= RFID_GEN2_LOCK_ACTION_NONE;																								break;
		
			case PERMWRT:						// 1-Region is permanently writeable with or without an Access Password.

						action |= RFID_GEN2_LOCK_ACTION_KILLPWD_PERM;																				break;

			case PWDWRT:						// 2-Access Password required to write to region.

						action |= RFID_GEN2_LOCK_ACTION_KILLPWD_PWD	;																				break;

			case NOWRITE:						// 3-Writing to region is permanently disabled.

						action |= RFID_GEN2_LOCK_ACTION_KILLPWD_PWD | RFID_GEN2_LOCK_ACTION_KILLPWD_PERM;		break;

		}//swtch

		mask |= RFID_GEN2_LOCK_MASK_KILLPWD_PWD | RFID_GEN2_LOCK_MASK_KILLPWD_PERM;
	}

	if (bank & ACCS)
	{
		switch (type)
		{
			case WRITE:							// 0-Region is writeable with or without an Access Password.
					
						action |= RFID_GEN2_LOCK_ACTION_NONE;																								break;
		
			case PERMWRT:						// 1-Region is permanently writeable with or without an Access Password.

						action |= RFID_GEN2_LOCK_ACTION_ACCESSPWD_PERM;																			break;

			case PWDWRT:						// 2-Access Password required to write to region.

						action |= RFID_GEN2_LOCK_MASK_ACCESSPWD_PWD;																				break;

			case NOWRITE:						// 3-Writing to region is permanently disabled.

						action |= RFID_GEN2_LOCK_MASK_ACCESSPWD_PWD + RFID_GEN2_LOCK_ACTION_ACCESSPWD_PERM;	break;

		}//swtch

		mask |= RFID_GEN2_LOCK_MASK_ACCESSPWD_PWD + RFID_GEN2_LOCK_MASK_ACCESSPWD_PERM;
	}

	if (bank & EPC)
	{
		switch (type)
		{
			case WRITE:							// 0-Region is writeable with or without an Access Password.
					
						action |= RFID_GEN2_LOCK_ACTION_NONE;
						//mask   |= RFID_GEN2_LOCK_MASK_NONE;
						break;
		
			case PERMWRT:						// 1-Region is permanently writeable with or without an Access Password.

						action |= RFID_GEN2_LOCK_ACTION_EPC_PERM;	
						//mask	 |=  RFID_GEN2_LOCK_MASK_EPC_PERM;
						break;

			case PWDWRT:						// 2-Access Password required to write to region.

						action |= RFID_GEN2_LOCK_ACTION_EPC_PWD;	
						//mask   |= RFID_GEN2_LOCK_MASK_EPC_PWD;
						break;

			case NOWRITE:						// 3-Writing to region is permanently disabled.

						action |= RFID_GEN2_LOCK_ACTION_EPC_PWD + RFID_GEN2_LOCK_ACTION_EPC_PERM;	
						//mask   |= RFID_GEN2_LOCK_MASK_EPC_PWD + RFID_GEN2_LOCK_MASK_EPC_PERM;
						break;

		}//swtch

		mask |= RFID_GEN2_LOCK_MASK_EPC_PWD + RFID_GEN2_LOCK_MASK_EPC_PERM;
	}


	if (bank & TID)
	{
		switch (type)
		{
			case WRITE:							// 0-Region is writeable with or without an Access Password.
					
						action |= RFID_GEN2_LOCK_ACTION_NONE;																								break;
		
			case PERMWRT:						// 1-Region is permanently writeable with or without an Access Password.

						action |= RFID_GEN2_LOCK_ACTION_TID_PERM;																						break;

			case PWDWRT:						// 2-Access Password required to write to region.

						action |= RFID_GEN2_LOCK_ACTION_TID_PWD;																						break;

			case NOWRITE:						// 3-Writing to region is permanently disabled.

						action |= RFID_GEN2_LOCK_ACTION_TID_PWD + RFID_GEN2_LOCK_ACTION_TID_PERM;						break;

		}//swtch

		mask |= RFID_GEN2_LOCK_MASK_TID_PWD + RFID_GEN2_LOCK_MASK_TID_PERM;
	}


	if (bank & USER)
	{
		switch (type)
		{
			case WRITE:							// 0-Region is writeable with or without an Access Password.
					
						action |= RFID_GEN2_LOCK_ACTION_NONE;																								break;
		
			case PERMWRT:						// 1-Region is permanently writeable with or without an Access Password.

						action |= RFID_GEN2_LOCK_ACTION_USER_PERM;																					break;

			case PWDWRT:						// 2-Access Password required to write to region.

						action |= RFID_GEN2_LOCK_ACTION_USER_PWD;																						break;

			case NOWRITE:						// 3-Writing to region is permanently disabled.

						action |= RFID_GEN2_LOCK_ACTION_USER_PWD + RFID_GEN2_LOCK_ACTION_USER_PERM;					break;

		}//swtch

		mask |= RFID_GEN2_LOCK_MASK_USER_PWD + RFID_GEN2_LOCK_MASK_USER_PERM;
	}


	items = 1;
	type	= RFID_TAG_TYPE_EPC_CLASSG2;							// set tag type to be processed
	rtn		= RFID_SetCapCurrValue(hReader, RFID_WRITECAP_TAGTYPE, &items, sizeof(type), &type);

	items	= 1;																		//set mask and mask that you wish to perform
	rtn		= RFID_SetCapCurrValue(hReader, RFID_WRITECAP_G2_LOCK_MASK, &items, sizeof(mask), &mask);

	items	= 1;																		//set action and mask that you wish to perform
	rtn		= RFID_SetCapCurrValue(hReader, RFID_WRITECAP_G2_LOCK_ACTION, &items, sizeof(action), &action);

	rtn = RFID_LockTag(hReader, TagLen, 3, 3, 0);

	return (rtn);
};






BOOL xrReader::FindAndOpenReader( const TCHAR *pszIPAddress, DWORD dwPort)
{

	BOOL bSuccess = FALSE;
	DWORD rtn;

	rtn = RFID_Close(&hReader);

	rtn = RFID_Open(&hReader);

	if(rtn == RFID_SUCCESS)
	{

		if(ConfigureTCPIP(pszIPAddress, dwPort))

		{
			rtn = RFID_OpenReader(hReader, 0);

			if(rtn == RFID_SUCCESS)
			{
			
				bSuccess = TRUE;
			};
			
		};
		
	};
	
	return(bSuccess);
};

void xrReader::DisplayCapabilities()
{
	DWORD *pCaps;
	DWORD dwNumCaps;
	CAPINFO CapInfo;
	// ask for the number of capabilities supported
	if(RFID_GetCapList(hReader, 0, &dwNumCaps) == RFID_SUCCESS)
	{
		// allocate a buffer for them
		pCaps = (DWORD *)malloc(dwNumCaps * sizeof(DWORD));
		if(pCaps)
		{
			// now we know how many there are, and we have a valid buffer to hold them
			// Ask for the cap list
			if(RFID_GetCapList(hReader, pCaps, &dwNumCaps) == RFID_SUCCESS)
			{
				DWORD dwCount;
				// now go through the list, and print the human readable name
				for(dwCount = 0; dwCount < dwNumCaps; dwCount++)
				{
					if(RFID_GetCapInfo(hReader, *(pCaps + dwCount), &CapInfo) == RFID_SUCCESS)
					{
						TCHAR szBuffer[512];
						swprintf(szBuffer, TEXT("Cap[%d] Name: %s\n"), dwCount, (CapInfo.tszName));
						wprintf(szBuffer);
						//TRACE(szBuffer);
						//printf("Cap[%d] Name: %s\n", dwCount, (CapInfo.tszName));
						
						try{
							DWORD numItems=128;
							DWORD bufSize;
							DWORD buf[128];
							DWORD ii;
							bufSize=sizeof(buf);
							memset(buf,0,bufSize);
							RFID_GetCapCurrValue(hReader, *(pCaps + dwCount) , &numItems, bufSize, buf);

							swprintf(szBuffer, TEXT("%ld: "),numItems);
							wprintf(szBuffer);
							for(ii=0;ii<numItems;ii++){
								swprintf(szBuffer, TEXT("%04X "), buf[ii]);
								wprintf(szBuffer);
							}
							
							wprintf(TEXT("\r\n"));
						}catch(...){

						}

					};
				};
			};
			free(pCaps);
		};
	};
};

xrReader::xrReader(const TCHAR* IPAddr, int IPPort, xrDevice *xDev){
		ReaderIP = IPAddr;
		ReaderPort = IPPort;
		dwTotalTags =0;

    Dev = xDev;

	bool bSuccess;

	bSuccess = FindAndOpenReader( ReaderIP , ReaderPort);

	if (!bSuccess)
	{
		//printf("Failed to open reader #%d\n", i);
	} 
	else
	{

		wsprintf(tszReadTagEventName,TEXT("%s"),TEXT("ReadTagEvent"));
		hReadTagEvent = CreateEvent(NULL, TRUE, FALSE, tszReadTagEventName);
		hGPIOEvent = CreateEvent(NULL, TRUE, FALSE, 0);
		hStopThreads	= CreateEvent(NULL, TRUE, FALSE, NULL);

		ConfigureReader();
		SetCombinedAntenna();
		SetAirProtocolGen2Only();
		ConfigureReadLoop(2,1);
		ConfigureOnDemandMode();
	}

	DisplayCapabilities();

	ResetEvent(hReadTagEvent);

	//hReadThread = CreateThread(NULL, 0, (unsigned long (__cdecl *)(void *))xrReader::ReadTagEventThread, (LPVOID)(this) ,0, (unsigned long *)0);

	
};

xrReader::~xrReader(){
		ConfigureOnDemandMode();
		SetEvent(hStopThreads);
		//WaitForMultipleObjects(1, &hReadThread, FALSE, INFINITE); 
		//CloseHandle(hReadThread);

		DWORD rtn;
		printf("Try to close reader\n");
		rtn = RFID_CloseReader(hReader);
		printf("Try to close reader status #%d ...\n", rtn);

		printf("Try to close handle for reader\n");
		rtn = RFID_Close(&hReader);
		printf("Try to close handle status #%d ...\n", rtn);

		printf("Try to close hReadTagEvent handle for reader\n");
		CloseHandle(hReadTagEvent);

		printf("Try to close hGPIOEvent handle for reader\n");
		CloseHandle(hGPIOEvent);

		CloseHandle(hStopThreads);
}

/*

DWORD WINAPI OnDemandThread(LPVOID pvarg)
{
	DWORD dwTotalReads = 0;
	// wait for api to signal that there are tags
	// the API will keep event signalled as long as tags are available...
	while(WaitForSingleObject(hTagEvent, INFINITE) == WAIT_OBJECT_0)
	{
	// we read another tag...
	// By default, we only get these events when a new tag is added to the TagList.
	// When RFID_ReadTagInventory returns, dwTotalReads will be the same as TagList.dwNewTags.
	dwTotalReads++;
	// We can see tag, and process it now...
	// On demand mode requires we reset the event...
	// this way, API knows when to overwrite the tag structure we provided.
		ResetEvent(hTagEvent);
	// On Demand mode does not allow us to call tag read functions on tag event.
	// Main thread will get entire list when call completes
	};
	return(0);
};
void OnDemandEventTest(void)
{
	TCHAR *szReadEvent = L"MyReadTagEvent";
	DWORD dwNumItems;
	DWORD dwSize;
	HANDLE hThread;
	TYPE_TAG *pTag;
	// create a named event. Use this event for listening to tag read events from api
	hTagEvent = CreateEvent(NULL, TRUE, FALSE, szReadEvent);
	if(hTagEvent)
	//RFID API Capabilities A-15
	{
		// Register for tag read events using a named event
		// include the null character
		dwNumItems = wcslen(szReadEvent) + 1;
		dwSize = dwNumItems * sizeof(szReadEvent[0]);
		RFID_SetCapCurrValue(hReader, RFID_READCAP_EVENTNAME, &dwNumItems, dwSize, szReadEvent);

		// Give the API a place to store tag
		dwNumItems = 1;
		dwSize = sizeof(&Tag);// Size of pointer to a tag
		// pass the address of the tag pointer. The API will use the tag pointer to store read tags
		pTag = &Tag;
		RFID_SetCapCurrValue(hReader, RFID_READCAP_EVENTTAGPTR, &dwNumItems, dwSize, &pTag);

		// create the listening thread
		hThread = CreateThread(NULL, 64*1024, OnDemandModeTagEventThread, (LPVOID)hReader, 0, NULL);
		if(hThread)
		{
			// start the read using OnDemand Mode.
			// This call will block until the read completes it's inventory
			if(RFID_ReadTagInventory(hReader, &TagList, TRUE) == RFID_SUCCESS)
			{
			PrintTagList();
			};
		};
	};
};
*/