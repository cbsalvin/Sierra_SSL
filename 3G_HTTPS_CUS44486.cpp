// **************************************************************
// Scenario:	3G_HTTPS_CUS44486.Cpp
// ----------+--------+-------|-------------------------------------------------
//    Date   | Author | Ver   |	Modification 
// ----------+--------+-------|-------------------------------------------------
//  25.06.13 | ACONG  | V1.0  |	- Initial revision
// ----------+--------+-------|-------------------------------------------------

// char ScriptVersion[10]="" should have the same value as that present in the Ver column 
//  Please Update this value whenever a new version of file is generated 

#include <stdio.h>
/* !!! The complete path of the CScenario header is mandatory !!! */
#include "C:\ValidationLibrary\ValidationLibrary\CScenario.h"
#include "C:\ValidationLibrary\XModemProtocol\DownloaderXModem.h"
char CurTestCase[64];
int download=1;	
void UserPostamble(void)
{
	printf("\n--------------UserPostamble------------\n");
	pLogFile->WriteComment("\r\n--------------UserPostamble------------\r\n");
    
	/* Here some user actions before the scenario ending ..........*/
	char* ListRsp[4] = {"", NULL, NULL, NULL};
	
	//
	ListRsp[1] = "*\r\nOK\r\n";

	
	if (ManageAtCommand(pCom1,"AT+WOPEN=0\r",ListRsp,50000) != 1)
		DisplayAndLog(pLogFile,"------------ AT+WOPEN=0 has not succeeded -----------\n\n\r");
	Wait (5);
	if (ManageAtCommand(pCom1,"AT+WIND=0\r",ListRsp,10000) != 1)
		DisplayAndLog(pLogFile,"------------ AT+WIND=0 has not succeeded -----------\n\n\r");
	if (ManageAtCommand(pCom1,"AT+WOPEN=4\r",ListRsp,600000) != 1)
		DisplayAndLog(pLogFile,"------------ Cannot delete the application -----------\n\n\r");	
	Wait (5);
	ListRsp[1] = "\r\nOK\r\n";
	if (ManageAtCommand(pCom1,"AT&F;&W\r",ListRsp,3000) != 1)
		DisplayAndLog(pLogFile,"------------ At&f has not succeeded -----------\n\n\r");
	if (ManageAtCommand(pCom1,"AT+WIND=0;&W\r",ListRsp,10000) != 1)
		DisplayAndLog(pLogFile,"------------ AT+WIND=0 has not succeeded -----------\n\n\r");
	if (ManageAtCommand(pCom1,"AT+CFUN=1\r",ListRsp,50000) != 1)
		DisplayAndLog(pLogFile,"------------ AT+CFUN=1 has not succeeded -----------\n\n\r");
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
	int  i,j,k,n,result,result1, result2, result3;
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
	int wip_dbg = 1;
	int error=0;
	if (ManageAtCommand(pCom1,"ATE0;+WIND=0;+CREG=0;+CGREG=0;&W\r",ListRsp,30000) != 1)
		pLogFile->WriteComment("\n\nProblem in ATE command\n");
    Wait(5);
	if (ManageAtCommand(pCom1,"AT+CMEE=1;&W\r",ListRsp,30000) != 1)
		pLogFile->WriteComment("\n\nProblem with AT+CMEE command\n"); 
		
	//*********************************************************************************//
	//*************************GENERATING PATH & TEST NAME*****************************//
	//*********************************************************************************//
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
	char strInputFile2[512];
	char MemoryType[10];
	char FileExtension[30];

	sprintf(strInputFile,"%s",pSoftCfg->GetValue("Soft","ApplicationPath","%s"));
	printf("\nstrInputFile= %s\n",strInputFile);
	sprintf(MemoryType,"%s",pSoftCfg->GetValue("Soft","MemoryType","%s"));
	printf("\nMemoryType = %s\n",MemoryType);
	sprintf(FileExtension,"%s",pSoftCfg->GetValue("Soft","FileExtension","%s"));
	printf("\FileExtension = %s\n",FileExtension);

//	strcat(strInputFile,TestName);
	strcat(strInputFile,"3G_HTTPS_CUS44486");
//	strcat(strInputFile,MemoryType);
	strcat(strInputFile,FileExtension);

	printf("\nPATH == %s", strInputFile);
	
	// START DOWNLOAD
  download=DownloadApplication(strInputFile,1,50000,50000);
  


	 
	// RECOPY ARM AS TESTNAME BECAUSE IN TD ADS & ARM FILES ARE LINKED TO ARM 
	if (compiler==3)
    strcpy(TestName,"ARM");
	// ===================================//
    // OPEN AT SETTINGS
    // ===================================//
	// Start the pre-setting and lanuch the OpenAt application.
	ListRsp[1] = "*\r\nOK\r\n";
	Wait (2);
	
	ManageAtCommand(pCom1,NULL,ListRsp,5000);

	if (ManageAtCommand(pCom1,"AT+WOPEN=1\r",ListRsp,50000) != 1)
	{
        DisplayAndLogTest(pLogFile, "Run the application\n");
    }
	
	
	/* Pin input */
	Wait(5);
	puts("-----PIN INPUT-----");
	pLogFile->WriteComment("\n\n-----PIN INPUT----- \n\n");
	EnterPin(pCom1,(char*)pTestedSim->GetValue("Security","Pin1","%s"));
	Wait (10);
	pLogFile->WriteComment("\n\n----- WAITING FOR NETWORK REGISTRATION\n");
	i = 0;
	while(AtCreg(pCom1,AT_Interrogation,1) != true)
		{
		i += 1;
		if(i == 100)
			{
			pLogFile->WriteComment("\n\n----- Not attached to network\n");
			}
		}
		
	if (ManageAtCommand(pCom1,"AT\r",ListRsp,5000) != 1)
    {
        DisplayAndLogTest(pLogFile, "The module has problem!!\n");
    }
	Wait (10);
	// Trigger GPRS Bearer
	sprintf(text, "%s", pTestedSim->GetValue("gprs","APN","%s"));
	sprintf(temp, "at+init=\"%s\"\r", text);
    if (ManageAtCommand(pCom1,temp,ListRsp,5000) != 1)
    {
        DisplayAndLogTest(pLogFile, "GPRS AT+GPRS setting problem\n");
    }
	ListRsp[2] = "*Type at+get1/2/3/4/5/... to get LCL/amendes/bnp/cnav https page*";
	if (ManageAtCommand(pCom1,NULL,ListRsp,15000) != 2)
    {
        DisplayAndLogTest(pLogFile, "The application failed to process!!!\n");
	
    }
	memset(text,0x00,sizeof(text));
	memset(temp,0x00,sizeof(temp));
	Wait (5);
	if (ManageAtCommand(pCom1,"AT+CCLK=\"10/01/01,00:00:00\"\r",ListRsp,5000) != 1)
    {
        DisplayAndLogTest(pLogFile, "Cannot set the time for the module\n");
    }
    
//------------------------- Run Test O_3G_WIP_HTTPS_APP_GCC_0011------------------------------------------//
	result=1;
	ListRsp[3] = "*http_ClientTestDataHandler: Status=200*http_ClientTestDataHandler: Reason=\"OK\"*";
	if (ManageAtCommand(pCom1,"AT+GET1\r",ListRsp,4000) != 1 )
	{
		DisplayAndLogTest(pLogFile, "Cannot run TEST1!!!\n");
		result=0;
	}
	if (ManageAtCommand(pCom1,NULL,ListRsp,20000) != 3)
    {
        DisplayAndLogTest(pLogFile, "The test cannot have the correct answer!!!\n");
		result=0;
    }
	
	if (result == 0) pLogFile->WriteComment("\n\nStatus O_3G_WIP_HTTPS_APP_GCC_0011: FAILED\r");
	else pLogFile->WriteComment("\n\nStatus O_3G_WIP_HTTPS_APP_GCC_0011: PASSED\r");
	Wait (5);
//---------------------------------O_3G_WIP_HTTPS_APP_GCC_0011 (end)---------------------------------------//

//------------------------- Run Test O_3G_WIP_HTTPS_APP_GCC_0012------------------------------------------//
	result1=1;
	if (ManageAtCommand(pCom1,"AT+GET2\r",ListRsp,4000) != 1 )
	{
		DisplayAndLogTest(pLogFile, "Cannot run TEST2!!!!\n");
		result1=0;
	}
	if (ManageAtCommand(pCom1,NULL,ListRsp,20000) != 3)
    {
        DisplayAndLogTest(pLogFile, "The test cannot have the correct answer!!!\n");
		result1=0;
    }
	Wait (5);
	
	if (result1 == 0) pLogFile->WriteComment("\n\nStatus O_3G_WIP_HTTPS_APP_GCC_0012: FAILED\r");
	else pLogFile->WriteComment("\n\nStatus O_3G_WIP_HTTPS_APP_GCC_0012: PASSED\r");
//---------------------------------O_3G_WIP_HTTPS_APP_GCC_0012 (end)---------------------------------------//

//------------------------- Run Test O_3G_WIP_HTTPS_APP_GCC_0028------------------------------------------//
	result2=1;
	ListRsp[4] = "*http_ClientTestDataHandler: Done*";
	if (ManageAtCommand(pCom1,"AT+GET3\r",ListRsp,4000) != 1 )
	{
		DisplayAndLogTest(pLogFile, "Cannot run TEST3!!!!\n");
		result2=0;
	}
	if (ManageAtCommand(pCom1,NULL,ListRsp,50000) != 4)
    {
        DisplayAndLogTest(pLogFile, "The test cannot have the correct answer!!!\n");
		result2=0;
    }
	
	if (result2 == 0) pLogFile->WriteComment("\n\nStatus O_3G_WIP_HTTPS_APP_GCC_0028: FAILED\r");
	else pLogFile->WriteComment("\n\nStatus O_3G_WIP_HTTPS_APP_GCC_0028: PASSED\r");
//---------------------------------O_3G_WIP_HTTPS_APP_GCC_0028 (end)---------------------------------------//
	
	printf("\n	T H E 	E N D		\n");
	Scenario.SetVerdict(Passed);

}