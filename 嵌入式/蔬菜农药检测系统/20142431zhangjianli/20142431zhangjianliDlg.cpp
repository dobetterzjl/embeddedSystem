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

static int rowCount = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//void Display(HANDLE hDev, DWORD value);



class CFontBitmap {
public:
	CFontBitmap() {
		m_hMemDC = NULL;
		m_hFont = NULL;
		m_hBitmap = NULL;
		m_Size.cx = m_Size.cy = 0;
		m_stride = 0;
		m_pBuffer = NULL;
	}
	~CFontBitmap() {
		if(m_hMemDC) DeleteDC(m_hMemDC);
		if(m_hFont) DeleteObject((HGDIOBJ)m_hFont);
		if(m_hBitmap) DeleteObject((HGDIOBJ)m_hBitmap);
		if(m_pBuffer) LocalFree((HLOCAL)m_pBuffer);
	}

	bool CreateMemDC(HWND hWnd) {
		HDC hdc = GetDC(hWnd);
		bool ret = CreateMemDC(hdc);
		ReleaseDC(hWnd, hdc);
		return ret;
	}

	bool CreateMemDC(HDC hdc) {
		if(m_hMemDC != NULL) {
			DeleteDC(m_hMemDC);
		}
		m_hMemDC = CreateCompatibleDC(hdc);
		ASSERT_RETAIL(m_hMemDC);
		return m_hMemDC ? true : false;
	}

	bool CreateFont(LONG lfHeight) {
		LOGFONT lf;
		if(m_hFont != NULL) {
			DeleteObject((HGDIOBJ)m_hFont);
		}
		memset(&lf, 0, sizeof(lf));
		lf.lfHeight = lfHeight;
		lf.lfWeight = FW_NORMAL;
		lf.lfCharSet = GB2312_CHARSET;
		wcscat(lf.lfFaceName, L"simsun");
		m_hFont = CreateFontIndirect(&lf);
		ASSERT_RETAIL(m_hFont);
		return m_hFont ? true : false;
	}

	bool CreateBitmapBuffer(LPCTSTR lpszText, int len) {
		if(GetTextExtentPoint(m_hMemDC, lpszText, len, &m_Size)) {
			RETAILMSG(1, (L"Font: %d,%d\r\n", m_Size.cx, m_Size.cy) );
			m_stride = (m_Size.cy + 7) / 8;
			UINT uBytes = m_stride * m_Size.cx;
			if(m_pBuffer) LocalFree((HLOCAL)m_pBuffer);
			m_pBuffer = (PBYTE)LocalAlloc(LPTR, uBytes);
		}
		ASSERT_RETAIL(m_pBuffer);
		return m_pBuffer ? true : false;
	}

	void CreateFontBitmap(LPCTSTR lpszText)
	{
		RECT rect;
		PBYTE pRow, pRowTop;
		ASSERT_RETAIL(m_hFont);
		ASSERT_RETAIL(m_hMemDC);
		int len = wcslen(lpszText);
		SetBkMode(m_hMemDC, OPAQUE);
		SetBkColor(m_hMemDC, RGB(255, 255, 255));
		SetTextColor(m_hMemDC, RGB(0, 0, 0));
		HGDIOBJ hOldFont = SelectObject(m_hMemDC, (HGDIOBJ)m_hFont);
		if(CreateBitmapBuffer(lpszText, len))
		{
			pRow = pRowTop = m_pBuffer;
			const UINT eto = ETO_CLIPPED|ETO_OPAQUE;
			SetRect(&rect, 0, 0, m_Size.cx, m_Size.cy);
			if(m_hBitmap) DeleteObject((HGDIOBJ)m_hBitmap);
			m_hBitmap = CreateCompatibleBitmap(m_hMemDC, rect.right, rect.bottom);
			HGDIOBJ hOldBitmap = SelectObject(m_hMemDC, (HGDIOBJ)m_hBitmap);
			ExtTextOut(m_hMemDC, rect.left, rect.top, eto, &rect, lpszText, len, NULL);
			for(int x = 0; x < m_Size.cx; x++) {
				for(int y = m_Size.cy - 1; y >= 0; ) {
					BYTE row_data = 0;
					for(int i=0; y >= 0 && i < 8; y--, i++) {
						if(GetPixel(m_hMemDC, x, y) == RGB(0,0,0)) {
							row_data |= 1 << i;
						}
					}
					*pRow++ = row_data;
				}
				pRow = (pRowTop += m_stride);
			}
			//BitBlt(hdc, 0, 0, rect.right, rect.bottom, m_hMemDC, 0, 0, SRCCOPY);
			SelectObject(m_hMemDC, hOldBitmap);
		}
		SelectObject(m_hMemDC, hOldFont);
	}

	int Rows() {
		return m_Size.cx;
	}

	int Columns() {
		return m_Size.cy;
	}

	int Stride() {
		return m_stride;
	}

	bool GetRowData(int row, PBYTE data, int len) {
		ASSERT_RETAIL(m_pBuffer);
		ASSERT_RETAIL(data && len);
		if(row >= Rows()) return false;
		PBYTE pRow = m_pBuffer + m_stride * row;
		memcpy(data, pRow, min(len, m_stride));
		return true;
	}

private:
	HDC		m_hMemDC;
	HFONT	m_hFont;
	HBITMAP m_hBitmap;
	SIZE	m_Size;
	LONG	m_stride;
	PBYTE	m_pBuffer;
};
CFontBitmap g_bitmap;

class CLedArray {
public:
	CLedArray() {
		m_hWnd = NULL;
		m_hMemDC = NULL;
		m_hBitmap = NULL;
		m_hDev = INVALID_HANDLE_VALUE;
	}
	~CLedArray() {
		Close();
		if(m_hMemDC) DeleteDC(m_hMemDC);
		if(m_hBitmap) DeleteObject((HGDIOBJ)m_hBitmap);
	}

	bool Open() {
		if(m_hDev == INVALID_HANDLE_VALUE) {
			if((m_hDev = OpenFpga()) != INVALID_HANDLE_VALUE) {
				SetRegBit(m_hDev, OFFSET_SYS_CTRL, ARRAY_OE);
				SetDataAll(0);
				Display();
			}
		}
		return m_hDev != INVALID_HANDLE_VALUE;
	}

	void Close() {
		if(m_hDev != INVALID_HANDLE_VALUE) {
			ClrRegBit(m_hDev, OFFSET_SYS_CTRL, ARRAY_OE);
			CloseHandle(m_hDev);
			m_hDev = INVALID_HANDLE_VALUE;
		}
	}

	void Display() {
		if(m_hDev != INVALID_HANDLE_VALUE) {
			for(BYTE i=0; i<16; i++) {
				WriteReg(m_hDev, OFFSET_LED_ARRAY+i, m_wLeds[i]);
			}
		}
		if(m_hWnd && m_hMemDC) {
			int w = m_Rect.right - m_Rect.left;
			int h = m_Rect.bottom - m_Rect.top;
			int cx = w / 16; int cy = h / 16;
			int ox = (w%16)/2; int oy = (h%16)/2;
			RECT rect = {0, 0, w, h};
			FillRect(m_hMemDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
			for(int y=0; y < 16; y++) {
				for(int x=0; x < 16; x++) {
					if(m_wLeds[x] & (1<<(15-y))) {
						int px = cx * x + ox; int py = cy * y + oy;
						Ellipse(m_hMemDC, px, py, px+cx-1, py+cy-1);
					}
				}
			}
			InvalidateRect(m_hWnd, &m_Rect, FALSE);
		}
	}

	void OnPaint(HDC hdc) {
		if(m_hMemDC) {
			BitBlt(hdc, m_Rect.left, m_Rect.top, m_Rect.right
				, m_Rect.bottom, m_hMemDC, 0, 0, SRCCOPY);
		}
	}

	void Scroll(WORD wData) {
		for(int i=0; i<15; i++)
			m_wLeds[i] = m_wLeds[i+1];
		m_wLeds[15] = wData;
	}

	void SetData(WORD wData[16]) {
		memcpy(m_wLeds, wData, sizeof(m_wLeds));
	}

	void SetDataAll(BYTE data) {
		memset(m_wLeds, data, sizeof(m_wLeds));
	}

	void SetWnd(HWND hWnd, RECT *rect) {
		m_hWnd = hWnd;
		HDC hdc = GetDC(hWnd);
		CopyRect(&m_Rect, rect);
		if(m_hMemDC) DeleteDC(m_hMemDC);
		m_hMemDC = CreateCompatibleDC(hdc);
		int w = m_Rect.right - m_Rect.left;
		int h = m_Rect.bottom - m_Rect.top;
		if(m_hBitmap) DeleteObject((HGDIOBJ)m_hBitmap);
		m_hBitmap = CreateCompatibleBitmap(hdc, w, h);
		SelectObject(m_hMemDC, (HGDIOBJ)m_hBitmap);
		ReleaseDC(hWnd, hdc);
	}

private:
	HWND	m_hWnd;
	RECT	m_Rect;
	HDC		m_hMemDC;
	HBITMAP	m_hBitmap;
	HANDLE	m_hDev;
	WORD	m_wLeds[16];
};


//CFontBitmap g_bitmap;
CLedArray   g_dev2;


LPCTSTR DataToStr(WORD data) {
	static TCHAR szStr[32];
	for(int i=0; i<16; i++) {
		if(data & (1<<i))
			szStr[i] = L'X';
		else
			szStr[i] = L'_';
	}
	szStr[16] = 0;
	return szStr;
}

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
	ON_STN_CLICKED(IDC_STATIC3, &CMy20142431zhangjianliDlg::OnStnClickedStatic3)
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
/////////////////////dianzhen
	g_dev2.Open();
	HWND hDlg;
	g_bitmap.CreateMemDC(hDlg);
		g_bitmap.CreateFont(16);
	WORD data[16]; rowCount = 0;
				g_bitmap.CreateFontBitmap(L"农药检测系统");
				for(int i=0; i < 16; i++) {
					if(rowCount >= g_bitmap.Rows()) rowCount = 0;
					g_bitmap.GetRowData(rowCount++, (PBYTE)&data[i], 2);
					DEBUGMSG(1, (L"%s\r\n", DataToStr(data[i])) );
				}
				g_dev2.SetData(data);
				g_dev2.Display();

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
	//点阵
	
			WORD data = 0;
			if(rowCount >= g_bitmap.Rows()) rowCount = 0;
			g_bitmap.GetRowData(rowCount++, (PBYTE)&data, 2);
			DEBUGMSG(1, (L"%s\r\n", DataToStr(data)) );
			g_dev2.Scroll(data);
			g_dev2.Display();
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

void CMy20142431zhangjianliDlg::OnStnClickedStatic3()
{
	// TODO: Add your control notification handler code here
}
