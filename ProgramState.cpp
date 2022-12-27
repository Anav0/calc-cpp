#include "ProgramState.h"

Mode* ProgramState::getCurrentMode() 
{
	for (Mode* mode : _modes) 
	{
		if (mode->type == CURRENT_MODE)
			return mode;
	}

	return NULL;
}