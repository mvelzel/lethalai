#ifndef MATH_HELPERS_H
#define MATH_HELPERS_H

namespace helpers {
    class Math {
        public:
            static float NormalizeRange(float in, float min, float max,
                    float new_min, float new_max);
            static float Normalize(float in, float min, float max);
    };
}

#endif
