#pragma once

#include <regex>

#include "Base.h"
#include "tinyexpr.h"

bool evaluate(Cell*, const std::vector<Cell>&, const std::vector<Column>&);