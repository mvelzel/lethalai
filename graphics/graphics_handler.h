#pragma once
#include <Windows.h>

namespace graphics {
    class GraphicsHandler {
        public:
            GraphicsHandler();

            void Inject();
			void Draw(RECT const *rect);
            void InitGL();
    };
}
