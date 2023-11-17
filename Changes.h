#pragma once

#include <queue>
#include "Base.h"

class Change {
public:
	virtual void applyRevert(std::vector<Cell>&) = 0;
};

class CellChange : public Change {
	Cell cell;

public:
	CellChange(Cell cell)
	{
		this->cell = cell;
	};

	// Inherited via Change
	virtual void applyRevert(std::vector<Cell>&) override;
};

class RowChange : public Change {
	int row_index;
	std::vector<Cell> cells;

	// Inherited via Change
	virtual void applyRevert(std::vector<Cell>&) override;
};

struct Changes
{
	std::deque<Change*> queque;

	void undo(std::vector<Cell>&);
	void clear();
	int length();
};