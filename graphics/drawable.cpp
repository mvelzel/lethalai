#include "drawable.h"

namespace graphics {
    Drawable::Drawable(int x, int y) {
        this->x = x;
        this->y = y;
    }

    int Drawable::GetX() {
        return this->x;
    }

    void Drawable::SetX(int x) {
        this->x = x;
    }

    int Drawable::GetY() {
        return this->y;
    }

    void Drawable::SetY(int y) {
        this->y = y;
    }
}
