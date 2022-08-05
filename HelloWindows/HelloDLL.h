#pragma once

#ifdef DLL_EXPORT
	#define DLL_VARABLE extern "C" __declspec(dllexport)
	#define DLL_CLASS __declspec(dllexport)
	#define DLL_API extern "C" __declspec(dllexport)
#else
	#define DLL_VARABLE extern "C" __declspec(dllimport)
	#define DLL_CLASS __declspec(dllimport)
	#define DLL_API extern "C" __declspec(dllimport)
#endif

typedef struct _POSITION {
	int x;
	int y;
}POSITION, * PPOSITION;

DLL_VARABLE int nValue;
DLL_VARABLE POSITION ps;


class DLL_CLASS HelloDLL {
public:
	HelloDLL(LPTSTR lpName, int nAge);
	~HelloDLL();

public:
	LPTSTR GetName();
	int GetAge();

private:
	TCHAR m_szName[64];
	int m_nAge;
};

DLL_API int WINAPI funcAdd(int a, int b);
DLL_API int WINAPI funcMul(int a, int b);