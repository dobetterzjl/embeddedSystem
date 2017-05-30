#pragma once


// DeviceIoControl 控代码定义
#define IOTRL_FPGA_SETBIT_REG16 0x01
#define IOTRL_FPGA_CLRBIT_REG16 0x02


// 寄存器地址
// READ
#define OFFSET_GPIN				0x010
#define OFFSET_DIP_REG_L		0x012
#define OFFSET_DIP_REG_H		0x013
#define OFFSET_SYS_STATUS		0x020
#define OFFSET_INT_FLAG			0x021
// WRITE
#define OFFSET_LED_ARRAY		0x000
#define OFFSET_GPOUT			0x010
#define OFFSET_LED				0x011
#define OFFSET_LED_REG_L		0x012
#define OFFSET_LED_REG_H		0x013
#define OFFSET_SYS_CTRL			0x020
#define OFFSET_INT_ENABLE		0x021


// 寄存器定义
// SYS_CTRL
#define REG_OE		(1 << 0)	// 八段数码管使能
#define ARRAY_OE	(1 << 1)	// LED点阵使能
#define DIP_OE		(1 << 2)	// 拨码开关使能


//通用FPGA函数

//打开FPGA设备
//返回值：成功：设备句柄，失败：INVALID_HANDLE_VALUE。
__inline HANDLE OpenFpga() {
	HANDLE hDev = CreateFile(L"FPG1:", GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);
	if(hDev == INVALID_HANDLE_VALUE) {
		RETAILMSG(1, (L"ERROR: Can not open FPG1:, error: %d\r\n", GetLastError()) );
		return INVALID_HANDLE_VALUE;
	}
	return hDev;
}


//读寄存器
//参数：hDev：设备句柄，offset：寄存器地址
//返回：寄存器值
__inline WORD ReadReg(HANDLE hDev, BYTE offset) {
	DWORD dwBytes; WORD wValue = 0;
	SetFilePointer(hDev, offset, NULL, 0); //选择寄存器
	ReadFile(hDev, &wValue, sizeof(wValue), &dwBytes, NULL); //读寄存器
	return wValue;
}


//写寄存器
//参数：hDev：设备句柄，offset：寄存器地址，wValue：要写入寄存器的值
//返回：无
__inline void WriteReg(HANDLE hDev, BYTE offset, WORD wValue) {
	DWORD dwBytes;
	SetFilePointer(hDev, offset, NULL, 0); //选择寄存器
	WriteFile(hDev, &wValue, sizeof(wValue), &dwBytes, NULL); //写寄存器
}


//寄存器位设置
//参数：hDev：设备句柄，offset：寄存器地址，wMask：当wMask中的位为1时，对应的寄存器位设置为1。
//返回：无
__inline void SetRegBit(HANDLE hDev, BYTE offset, WORD wMask) {
	SetFilePointer(hDev, offset, NULL, 0); //选择寄存器
	DeviceIoControl(hDev, IOTRL_FPGA_SETBIT_REG16, &wMask, sizeof(wMask), NULL, 0, NULL, NULL);
}


//寄存器位清除
//参数：hDev：设备句柄，offset：寄存器地址，wMask：当wMask中的位为1时，对应的寄存器位设置为0。
//返回：无
__inline void ClrRegBit(HANDLE hDev, BYTE offset, WORD wMask) {
	SetFilePointer(hDev, offset, NULL, 0); //选择寄存器
	DeviceIoControl(hDev, IOTRL_FPGA_CLRBIT_REG16, &wMask, sizeof(wMask), NULL, 0, NULL, NULL);
}

