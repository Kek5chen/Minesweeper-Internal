#pragma once

#include <Windows.h>
#include <stdio.h>
#include <map>
#include <vector>

namespace hack {
	void th_main(HINSTANCE hInstance);
	std::map<int, std::vector<int>> parseFieldMatrix(DWORD matrixStart);
	void makeHi(DWORD matrixStart, int* bombAmountSetting);
}