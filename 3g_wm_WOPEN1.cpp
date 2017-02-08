/******************************************************************/
/*Scenario:	wm_WOPEN1.cpp       		                  */
/* ------------------------    					  */
/*       							  */
/*   Check that open AT application activation 	*/
/*   does not change after AT+CFUN=1			  */
/*								  */
/*								  */
/*----------+--------+--------------------------------------------*/
/*   Date   | Author | Modification                               */
/*----------+--------+--------------------------------------------*/
/* 01.10.08 | MCO    | - File Creation                            */
/******************************************************************/

#include <stdio.h>
#include "C:\VALIDATIONLIBRARY\VALIDATIONLIBRARY\CScenario.h"
#include <iostream>
using namespace std;

#define WAIT_RESPONSE 	0     	// response not received
#define CMD_OK		1       // Response OK
#define NOT_ALLOWED 	2       // +CME ERROR: 3

char* ListRsp[16] = {"",NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
//Some AT predefined answers
ListRsp[CMD_OK] = "\r\nOK\r\n";
ListRsp[NOT_ALLOWED] = "\r\n+CME ERROR: 3\r\n";
char ScriptVersion[10]="V001";


void DisplayAndLog( CLog *pLog, char * String )  //Affiche et log une chaine de caracteres
{
	printf( String );
	pLog->WriteComment( String );
}


void UserPostamble(void) {

}


void main(int argc,char** argv)
{
	CScenario Scenario(argc,argv,UserPostamble);
	Scenario.SetVerdict(Failed);
	
	bool autobaud;
	int result=true;
	
	/* Get the Result of environnement checking, Mandatory !!!  */
	if(Scenario.GetResultEnvChecking() == false)
		return ;
		
//////////////////////////////////////////////////////////////////////////////////////////////////
//												//
//					OPEN AT SETTINGS					//
//												//
/////////////////////////////////////////////////////////////////////////////////////////////////	
	
	if (ManageAtCommand(pCom1,"ATE0;&w\r",ListRsp,3000) != 1)
		DisplayAndLog(pLogFile,"Problem in running the Open AT application");
	
	if (ManageAtCommand(pCom1,"AT+CMEE=1;&w\r",ListRsp,3000) != 1)
		DisplayAndLog(pLogFile,"Problem in running the Open AT application");

	if (ManageAtCommand(pCom1,"AT+WIND=0\r",ListRsp,3000) != 1)
		DisplayAndLog(pLogFile,"Problem with AT+WIND command ");
	
	if (ManageAtCommand(pCom1,"AT+WOPEN=0\r",ListRsp,3000) != 1)
		DisplayAndLog(pLogFile,"Problem in running the Open AT application");

	if (ManageAtCommand(pCom1,"AT+WIND=0\r",ListRsp,3000) != 1)
		DisplayAndLog(pLogFile,"Problem with AT+WIND command ");
	
	////////////////////// Generating Path & Test Name ///////////////////////////////////////////
	char softversion[10];
	sprintf(softversion,"%s",pSoftCfg->GetValue("Soft","OpenATVersion","%s"));

	int compiler=0; // 1=arm ; 2=gcc
	int mode=1;    //1=embedded ; 2=Remote; 3=CMUX
	compiler=pSoftCfg->GetValue("OAT_Default_values","Compiler","%d");
	mode=pSoftCfg->GetValue("OAT_Default_values","Mode","%d");

	char CmplName[5];
	if(compiler==1)
		strcpy(CmplName,"ARM");
        else if (compiler==2)
        	strcpy(CmplName,"GCC");
        else if (compiler==3)
		strcpy(CmplName,"ADS");
        else
        	strcpy(CmplName,"XXX");     
        	   			
	int answer,result=1;
	char strInputFile[512];
	char MemoryType[10];
	char FileExtension[30];
	char commande[1024],buffer[1024];

	sprintf(strInputFile,"%s",pSoftCfg->GetValue("Soft","ApplicationPath","%s"));
	printf("\nstrInputFile= %s\n",strInputFile);
	sprintf(MemoryType,"%s",pSoftCfg->GetValue("Soft","MemoryType","%s"));
	printf("\nMemoryType = %s\n",MemoryType);
	sprintf(FileExtension,"%s",pSoftCfg->GetValue("Soft","FileExtension","%s"));
	printf("\nFileExtension = %s\n",FileExtension);

	//strcat(strInputFile,CmplName);
	strcat(strInputFile,"Hello_World");
	//strcat(strInputFile,MemoryType);
	strcat(strInputFile,FileExtension);

	printf("\nPATH == %s", strInputFile);

	int download=0;
	download=DownloadApplication(strInputFile,1,50000,50000);

	if(mode!=1)
	{
		DisplayAndLog(pLogFile,"\r\nOnly embedded mode is supported\r\n");
		printf("\nOnly embedded mode is supported\n");
		return;
	}

	if(download!=1)
	{
		DisplayAndLog(pLogFile,"\r\nFile not downloaded hence Abort \r\n");
		return;
	}

	//////Recopy ARM as testname because in T.D ads & arm files are linked to ARM
	if (compiler==3)
        strcpy(CmplName,"ARM");
        	
    /////////////////  Write Application version & Script Version in LOG ///////////////////////////
	DisplayAndLog(pLogFile,"\r\n***********************************************************************\r\n");
	ListRsp[2]="\r\nApplication Version:*\r\n";
	if (ManageAtCommand(pCom1,NULL,ListRsp,10000) != 2)
		DisplayAndLog(pLogFile,"\nApplication Version Expected!!!!! \n");

	sprintf(buffer,"\r\nScript Version:%s\r\n",ScriptVersion);
	DisplayAndLog(pLogFile,buffer);

	sprintf(buffer,"\r\nOpenAT Soft Version:%s\r\n",softversion);
	DisplayAndLog(pLogFile,buffer);

	sprintf(buffer,"\r\nPath from where binary was downloaded:%s\r\n",strInputFile);
	DisplayAndLog(pLogFile,buffer);
	DisplayAndLog(pLogFile,"\r\n***********************************************************************\r\n");

    ////////////////////////////////////////////////////////////////////////////////////////////////

	if (ManageAtCommand(pCom1,"AT+WIND=0;&w\r",ListRsp,3000) != 1)
	{
		DisplayAndLog(pLogFile,"Problem with deactivation of WIND notifications");
		result=false;
	}
	
	// start open AT application
	if (ManageAtCommand(pCom1,"AT+WOPEN=1\r",ListRsp,50000) != 1)
	{
		DisplayAndLog(pLogFile,"\nCannot start Open AT application\n\n\n");
		result=false;
	}
	
	ListRsp[2]="\r\nHello World from Open-AT*\r\n";
	for (int i=0;(i<10)&&result;i++)
	{
		// Check open AT application is alive
		if (ManageAtCommand(pCom1,NULL,ListRsp,8000) != 2)
		{
			DisplayAndLog(pLogFile,"\nProblem: Open AT application does not work correctly\n\n\n");
			result=false;
		}
	}
	
	// Check the application state
	ListRsp[2]="\r\n+WOPEN: 1\r\n\r\nOK\r\n";
	if (ManageAtCommand(pCom1,"AT+WOPEN?\r",ListRsp,20000) != 2)
	{
		DisplayAndLog(pLogFile,"Problem: Open AT Application is not activated !");
		result=false;
	}
	
	if (pSoftCfg->GetValue("Features","WOPEN7","%d" ) != 0)
	{
		ListRsp[2]="\r\n+WOPEN: 7,12\r\n\r\nOK\r\n";
		if (ManageAtCommand(pCom1,"AT+WOPEN=7\r",ListRsp,15000) != 2)
		{
			DisplayAndLog(pLogFile,"+WOPEN: 7,12 not returned !");
			result=false;
		}
	}
	
	if (ManageAtCommand(pCom1,"AT+CFUN=1\r",ListRsp,50000) != 1)
	{
		DisplayAndLog(pLogFile,"\nCannot reset the module\n\n\n");
		result=false;
	}
	
	ListRsp[2]="\r\nHello World from Open-AT\r\n";
	for (i=0;(i<10)&&result;i++)
	{
		// Check open AT application is alive despite the reset
		if (ManageAtCommand(pCom1,NULL,ListRsp,8000) != 2)
		{
			DisplayAndLog(pLogFile,"\nProblem: Open AT application does not work correctly\n\n\n");
			result=false;
		}
	}
	
	// Check the application state
	ListRsp[2]="\r\n+WOPEN: 1\r\n\r\nOK\r\n";
	if (ManageAtCommand(pCom1,"AT+WOPEN?\r",ListRsp,20000) != 2)
	{
		DisplayAndLog(pLogFile,"Problem: Open AT Application is not activated !");
		result=false;
	}
	
	if (pSoftCfg->GetValue("Features","WOPEN7","%d" ) != 0)
	{
		ListRsp[2]="\r\n+WOPEN: 7,12\r\n\r\nOK\r\n";
		if (ManageAtCommand(pCom1,"AT+WOPEN=7\r",ListRsp,30000) != 2)
		{
			DisplayAndLog(pLogFile,"+WOPEN: 7,12 not returned !");
			result=false;
		}
	}
	
	if (ManageAtCommand(pCom1,"AT+CFUN=1\r",ListRsp,50000) != 1)
	{
		DisplayAndLog(pLogFile,"\nCannot reset the module\n\n\n");
		result=false;
	}
	
	ListRsp[2]="\r\nHello World from Open-AT*\r\n";
	for (i=0;(i<10)&&result;i++)
	{
		// Check open AT application is alive despite the 2 resets
		if (ManageAtCommand(pCom1,NULL,ListRsp,8000) != 2)
		{
			DisplayAndLog(pLogFile,"\nProblem: Open AT application does not work correctly\n\n\n");
			result=false;
		}
	}
	
	// Check the application state
	ListRsp[2]="\r\n+WOPEN: 1\r\n\r\nOK\r\n";
	if (ManageAtCommand(pCom1,"AT+WOPEN?\r",ListRsp,50000) != 2)
	{
		DisplayAndLog(pLogFile,"Problem: Open AT Application is not activated !");
		result=false;
	}
	
	if (pSoftCfg->GetValue("Features","WOPEN7","%d" ) != 0)
	{
		ListRsp[2]="\r\n+WOPEN: 7,12\r\n\r\nOK\r\n";
		if (ManageAtCommand(pCom1,"AT+WOPEN=7\r",ListRsp,30000) != 2)
		{
			DisplayAndLog(pLogFile,"+WOPEN: 7,12 not returned !");
			result=false;
		}
	}
	
	// Stop open AT application
	if (ManageAtCommand(pCom1,"AT+WOPEN=0\r",ListRsp,50000) != 1)
	{
		DisplayAndLog(pLogFile,"\nCannot reset the module\n\n\n");
		result=false;
	}
	
	// Empty buffer from wind notifications
	ManageAtCommand(pCom1,NULL,ListRsp,5000);
	
	// Deactivate WIND notifications
	if (ManageAtCommand(pCom1,"AT+WIND=0;&w\r",ListRsp,5000) != 1)
	{
		DisplayAndLog(pLogFile,"Problem with deactivation of WIND notifications");
		result=false;
	}
	
	// Check the application state
	ListRsp[2]="\r\n+WOPEN: 0\r\n\r\nOK\r\n";
	if (ManageAtCommand(pCom1,"AT+WOPEN?\r",ListRsp,50000) != 2)
	{
		DisplayAndLog(pLogFile,"Problem: Open AT Application is not deactivated !");
		result=false;
	}
	
	if (pSoftCfg->GetValue("Features","WOPEN7","%d" ) != 0)
	{
		ListRsp[2]="\r\n+WOPEN: 7,0\r\n\r\nOK\r\n";
		if (ManageAtCommand(pCom1,"AT+WOPEN=7\r",ListRsp,30000) != 2)
		{
			DisplayAndLog(pLogFile,"+WOPEN: 7,0 not returned !");
			result=false;
		}
	}
	
	// Erase open AT application
	if (ManageAtCommand(pCom1,"AT+WOPEN=4\r",ListRsp,80000) != 1)
	{
		DisplayAndLog(pLogFile,"\nCannot erase the open AT application\n\n\n");
		result=false;
	}
	
	// Check the application state
	ListRsp[2]="\r\n+WOPEN: 0\r\n\r\nOK\r\n";
	if (ManageAtCommand(pCom1,"AT+WOPEN?\r",ListRsp,50000) != 2)
	{
		DisplayAndLog(pLogFile,"Problem: Open AT Application is not deactivated !");
		result=false;
	}
	
	if (pSoftCfg->GetValue("Features","WOPEN7","%d" ) != 0)
	{
		ListRsp[2]="\r\n+WOPEN: 7,0\r\n\r\nOK\r\n";
		if (ManageAtCommand(pCom1,"AT+WOPEN=7\r",ListRsp,8000) != 2)
		{
			DisplayAndLog(pLogFile,"+WOPEN: 7,0 not returned !");
			result=false;
		}
	}
	
	// Re-download the application
	if(DownloadApplication(strInputFile,1,50000,50000)!=1)
	{
		DisplayAndLog(pLogFile,"\r\nFile not downloaded hence Abort \r\n");
		result=false;
	}
	
	// start open AT application
	if (ManageAtCommand(pCom1,"AT+WOPEN=1\r",ListRsp,50000) != 1)
	{
		DisplayAndLog(pLogFile,"\nCannot start Open AT application\n\n\n");
		result=false;
	}
	
	ListRsp[2]="\r\nHello World from Open-AT\r\n";
	for (i=0;(i<10)&&result;i++)
	{
		// Check open AT application is alive
		if (ManageAtCommand(pCom1,NULL,ListRsp,8000) != 2)
		{
			DisplayAndLog(pLogFile,"\nProblem: Open AT application does not work correctly\n\n\n");
			result=false;
		}
	}
	
	// Check the application state
	ListRsp[2]="\r\n+WOPEN: 1\r\n\r\nOK\r\n";
	if (ManageAtCommand(pCom1,"AT+WOPEN?\r",ListRsp,8000) != 2)
	{
		DisplayAndLog(pLogFile,"Problem: Open AT Application is not activated !");
		result=false;
	}
	
	if (pSoftCfg->GetValue("Features","WOPEN7","%d" ) != 0)
	{
		ListRsp[2]="\r\n+WOPEN: 7,12\r\n\r\nOK\r\n";
		if (ManageAtCommand(pCom1,"AT+WOPEN=7\r",ListRsp,8000) != 2)
		{
			DisplayAndLog(pLogFile,"+WOPEN: 7,12 not returned !");
			result=false;
		}
	}
	
	if (ManageAtCommand(pCom1,"AT+CFUN=1\r",ListRsp,50000) != 1)
	{
		DisplayAndLog(pLogFile,"\nCannot reset the module\n\n\n");
		result=false;
	}
	
	ListRsp[2]="\r\nHello World from Open-AT*\r\n";
	for (i=0;(i<10)&&result;i++)
	{
		// Check open AT application is alive despite the reset
		if (ManageAtCommand(pCom1,NULL,ListRsp,8000) != 2)
		{
			DisplayAndLog(pLogFile,"\nProblem: Open AT application does not work correctly\n\n\n");
			result=false;
		}
	}
	
	// Check the application state
	ListRsp[2]="\r\n+WOPEN: 1\r\n\r\nOK\r\n";
	if (ManageAtCommand(pCom1,"AT+WOPEN?\r",ListRsp,8000) != 2)
	{
		DisplayAndLog(pLogFile,"Problem: Open AT Application is not activated !");
		result=false;
	}
	
	if (pSoftCfg->GetValue("Features","WOPEN7","%d" ) != 0)
	{
		ListRsp[2]="\r\n+WOPEN: 7,12\r\n\r\nOK\r\n";
		if (ManageAtCommand(pCom1,"AT+WOPEN=7\r",ListRsp,30000) != 2)
		{
			DisplayAndLog(pLogFile,"+WOPEN: 7,12 not returned !");
			result=false;
		}
	}
	
	if (ManageAtCommand(pCom1,"AT+CFUN=1\r",ListRsp,50000) != 1)
	{
		DisplayAndLog(pLogFile,"\nCannot reset the module\n\n\n");
		result=false;
	}
	
	ListRsp[2]="\r\nHello World from Open-AT*\r\n";
	for (i=0;(i<10)&&result;i++)
	{
		// Check open AT application is alive despite the 2 resets
		if (ManageAtCommand(pCom1,NULL,ListRsp,8000) != 2)
		{
			DisplayAndLog(pLogFile,"\nProblem: Open AT application does not work correctly\n\n\n");
			result=false;
		}
	}
	
	// Check the application state
	ListRsp[2]="\r\n+WOPEN: 1\r\n\r\nOK\r\n";
	if (ManageAtCommand(pCom1,"AT+WOPEN?\r",ListRsp,8000) != 2)
	{
		DisplayAndLog(pLogFile,"Problem: Open AT Application is not activated !");
		result=false;
	}
	
	if (pSoftCfg->GetValue("Features","WOPEN7","%d" ) != 0)
	{
		ListRsp[2]="\r\n+WOPEN: 7,12\r\n\r\nOK\r\n";
		if (ManageAtCommand(pCom1,"AT+WOPEN=7\r",ListRsp,8000) != 2)
		{
			DisplayAndLog(pLogFile,"+WOPEN: 7,12 not returned !");
			result=false;
		}
	}
	
	// Stop open AT application
	if (ManageAtCommand(pCom1,"AT+WOPEN=0\r",ListRsp,50000) != 1)
	{
		DisplayAndLog(pLogFile,"\nCannot reset the module\n\n\n");
		result=false;
	}
	
	// Empty buffer from wind notifications
	ManageAtCommand(pCom1,NULL,ListRsp,5000);
	
	// Deactivate WIND notifications
	if (ManageAtCommand(pCom1,"AT+WIND=0;&w\r",ListRsp,3000) != 1)
	{
		DisplayAndLog(pLogFile,"Problem with deactivation of WIND notifications");
		result=false;
	}
	
	// Check the application state
	ListRsp[2]="\r\n+WOPEN: 0\r\n\r\nOK\r\n";
	if (ManageAtCommand(pCom1,"AT+WOPEN?\r",ListRsp,50000) != 2)
	{
		DisplayAndLog(pLogFile,"Problem: Open AT Application is not deactivated !");
		result=false;
	}
	
	if (pSoftCfg->GetValue("Features","WOPEN7","%d" ) != 0)
	{
		ListRsp[2]="\r\n+WOPEN: 7,0\r\n\r\nOK\r\n";
		if (ManageAtCommand(pCom1,"AT+WOPEN=7\r",ListRsp,30000) != 2)
		{
			DisplayAndLog(pLogFile,"+WOPEN: 7,0 not returned !");
			result=false;
		}
	}
	
	// Erase open AT application
	if (ManageAtCommand(pCom1,"AT+WOPEN=4\r",ListRsp,50000) != 1)
	{
		DisplayAndLog(pLogFile,"\nCannot erase the open AT application\n\n\n");
		result=false;
	}
	
	// Check the application state
	ListRsp[2]="\r\n+WOPEN: 0\r\n\r\nOK\r\n";
	if (ManageAtCommand(pCom1,"AT+WOPEN?\r",ListRsp,50000) != 2)
	{
		DisplayAndLog(pLogFile,"Problem: Open AT Application is not deactivated !");
		result=false;
	}
	
	if (pSoftCfg->GetValue("Features","WOPEN7","%d" ) != 0)
	{
		ListRsp[2]="\r\n+WOPEN: 7,0\r\n\r\nOK\r\n";
		if (ManageAtCommand(pCom1,"AT+WOPEN=7\r",ListRsp,30000) != 2)
		{
			DisplayAndLog(pLogFile,"+WOPEN: 7,0 not returned !");
			result=false;
		}
	}
	
	if (result)
		DisplayAndLog(pLogFile,"\nStatus A_MISC_0076: PASSED\n\n\n");
	else
		DisplayAndLog(pLogFile,"\nStatus A_MISC_0076: FAILED\n\n\n");

	
	/* Here the scenario is passed */
	Scenario.SetVerdict(Passed);
	
}