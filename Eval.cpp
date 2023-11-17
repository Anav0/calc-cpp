#include "Eval.h"

void evaluate(Cell* cell, const std::vector<Cell>& cells, const std::vector<Column>& columns) {
	std::string expr = cell->content;

	if (cell->formula != "") {
		expr = cell->formula;
	}

	expr = expr.erase(0, 1);

	std::regex labelsRgx(R"(([A-Z])+(\d)+)");
	std::smatch match;

	bool haveCellPosToReplace = true;
	while (haveCellPosToReplace) {
		std::string exprCopy = expr;
		auto regexIterStart = std::sregex_iterator(exprCopy.begin(), exprCopy.end(), labelsRgx);
		auto regexIterEnd = std::sregex_iterator();

		haveCellPosToReplace = false;
		for (std::sregex_iterator i = regexIterStart; i != regexIterEnd; ++i) {

			haveCellPosToReplace = true;

			std::smatch match = *i;

			assert(match.length() == 2);

			std::string cellPos = match[0];
			std::string col = match[1];

			int rowIndex = std::stoi(match[2]) - 1;
			int colIndex = (int)col[0] - 65;
			int cellIndex = (rowIndex * columns.size()) + colIndex;

			assert(cellIndex <= cells.size());

			auto c = cells[cellIndex];

			std::regex r(cellPos);
			expr = std::regex_replace(expr, r, c.content); //Info(Igor): slooooow
		}
	}

	double result = te_interp(expr.c_str(), 0);

	if (result == NAN) {
		printf("Failed to parse expression: '%s'", expr);
		assert(result != NAN);
	}

	cell->formula = cell->content;
	cell->content = std::to_string(result);
}