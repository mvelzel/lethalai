#pragma once
#include "../drawable.h"
#include <string>

namespace graphics {
    class Text : public Drawable {
        private:
            std::string text;
            float r;
            float g;
            float b;
        public:
            Text(std::string text, int x, int y,
                    float r=1.0f, float g=1.0f, float b=1.0f);

            void Draw(RECT const *rect) override;
            void SetText(std::string text);
    };
}
