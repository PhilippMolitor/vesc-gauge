#pragma once

#include <cmath>

#define Hz(x) (1000u / (x))

#define FLOAT_COMPARE_E(x, y, epsilon) (fabs((x) - (y)) < (epsilon))
#define FLOAT_COMPARE(x, y) FLOAT_COMPARE_E(x, y, 0.0001f)
