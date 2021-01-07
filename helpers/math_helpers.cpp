#include "math_helpers.h"

namespace helpers {
    float Math::NormalizeRange(float in, float min, float max,
            float new_min, float new_max) {
        return (new_max-new_min)*(in-min)/(max-min)+new_min;
    }

    float Math::Normalize(float in, float min, float max) {
        return Math::NormalizeRange(in, min, max, 0, 1);
    }
}
