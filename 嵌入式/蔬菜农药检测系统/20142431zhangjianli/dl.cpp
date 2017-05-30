// dl.cpp : implementation file
//

#include "stdafx.h"
#include "20142431zhangjianli.h"
#include "dl.h"


// Cdl dialog

IMPLEMENT_DYNAMIC(Cdl, CDialog)

Cdl::Cdl(CWnd* pParent /*=NULL*/)
	: CDialog(Cdl::IDD, pParent)
{

}

Cdl::~Cdl()
{
}

void Cdl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Cdl, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &Cdl::OnBnClickedButton1)
END_MESSAGE_MAP()


// Cdl message handlers

void Cdl::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	CString s;
	
	GetDlgItem(IDC_EDIT1)->GetWindowTextW(s);
	
	if(s != "20142431")
		exit(0);
	OnOK();

}
