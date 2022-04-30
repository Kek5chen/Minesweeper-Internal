#include "hackmain.h"

#include "offsets.h"
#include "consoleutils.h"

namespace hack {
	typedef void(_stdcall* tPlaySound)(int soundId);
	tPlaySound playSound;
	typedef void(_stdcall* tRightClickField)(int fieldX, int fieldY);
	tRightClickField rightClickField;
	DWORD moduleBase;

	void th_main(HINSTANCE hInstance) {
		moduleBase = (DWORD) GetModuleHandle(0);
		CreateConsole();
		playSound = (tPlaySound)(moduleBase + funcPlaySound);
		rightClickField = (tRightClickField)(moduleBase + funcRightClickField);
		while (!GetAsyncKeyState(VK_END)) {
			if (GetAsyncKeyState(0x54) & 1) {
				*(int*)(moduleBase + iTimerValue) = 0;
				playSound(iSoundTimerTick);
			}
			if (GetAsyncKeyState(0x47) & 1) {
				DWORD fieldStart = (moduleBase + mFieldMatrix);
				std::map<int, std::vector<int>> fieldMatrix = parseFieldMatrix(fieldStart);
				for (unsigned int line = 1; line <= fieldMatrix.size(); line++) {
					std::vector<int> values = fieldMatrix[line];
					for (unsigned int value = 0; value < values.size(); value++) {
						if (values[value] & 0x80) {
							printf("M");
							if((values[value] & 0xF) == 0xF)
								rightClickField(value + 1, line);
						}
						else {
							printf("O");
						}
					}
					printf("\n");
				}
			}
			if (GetAsyncKeyState(0x46) & 1) {
				if (*(int*)(moduleBase + iFieldSizeX) < 9 || *(int*)(moduleBase + iFieldSizeY) < 9)
					continue;
				DWORD fieldStart = (moduleBase + mFieldMatrix);
				makeHi(fieldStart, (int*)(moduleBase + iBombAmountSetting));
			}
		}
		FreeConsole();
		FreeLibraryAndExitThread(hInstance, 0);
	}

	std::map<int, std::vector<int>> parseFieldMatrix(DWORD matrixStart) {
		std::map<int, std::vector<int>> parsedMatrix = std::map<int, std::vector<int>>();
		int endCounter = 0, concurrentEndLines = 0, currentLine = 1;
		DWORD currentOffset = 0;
		while (concurrentEndLines <= 2) {
			//printf("endCounter: %i\n", endCounter);
			BYTE currentValue = *(BYTE*)(matrixStart + currentOffset);
			currentOffset++;
			//printf("curVal: 0x%x\n", currentValue);
			if (currentValue == 0x10) {
				if (endCounter > 1) {
					endCounter--;
					currentLine++;
				} else
					endCounter++;
				concurrentEndLines++;
				continue;
			}
			concurrentEndLines = 0;
			if (endCounter == 1) {
				std::vector<int> values;
				if (parsedMatrix.count(currentLine) == 0)
					values = std::vector<int>();
				else
					values = parsedMatrix[currentLine];
				values.push_back(currentValue);
				parsedMatrix.insert_or_assign(currentLine, values);
			}
		}
		return parsedMatrix;
	}

	struct tuple {
		int x;
		int y;
	};

	tuple hiTuples[] = { {2,2} , {5, 2}, {8, 2}, {2, 3}, {5, 3}, {2, 4}, {5, 4}, {8, 4}, {2, 5}, {3, 5},
		{4, 5}, {5, 5}, {8, 5}, {2, 6}, {5, 6}, {8, 6}, {2, 7}, {5, 7}, {8, 7}, {2, 8}, {5, 8}, {8, 8} };

	void makeHi(DWORD matrixStart, int* bombAmountSetting) {
		int endCounter = 0, concurrentEndLines = 0, fieldY = 0, fieldX = 0;
		DWORD currentOffset = 0;
		while (concurrentEndLines <= 2) {
			//printf("endCounter: %i\n", endCounter);
			BYTE* currentValue = (BYTE*)(matrixStart + currentOffset);
			currentOffset++;
			//printf("curVal: 0x%x\n", currentValue);
			//printf("0x%x ", *currentValue);
			if (*currentValue == 0x10) {
				if (endCounter > 1) {
					endCounter--;
					fieldY++;
					fieldX = 1;
				} else
					endCounter++;
				concurrentEndLines++;
				continue;
			}
			concurrentEndLines = 0;
			if (endCounter == 1) {
				fieldX++;
				bool found = false;
				for (int i = 0; i < sizeof(hiTuples) / sizeof(tuple); i++) {
					if (hiTuples[i].x == fieldX - 1 && hiTuples[i].y == fieldY + 1) {
						*currentValue = 0x8F;	
						found = true;
					}
				}
				if(!found)
					*currentValue = 0x0F;
			}
			*bombAmountSetting = sizeof(hiTuples) / sizeof(tuple);
		}

	}
}