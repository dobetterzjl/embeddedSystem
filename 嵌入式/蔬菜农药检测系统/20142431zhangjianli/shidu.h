#pragma once
#include "afxwin.h"


// Cshidu dialog

class Cshidu : public CDialog
{
	DECLARE_DYNAMIC(Cshidu)

public:
	Cshidu(CWnd* pParent = NULL);   // standard constructor
	virtual ~Cshidu();

// Dialog Data
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton5();
	CEdit max_val1;
};
