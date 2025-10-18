/*
 * ZenithOS SDK - Header File
 *
 * Copyright (C) 2025 ne5link
 *
 * Licensed under the GNU General Public License v3.0 (GPLv3).
 * See <https://www.gnu.org/licenses/> for details.
 *
 * Made by ne5link <3
 */

#ifndef ZEN_MATH_H
#define ZEN_MATH_H

#include <math.h>
#include <stdlib.h>
#include <time.h>

// basic arithmetic operations
static inline double add(double a, double b) { return a + b; }
static inline double subtract(double a, double b) { return a - b; }
static inline double multiply(double a, double b) { return a * b; }
static inline double divide(double a, double b) { return (b != 0) ? a / b : 0; }

// trigonometric functions
static inline double zen_sin(double angle) { return sin(angle); }
static inline double zen_cos(double angle) { return cos(angle); }
static inline double zen_tan(double angle) { return tan(angle); }
static inline double zen_arctan(double x) { return atan(x); }
static inline double zen_arctan2(double y, double x) { return atan2(y, x); }

// algebraic functions
static inline double power(double base, double exponent) { return pow(base, exponent); }
static inline double zen_sqrt(double value) { return sqrt(value); }
static inline double logarithm(double value) { return log(value); }
static inline double logarithm10(double value) { return log10(value); }
static inline double zen_exp(double value) { return exp(value); }
static inline double absolute(double value) { return fabs(value); }
static inline double round_zen(double value) { return round(value); }
static inline double floor_zen(double value) { return floor(value); }
static inline double ceil_zen(double value) { return ceil(value); }
static inline double clamp(double value, double min, double max) {
    return (value < min) ? min : (value > max) ? max : value;
}
static inline double lerp(double a, double b, double t) { return a + t * (b - a); }

// angle conversions
static inline double deg_to_rad(double deg) { return deg * (M_PI / 180.0); }
static inline double rad_to_deg(double rad) { return rad * (180.0 / M_PI); }

// min/max
static inline double max_double(double a, double b) { return (a > b) ? a : b; }
static inline double min_double(double a, double b) { return (a < b) ? a : b; }

#endif // ZEN_MATH_H

