// 20142431zhangjianliDlg.cpp : implementation file
//

#include "stdafx.h"
#include "20142431zhangjianli.h"
#include "20142431zhangjianliDlg.h"
#include "fpga_def.h"
#include "humi.h"
#include "shidu.h"

#include "atlconv.h"
#include <windows.h>
#include <nled.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//void Display(HANDLE hDev, DWORD value);

///shumaguan

void Display(HANDLE hDev, DWORD value)
{
	DWORD num0, num1, num2, num3;
	static const BYTE code[] = {
		0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
		0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71
	};

	num0  = value % 10; value = value / 10;
	num1  = value % 10; value = value / 10;
	num2  = value % 10; value = value / 10;
	num3  = value % 10; value = value / 10;

	UINT16 ledreg_h = ((UINT16)code[num0] << 8) | ((UINT16)code[num1] << 0);
	UINT16 ledreg_l = ((UINT16)code[num2] << 8) | ((UINT16)code[num3] << 0);

	WriteReg(hDev, OFFSET_LED_REG_H, ledreg_h);
	WriteReg(hDev, OFFSET_LED_REG_L, ledreg_l);
}
	HANDLE ha_hDev = INVALID_HANDLE_VALUE;//声明数码管

///
// CMy20142431zhangjianliDlg dialog
////led

class CLed {
public:
	CLed() {
		m_hDev = INVALID_HANDLE_VALUE;
	}
	~CLed() {
		Close();
	}

	bool Open() {
		if(m_hDev == INVALID_HANDLE_VALUE) {
			m_hDev = OpenFpga();
		}
		return m_hDev != INVALID_HANDLE_VALUE;
	}

	void Close() {
		if(m_hDev != INVALID_HANDLE_VALUE) {
			CloseHandle(m_hDev);
			m_hDev = INVALID_HANDLE_VALUE;
		}
	}

	void SetCpuLed(WORD dwMask) {
		NLED_SETTINGS_INFO info;
		for(UINT nId=0; nId<3; nId++) {
			info.LedNum = nId;
			info.OffOnBlink = ((dwMask & (1<<nId)) ? 1 : 0);//初始运行时状态，若为1，则亮；若为0，则灭
			NLedSetDevice(NLED_SETTINGS_INFO_ID, &info);
		}
	}

	void SetCpuLed(WORD nId, BOOL bOnOff) {
		NLED_SETTINGS_INFO info;
		info.LedNum = nId;
		info.OffOnBlink = (bOnOff ? 1 : 0);//运行过程中控制灯的亮灭，若为0，无论如何选都是灭灯状态
		NLedSetDevice(NLED_SETTINGS_INFO_ID, &info);
	}

	void SetFpgaLed(WORD dwMask) {
		WriteReg(m_hDev, OFFSET_LED, ~dwMask & 0x3f);//初始运行时状态，若WriteReg(m_hDev, OFFSET_LED,1);则初始运行时全亮
	}

	void SetFpgaLed(WORD nId, BOOL bOnOff) {
		if(nId < 6) {
			if(bOnOff) {
				ClrRegBit(m_hDev, OFFSET_LED, (1<<nId));//运行过程中控制某个灯亮和灭。如ClrRegBit(m_hDev, OFFSET_LED, 1);只能控制第一个灯的亮与灭
			} else {
				SetRegBit(m_hDev, OFFSET_LED, (1<<nId));//运行过程中控制某个灯亮和灭。如SetRegBit(m_hDev, OFFSET_LED, (1<<nId));只能控制第一个灯的亮与灭
			}
		}
	}

private:
	HANDLE m_hDev;
};
	CLed	g_dev;//声明LED类


////led
CMy20142431zhangjianliDlg::CMy20142431zhangjianliDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMy20142431zhangjianliDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy20142431zhangjianliDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMy20142431zhangjianliDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CMy20142431zhangjianliDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMy20142431zhangjianliDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CMy20142431zhangjianliDlg message handlers

BOOL CMy20142431zhangjianliDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
///////////////打开设备数码管
	if((ha_hDev = OpenFpga()) == INVALID_HANDLE_VALUE) {
		MessageBox( _T("不能打开FPGA设备FPG1:。"), _T("Error"), MB_OK | MB_ICONINFORMATION );
		} else {
			WriteReg(ha_hDev, OFFSET_LED_REG_H, 0);
			WriteReg(ha_hDev, OFFSET_LED_REG_L, 0);
			SetRegBit(ha_hDev, OFFSET_SYS_CTRL, REG_OE);
			
		}
///打开AD模拟
	// TODO: Add extra initialization here
	m_hDev = CreateFile(_T("ADS1:"), GENERIC_READ|GENERIC_WRITE
		, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	//g_dev.Open
///打开拨码开关
	m_hDev1= OpenFpga();
	SetRegBit(m_hDev1,OFFSET_SYS_CTRL,DIP_OE);
	SetTimer(1, 100, NULL);
	
////////led初始化
	g_dev.SetCpuLed(0);

	if(g_dev.Open())
	{
	g_dev.SetFpgaLed(0,false);
	g_dev.SetFpgaLed(1,false);
	g_dev.SetFpgaLed(2,false);
	g_dev.SetFpgaLed(3,false);
	g_dev.SetFpgaLed(4,false);
	g_dev.SetFpgaLed(5,false);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control除非将焦点设置到控件，否则返回 TRUE
}
///初始化结束

//#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
//void CMy20142431zhangjianliDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
/*{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_MY20142431ZHANGJIANLI_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_MY20142431ZHANGJIANLI_DIALOG));
	}
}
#endif*/

void CMy20142431zhangjianliDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
// 拨码开关 
	DWORD dwState;
    float value;
    TCHAR szValue[16];
	if(m_hDev1 != INVALID_HANDLE_VALUE) {
			 dwState = ReadReg(m_hDev1, OFFSET_DIP_REG_L);
			dwState= dwState | (DWORD)ReadReg(m_hDev, OFFSET_DIP_REG_H) << 16;
		} 
	value = dwState;
//////AD模拟
	float value2;
	if(nIDEvent == 1 && m_hDev != INVALID_HANDLE_VALUE) {
		DWORD dwBtyes;
		if(ReadFile(m_hDev, &value2, sizeof(value2), &dwBtyes, NULL)) {
			value2 = (value2+2.1)/4.2*100;
			_stprintf(szValue, _T("%.3f"), value2);
			//SetDlgItemText(IDC_STATIC2, szValue);
			//SetDlgItemText(IDC_STATIC3, szValue);
		}
	}
	if(value==1){
         SetDlgItemText(IDC_STATIC2, szValue);
		///////////////////////////////////////////////////
		 CString str;
	     CStdioFile myFile;
		if(myFile.Open(_T("zjlnd.txt"),CFile::modeRead))
		{
		
			myFile.ReadString(str);
		}
		USES_CONVERSION;
		float f;
		f = atof(W2CA((LPCWSTR)str));
		Display(ha_hDev, f);
	

		if(f<value2){	
		    g_dev.SetFpgaLed(0,true);
			g_dev.SetFpgaLed(1,true);
			g_dev.SetFpgaLed(2,true);
			g_dev.SetFpgaLed(3,true);
			g_dev.SetFpgaLed(4,true);
			g_dev.SetFpgaLed(5,true);
			SetDlgItemText(IDC_STATICTEST, _T("危险"));
		}
		else
		{
		   g_dev.SetFpgaLed(0,false);
		   g_dev.SetFpgaLed(1,false);
		   g_dev.SetFpgaLed(2,false);
		   g_dev.SetFpgaLed(3,false);
	       g_dev.SetFpgaLed(4,false);
	       g_dev.SetFpgaLed(5,false);
		   SetDlgItemText(IDC_STATICTEST, _T("安全"));
		}

	}
	///////////////////////////////////////////////////////////

	else{
	    SetDlgItemText(IDC_STATIC3, szValue);


		CString str;

	    CStdioFile myFile;
	
		if(myFile.Open(_T("zjlshidu.txt"),CFile::modeRead))
		{
		
			myFile.ReadString(str);
		}
		
		USES_CONVERSION;
		float f;
		f = atof(W2CA((LPCWSTR)str));
		Display(ha_hDev, f);
	/////////////////////////////

		if(f<value2){	
		    g_dev.SetCpuLed(0,true);
			g_dev.SetCpuLed(1,true);
			g_dev.SetCpuLed(2,true);
			g_dev.SetCpuLed(3,true);
			g_dev.SetCpuLed(4,true);
			g_dev.SetCpuLed(5,true);
			SetDlgItemText(IDC_STATICSD, _T("危险"));
		}
		else
		{
		   g_dev.SetCpuLed(0,false);
		   g_dev.SetCpuLed(1,false);
		   g_dev.SetCpuLed(2,false);
		   g_dev.SetCpuLed(3,false);
	       g_dev.SetCpuLed(4,false);
	       g_dev.SetCpuLed(5,false);
		   SetDlgItemText(IDC_STATICSD, _T("安全"));
		}

	}
	CDialog::OnTimer(nIDEvent);
}

void CMy20142431zhangjianliDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	Chumi dlg;
	dlg.DoModal();
	
}

void CMy20142431zhangjianliDlg::OnBnClickedButton2()
{
	Cshidu dlg;
	dlg.DoModal();
	// TODO: Add your control notification handler code here
}
