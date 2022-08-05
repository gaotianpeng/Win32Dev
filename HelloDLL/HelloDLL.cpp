#include <Windows.h>
#include <strsafe.h>
#include <tchar.h>

#define DLL_EXPORT 
#include "HelloDLL.h"


int nValue;
POSITION ps;

HelloDLL::HelloDLL(LPTSTR lpName, int nAge) {
	if (m_szName) {
		StringCchCopy(m_szName, _countof(m_szName), lpName);
	}

	m_nAge = nAge;
}

HelloDLL::~HelloDLL() {
}

LPTSTR HelloDLL::GetName() {
	return m_szName;
}

int HelloDLL::GetAge() {
	return m_nAge;
}

int WINAPI funcAdd(int a, int b) {
	return a + b;
}

int WINAPI funcMul(int a, int b) {
	return a * b;
}
