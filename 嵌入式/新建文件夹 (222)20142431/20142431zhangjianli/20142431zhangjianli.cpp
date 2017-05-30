// 20142431zhangjianli.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "20142431zhangjianli.h"
#include "20142431zhangjianliDlg.h"
#include "fpga_def.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy20142431zhangjianliApp

BEGIN_MESSAGE_MAP(CMy20142431zhangjianliApp, CWinApp)
END_MESSAGE_MAP()


// CMy20142431zhangjianliApp construction
CMy20142431zhangjianliApp::CMy20142431zhangjianliApp()
	: CWinApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CMy20142431zhangjianliApp object
CMy20142431zhangjianliApp theApp;

// CMy20142431zhangjianliApp initialization

BOOL CMy20142431zhangjianliApp::InitInstance()
{

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CMy20142431zhangjianliDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
