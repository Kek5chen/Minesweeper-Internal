#pragma once

// ==== FUNCTIONS ====
#define funcPlaySound				0x38ED
#define funcRightClickField			0x374F

// ==== OFFSETS ====
#define iGameRunning				0x5164	
#define iUnflaggedBombAmount		0x5194
#define ibombAmount					0x5330
#define iFieldSizeX					0x5334	
#define iFieldSizeY					0x5338	
#define mFieldMatrixContainer		0x5340	
#define mFieldMatrix				0x5360	
#define iBombAmountSetting			0x56A4	
#define iFieldSizeSettingX			0x56A8	
#define iFieldSizeSettingY			0x56AC	
#define iSoundToggle				0x56B8	
#define iTimerValue					0x579C	
#define iTotalSquares				0x57A0	
#define iOpenedSquares				0x57A4

// ==== VALUES ====
#define iSoundTimerTick				0x1
#define iSoundWin					0x2
#define iSoundLose					0x3
#define iFieldArraySize				832

// ==== BITMASKS ====
#define	bRevealedBombField			0xA
#define	bRevealedNBombFlaggedField	0xB
#define	bQuestionmarkedField		0xD
#define	bFlaggedField				0xE
#define	bUnflaggedField				0xF
#define	bMatrixRowStartEnd			0x10
#define	bRevealedField				0x40
#define	bBombField					0x80
#define	bRevealedClickedBombField	0xCC