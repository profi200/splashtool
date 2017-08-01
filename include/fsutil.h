#pragma once

#include <vector>
#include "types.h"



std::vector<u8> vectorFromFile(const char *const path);
bool vectorToFile(const std::vector<u8>& v, const char *const path);
