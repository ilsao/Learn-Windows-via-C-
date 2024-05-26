#include <TCHAR.h>
#include <stdio.h>
#include <shlobj_core.h>
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

BOOL GetProcessElevation(TOKEN_ELEVATION_TYPE* pElevationType, BOOL* pIsAdmin) {
	HANDLE hToken = NULL;
	DWORD dwSize;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
		return FALSE;

	BOOL bResult = FALSE;
	if (GetTokenInformation(hToken, TokenElevationType, pElevationType,
		sizeof(TOKEN_ELEVATION_TYPE), &dwSize)) {

		BYTE adminSID[SECURITY_MAX_SID_SIZE];
		dwSize = sizeof(adminSID);
		CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &adminSID, &dwSize);

		if (*pElevationType == TokenElevationTypeLimited) {
			HANDLE hUnfilteredToken = NULL;
			GetTokenInformation(hToken, TokenLinkedToken, (VOID*)&hUnfilteredToken,
				sizeof(HANDLE), &dwSize);

			if (CheckTokenMembership(hUnfilteredToken, &adminSID, pIsAdmin)) {
				bResult = TRUE;
			}

			CloseHandle(hUnfilteredToken);
		}
	}
	else {
		*pIsAdmin = IsUserAnAdmin();
		bResult = TRUE;
	}

	CloseHandle(hToken);
	return bResult;
}

int main() {

	HMODULE hModule = GetModuleHandle(NULL);
	std::wcout << hModule << std::endl;
	
	hModule = NULL;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)main, &hModule);
	std::wcout << hModule << std::endl;

	TCHAR szCurDir[MAX_PATH];
	DWORD cchLength = GetFullPathName(TEXT("C:"), MAX_PATH, szCurDir, NULL);
	std::wcout << szCurDir << std::endl;

	/*
	* Create Process "notepad"
	* 
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	TCHAR szCommandLine[] = TEXT("notepad.exe");
	CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD dwExitCode;
	GetExitCodeProcess(pi.hProcess, &dwExitCode);

	std::cout << dwExitCode << std::endl;

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	*/

	/* 
	* Run the cmd in Administor mode
	*
	SHELLEXECUTEINFO sei = { sizeof(sei) };
	// This variable should always be set to "runas"
	sei.lpVerb = TEXT("runas");
	sei.lpFile = TEXT("cmd.exe");
	// Don't forget this parameter; otherwise, the window will be hidden
	sei.nShow = SW_SHOWNORMAL;
	ShellExecuteEx(&sei);

	*/
	TOKEN_ELEVATION_TYPE TokenType;
	BOOL IsAdmin;
	GetProcessElevation(&TokenType, &IsAdmin);
	
	if (TokenType == TokenElevationTypeDefault) {
		std::cout << "Default account permission" << std::endl;
	}
	else if (TokenType == TokenElevationTypeFull) {
		std::cout << "Process account permission has been upgraded" << std::endl;
	}
	else {
		std::cout << "limitted account permission" << std::endl;
	}

	if (IsAdmin) {
		std::cout << "Admin permission" << std::endl;
	}
	else {
		std::cout << "Not Admin permission" << std::endl;
	}

	system("pause");

	return 0;
}