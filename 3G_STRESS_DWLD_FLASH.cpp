#include <stdio.h>
/* !!! The complete path of the CScenario header is mandatory !!! */
#include "C:\ValidationLibrary\ValidationLibrary\CScenario.h"



int download=1;	
char* ListRsp[10] = {"", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
ListRsp[1] = "\r\nOK\r\n";
void UserPostamble(void)
{
	printf("\n--------------UserPostamble------------\n");
	pLogFile->WriteComment("\r\n--------------UserPostamble------------\r\n");
    
	/* Here some user actions before the scenario ending ..........*/
	char* ListRsp[4] = {"", NULL, NULL, NULL};
	
	//
	ListRsp[1] = "*\r\nOK\r\n";
	
	if (ManageAtCommand(pCom1,"AT+CFUN=1\r",ListRsp,50000) != 1)
		DisplayAndLog(pLogFile,"------------ AT+CFUN=1 has not succeeded -----------\n\n\r");
	
	if (ManageAtCommand(pCom1,"AT+WIND=0\r",ListRsp,10000) != 1)
		DisplayAndLog(pLogFile,"------------ AT+WIND=0 has not succeeded -----------\n\n\r");
	ListRsp[1] = "\r\nOK\r\n";
	if (ManageAtCommand(pCom1,"AT&F;&W\r",ListRsp,3000) != 1)
		DisplayAndLog(pLogFile,"------------ At&f has not succeeded -----------\n\n\r");
	if (ManageAtCommand(pCom1,"AT+WIND=0;&W\r",ListRsp,10000) != 1)
		DisplayAndLog(pLogFile,"------------ AT+WIND=0 has not succeeded -----------\n\n\r");
}    

void DisplayAndLog( CLog *pLog, char *String )
{
	printf( String );
	pLog->WriteComment( String );
}
void DisplayAndLogTest( CLog *pLog, char * String )
{
    char buffer[512];
    
	sprintf( buffer, "\r\n\n%s: %s", CurTestCase, String );
	printf( buffer );
	pLog->WriteComment( buffer );
}
bool OpenAT (void)
{
	Wait (5);
	ListRsp[1] = "\r\nOK\r\n";
	ListRsp[2] = "\r\n+WOPEN: 2,\"*\",\"*\"\r\n\r\nOK\r\n";
	if (ManageAtCommand(pCom1,"AT+WOPEN=2\r",ListRsp,5000) != 2)
	{
		DisplayAndLog(pLogFile,"------------ The application has not been downloaded to Module!! -----------\n\n\r");
		if (ManageAtCommand(pCom1,"AT+WOPEN=4\r",ListRsp,50000) != 1)
			{
			DisplayAndLog(pLogFile,"------------ Cannot Delete the OpenAT -----------\n\n\r");
			}
		return 0;
	}
	ListRsp[1] = "*\r\nOK\r\n";
	if (ManageAtCommand(pCom1,"AT+WOPEN=1\r",ListRsp,80000) != 1)
		DisplayAndLog(pLogFile,"------------ AT+WOPEN=1 has not succeeded -----------\n\n\r");
	Wait (2);
	ManageAtCommand(pCom1,NULL,ListRsp,5000);
	
	if (ManageAtCommand(pCom1,"AT+WOPEN=0\r",ListRsp,80000) != 1)
		DisplayAndLog(pLogFile,"------------ AT+WOPEN=0 has not succeeded -----------\n\n\r");
	Wait (2);
	ManageAtCommand(pCom1,NULL,ListRsp,5000);

	if (ManageAtCommand(pCom1,"AT+WOPEN=4\r",ListRsp,50000) != 1)
	{
		DisplayAndLog(pLogFile,"------------ Delete the OpenAT -----------\n\n\r");
		return 0;
	}
	if (ManageAtCommand(pCom1,"AT+WIND=0;&w\r",ListRsp,10000) != 1)
		DisplayAndLog(pLogFile,"------------ AT+WIND=0 has not succeeded -----------\n\n\r");
	return 1;

}
	
/*--------------------------*/
/* The scenario entry point */
/*--------------------------*/
void main(int argc,char** argv)
{
	char* ListRsp[8] = {"",NULL,NULL,NULL,NULL,NULL,NULL,NULL};
	char buff[1024] = {0};
	char buff2[512] = {0};
	char temp[512] = {0};
	char text[512] = {0};
	int  i,j,k,n,result;
	bool autobaud;

	printf("\n****   Test of phonebook group commands    ****\n\n");
	
	CScenario Scenario(argc,argv,UserPostamble);
	Scenario.SetVerdict(Failed);
	
	/* Get the Result of environnement checking, Mandatory !!!  */
	if(Scenario.GetResultEnvChecking() == false)
	{
		return ;
	}

	
	// To check the accessibility of all configuration files
	printf("Radio Feature in hard cfg       : %d\n",pHardCfg->GetValue("Module","RadioFeature","%d"));
	printf("Network ID in Network cfg       : %s\n",pNetCfg->GetValue("Network","ID","%s"));
	printf("Version in Software cfg         : %s\n",pSoftCfg->GetValue("Soft","Version","%s"));
	printf("VoiceNumber in Tested SIM cfg   : %s\n",pTestedSim->GetValue("Identification","VoiceNumber","%s"));
	printf("VoiceNumber in Tester SIM cfg   : %s\n",pTesterSim->GetValue("Identification","VoiceNumber","%s"));	
	printf("Speed in Tested Port cfg        : %d\n",pTestedPort->GetSpeed(autobaud));
	printf("Speed in Tester Port cfg        : %d\n",pTesterPort->GetSpeed(autobaud));
			
	ListRsp[1] = "\r\nOK\r\n";
	int error=0;
	if (ManageAtCommand(pCom1,"ATE0;+WIND=0;+CREG=0;+CGREG=0;&W\r",ListRsp,30000) != 1)
		pLogFile->WriteComment("\n\nProblem in ATE command\n");
    Wait(5);
	if (ManageAtCommand(pCom1,"AT+CMEE=1;&W\r",ListRsp,30000) != 1)
		pLogFile->WriteComment("\n\nProblem with AT+CMEE command\n"); 
	

	//***********************************************************************************************//
	//*************************GENERATING PATH & TEST NAME & DOWNLOAD FW*****************************//
	//***********************************************************************************************//
	char softversion[10];


  // 1=arm ; 2=gcc
	int compiler=2;
  // 1=embedded ; 2=Remote; 3=CMUX 
	int mode=1;  
	char TestName[5];

	compiler=pSoftCfg->GetValue("OAT_Default_values","Compiler","%d");
	mode=pSoftCfg->GetValue("OAT_Default_values","Mode","%d");

	if(compiler==1)
    strcpy(TestName,"ARM");
  else if (compiler==2)
    strcpy(TestName,"GCC");
  else if (compiler==3)
    strcpy(TestName,"ads");
  else
    strcpy(TestName,"XXX");     

	char strInputFile[512];
	char MemoryType[10];
	char FileExtension[30];

	//********************************************************************************************************//
	//*************************GENERATING PATH & TEST NAME & DOWNLOAD HELLO_WORLD*****************************//
	//********************************************************************************************************//
	sprintf(strInputFile,"%s",pSoftCfg->GetValue("Soft","ApplicationPath","%s"));
	printf("\nstrInputFile= %s\n",strInputFile);
	sprintf(MemoryType,"%s",pSoftCfg->GetValue("Soft","MemoryType","%s"));
	printf("\nMemoryType = %s\n",MemoryType);
	sprintf(FileExtension,"%s",pSoftCfg->GetValue("Soft","FileExtension","%s"));
	printf("\FileExtension = %s\n",FileExtension);

//	strcat(strInputFile,TestName);
	strcat(strInputFile,"Hello_World");
//	strcat(strInputFile,MemoryType);
	strcat(strInputFile,FileExtension);

	printf("\nPATH == %s", strInputFile);
	
	// START DOWNLOAD
	for (i=1;i<=100;i++)
	{
	printf ("\r\nThis is the loop %d th for OPEN AT application\r\n",i);
	download=DownloadApplication(strInputFile,1,80000,80000);
		if (download!=1)
		{
			Wait (3);
			DisplayAndLog(pLogFile,"\n\n\r--Download OPEN AT application FAILED----\n\n\r");
		return;
		}
	result = OpenAT ();
	if (result==0)
	{
		DisplayAndLog(pLogFile,"\n\n\r--Download OPEN AT application failed----\n\n\r");
		return;
	}
	
	}
	Wait (2);
	DisplayAndLog(pLogFile,"\n\n\r--Download OPEN AT application 100 times PASSED----\n\n\r");
	memset (strInputFile,0x00,sizeof(strInputFile));
	memset (MemoryType,0x00,sizeof(MemoryType));
	memset (FileExtension,0x00,sizeof(FileExtension));
	//***********************************************************************************************//
	//*************************GENERATING PATH & TEST NAME & DOWNLOAD QCT_FW*****************************//
	//***********************************************************************************************//


	sprintf(strInputFile,"%s",pSoftCfg->GetValue("Soft","ApplicationPath","%s"));
	printf("\nstrInputFile= %s\n",strInputFile);
	sprintf(MemoryType,"%s",pSoftCfg->GetValue("Soft","MemoryType","%s"));
	printf("\nMemoryType = %s\n",MemoryType);
	sprintf(FileExtension,"%s",pSoftCfg->GetValue("Soft","FileExtension","%s"));
	printf("\FileExtension = %s\n",FileExtension);

//	strcat(strInputFile,TestName);

//******************************************************************************************************//
//*************	Please modify the download application name here !!!!  **************************************//
	strcat(strInputFile,"SIERRA_BOOT_S4.1.0.8A_sl808x");
//******************************************************************************************************//


//	strcat(strInputFile,MemoryType);
	strcat(strInputFile,FileExtension);

	printf("\nPATH == %s", strInputFile);
	
	// START DOWNLOAD
	for (i=1;i<=100;i++)
	{
	printf ("\r\nThis is the loop %d th for QCT_FW\r\n",i);
	download=DownloadApplication(strInputFile,1,50000,50000);
		if (download!=1)
		{
			Wait (3);
			DisplayAndLog(pLogFile,"\n\n\r--Download QCT_FW FAILED----\n\n\r");
		return;
		}
	}
	Wait (2);
	DisplayAndLog(pLogFile,"\n\n\r--Download QCT_FW 100 times PASSED----\n\n\r");
	memset (strInputFile,0x00,sizeof(strInputFile));
	memset (MemoryType,0x00,sizeof(MemoryType));
	memset (FileExtension,0x00,sizeof(FileExtension));
	
	Wait (10);
	// Test end //
	DisplayAndLog(pLogFile,"\n\nStatus A_3G_MISC_0117: PASSED\n\n\r");
    Scenario.SetVerdict(Passed);
}