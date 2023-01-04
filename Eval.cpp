#include "Eval.h"

void evaluate(Cell* cell, const std::vector<Cell>& cells, const std::vector<Column>& columns) {
	std::string expr = cell->content;

	if(expr[0] == '=') 
		cell->formula = expr;

	if (cell->formula != "")
		expr = cell->formula;

	expr = expr.erase(0, 1); // Removes '='

	std::regex labelsRgx(R"(([A-Z])+(\d)+)");
	std::smatch match;

	bool haveCellPosToReplace = true;
	while (haveCellPosToReplace) {
		std::string exprCopy = expr;
		auto regexIterStart = std::sregex_iterator(exprCopy.begin(), exprCopy.end(), labelsRgx);
		auto regexIterEnd   = std::sregex_iterator();

		haveCellPosToReplace = false;
		for (std::sregex_iterator i = regexIterStart; i != regexIterEnd; ++i) {

			haveCellPosToReplace = true;

			std::smatch match = *i;
			std::string cellPos = match[0];

			//Note(Igor): this will not work on two and more char columns 
			std::string col = cellPos.substr(0, 1);
			int rowIndex = std::stoi(cellPos.substr(1)) - 1;

			int colIndex = (int)col[0] - 65;
			int cellIndex = (rowIndex * columns.size()) + colIndex;

			assert(cellIndex <= cells.size());

			auto c = cells[cellIndex];

			std::regex r(cellPos);
			expr = std::regex_replace(expr, r, c.content); //Info(Igor): slooooow
		}
	}

	int err = 0;
	double result = te_interp(expr.c_str(), &err);
	cell->formula = cell->content;

	if (err) {
		cell->content = "Err";
	} else {
		cell->content = std::to_string(result);
	}
}