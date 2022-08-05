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

	// 获取指定加密服务提供程序(CSP)中指定密钥容器的句柄
	if (!CryptAcquireContext(&hProv, szContainerp, NULL, PROV_RSA_FULL, 0)) {
		if (!CryptAcquireContext(&hProv, szContainerp, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET)) {
			return FALSE;
		}
	}

	// 创建加密服务提供程序(CSP)哈希对象的句柄，第2个参数指定为不同的值可以获取不同的哈希例如SHA
	if (!CryptCreateHash(hProv, CALG_MD5, NULL, 0, &hHash)) {
		return FALSE;
	}

	// 为了能够处理大型文件，所以使用内存映射文件。为hFile文件对象创建一个文件映射内核对象
	hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (!hFileMap) {
		return FALSE;
	}

	// 获取文件大小，内存分配粒度
	__int64 qwFileSize;
	DWORD dwFileSizeHigh;
	SYSTEM_INFO si;
	qwFileSize = GetFileSize(hFile, &dwFileSizeHigh);
	qwFileSize += (((__int64)dwFileSizeHigh) << 32);
	GetSystemInfo(&si);

	// 把文件映射对象hFileMap不断映射到进程的虚拟地址空间中
	__int64 qwFileOffset = 0; // 文件映射对象偏移量
	DWORD dwByteInBlock; // 本次映射大小
	while (qwFileSize > 0) {
		dwByteInBlock = si.dwAllocationGranularity;
		if (qwFileSize < dwByteInBlock) {
			dwByteInBlock = (DWORD)qwFileSize;

			lpMemory = MapViewOfFile(hFileMap, FILE_MAP_READ,
				(DWORD)(qwFileOffset >> 32), (DWORD)(qwFileOffset&0xFFFFFFFF), dwByteInBlock);
			if (!lpMemory) {
				return FALSE;
			}

			// 对已映射部分进行操作，将数据添加到哈希对象
			if (!CryptHashData(hHash, (LPBYTE)lpMemory, dwByteInBlock, 0)) {
				return FALSE;
			}

			UnmapViewOfFile(lpMemory);
			qwFileOffset += dwByteInBlock;
			qwFileSize -= dwByteInBlock;
		}

		// 获取哈希值的字节数
		DWORD dwHashLen = 0;
		if (!CryptGetHashParam(hHash, HP_HASHVAL, NULL, &dwHashLen, 0)) {
			return FALSE;
		}

		// 获取哈希值
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

