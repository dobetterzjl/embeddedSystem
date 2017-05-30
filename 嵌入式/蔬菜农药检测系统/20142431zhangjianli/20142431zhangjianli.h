// 20142431zhangjianli.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#ifdef STANDARDSHELL_UI_MODEL
#include "resource.h"
#endif

// CMy20142431zhangjianliApp:
// See 20142431zhangjianli.cpp for the implementation of this class
//

class CMy20142431zhangjianliApp : public CWinApp
{
public:
	CMy20142431zhangjianliApp();
	
// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMy20142431zhangjianliApp theApp;
