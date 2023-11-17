#include "Changes.h"

void RowChange::applyRevert(std::vector<Cell>& cells)
{
	for (Cell& cell : cells) {
		cells.at(cell.index) = cell;
	}
}

void CellChange::applyRevert(std::vector<Cell>& cells)
{
	cells.at(cell.index) = cell;
}

void Changes::undo(std::vector<Cell>& cells) {
	if (queque.empty()) return;
	
	Change* change = queque.front();

	change->applyRevert(cells);

	queque.pop_front();
}