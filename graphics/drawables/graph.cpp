#include "graph.h"
#include <algorithm>
#include <iostream>

namespace graphics {
    Graph::Graph(int x, int y, float range_x, float range_y, float mid_point,
            std::string graph_title) : Drawable(x, y) {
        this->range_x = range_x;
        this->range_y = range_y;
        this->mid_point = mid_point;
        this->graph_vertices2 = new GLfloat[range_x*3];
        this->graph_vertices1 = new GLfloat[range_x*3];
        this->graph_title = new Text(graph_title, x + this->padding, y + this->padding + 5.0f);
        this->graph_height = this->height - 20.0f;

        float mid_pos = -mid_point * (this->graph_height / range_y) + this->graph_height;

        this->grid_lines = new GLfloat[4*3] {
            x + this->padding, y + this->padding + 20.0f, 0.0f,
            x + this->padding, y + this->padding + 20.0f + this->graph_height, 0.0f,
            x + this->padding, y + this->padding + 20.0f + mid_pos, 0.0f,
            x + this->padding + this->width, y + this->padding + 20.0f + mid_pos, 0.0f
        };
        this->gridline_size = 4;
    }

    void Graph::AddData(float value, int graph_index) {
        float value_y = value + this->mid_point;
        value_y = (std::min)(this->range_y, (std::max)(0.0f, value_y));
        value_y = -value_y * (this->graph_height / this->range_y) + this->graph_height;
        value_y = this->GetY() + this->padding + value_y + 20.0f;
        if (((float) this->data_size1 >= this->range_x && graph_index == 0) ||
                ((float) this->data_size2 >= this->range_x && graph_index == 1)) {
            for (int i = 0; i + 4 <= (int)this->range_x*3; i += 3) {
                if (graph_index == 0)
                    this->graph_vertices1[i + 1] = this->graph_vertices1[i + 4];
                else
                    this->graph_vertices2[i + 1] = this->graph_vertices2[i + 4];
            }
            if (graph_index == 0)
                this->graph_vertices1[(int)range_x*3 - 2] = value_y;
            else
                this->graph_vertices2[(int)range_x*3 - 2] = value_y;
        } else {
            float value_x = (graph_index == 0 ? this->data_size1 : this->data_size2)
                * (this->width / this->range_x);
            value_x = this->GetX() + this->padding + value_x;
            if (graph_index == 0) {
                this->graph_vertices1[this->data_size1 * 3] = value_x;
                this->graph_vertices1[this->data_size1 * 3 + 1] = value_y;
                this->graph_vertices1[this->data_size1 * 3 + 2] = 0;
                this->data_size1 += 1;
            } else {
                this->graph_vertices2[this->data_size2 * 3] = value_x;
                this->graph_vertices2[this->data_size2 * 3 + 1] = value_y;
                this->graph_vertices2[this->data_size2 * 3 + 2] = 0;
                this->data_size2 += 1;
            }
        }
    }

    void Graph::Draw(const RECT *rect) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
        glColor4f(0, 0, 0, 0.5f);
        glVertex2f(this->GetX(), this->GetY());
        glVertex2f(this->GetX(), this->GetY() + this->padding * 2 + this->height);
        glVertex2f(this->GetX() + this->padding * 2 + this->width,
                this->GetY() + this->padding * 2 + this->height);
        glVertex2f(this->GetX() + this->padding * 2 + this->width, this->GetY());       
        glEnd();
        glDisable(GL_BLEND);

        this->graph_title->Draw(rect);

        glColor3f(1.0f, 1.0f, 1.0f);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, this->grid_lines);
        glDrawArrays(GL_LINE_STRIP, 0, this->gridline_size);
        glDisableClientState(GL_VERTEX_ARRAY);

        glEnable(GL_LINE_SMOOTH);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, this->graph_vertices1);
        glColor3f(1.0f, 0.0f, 0.0f);
        glDrawArrays(GL_LINE_STRIP, 0, this->data_size2);
        glDisableClientState(GL_VERTEX_ARRAY);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, this->graph_vertices2);
        glColor3f(0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_LINE_STRIP, 0, this->data_size2);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_LINE_SMOOTH);
    }
}
