#pragma once


// Cdl dialog

class Cdl : public CDialog
{
	DECLARE_DYNAMIC(Cdl)

public:
	Cdl(CWnd* pParent = NULL);   // standard constructor
	virtual ~Cdl();

// Dialog Data
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};
