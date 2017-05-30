// shidu.cpp : implementation file
//

#include "stdafx.h"
#include "20142431zhangjianli.h"
#include "shidu.h"

#include "string"
#include "atlconv.h"
using namespace std;
// Cshidu dialog

IMPLEMENT_DYNAMIC(Cshidu, CDialog)

Cshidu::Cshidu(CWnd* pParent /*=NULL*/)
	: CDialog(Cshidu::IDD, pParent)
{

}

Cshidu::~Cshidu()
{
}

void Cshidu::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT3, max_val1);
}


BEGIN_MESSAGE_MAP(Cshidu, CDialog)
	ON_BN_CLICKED(IDOK, &Cshidu::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON5, &Cshidu::OnBnClickedButton5)
END_MESSAGE_MAP()


// Cshidu message handlers

void Cshidu::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void Cshidu::OnBnClickedButton5()

{
	USES_CONVERSION;
	//if(m_tempFile=CreateFile(_T("zjl.txt"),GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_ALWAYS,NULL,0))
	//{
		CString temp;
		max_val1.GetWindowTextW(temp);

	CStdioFile myFile;
	if(myFile.Open(_T("zjlshidu.txt"),CFile::modeCreate|CFile::modeWrite))
	{
	
		myFile.WriteString(temp);

	}
	 OnOK();

	// TODO: Add your control notification handler code here
}
