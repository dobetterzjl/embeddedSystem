#pragma once


// DeviceIoControl �ش��붨��
#define IOTRL_FPGA_SETBIT_REG16 0x01
#define IOTRL_FPGA_CLRBIT_REG16 0x02


// �Ĵ�����ַ
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


// �Ĵ�������
// SYS_CTRL
#define REG_OE		(1 << 0)	// �˶������ʹ��
#define ARRAY_OE	(1 << 1)	// LED����ʹ��
#define DIP_OE		(1 << 2)	// ���뿪��ʹ��


//ͨ��FPGA����

//��FPGA�豸
//����ֵ���ɹ����豸�����ʧ�ܣ�INVALID_HANDLE_VALUE��
__inline HANDLE OpenFpga() {
	HANDLE hDev = CreateFile(L"FPG1:", GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);
	if(hDev == INVALID_HANDLE_VALUE) {
		RETAILMSG(1, (L"ERROR: Can not open FPG1:, error: %d\r\n", GetLastError()) );
		return INVALID_HANDLE_VALUE;
	}
	return hDev;
}


//���Ĵ���
//������hDev���豸�����offset���Ĵ�����ַ
//���أ��Ĵ���ֵ
__inline WORD ReadReg(HANDLE hDev, BYTE offset) {
	DWORD dwBytes; WORD wValue = 0;
	SetFilePointer(hDev, offset, NULL, 0); //ѡ��Ĵ���
	ReadFile(hDev, &wValue, sizeof(wValue), &dwBytes, NULL); //���Ĵ���
	return wValue;
}


//д�Ĵ���
//������hDev���豸�����offset���Ĵ�����ַ��wValue��Ҫд��Ĵ�����ֵ
//���أ���
__inline void WriteReg(HANDLE hDev, BYTE offset, WORD wValue) {
	DWORD dwBytes;
	SetFilePointer(hDev, offset, NULL, 0); //ѡ��Ĵ���
	WriteFile(hDev, &wValue, sizeof(wValue), &dwBytes, NULL); //д�Ĵ���
}


//�Ĵ���λ����
//������hDev���豸�����offset���Ĵ�����ַ��wMask����wMask�е�λΪ1ʱ����Ӧ�ļĴ���λ����Ϊ1��
//���أ���
__inline void SetRegBit(HANDLE hDev, BYTE offset, WORD wMask) {
	SetFilePointer(hDev, offset, NULL, 0); //ѡ��Ĵ���
	DeviceIoControl(hDev, IOTRL_FPGA_SETBIT_REG16, &wMask, sizeof(wMask), NULL, 0, NULL, NULL);
}


//�Ĵ���λ���
//������hDev���豸�����offset���Ĵ�����ַ��wMask����wMask�е�λΪ1ʱ����Ӧ�ļĴ���λ����Ϊ0��
//���أ���
__inline void ClrRegBit(HANDLE hDev, BYTE offset, WORD wMask) {
	SetFilePointer(hDev, offset, NULL, 0); //ѡ��Ĵ���
	DeviceIoControl(hDev, IOTRL_FPGA_CLRBIT_REG16, &wMask, sizeof(wMask), NULL, 0, NULL, NULL);
}

