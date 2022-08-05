#pragma once

#ifdef DLL_EXPORT
	#define DLL_API extern "C" __declspec(dllexport)
#else
	#define DLL_API extern "C" __declspec(dllimport)
#endif 

DLL_API BOOL GetMd5(LPCTSTR lpFileName, LPTSTR lpMd5);
