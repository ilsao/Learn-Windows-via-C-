#include <iostream>
#include <windows.h>


#ifdef UNICODE
#define StringReverse StringReverseW
#else
#define StringReverse StringReverseA
#endif

BOOL StringReverseW(PWSTR pWideCharStr, DWORD cchLength) {

	PWSTR pEndOfStr = pWideCharStr + wcsnlen_s(pWideCharStr, cchLength) - 1;

	wchar_t cCharT;

	while (pWideCharStr < pEndOfStr) {
		cCharT = *pWideCharStr;

		*pWideCharStr = *pEndOfStr;
		*pEndOfStr = cCharT;

		pWideCharStr++;
		pEndOfStr--;
	}

	return TRUE;
}

BOOL StringReverseA(PSTR pMultiByteStr, DWORD cchLength) {

	PWSTR pWideCharStr;
	int nLenOfWideCharStr;
	BOOL fOk = FALSE;

	nLenOfWideCharStr = MultiByteToWideChar(CP_ACP, 0, pMultiByteStr, cchLength, NULL, 0);

	// Remember that nLenOfWideCharStr is the number of characters, not the bytes.
	// So I need to multiply by the size of a wide character.
	pWideCharStr = (PWSTR)HeapAlloc(GetProcessHeap(), 0, nLenOfWideCharStr * sizeof(wchar_t));

	if (!pWideCharStr)
		return fOk;

	MultiByteToWideChar(CP_ACP, 0, pMultiByteStr, cchLength, pWideCharStr, nLenOfWideCharStr);

	fOk = StringReverseW(pWideCharStr, cchLength);

	if (fOk) {
		WideCharToMultiByte(CP_ACP, 0, pWideCharStr, cchLength, pMultiByteStr,
			(int)strlen(pMultiByteStr), NULL, NULL);
	}

	HeapFree(GetProcessHeap(), 0, pWideCharStr);

	return fOk;
}

int main() {

	HMODULE hModule = GetModuleHandle(NULL);
	std::wcout << hModule << std::endl;
	
	hModule = NULL;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)main, &hModule);
	std::wcout << hModule << std::endl;

	system("pause");
	return 0;
}