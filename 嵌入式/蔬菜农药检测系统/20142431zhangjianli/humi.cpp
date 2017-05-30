// humi.cpp : implementation file
//

#include "stdafx.h"
#include "20142431zhangjianli.h"
#include "humi.h"
#include "string"
#include "atlconv.h"
using namespace std;

// Chumi dialog

IMPLEMENT_DYNAMIC(Chumi, CDialog)

Chumi::Chumi(CWnd* pParent /*=NULL*/)
	: CDialog(Chumi::IDD, pParent)
{

}

Chumi::~Chumi()
{
}

void Chumi::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_nongdu, max_val);
}


BEGIN_MESSAGE_MAP(Chumi, CDialog)
	ON_EN_CHANGE(IDC_nongdu, &Chumi::OnEnChangenongdu)
	ON_BN_CLICKED(IDC_BUTTON1, &Chumi::OnBnClickedButton1)
END_MESSAGE_MAP()


// Chumi message handlers

void Chumi::OnEnChangenongdu()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void Chumi::OnBnClickedButton1()
{
		USES_CONVERSION;
		CString temp;
		max_val.GetWindowTextW(temp);
		CStdioFile myFile;
		if(myFile.Open(_T("zjlnd.txt"),CFile::modeCreate|CFile::modeWrite))
		{
	
			myFile.WriteString(temp);

		}

		OnOK();
	// TODO: Add your control notification handler code here
}
