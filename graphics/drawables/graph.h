#pragma once
#include "../drawable.h"
#include <GL/GL.h>
#include <Windows.h>
#include "text.h"

namespace graphics {
    class Graph : public Drawable {
        private:
            float range_x;
            float range_y;
            float mid_point;
            GLfloat *graph_vertices1;
            GLfloat *graph_vertices2;
            GLfloat *grid_lines;
            int gridline_size = 0;
            int data_size1 = 0;
            int data_size2 = 0;

            Text* graph_title;

            float graph_height;

            const float width = 260.0f;
            const float height = 260.0f;
            const float padding = 20.0f;
        public:
            Graph(int x, int y, float range_x, float range_y, float mid_point,
                    std::string graph_title);
            void AddData(float value, int graph_index);
            void Draw(RECT const *rect) override;
    };
}
