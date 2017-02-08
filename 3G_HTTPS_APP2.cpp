// **************************************************************
// Scenario:	HTTPS_APP1.Cpp
// ----------+--------+-------|-------------------------------------------------
//    Date   | Author | Ver   |	Modification 
// ----------+--------+-------|-------------------------------------------------
//  29.05.13 | ACONG  | V1.0  |	- Initial revision
// ----------+--------+-------|-------------------------------------------------

// char ScriptVersion[10]="" should have the same value as that present in the Ver column 
//  Please Update this value whenever a new version of file is generated 

#include <stdio.h>
#include "C:\VALIDATIONLIBRARY\VALIDATIONLIBRARY\CScenario.h"
#include "C:\ValidationLibrary\XModemProtocol\DownloaderXModem.h"

#define TEST_STATUS_SKIPPED  0
#define TEST_STATUS_PASSED   1
#define TEST_STATUS_FAILED  -1

char ScriptVersion[10]="V1.0";
int download=1; // 1=success, 0= fail put default value=1 if tester wants to skip running the d/l part for each script run while debugging or developing
int result;
char CurTestCase[64];	// name of current test case
char CurIpServer[200]="https://116.66.221.42";  // the current IP address for public server

char* ListRsp[10] = {"", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
ListRsp[1] = "\r\nOK\r\n";

typedef struct {
	char *str_test;   // string: test name
	char *str_wtrun;  // string: AT+WTRUN
	int gprs_act;     // GPRS active flag
	int do_logging;   // flag: do logging = 1 (yes) / 0 (no)
	int need_start;   // flag: need start = 1 (yes) / 0 (no) / -1 (don't care)
	int need_passed;  // flag: need passed
	int need_failed;  // flag: need failed
	int need_error;   // flag: need error
	int need_end;     // flag: need end
	int http_opt;     // HTTP processing option flags
	unsigned long timeout;  // response timeout in milliseconds
} TestParam_t;

typedef struct {
	int test_start;   // flag: test start = 1 (yes) / 0 (no)
	int test_passed;  // flag: test passed
	int test_failed;  // flag: test failed
	int test_error;   // flag: test error
	int test_end;     // flag: test end
} TestStatus_t;

void TestParam_init(TestParam_t *params)
{
	params->need_start  = 1;   // flag: need start = yes
	params->need_passed = 1;   // flag: need passed = yes
	params->need_failed = 0;   // flag: need failed = no
	params->need_error  = 0;   // flag: need error = no
	params->need_end    = 1;  // flag: need end = don't care
	params->do_logging  = 1;   // flag: do logging = yes
	params->http_opt    = -1;  // HTTP processing option = nil
	params->timeout     = 10000;  // response timeout = 10 seconds
}

void UserPostamble(void)
{
	printf("\n--------------UserPostamble------------\n");
	pLogFile->WriteComment("\r\n--------------UserPostamble------------\r\n");
    
	/* Here some user actions before the scenario ending ..........*/
	char* ListRsp[4] = {"", NULL, NULL, NULL};
	
	//
	ManageAtCommand(pCom1,NULL,ListRsp,25000);
	ListRsp[1] = "*\r\nOK\r\n";

	Wait (15);
	if (ManageAtCommand(pCom1,"AT+WOPEN=0\r",ListRsp,80000) != 1)
		DisplayAndLog(pLogFile,"------------ AT+WOPEN=0 has not succeeded -----------\n\n\r");
	Wait (5);
	ManageAtCommand(pCom1,NULL,ListRsp,10000);
	
	if (ManageAtCommand(pCom1,"AT+WIND=0\r",ListRsp,10000) != 1)
		DisplayAndLog(pLogFile,"------------ AT+WIND=0 has not succeeded -----------\n\n\r");
	if (ManageAtCommand(pCom1,"AT+WOPEN=4\r",ListRsp,60000) != 1)
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


// For use with COM-1 only.  For multi-port, see below
void OATSetting()
{
	char* ListRsp[10] = {"",NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL,NULL};

	ListRsp[1] = "*\r\nOK\r\n";
	
	if (ManageAtCommand(pCom1,"ATE0;+WIND=0;+CREG=0;+CGREG=0;&W\r",ListRsp,30000) != 1)
		pLogFile->WriteComment("\n\nProblem in ATE command\n");
    Wait(5);
	if (ManageAtCommand(pCom1,"AT+WOPEN=1\r",ListRsp,30000) != 1)
		pLogFile->WriteComment("\n\nProblem in running the Open AT application\n");
    Wait(5);
	if (ManageAtCommand(pCom1,"ATE0;+WIND=0;&W\r",ListRsp,30000) != 1)
		pLogFile->WriteComment("\n\nProblem in ATE and WIND command\n");
	if (ManageAtCommand(pCom1,"AT+CMEE=1;&W\r",ListRsp,30000) != 1)
		pLogFile->WriteComment("\n\nProblem with AT+CMEE command\n");
	Wait (5);
}


void DisplayAndLog( CLog *pLog, char * String )
{
	printf( String );
	pLog->WriteComment( String );
}
void DisplayAndLog_header( CLog *pLog, char * strTest )
{
	char strBuff[80];

	// show text header
	sprintf(strBuff, "Test Case: %s\n", strTest);
	DisplayAndLog(pLog,"--------------------------------------\n");
	DisplayAndLog(pLog,strBuff);
	DisplayAndLog(pLog,"--------------------------------------\n");
}
void DisplayAndLog_result( CLog *pLog, char * strTest, int ret )
{
	char strBuff[80];
	char *strStat;

	switch(ret)
	{
	case TEST_STATUS_SKIPPED:
		strStat = "SKIPPED";
		break;
	case TEST_STATUS_PASSED:
		strStat = "PASSED";
		break;
	case TEST_STATUS_FAILED:
		strStat = "FAILED";
		break;
	default:
		strStat = "UNKNOWN";
		break;
	}

	sprintf(strBuff, "\nStatus %s: %s\n", strTest, strStat);
	DisplayAndLog(pLog, strBuff);
}
void DisplayAndLogTest( CLog *pLog, char * String )
{
    char        buffer[512];
    
	sprintf( buffer, "\r\n\n%s: %s", CurTestCase, String );
	printf( buffer );
	pLog->WriteComment( buffer );
}

// ========================================================================== //
//                              Individual tests                              //
// ========================================================================== //

bool PreSetting()
{
    char        cmdStr[64];
    char        apn[24];
    char        login[24];
    char        pwd[24];
    int i=0;
	
    DisplayAndLog(pLogFile, "\n\n---- PreSetting  starts ---- \n\n");
    strcpy(CurTestCase, "PreSetting ");
	ManageAtCommand(pCom1,NULL,ListRsp,5000);
    // ---------------------------------------
    // Part-1 : Open GRPS Bearer
    // ---------------------------------------
    DisplayAndLogTest(pLogFile, "PreSetting: Part-1\n\n");
    //
    sprintf(apn, "%s", pTestedSim->GetValue("gprs","APN","%s"));
    // sprintf(login, "%s", pTestedSim->GetValue("gprs","username","%s"));
    // sprintf(pwd,   "%s", pTestedSim->GetValue("gprs","password","%s"));
    sprintf(cmdStr, "AT+GPRS=\"%s\",\"\",\"\"\r", apn);
    while (i<=4)
	{
		if (ManageAtCommand(pCom1,cmdStr,ListRsp,5000) != 1)
		{
			DisplayAndLogTest(pLogFile, "GPRS AT+GPRS setting problem\n");
		}
		Wait (5);
		ListRsp[2] = "*CONNECTED*IP address:*\r\n";
		if (ManageAtCommand(pCom1,NULL,ListRsp,80000) != 2)
		{
			DisplayAndLogTest(pLogFile, "The GPRS Bearer set up failed!!n");
			i++;
		}
		else
		{
			i=5;
		}
		Wait (5);
	}
	if (i!=5)
	{
		DisplayAndLogTest(pLogFile, "The Bearer failed to set up!!!!!\n");
        return false;
	}
	
	ManageAtCommand(pCom1,NULL,ListRsp,5000);

    // ----------------------------------------------------------------
    // Part-2 : Do the environment setting
    // ----------------------------------------------------------------
    DisplayAndLogTest(pLogFile, "PreSetting: Part-2\n\n");
	ListRsp[1] = "\r\nOK\r\n";
    // Make sure the time can meet the SSL key requirements.
    if (ManageAtCommand(pCom1,"AT+CCLK=\"10/01/01,00:00:00\"\r",ListRsp,5000) != 1)
    {
        DisplayAndLogTest(pLogFile, "Cannot set the time for the module\n");
        return false;
    }
	// Close the log trace from the module.
	if (ManageAtCommand(pCom1,"AT+WIPDBG=0\r",ListRsp,5000) != 1)
    {
        DisplayAndLogTest(pLogFile, "Cannot close the log trace on port\n");
        return false;
    }
	
    return true;
}
int TestStatus_check(TestStatus_t *status, TestParam_t *params)
{
	int ret;

	ret = TEST_STATUS_PASSED;  // assuming passed

	if (params->need_start != -1 && status->test_start != params->need_start)
	{
		ret = TEST_STATUS_FAILED;  // failed
	}

	if (params->need_passed != -1 && status->test_passed != params->need_passed)
	{
		ret = TEST_STATUS_FAILED;  // failed
	}

	if (params->need_failed != -1 && status->test_failed != params->need_failed)
	{
		ret = TEST_STATUS_FAILED;  // failed
	}

	if (params->need_error != -1 && status->test_error != params->need_error)
	{
		ret = TEST_STATUS_FAILED;  // failed
	}

	if (params->need_end != -1 && status->test_end != params->need_end)
	{
		ret = TEST_STATUS_FAILED;  // failed
	}

	return ret;  // return check result
}
void TestStatus_init(TestStatus_t *params)
{
	params->test_start  = 0;   // flag: test start = no
	params->test_passed = 0;   // flag: test passed = no
	params->test_failed = 0;   // flag: test failed = no
	params->test_error  = 0;   // flag: test error = no
	params->test_end    = 0;   // flag: test end = no
}
int TestRunWT(int NumWT, TestParam_t *params)
{
	int ret = TEST_STATUS_SKIPPED;  // skipped
	char strTest[30];
	char strCmmd[30];
	char *pRunCommand;
	char strList1[100];
	char strList2[100];
	char strList3[100];
	char strList4[100];
	char strList5[100];
	char strList6[100];
	char* ListRsp[12] = {"",NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
	int test_ret = 0;
	char strTmep[100];
	
	sprintf(strTest, "O_WIP_HTTPS_APP_GCC_%04d", NumWT);

	//Set the Target TEST IP address;
	sprintf(strTmep, "AT+HTTPURL=%d,\"%s\"\r", NumWT,CurIpServer);
	ListRsp[1]="\r\nOK\r\n";
	if (ManageAtCommand(pCom1,strTmep,ListRsp,5000) != 1)
    {
        DisplayAndLogTest(pLogFile, "Cannot set the target IP address!\n");
    }
	TestStatus_t status;
	sprintf(strTest, "%s", params->str_test);

	// show text header
	if (params->do_logging == 1)
	{
		DisplayAndLog_header(pLogFile,strTest);
	}

	// Run the test case;
	TestStatus_init(&status);

	sprintf(strCmmd, "AT+WTRUN=1,%d\r", NumWT);
	pRunCommand = strCmmd;
		
	// run test, check response messages
	sprintf(strList1, "*[ WTF - Test result] %s\t:\tINPROGRESS\t\r\n", params->str_wtrun);
	sprintf(strList2, "*[ WTF - Test %s ]\tWIP_CEV_ERROR\r\n", params->str_wtrun);
	sprintf(strList3, "*[ WTF - Test result] %s\t:\tFAILED\t*\r\n", params->str_wtrun);
	sprintf(strList4, "*[ WTF - Test result] %s\t:\tPASSED\t\r\n", params->str_wtrun);
	sprintf(strList5, "*http_ClientTestDataHandler: Reason=\"*OK\"\r\n*");
	sprintf(strList6, "*[ WTF - Test %s ]\t*\r\n", params->str_wtrun);
	ListRsp[1] = strList1;
	ListRsp[2] = strList2;
	ListRsp[3] = strList3;
	ListRsp[4] = strList4;
	ListRsp[5] = strList5;
	ListRsp[6] = strList6;
	while ((test_ret = ManageAtCommand(pCom1,pRunCommand,ListRsp,params->timeout)) > 0)
		{
			pRunCommand = NULL;  // send run command only once

			if (test_ret == 1)
			{
				status.test_start = 1;  // received test start message
			}
			else if (test_ret == 2)
			{
				status.test_error = 1;  // received test error message
			}
			else if (test_ret == 3)
			{
				status.test_failed = 1;  // received test failed message
			}
			else if (test_ret == 4)
			{
				status.test_passed = 1;  // received test passed message
			}
			else if (test_ret == 5)
			{
				status.test_end = 1;  // received test ended message
				break;
			}
		}

		// check test status flags
		ret = TestStatus_check(&status, params);
		ManageAtCommand(pCom1,NULL,ListRsp,5000);

	// show test result
	if (params->do_logging == 1)
	{
		DisplayAndLog_result(pLogFile,strTest,ret);
	}

	return ret;
}
int TestRunWT5(int NumWT, TestParam_t *params)
{
	int ret = TEST_STATUS_SKIPPED;  // skipped
	char strTest[30];
	char strCmmd[30];
	char *pRunCommand;
	char strList1[100];
	char strList2[100];
	char strList3[100];
	char strList4[100];
	char strList5[100];
	char strList6[100];
	char* ListRsp[12] = {"",NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
	int test_ret = 0;
	char strTmep[100];
	
	sprintf(strTest, "O_3G_WIP_HTTPS_APP_GCC_%04d", NumWT);

	//Set the Target TEST IP address;
	sprintf(strTmep, "AT+HTTPURL=%d,\"%s\"\r", NumWT,CurIpServer);
	ListRsp[1]="\r\nOK\r\n";
	if (ManageAtCommand(pCom1,strTmep,ListRsp,5000) != 1)
    {
        DisplayAndLogTest(pLogFile, "Cannot set the target IP address!\n");
    }
	TestStatus_t status;
	sprintf(strTest, "%s", params->str_test);

	// show text header
	if (params->do_logging == 1)
	{
		DisplayAndLog_header(pLogFile,strTest);
	}

	// Run the test case;
	TestStatus_init(&status);

	sprintf(strCmmd, "AT+WTRUN=1,%d\r", NumWT);
	pRunCommand = strCmmd;
		
	// run test, check response messages
	sprintf(strList1, "*[ WTF - Test result] %s\t:\tINPROGRESS\t\r\n", params->str_wtrun);
	sprintf(strList2, "*[ WTF - Test %s ]\tWIP_CEV_ERROR\r\n", params->str_wtrun);
	sprintf(strList3, "*[ WTF - Test result] %s\t:\tFAILED\t*\r\n", params->str_wtrun);
	sprintf(strList4, "*[ WTF - Test result] %s\t:\tPASSED\t\r\n", params->str_wtrun);
	sprintf(strList5, "*http_ClientTestDataHandler: ERROR*");
	sprintf(strList6, "*[ WTF - Test %s ]\t*\r\n", params->str_wtrun);
	ListRsp[1] = strList1;
	ListRsp[2] = strList2;
	ListRsp[3] = strList3;
	ListRsp[4] = strList4;
	ListRsp[5] = strList5;
	ListRsp[6] = strList6;
	while ((test_ret = ManageAtCommand(pCom1,pRunCommand,ListRsp,params->timeout)) > 0)
		{
			pRunCommand = NULL;  // send run command only once

			if (test_ret == 1)
			{
				status.test_start = 1;  // received test start message
			}
			else if (test_ret == 2)
			{
				status.test_error = 1;  // received test error message
			}
			else if (test_ret == 3)
			{
				status.test_failed = 1;  // received test failed message
			}
			else if (test_ret == 4)
			{
				status.test_passed = 1;  // received test passed message
			}
			else if (test_ret == 5)
			{
				status.test_end = 1;  // received test ended message
				break;
			}
		}

		// check test status flags
		ret = TestStatus_check(&status, params);
		ManageAtCommand(pCom1,NULL,ListRsp,5000);

	// show test result
	if (params->do_logging == 1)
	{
		DisplayAndLog_result(pLogFile,strTest,ret);
	}

	return ret;
}
int TestRunWT10(int NumWT, TestParam_t *params)
{
	int ret = TEST_STATUS_SKIPPED;  // skipped
	char strTest[30];
	char strCmmd[30];
	char *pRunCommand;
	char strList1[100];
	char strList2[100];
	char strList3[100];
	char strList4[100];
	char strList5[100];
	char strList6[100];
	char* ListRsp[12] = {"",NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
	int test_ret = 0;
	char strTmep[100];
	int i=1;
	sprintf(strTest, "O_3G_WIP_HTTPS_APP_GCC_%04d", NumWT);

	//Set the Target TEST IP address;
	sprintf(strTmep, "AT+HTTPURL=%d,\"%s\"\r", NumWT,CurIpServer);
	ListRsp[1]="\r\nOK\r\n";
	if (ManageAtCommand(pCom1,strTmep,ListRsp,5000) != 1)
    {
        DisplayAndLogTest(pLogFile, "Cannot set the target IP address!\n");
    }
	TestStatus_t status;
	sprintf(strTest, "%s", params->str_test);

	// show text header
	if (params->do_logging == 1)
	{
		DisplayAndLog_header(pLogFile,strTest);
	}

	// Run the test case;
	TestStatus_init(&status);

	sprintf(strCmmd, "AT+WTRUN=1,%d\r", NumWT);
	pRunCommand = strCmmd;
		
	// run test, check response messages
	sprintf(strList1, "*[ WTF - Test result] %s\t:\tINPROGRESS\t\r\n", params->str_wtrun);
	sprintf(strList2, "*[ WTF - Test %s ]\tWIP_CEV_ERROR\r\n", params->str_wtrun);
	sprintf(strList3, "*[ WTF - Test result] %s\t:\tFAILED\t*\r\n", params->str_wtrun);
	sprintf(strList4, "*[ WTF - Test result] %s\t:\tPASSED\t\r\n", params->str_wtrun);
	sprintf(strList5, "*FINALIZER OK");
	sprintf(strList6, "*[ WTF - Test %s ]\t*\r\n", params->str_wtrun);
	ListRsp[1] = strList1;
	ListRsp[2] = strList2;
	ListRsp[3] = strList3;
	ListRsp[4] = strList4;
	ListRsp[5] = strList5;
	ListRsp[6] = strList6;
	while ((test_ret = ManageAtCommand(pCom1,pRunCommand,ListRsp,params->timeout)) > 0)
		{
			pRunCommand = NULL;  // send run command only once

			if (test_ret == 1)
			{
				status.test_start = 1;  // received test start message
			}
			else if (test_ret == 2)
			{
				status.test_error = 1;  // received test error message
			}
			else if (test_ret == 3)
			{
				status.test_failed = 1;  // received test failed message
			}
			else if (test_ret == 4)
			{
				status.test_passed = 1;  // received test passed message
			}
			else if (test_ret == 5)
			{
				i++;
			}
		}
		if (i=30)
		{
		status.test_end = 1;	// Check all the Open channel for GET request/Close channel process has
								// been done!
		}

		// check test status flags
		ret = TestStatus_check(&status, params);
		ManageAtCommand(pCom1,NULL,ListRsp,5000);

	// show test result
	if (params->do_logging == 1)
	{
		DisplayAndLog_result(pLogFile,strTest,ret);
	}

	return ret;
}


// ========================================================================== //
//                                Main program                                //
// ========================================================================== //

void main(int argc,char** argv)
{	
	printf("\n*--------------------------------------------------------*\n");
	printf("****    Bearers Test	     ****\n");
	printf("*--------------------------------------------------------*\n");

	// By default the test is considered as failed until the end where it is considered as passed
	CScenario Scenario(argc,argv,UserPostamble);
	Scenario.SetVerdict(Failed);

	// Get the Result of environnement checking, Mandatory !!!
    if(Scenario.GetResultEnvChecking() == false)
		return ;
	
	char* ListRsp[10] = {"",NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
		

	//==========Start up setting==============//
	
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
	strcat(strInputFile,"3G_HTTPS_APP2_WTF");
//	strcat(strInputFile,MemoryType);
	strcat(strInputFile,FileExtension);

	printf("\nPATH == %s", strInputFile);
	
	// START DOWNLOAD
  download=DownloadApplication(strInputFile,1,50000,50000);
  
  if(mode!=1)
	{
	  DisplayAndLog(pLogFile,"\r\nOnly embedded mode is supported\r\n");
		return;
	}

	if(download!=1)
	{
	  DisplayAndLog(pLogFile,"\r\nFile not downloaded hence Abort\r\n");
		return;
	}
	 
	// RECOPY ARM AS TESTNAME BECAUSE IN TD ADS & ARM FILES ARE LINKED TO ARM 
	if (compiler==3)
    strcpy(TestName,"ARM");
	// ===================================//
    // OPEN AT SETTINGS
    // ===================================//
	// Start the pre-setting and lanuch the OpenAt application.
	OATSetting();
    Wait(5);
	BearerSet=PreSetting();
	if (BearerSet==0)
	{
		DisplayAndLog(pLogFile,"\n\n\r--Test will be aborted!!!!!!----\n\n\r");
		return;
	}
	
	int gprs_act = 1;
	TestParam_t test_params;

	test_params.gprs_act = gprs_act;
	//----------------------O_3G_WIP_HTTPS_APP_GCC_0014 -------------------------//

	// execute test 0001
	TestParam_init(&test_params);
	test_params.need_passed=0;
	test_params.timeout=15000;
	test_params.str_test  = "O_3G_WIP_HTTPS_APP_GCC_0014";
	test_params.str_wtrun = "O_TEST_0001";
	if(TestRunWT(1, &test_params) < 0)
	{
		error = 1;
	}

//----------------------O_3G_WIP_HTTPS_APP_GCC_0014 (end)--------------------//

//----------------------O_3G_WIP_HTTPS_APP_GCC_0016 -------------------------//

	// execute test 0002
	TestParam_init(&test_params);
	test_params.need_passed=0;
	test_params.str_test  = "O_3G_WIP_HTTPS_APP_GCC_0016";
	test_params.str_wtrun = "O_TEST_0002";
	if(TestRunWT(2, &test_params) < 0)
	{
		error = 1;
	}

//----------------------O_3G_WIP_HTTPS_APP_GCC_0016 (end)--------------------//

//----------------------O_3G_WIP_HTTPS_APP_GCC_0017 -------------------------//

	// execute test 0003
	TestParam_init(&test_params);
	test_params.need_passed=0;
	test_params.str_test  = "O_3G_WIP_HTTPS_APP_GCC_0017";
	test_params.str_wtrun = "O_TEST_0003";
	if(TestRunWT(3, &test_params) < 0)
	{
		error = 1;
	}

//----------------------O_3G_WIP_HTTPS_APP_GCC_0017 (end)--------------------//

//----------------------O_WIP_HTTPS_APP_GCC_0019 -------------------------//

	// execute test 0004
	TestParam_init(&test_params);
	test_params.need_passed=0;
	test_params.timeout=25000;
	test_params.str_test  = "O_3G_WIP_HTTPS_APP_GCC_0019";
	test_params.str_wtrun = "O_TEST_0004";
	if(TestRunWT(4, &test_params) < 0)
	{
		error = 1;
	}

//----------------------O_3G_WIP_HTTPS_APP_GCC_0019 (end)--------------------//

//----------------------O_3G_WIP_HTTPS_APP_GCC_0020 -------------------------//

	// execute test 0005
	TestParam_init(&test_params);
	test_params.need_passed=0;
	test_params.str_test  = "O_3G_WIP_HTTPS_APP_GCC_0020";
	test_params.str_wtrun = "O_TEST_0005";
	if(TestRunWT5(5, &test_params) < 0)
	{
		error = 1;
	}

//----------------------O_3G_WIP_HTTPS_APP_GCC_0020 (end)--------------------//
// Reset the module for the rest test cases.
	ListRsp[1] = "*\r\nOK\r\n";
	if (ManageAtCommand(pCom1,"AT+WIND=0\r",ListRsp,10000) != 1)
		DisplayAndLog(pLogFile,"------------ AT+WIND=0 has not succeeded -----------\n\n\r");
	if (ManageAtCommand(pCom1,"AT+WOPEN=0\r",ListRsp,60000) != 1)
		DisplayAndLog(pLogFile,"------------ AT+WOPEN=0 has not succeeded -----------\n\n\r");
	if (ManageAtCommand(pCom1,"AT+CFUN=1\r",ListRsp,50000) != 1)
		DisplayAndLog(pLogFile,"------------ AT+CFUN=1 has not succeeded -----------\n\n\r");
	if (ManageAtCommand(pCom1,"AT+WIND=0;&w\r",ListRsp,10000) != 1)
		DisplayAndLog(pLogFile,"------------ AT+WIND=0 has not succeeded -----------\n\n\r");
	ListRsp[1] = "\r\nOK\r\n";	
	OATSetting();
    Wait(5);
	BearerSet=PreSetting();
	if (BearerSet==0)
	{
		DisplayAndLog(pLogFile,"\n\n\r--Test will be aborted!!!!!!----\n\n\r");
		return;
	}
	
	int gprs_act = 1;
	TestParam_t test_params;

	test_params.gprs_act = gprs_act;

//----------------------O_3G_WIP_HTTPS_APP_GCC_0021 -------------------------//

	// execute test 0006
	TestParam_init(&test_params);
	test_params.need_passed=0;
	test_params.timeout=15000;
	test_params.str_test  = "O_3G_WIP_HTTPS_APP_GCC_0021";
	test_params.str_wtrun = "O_TEST_0006";
	if(TestRunWT(6, &test_params) < 0)
	{
		error = 1;
	}

//----------------------O_3G_WIP_HTTPS_APP_GCC_0021 (end)--------------------//

//----------------------O_3G_WIP_HTTPS_APP_GCC_0025 -------------------------//

	// execute test 0009
	TestParam_init(&test_params);
	test_params.need_passed=0;
	test_params.timeout=20000;
	test_params.str_test  = "O_3G_WIP_HTTPS_APP_GCC_0025";
	test_params.str_wtrun = "O_TEST_0009";
	if(TestRunWT(9, &test_params) < 0)
	{
		error = 1;
	}

//----------------------O_3G_WIP_HTTPS_APP_GCC_0025 (end)--------------------//

//----------------------O_3G_WIP_HTTPS_APP_GCC_0028 -------------------------//

	// execute test 0010
	TestParam_init(&test_params);
	test_params.need_passed=0;
	test_params.timeout=25000;
	test_params.str_test  = "O_3G_WIP_HTTPS_APP_GCC_0028";
	test_params.str_wtrun = "O_TEST_0010";
	if(TestRunWT10(10, &test_params) < 0)
	{
		error = 1;
	}

//----------------------O_3G_WIP_HTTPS_APP_GCC_0028 (end)--------------------//


    Scenario.SetVerdict(Passed);
}

