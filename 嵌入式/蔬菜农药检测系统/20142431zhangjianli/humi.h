#pragma once
#include "afxwin.h"


// Chumi dialog

class Chumi : public CDialog
{
	DECLARE_DYNAMIC(Chumi)

public:
	Chumi(CWnd* pParent = NULL);   // standard constructor
	virtual ~Chumi();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangenongdu();
	CEdit max_val;
	afx_msg void OnBnClickedButton1();
	 HANDLE m_tempFile;
};
