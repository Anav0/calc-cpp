#include "ProgramState.h"

Mode* ProgramState::getCurrentMode() 
{
	for (Mode* mode : _modes) 
	{
		if (mode->type == currentMode)
			return mode;
	}

	return NULL;
}