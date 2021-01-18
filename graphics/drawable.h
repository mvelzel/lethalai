#pragma once

namespace graphics {
    class Drawable {
        private:
            int x;
            int y;
        protected:
            int GetX();
            void SetX(int x);
            int GetY();
            void SetY(int y);
        public:
            Drawable(int x, int y);

            virtual void Draw() = 0;
    };
}