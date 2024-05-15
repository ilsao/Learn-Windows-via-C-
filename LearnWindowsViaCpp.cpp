#include <iostream>
#include <windows.h>

int main() {

	WCHAR c = TEXT('a');

	WCHAR szZBuffer[3] = L"";
	wcscpy_s(szZBuffer, L"hi");

	std::cout << szZBuffer;

	return 0;
}