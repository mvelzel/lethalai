#pragma once
#include <Windows.h>
#include <vector>
#include "drawable.h"

namespace graphics {
    class GraphicsHandler {
        private:
            std::vector<Drawable*> draw_list;
        public:
            GraphicsHandler();

            void AddDrawable(Drawable* drawable);
            void Inject();
			void Draw(RECT const *rect);
            void InitGL();
    };
}
