#define _USE_MATH_DEFINES
#include <cmath>

float radians(float degrees) {
    return degrees * M_PI / 180.0;
}

float degrees(float radians) {
    return radians * 180.0 / M_PI;
}

int comparefloats(float x, float y, float epsilon) {
    if (fabs(x-y) < epsilon) {
        return 0;
    }
    else if (x < y) {
        return -1;
    }
    else {
        return 1;
    }
}