#include <Windows.h>

#define DLL_EXPORT

#include "GetMd5.h"

BOOL GetMd5(LPCTSTR lpFileName, LPTSTR lpMd5) {
	HANDLE hFile, hFileMap;
	HCRYPTPROV hProv, hHash;
	TCHAR szContainerp[] = TEXT("MyKeyContainer");
	LPVOID lpMemory;

	hFile = CreateFile(lpFileName, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	// ��ȡָ�����ܷ����ṩ����(CSP)��ָ����Կ�����ľ��
	if (!CryptAcquireContext(&hProv, szContainerp, NULL, PROV_RSA_FULL, 0)) {
		if (!CryptAcquireContext(&hProv, szContainerp, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET)) {
			return FALSE;
		}
	}

	// �������ܷ����ṩ����(CSP)��ϣ����ľ������2������ָ��Ϊ��ͬ��ֵ���Ի�ȡ��ͬ�Ĺ�ϣ����SHA
	if (!CryptCreateHash(hProv, CALG_MD5, NULL, 0, &hHash)) {
		return FALSE;
	}

	// Ϊ���ܹ����������ļ�������ʹ���ڴ�ӳ���ļ���ΪhFile�ļ����󴴽�һ���ļ�ӳ���ں˶���
	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (!hFileMap) {
		return FALSE;
	}

	// ��ȡ�ļ���С���ڴ��������
	__int64 qwFileSize;
	DWORD dwFileSizeHigh;
	SYSTEM_INFO si;
	qwFileSize = GetFileSize(hFile, &dwFileSizeHigh);
	qwFileSize += (((__int64)dwFileSizeHigh) << 32);
	GetSystemInfo(&si);

	// ���ļ�ӳ�����hFileMap����ӳ�䵽���̵������ַ�ռ���
	__int64 qwFileOffset = 0; // �ļ�ӳ�����ƫ����
	DWORD dwByteInBlock; // ����ӳ���С
	while (qwFileSize > 0) {
		dwByteInBlock = si.dwAllocationGranularity;
		if (qwFileSize < dwByteInBlock) {
			dwByteInBlock = (DWORD)qwFileSize;

			lpMemory = MapViewOfFile(hFileMap, FILE_MAP_READ,
				(DWORD)(qwFileOffset >> 32), (DWORD)(qwFileOffset&0xFFFFFFFF), dwByteInBlock);
			if (!lpMemory) {
				return FALSE;
			}

			// ����ӳ�䲿�ֽ��в��������������ӵ���ϣ����
			if (!CryptHashData(hHash, (LPBYTE)lpMemory, dwByteInBlock, 0)) {
				return FALSE;
			}

			UnmapViewOfFile(lpMemory);
			qwFileOffset += dwByteInBlock;
			qwFileSize -= dwByteInBlock;
		}

		// ��ȡ��ϣֵ���ֽ���
		DWORD dwHashLen = 0;
		if (!CryptGetHashParam(hHash, HP_HASHVAL, NULL, &dwHashLen, 0)) {
			return FALSE;
		}

		// ��ȡ��ϣֵ
		LPBYTE lpHash = new BYTE[dwHashLen];
		if (!CryptGetHashParam(hHash, HP_HASHVAL, lpHash, &dwHashLen, 0)) {
			return FALSE;
		}

		for (DWORD i = 0; i < dwHashLen;  i++) {
			wsprintf(lpMd5 + i * 2, TEXT("%02X"), lpHash[i]);
		}
		delete[] lpHash;

		CloseHandle(hFileMap);
		CloseHandle(hFile);
		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
	}



	return TRUE;
}
