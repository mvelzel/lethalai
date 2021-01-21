#include "text.h"
#include "GL/freeglut_std.h"
#include <GL/glut.h>

namespace graphics {
    Text::Text(std::string text, int x, int y,
            float r, float g, float b) : Drawable(x, y) {
        this->text = text;
        this->r = r;
        this->g = g;
        this->b = b;
    }

    void Text::Draw(RECT const *rect) {
        glColor3f(this->r, this->g, this->b);
        glRasterPos2i(this->GetX(), this->GetY());
        for (char &c : this->text) {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
        }
    }

    void Text::SetText(std::string text) {
        this->text = text;
    }
}
