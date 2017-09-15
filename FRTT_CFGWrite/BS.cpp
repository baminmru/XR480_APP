#include "frtt.h"



char xrByteStream::GPIO;

// converts a unsigned char  array into a hex null terminated string
 void xrByteStream::MakeHexString(unsigned char *pBytes, int nBytes, char *pOutBuf)
{
	int i;
	char *psz = pOutBuf;

	for(i = 0; i < nBytes; i++)
	{
		psz += sprintf(psz, "%2.2X", (int)*pBytes);
		pBytes++;
	};
};


 int xrByteStream::MyMakeHexBuffer(char *pBytes, int nBytes, unsigned char *pOutBuf)
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
};

int xrByteStream::HexifyNib(char *str, unsigned char *hex)		// returns nibbles
{
	unsigned char cnt = 0;
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
};


// формирует шестнадцатиричное значение маски из строки  
int xrByteStream::Hexify(char *str, unsigned char *hex)		// returns bytes
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

	if (strlen(str) % 2)										// if odd nibbles in string pickup odd nibb and pad zero into Least sig nibble in unsigned char 
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





unsigned short xrByteStream::crcTable[256] =
        {
        0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
        0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
        0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
        0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
        0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
        0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
        0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
        0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
        0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
        0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
        0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
        0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
        0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
        0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
        0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
        0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
        0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
        0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
        0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
        0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
        0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
        0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
        0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
        0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
        0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
        0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
        0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
        0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
        0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
        0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
        0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
        0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
        };


unsigned short xrByteStream::calcBlockCRC(long count, unsigned short crc, unsigned char* buffer)
{
    long PBuf=0;


    while (count-- > 0)
    {
        crc = (unsigned short)((crc >> 8) ^ crcTable[(unsigned char)(crc ^ buffer[PBuf])]);
        PBuf++;
    }

    return (unsigned short)(crc ^ ((ushort)0xFFFF));
}


xrByteStream::xrByteStream(xrDevice * Device,char * ip, unsigned short port, char * user, char* password) : CCESocket()
{
   	myDevice =Device;
	SetupString(ip,IP);
	SetupString(user,User);
	SetupString(password,Password);
	Port=port;
	Loged=false;
	
}



xrByteStream::~xrByteStream()
{
	ifdela(IP);
	ifdela(User);
	ifdela(Password);
}

bool xrByteStream::Reconect(){

	if(GetSocketState() == CCESocket::CONNECTED){

		// try logout
		if(Loged){
			Logout();
			Loged = false;
		}
		printf("Disconnect from ByteStream server \r\n");
		Disconnect();
	}

	while(m_readThreadState == RUNNING){
		Sleep(1);
		printf("w");
	};
    printf("\r\n");
	Loged = false;

	bool result =Create(SOCK_STREAM);
	printf("Create socket to  ByteStream server: %d \r\n",result);

	//if(result){
	
	int connectpass=600;
	bool Connected = false;
	while (!Connected  && connectpass){
		printf("Try to connect to  ByteStream server countdown=%d \r\n",connectpass);
		connectpass--;
		Connected=Connect(IP,Port);
		Sleep(100);
	}
	if(Connected){
		printf("BS Connected, try to login\r\n");
		if(Login(User,Password)){
			Loged=true;
			printf("Logon OK\r\n");
			return true;
		}
		printf("Logon Error\r\n");
		return false;
	}
	else
	{
		printf("BS Connect error %d\r\n",GetLastError());
		return false;
	}


}
bool xrByteStream::OnAccept(SOCKET serviceSocket)
{
       return TRUE;
}

void xrByteStream::OnReceive()
{

}


bool xrByteStream::OnReceive(char * buf, int  len)
{
	return FALSE;
}

void xrByteStream::OnClose(int closeEvent)
{

}




///////////////////// Commands ///////////////////////////////


 //Login Command (01hex)
 bool xrByteStream::Login(char * UserName, char * Password)
        {
            unsigned char * buf;
            char * res;
            unsigned short crc;
            unsigned short  cnt=4;
            buf = new unsigned char [8 + strlen(UserName)+strlen(Password)];
			buf[0]=0x01;
            buf[1] = 0x04;
            buf[2] = 0;
            buf[3] = 0x01;
            buf[4] = strlen(UserName);
			if(strlen(UserName)>0){
				cnt=cnt+strlen(UserName)+1;
				memcpy(buf+5,UserName,strlen(UserName));
				
			}else{
				cnt=4+1;
			}
			buf[cnt] = strlen(Password);
			if(strlen(Password)){
				memcpy(buf+cnt+1,Password,strlen(Password));
				cnt=cnt+strlen(Password);
			}

			buf[cnt+1] = 0x0;
            buf[cnt+2] = 0x0;
			buf[2] = cnt+2;
            
			crc=0Xbeef;
            crc = calcBlockCRC(cnt, crc, buf+1);
            buf[6+strlen(UserName)+strlen(Password)] = (unsigned char )(crc & 0xFF);
            buf[7+strlen(UserName)+strlen(Password)] = (unsigned char )((crc & 0xFF00)>>8);

            /*for (int i = 0; i <= 7+strlen(UserName)+strlen(Password); i++)
            {
               printf("%d=%x ",i, (unsigned long)buf[i]);
            }
           printf("\r\n");
			*/
			int l=0;
		   int wcnt=1000;

		   this->DropData();

           printf("Sending login packet\r\n");
		   l=this->Send((const  char*)buf,8 + strlen(UserName)+strlen(Password));
           printf("Sent %d  bytes\r\n",l);
		   delete buf;

/*
unsigned char  
Order Value Description
0 0x01 SOF, Start of Frame
1 0x04 RS485 network node address (0x00 ~ 0x1F)
2 0x11 Packet length excludes SOF, but includes the CRC
3 0x01 Login command
4 0x05 User name length (n)
5 0x61 'a' - 1st character of user name
6 0x64 'd' - 2nd ``
7 0x6D 'm'- 3rd ``
8 0x69 'i' - 4th ``
9 0x6E 'n' - nth ``
10 0x05 Password length (m)
11 0x61 'a' - 1st character of user name
12 0x64 'd' - 2nd ``
13 0x6D 'm'- 3rd ``
14 0x69 'i' - 4th ``
15 0x6E 'n' - nth ``
16 CRC LSB of the CRC-16
17 CRC MSB of the CRC-16
*/
           
           printf("Receive packets\r\n");
		   
         
		   while(wcnt && (this->GetDataSize()==0)){
			   Sleep(10);
			   wcnt--;
			   printf(".");
		   }
		   printf("\r\n");
		   this->GetPacket(res,&l);
           printf("Receive size=%d\r\n",  l);
		   
			/*for ( i = 0; i < l; i++)
			{
				printf("%d=%x ",i, (unsigned long)(res[i]));
			}
           printf("\r\n");
            */

/*
0 0x01 SOF, Start of Frame
1 > RS485 network node address (0x00 ~ 0x1F)
2 6 Packet length excludes SOF, but includes the CRC
3 0x01 Mirror of command
4 0x00 Status: complete
5 CRC LSB of the CRC-16
6 CRC MSB of the CRC-16
 */
           bool OK;
		   if(l==7){
		 	 if(res[4]==0) {
				OK= true;
				Loged=true;
			 }else{
				OK = false;
			 }
		   }else{
			OK = false;
		   }

		   if(res)
			delete res;
		   return OK;

}

        //Logout Command (02hex)
bool xrByteStream::Logout()
{


    /*
    0 0x01 SOF, Start of Frame
    1 0x04 RS485 network node address (0x00 ~ 0x1F)
    2 0x05 Packet length excludes SOF, but includes the CRC
    3 0x02 Logoutcommand
    4 CRC LSB of the CRC-16
    5 CRC MSB of the CRC-16
   */

			unsigned char * buf;
            char * res;
            unsigned short crc;
            unsigned short  cnt=3;
            buf = new unsigned char [6];
			buf[0]=0x01;
            buf[1] = 0x04;
            buf[2] = 0x05;
            buf[3] = 0x02;
            buf[cnt+1] = 0x0;
            buf[cnt+2] = 0x0;
			buf[2] = cnt+2;
            
			crc=0Xbeef;
            crc = calcBlockCRC(cnt, crc, buf+1);
            buf[cnt+1] = (unsigned char )(crc & 0xFF);
            buf[cnt+2] = (unsigned char )((crc & 0xFF00)>>8);

		    int l=0;
		    int wcnt=1000;
			
		    this->DropData();
		    printf("Sending logout packet\r\n");
		    l=this->Send((const  char*)buf,cnt+3);
		    printf("Sent %d  bytes\r\n",l);
			delete buf;



    /*
    0 0x01 SOF, Start of Frame
    1 > RS485 network node address (0x00 ~ 0x1F)
    2 6 Packet length excludes SOF, but includes the CRC
    3 0x02 Mirror of command
    4 0x00 Status: complete
    5 CRC LSB of the CRC-16
    6 CRC MSB of the CRC-16


     */
		   while(wcnt && (this->GetDataSize()==0)){
			   Sleep(10);
			   wcnt--;
			   printf(".");
		   }
		   printf("\r\n");
		   this->GetPacket(res,&l);
           printf("Receive size=%d\r\n",  l);

		   bool OK;
		   if(l==7){
		 	 if(res[4]==0) {
				OK= true;
			 }else{
				OK = false;
			 }
		   }else{
			OK = false;
		   }

		   if(res)
			delete res;
		   return OK;

  }

        //Get Reader Status Command (14hex)
        //Read Full Field Command (22hex)  1 antena tags read

        //Set Light On/Off Command (41hex)
        /*
        0 0x01 SOF, Start of Frame Request packet header
        1 0x04 RS485 network node address (0x00 ~ 0x1F)
        2 0x07 Packet length excludes SOF, but includes the CRC
        3 0x41 Command
        4 0x00~0x3F Selection mask for the 6 lights.
            Bit order Name Value
            0 Right Red 1-Selected/0-Not
            1 Right Yellow 1-Selected/0-Not
            2 Right Green 1-Selected/0-Not
            3 Left Red 1-Selected/0-Not
            4 Left Yellow 1-Selected/0-Not
            5 Left Green 1-Selected/0-Not
        5 0x00~0x3F Specify on/off for the selected lights. This field has the same bit order definition as above field.
        Only selected lights are set to the on/off, bit value for non-selected lights are ignored.
            Bit order Name Value
            0 Right Red 1-On/0-Off
            1 Right Yellow 1-On/0-Off
            2 Right Green 1-On/0-Off
            3 Left Red 1-On/0-Off
            4 Left Yellow 1-On/0-Off
            5 Left Green 1-On/0-Off
        6 CRC LSB of the CRC-16
        CRC
        7 CRC MSB of the CRC-16
         */

        /*
        0 0x01 SOF, Start of Frame Response packet header
        1 0x04 RS485 network node address
        2 0x06 Packet length except SOF
        3 0x41 Mirror of the Command
        4 0x00 Status: complete, =0x40 if parameter has not been set
        5 CRC LSB of the CRC-16
        CRC
        6 CRC MSB of the CRC-16
         */







bool xrByteStream::SetGPIOEX(char Mask, char Value){
	/*
	
0 0x01 SOF, Start of Frame
Request packet header
1 0x04 RS485 network node address (0x00 ~ 0x1F)
2 0x07 Packet length excludes SOF, but includes the CRC
3 0x41 Command
4 0x00~0x3F Selection mask for the 6 lights.
	Bit order Name Value
	0 Right Red 1-Selected/0-Not
	1 Right Yellow 1-Selected/0-Not
	2 Right Green 1-Selected/0-Not
	3 Left Red 1-Selected/0-Not
	4 Left Yellow 1-Selected/0-Not
	5 Left Green 1-Selected/0-Not
5 0x00~0x3F Specify on/off for the selected lights. This field has the same bit order definition as above field.
	Only selected lights are set to the on/off, bit value for non-selected lights are ignored.
	Bit order Name Value
	0 Right Red 1-On/0-Off
	1 Right Yellow 1-On/0-Off
	2 Right Green 1-On/0-Off
	3 Left Red 1-On/0-Off
	4 Left Yellow 1-On/0-Off
	5 Left Green 1-On/0-Off
6 CRC LSB of the CRC-16 CRC
7 CRC MSB of the CRC-16
	*/

	unsigned char * buf;
    char * res;
    unsigned short crc;
    unsigned short  cnt=5;
    buf = new unsigned char [8];
	buf[0]=0x01;
    buf[1] = 0x04;
    buf[2] = 0x07;
    buf[3] = 0x41;
	buf[4]=Mask;
	buf[5]=Value;
	buf[cnt+1] = 0x0;
    buf[cnt+2] = 0x0;
	buf[2] = cnt+2;
    
	crc=0Xbeef;
    crc = calcBlockCRC(cnt, crc, buf+1);
    buf[cnt+1] = (unsigned char )(crc & 0xFF);
    buf[cnt+2] = (unsigned char )((crc & 0xFF00)>>8);

	int l=0;
	int wcnt=1000;
	printf("Sending GPIO packet\r\n");
	this->DropData();
	l=this->Send((const  char*)buf,cnt+3);
	printf("Sent %d  bytes\r\n",l);
	delete buf;



    /*
    0 0x01 SOF, Start of Frame
    1 > RS485 network node address (0x00 ~ 0x1F)
    2 6 Packet length excludes SOF, but includes the CRC
    3 03A Mirror of command
    4 0x00 Status: complete
    5 CRC LSB of the CRC-16
    6 CRC MSB of the CRC-16


     */
		   while(wcnt && (this->GetDataSize()==0)){
			   Sleep(10);
			   wcnt--;
			   printf(".");
		   }
		   printf("\r\n");
		   this->GetPacket(res,&l);
           printf("Receive size=%d\r\n",  l);

		   bool OK;
		   if(l==7){
		 	 if(res[4]==0) {
				OK= true;
			 }else{
				OK = false;
			 }
		   }else{
			OK = false;
		   }

		   if(res)
			delete res;
		   return OK;


}




bool xrByteStream::SetGPIO( char Mask, char Value){

	/*
	0 0x01 SOF, Start of Frame
1 0x04 RS485 network node address
2 0x0B Packet length excludes SOF, but includes the CRC
3 0x3A Command
4 0x58 Light selection mask, its bits determines which light(s) are affected by this command. Set the bit value to "1" to select the light. All
reserved bits should always set as "0".
	Bit order Definition
	0 Reserved
	1 Right side red light
	2 Reserved
	3 Right side green light
	4 Left side red light
	5 Reserved
	6 Left side green light
	7 Reserved
5 0x48 Light action mask, with the same bit definition as the above Light selection mask. Set the bit value to "1" to turn it on and "0" to turn
it off. All reserved bits should always set as "0".
6 0x00 Two optional time out fields.
Х For each bit set in the above action mask, there is a corresponding time out field which is two byte long, with MSB first.
Х The first time out field corresponds to the least significant bit that is set to "1" in the action mask.
Х The number in this field indicates how long the light will stay on, in units of one hundred milliseconds.
7 0x00
8 0x00
9 0x0A
10 CRC LSB of the CRC-16
11 CRC MSB of the CRC-16
	*/

			unsigned char * buf;
            char * res;
            unsigned short crc;
            unsigned short  cnt=9;
            buf = new unsigned char [12];
			buf[0]=0x01;
            buf[1] = 0x04;
            buf[2] = 0x0B;
            buf[3] = 0x3A;
			buf[4]=Mask;
			buf[5]=Value;
			buf[6]=0;
			buf[7]=0;
			buf[8]=0;
			buf[9]=0;
            buf[cnt+1] = 0x0;
            buf[cnt+2] = 0x0;
			//buf[2] = cnt+2;
            
			crc=0Xbeef;
            crc = calcBlockCRC(cnt, crc, buf+1);
            buf[cnt+1] = (unsigned char )(crc & 0xFF);
            buf[cnt+2] = (unsigned char )((crc & 0xFF00)>>8);

		    int l=0;
		    int wcnt=1000;
		    printf("Sending GPIO packet\r\n");
			this->DropData();
		    l=this->Send((const  char*)buf,cnt+3);
		    printf("Sent %d  bytes\r\n",l);
			delete buf;



    /*
    0 0x01 SOF, Start of Frame
    1 > RS485 network node address (0x00 ~ 0x1F)
    2 6 Packet length excludes SOF, but includes the CRC
    3 03A Mirror of command
    4 0x00 Status: complete
    5 CRC LSB of the CRC-16
    6 CRC MSB of the CRC-16


     */
		   while(wcnt && (this->GetDataSize()==0)){
			   Sleep(10);
			   wcnt--;
			   printf(".");
		   }
		   printf("\r\n");
		   this->GetPacket(res,&l);
           printf("Receive size=%d\r\n",  l);

		   bool OK;
		   if(l==7){
		 	 if(res[4]==0) {
				OK= true;
			 }else{
				OK = false;
			 }
		   }else{
			OK = false;
		   }

		   if(res)
			delete res;
		   return OK;

};


TagInfo * xrByteStream::TagBuffer;
int xrByteStream::TagBufferSize;

int xrByteStream::GetTagCount(int Antenna){

	int aCnt=0;

	TagBufferSize=0;
	if( TagBuffer){
		delete TagBuffer;
		TagBuffer=NULL;
	}

	unsigned char * buf;
    char * res;
    unsigned short crc;
    unsigned short  cnt=4;
    buf = new unsigned char [7];
	buf[0]=0x01;
    buf[1] = 0x04;
    buf[2] = 0x06;
    buf[3] = 0x22;

    buf[cnt+1] = 0x0;
    buf[cnt+2] = 0x0;
	//buf[2] = cnt+2;

	switch (Antenna){ 
	case 1:
		buf[4]=0xA0;
		break;
	case 2:
		buf[4]=0xb0;
		break;
	case 3:
		buf[4]=0xc0;
		break;
	case 4:
		buf[4]=0xd0;
		break;
	case 5:
		buf[4]=0xA1;
		break;
	case 6:
		buf[4]=0xb1;
		break;
	case 7:
		buf[4]=0xc1;
		break;
	case 8:
		buf[4]=0xd1;
		break;
	default:
		buf[4]=0xA0;
		break;
	}

    
	crc=0Xbeef;
    crc = calcBlockCRC(cnt, crc, buf+1);
    buf[cnt+1] = (unsigned char )(crc & 0xFF);
    buf[cnt+2] = (unsigned char )((crc & 0xFF00)>>8);

	
	//printf("Sending GetTag packet\r\n");
	//l=this->Send((const  char*)buf,cnt+3);
	//printf("Sent %d  bytes\r\n",l);


	/*

0 0x01 SOF, Start of Frame
1 0x04 RS485 network node address (0x00 ~ 0x1F)
2 0x06 Packet length excludes SOF, but includes the CRC
3 0x22 Command
4 0xA0
Logical antenna indicator, defined as:
	0xA0: Antenna Port #1
	0xB0: Antenna Port #2
	0xC0: Antenna Port #3
	0xD0: Antenna Port #4
	0xA1: Antenna Port #5
	0xB1: Antenna Port #6
	0xC1: Antenna Port #7
	0xD1: Antenna Port #8
5 CRC LSB of the CRC-16
6 CRC MSB of the CRC-16

*/

		   //Sleep(200);
		   //8,21	
		   
		int l=0;
		int wcnt=1000;
		  while(true)
		  {
			   wcnt=10;
			   printf("Sending GetTag packet\r\n");
			   this->DropData();
			   l=this->Send((const  char*)buf,cnt+3);
			   printf("Sent %d  bytes\r\n",l);

			   while(wcnt && (this->GetDataSize()==0)){
				   Sleep(10);
				   wcnt--;
				   printf(".");
			   }

			   printf("\r\n");

			   this->GetPacket(res,&l);

			   printf("Receive size=%d\r\n",  l);

			   if (l==8){
				   delete res;
				   return TagBufferSize;
			   }

			   if (l==13){
				   printf("Return Tags=%d\r\n", TagBufferSize);
				   delete res;
				   return TagBufferSize;
			   }

			   if (l>13){
					
					aCnt = res[6];
					if (aCnt >0){
						if (TagBufferSize){
							TagInfo * Tmp = TagBuffer;
							TagBuffer = new TagInfo[TagBufferSize+aCnt];
							memcpy(TagBuffer,Tmp,sizeof(TagInfo)*TagBufferSize);
							delete Tmp;
						} else{
							TagBuffer = new TagInfo[aCnt];
						}
						for(int i=TagBufferSize;i<TagBufferSize+aCnt;i++){
							TagBuffer[i].Antenna=Antenna;
							MakeHexString( (unsigned char*)res +6+13*i , 12,  TagBuffer[i].TagID );
						}
						TagBufferSize+=aCnt;
					}
				}
			   	if(res) 
					delete res;
		  }

	delete buf;


	return TagBufferSize;
};

TagInfo * xrByteStream::GetTagInfo(int TagNum){

	if(TagBuffer!=NULL){
		if(TagBufferSize > TagNum){
			if( TagNum >=0){
				return TagBuffer + TagNum;
			}
		}
	}
	return NULL;
};



int xrByteStream::GetTagCount2(int Antenna){

	/*
	0 1 0x01 SOF, Start of Frame
	1 1 > RS485 network node address
	2 1 0x10 Packet length excludes SOF, but includes the CRC
	3 1 0x51 Command
	4 1 > Antenna port indicator:
		0xA0: Port #1
		0xB0: Port #2
		0xC0: Port #3
		0xD0: Port #4
		0xA1: Port #5
		0xB1: Port #6
		0xC1: Port #7
		0xD1: Port #8
	5 1 0x00 Reserved (set to "0")
	6 4 > Access Password (LSB first).
	10 1 > MB, Memory Bank:
		0: Reserved memory bank
		1: EPC
		2: TID
		3: User
		255 (0xFF): no payload, just read EPC ID

	11 2 > Start Word, LSB first Specifies the starting word address for the
	payload. Words are 16 bits in length.
	
	13 2 > WordCount, LSB first.
	Specifies the number of 16-bit words to be returned in the payload.
	15 1 CRC LSB of the CRC-16
	16 1 CRC MSB of the CRC-16
	*/



		int aCnt=0;

	TagBufferSize=0;
	if( TagBuffer){
		delete TagBuffer;
		TagBuffer=NULL;
	}

	unsigned char * buf;
    char * res;
    unsigned short crc;
    unsigned short  cnt=14;
    buf = new unsigned char [17];
	buf[0]=0x01;
    buf[1] = 0x04;
    buf[2] = 0x10;
    buf[3] = 0x51;
	buf[5]=0;
	// access
	buf[6]=0;
	buf[7]=0;
	buf[8]=0;
	buf[9]=0;

	//EPC ID ?
	buf[10]=0xFF;
	buf[11]=2;
	buf[12]=0;
	buf[13]=0;
	buf[14]=0;

    buf[cnt+1] = 0x0;
    buf[cnt+2] = 0x0;
	//buf[2] = cnt+2;

	switch (Antenna){ 
	case 1:
		buf[4]=0xA0;
		break;
	case 2:
		buf[4]=0xb0;
		break;
	case 3:
		buf[4]=0xc0;
		break;
	case 4:
		buf[4]=0xd0;
		break;
	case 5:
		buf[4]=0xA1;
		break;
	case 6:
		buf[4]=0xb1;
		break;
	case 7:
		buf[4]=0xc1;
		break;
	case 8:
		buf[4]=0xd1;
		break;
	default:
		buf[4]=0xA0;
		break;
	}

    
	crc=0Xbeef;
    crc = calcBlockCRC(cnt, crc, buf+1);
    buf[cnt+1] = (unsigned char )(crc & 0xFF);
    buf[cnt+2] = (unsigned char )((crc & 0xFF00)>>8);



	int l=0;
	int wcnt=1000;
	wcnt=10;
	   //printf("Sending GetTag packet\r\n");
		this->DropData();
	   l=this->Send((const  char*)buf,cnt+3);
	   //printf("Sent %d  bytes\r\n",l);
		delete buf;

		  while(true)
		  {
			   wcnt=10;
			   while(wcnt && (this->GetDataSize()==0)){
				   Sleep(2);
				   wcnt--;
			   }
			   
			   if (this->GetDataSize()>0){

				   this->GetPacket(res,&l);

				   //printf("Receive size=%d\r\n",  l);

				   if (l==8){
				   		for (int i = 0; i < l; i++)
						{
							printf("%d=%x ",i, (unsigned long)(res[i]));
						}
						printf("Error! l=8\r\n");
						delete res;
						return TagBufferSize;
				   }

				   if (l==11){
					   //printf("Return Tags=%d\r\n", TagBufferSize);
					   delete res;
					   return TagBufferSize;
				   }

				  
				   if (l>11){
					   
						
						aCnt = res[6];
						if (aCnt >0){
							if (TagBufferSize){
								TagInfo * Tmp = TagBuffer;
								TagBuffer = new TagInfo[TagBufferSize+aCnt];
								memcpy(TagBuffer,Tmp,sizeof(TagInfo)*TagBufferSize);
								delete Tmp;
							} else{
								TagBuffer = new TagInfo[aCnt];
							}
							for(int i=TagBufferSize;i<TagBufferSize+aCnt;i++){
								TagBuffer[i].Antenna=Antenna;
								unsigned char sw[12];
								for(int j=0;j<12;j++){
									sw[11-j]=*((unsigned char*)res +17+31*i+j);
								}
								MakeHexString( sw , 12,  TagBuffer[i].TagID );
								//printf("%d - %s\r\n",i,TagBuffer[i].TagID);
							}
							TagBufferSize+=aCnt;
						}
						
						/*
				   		for (int i = 0; i < l; i++)
						{
							printf("%d=%x ",i, (unsigned long)(res[i]));
						}
						 printf("\r\n");
						 */
						
					}
				   if(res)
					delete res;
			   }
				
		  }



	return TagBufferSize;

}



bool xrByteStream::WriteTag( char * TagID, char * NewTag, int Antenna){
	if(!_stricmp(TagID,NewTag)) {
		printf("WT:Tag already written %s\r\n", NewTag);
		return true;
	}
	
	/*

0 1 0x01 SOF, Start of Frame
1 1 > RS485 network node address
2 1 > Packet length excludes SOF, but includes the CRC
3 1 0x52 Command
4 1 > Antenna port indicator
	0xA0: Port #1
	0xB0: Port #2
	0xC0: Port #3
	0xD0: Port #4
	0xA1: Port #5
	0xB1: Port #6
	0xC1: Port #7
	0xD1: Port #8
5 1 0x00 Reserved (set to "0")
6 4 > Access Password (LSB first).
10 1 > MB, Memory Bank:
	00: Reserved
	01: EPC
	02: TID
	03: User
11 2 > Starting Word, LSB first.
Specifies the starting word address for the memory write. Words are 16 bits in length.
13 2 N Word Count, LSB first.
15 ~ 15 + 2*n-1 2*n > Data to be written (LSB first)
15+2*n 1 CRC LSB of the CRC-16
16+2*n 1 CRC MSB of the CRC-16
	*/

	SetFilter(TagID,Antenna);
	bool OK = false;
	unsigned char *  HEX = new unsigned char[12];

	Hexify(NewTag,HEX);

	unsigned char * buf;
    char * res;
    unsigned short crc;
    unsigned short  cnt=26;
    buf = new unsigned char [29];
	buf[0]=0x01;
    buf[1] = 0x04;
    buf[2] = 0;
    buf[3] = 0x52;


	switch (Antenna){ 
	case 1:
		buf[4]=0xA0;
		break;
	case 2:
		buf[4]=0xb0;
		break;
	case 3:
		buf[4]=0xc0;
		break;
	case 4:
		buf[4]=0xd0;
		break;
	case 5:
		buf[4]=0xA1;
		break;
	case 6:
		buf[4]=0xb1;
		break;
	case 7:
		buf[4]=0xc1;
		break;
	case 8:
		buf[4]=0xd1;
		break;
	default:
		buf[4]=0xA0;
		break;
	}

	buf[5]=0;
	// access
	buf[6]=0;
	buf[7]=0;
	buf[8]=0;
	buf[9]=0;

	//EPC ID ?
	buf[10]=0x01;

	buf[11]=2;
	buf[12]=0;

	buf[13]=0x06;
	buf[14]=0;

	for(int i=0;i<12;i++){
		buf[15+i]=HEX[11-i];
	}

    buf[cnt+1] = 0x0;
    buf[cnt+2] = 0x0;
	buf[2]=cnt+2;

	crc=0Xbeef;
    crc = calcBlockCRC(cnt, crc, buf+1);
    buf[cnt+1] = (unsigned char )(crc & 0xFF);
    buf[cnt+2] = (unsigned char )((crc & 0xFF00)>>8);

	// writing 
	int l=0;
	int wcnt;
	int passcnt=1;
	wcnt=10;

	while(!OK && passcnt>0){

	   passcnt--;
	   printf("Sending Write Tag packet\r\n");

	   this->DropData();
	   l=this->Send((const  char*)buf,cnt+3);
	   printf("Sent %d  bytes\r\n",l);

	   int getpasscnt;

	   getpasscnt=2;
	   do{
		   getpasscnt--;
		   l=0;
		   wcnt=10;
		   while(wcnt && (this->GetDataSize()==0)){
			   Sleep(20);
			   wcnt--;
		   }
	   
		   if (this->GetDataSize()>0)
		   {

			   this->GetPacket(res,&l);

				printf("Receive size=%d\r\n",  l);		
				/*
				for (int i = 0; i < l; i++)
				{
					printf("%d=%x ",i, (unsigned long)(res[i]));
				}
				printf("\r\n");
				*/
				if(l>29){
					unsigned char sw[12];
					char WTagID[25];
					for(int j=0;j<12;j++){
						sw[11-j]=*((unsigned char*)res +17+j);
					}
					MakeHexString( sw , 12,  WTagID );
					printf("Writen Tag %s\r\n",WTagID);
					
				}
				if(l==11){
					if(res[5]==1 && res[7]==0){
						OK = true;
						delete res;
						goto cleaning;
					}
				}
				delete res;
			}
		}while(getpasscnt && (l>29 || l==0));


	}
cleaning:

	delete HEX;
	delete buf;

	ClearFilter(Antenna);	
	return OK;
};


bool xrByteStream::SetFilter( char * TagID, int Antenna){

/*
	
	0 1 0x01 SOF, Start of Frame
	1 1 > RS485 network node address
	2 1 > Packet length excludes SOF, but includes the CRC
	3 1 0x50 Command
	4 4 >
		Antenna port mask
		Set Bits 0-31, corresponding to Antenna #1-8 to configure, clear to not configure, LSW first.
		Antenna Port #1 (0xA0): 0x00 00 00 01 (bit 0)
		Antenna Port #2 (0xB0): 0x00 00 01 00 (bit 8)
		Antenna Port #3 (0xC0): 0x00 01 00 00 (bit 16)
		Antenna Port #4 (0xD0): 0x01 00 00 00 (bit 24)
		Antenna Port #5 (0xA1): 0x00 00 00 02 (bit 1)
		Antenna Port #6 (0xB1): 0x00 00 02 00 (bit 9)
		Antenna Port #7 (0xC1): 0x00 02 00 00 (bit 17)
		Antenna Port #8 (0xD1): 0x02 00 00 00 (bit 25)
	8 1 >
		Memory bank:
		01: EPC
		DSP filtering on other mem banks not yet supported
	9 2 > Filter start bit
	11 2 n Filter length bits
	13 m=(n+7)/8 > Tag data: Bit vector (tag filter Pattern), MSB first If Filter length is zero then tag data field does not exist.
	13 + m m=(n+7)/8 >
	Tag mask: Bit vector, MSB first. The TagMask vector specifies target tag bits are to be considered in the filtering
	process. If bit in TagMask is "0" then bit of the target tag is a do not care (X). If Filter length is zero then tag mask
	field does not exist
	13 + (2*m) 1 CRC LSB of the CRC-16
	14 + (2*m) 1 CRC MSB of the CRC-16
	*/

	unsigned char HEX[12];

	Hexify(TagID,HEX);

	unsigned char * buf;
    char * res;
    unsigned short crc;
    unsigned short  cnt=36;
    buf = new unsigned char [38];
	buf[0] = 0x01;
    buf[1] = 0x04;
    buf[2] = 0;
    buf[3] = 0x50;


	switch (Antenna){ 
	case 4:
		buf[4]=0x0;
		buf[5]=0x0;
		buf[6]=0x0;
		buf[7]=0x1;
		break;
	case 3:
		buf[4]=0x0;
		buf[5]=0x0;
		buf[6]=0x1;
		buf[7]=0x0;
		break;
	case 2:
		buf[4]=0x0;
		buf[5]=0x1;
		buf[6]=0x0;
		buf[7]=0x0;
		break;
	case 1:
		buf[4]=0x1;
		buf[5]=0x0;
		buf[6]=0x0;
		buf[7]=0x0;
		break;
	case 8:
		buf[4]=0x0;
		buf[5]=0x0;
		buf[6]=0x0;
		buf[7]=0x2;
		break;
	case 7:
		buf[4]=0x0;
		buf[5]=0x0;
		buf[6]=0x2;
		buf[7]=0x0;
		break;
	case 6:
		buf[4]=0x0;
		buf[5]=0x2;
		buf[6]=0x0;
		buf[7]=0x0;
		break;
	case 5:
		buf[4]=0x2;
		buf[5]=0x0;
		buf[6]=0x0;
		buf[7]=0x0;
		break;
	default:
		buf[4]=0x3;
		buf[5]=0x3;
		buf[6]=0x3;
		buf[7]=0x3;
		break;
	}


	//EPC ID ?
	buf[8]=0x01;

	// satrt
	buf[9]=0x20;
	buf[10]=0x0;

	// len
	buf[11]=96;
	buf[12]=0;

	int i;
	for(i=0;i<12;i++){
		buf[13+i]=HEX[i];
	}

	for( i=0;i<12;i++){
		buf[25+i]=0xFF;
	}

    buf[cnt+1] = 0x0;
    buf[cnt+2] = 0x0;
	buf[2]=cnt+2;

	crc=0Xbeef;
    crc = calcBlockCRC(cnt, crc, buf+1);
    buf[cnt+1] = (unsigned char )( crc & 0xFF  );
    buf[cnt+2] = (unsigned char )((crc & 0xFF00)>>8);


	// set filter 
	int l=0;
	int wcnt;
	wcnt=10;
	printf("Sending Set Filter packet\r\n");
	this->DropData();
	l=this->Send((const  char*)buf,cnt+3);
	delete buf;

	/*
	for (i = 0; i < l; i++)
	{
		printf("%d=%x ",i, (unsigned long)(buf[i]));
	}
	*/
	printf("Sent %d  bytes\r\n",l);

	for(int ttt=0;ttt<3;ttt++){
	   wcnt=10;
	   while(wcnt && (this->GetDataSize()==0)){
		   Sleep(2);
		   wcnt--;
	   }
	   
	   if (this->GetDataSize()>0){

			   this->GetPacket(res,&l);

				printf("Receive size=%d\r\n",  l);		
				/*
				for (i = 0; i < l; i++)
				{
					printf("%d=%x ",i, (unsigned long)(res[i]));
				}
				printf("\r\n");
				*/
				delete res;
				break;
		}
	}


	return true;
}


bool xrByteStream::ClearFilter(  int Antenna){
	/*
	
	0 1 0x01 SOF, Start of Frame
	1 1 > RS485 network node address
	2 1 > Packet length excludes SOF, but includes the CRC
	3 1 0x50 Command
	4 4 >
		Antenna port mask
		Set Bits 0-31, corresponding to Antenna #1-8 to configure, clear to not configure, LSW first.
		Antenna Port #1 (0xA0): 0x00 00 00 01 (bit 0)
		Antenna Port #2 (0xB0): 0x00 00 01 00 (bit 8)
		Antenna Port #3 (0xC0): 0x00 01 00 00 (bit 16)
		Antenna Port #4 (0xD0): 0x01 00 00 00 (bit 24)
		Antenna Port #5 (0xA1): 0x00 00 00 02 (bit 1)
		Antenna Port #6 (0xB1): 0x00 00 02 00 (bit 9)
		Antenna Port #7 (0xC1): 0x00 02 00 00 (bit 17)
		Antenna Port #8 (0xD1): 0x02 00 00 00 (bit 25)
	8 1 >
		Memory bank:
		01: EPC
		DSP filtering on other mem banks not yet supported
	9 2 > Filter start bit
	11 2 n Filter length bits
	13 m=(n+7)/8 > Tag data: Bit vector (tag filter Pattern), MSB first If Filter length is zero then tag data field does not exist.
	13 + m m=(n+7)/8 >
	Tag mask: Bit vector, MSB first. The TagMask vector specifies target tag bits are to be considered in the filtering
	process. If bit in TagMask is "0" then bit of the target tag is a do not care (X). If Filter length is zero then tag mask
	field does not exist
	13 + (2*m) 1 CRC LSB of the CRC-16
	14 + (2*m) 1 CRC MSB of the CRC-16
	*/

	unsigned char * buf;
    char * res;
    unsigned short crc;
    unsigned short  cnt=12;
    buf = new unsigned char [15];
	buf[0]=0x01;
    buf[1] = 0x04;
    buf[2] = 0;
    buf[3] = 0x50;

	buf[4]=0x3;
	buf[5]=0x3;
	buf[6]=0x3;
	buf[7]=0x3;


	//EPC ID ?
	buf[8]=0x01;

	// satrt
	buf[9]=0x20;
	buf[10]=0x0;

	// len
	buf[11]=0;
	buf[12]=0;

	
    buf[cnt+1] = 0x0;
    buf[cnt+2] = 0x0;
	buf[2]=cnt+2;

	
    
	crc=0Xbeef;
    crc = calcBlockCRC(cnt, crc, buf+1);
    buf[cnt+1] = (unsigned char )(crc & 0xFF);
    buf[cnt+2] = (unsigned char )((crc & 0xFF00)>>8);



	// set filter 

	int l=0;
	int wcnt;
	wcnt=10;
   printf("Sending Clear Filter packet\r\n");
   this->DropData(); 
   l=this->Send((const  char*)buf,cnt+3);
   printf("Sent %d  bytes\r\n",l);
   delete buf;


   for(int ttt=0;ttt<3;ttt++){
	   wcnt=10;
	   while(wcnt && (this->GetDataSize()==0)){
		   Sleep(2);
		   wcnt--;
	   }
	   
	   if (this->GetDataSize()>0){

			   this->GetPacket(res,&l);

				printf("Receive size=%d\r\n",  l);		
				/*
				for (int i = 0; i < l; i++)
				{
					printf("%d=%x ",i, (unsigned long)(res[i]));
				}
				printf("\r\n");
				*/
				delete res;
				break;
		}
	}

	return true;
}