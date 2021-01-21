#include "info_list.h"
#include <GL/GL.h>

namespace graphics {
    InfoList::InfoList(int x, int y) : Drawable(x, y) {
        this->episode_text = new Text(episode_text_pre + std::to_string(0),
                this->GetX() + this->padding, this->GetY() + (int) 2*this->padding);

        this->player1_text = new Text("Player 1",
                this->GetX() + this->padding, this->GetY() + 30 + (int) 2*this->padding);
        this->score_text_p1 = new Text(score_text_pre + std::to_string(0.0f),
                this->GetX() + this->padding, this->GetY() + 45 + (int) 2*this->padding);
        this->recent_text_p1 = new Text(recent_text_pre + std::to_string(0.0f),
                this->GetX() + this->padding, this->GetY() + 60 + (int) 2*this->padding);
        this->average_text_p1 = new Text(average_text_pre + std::to_string(0.0f),
                this->GetX() + this->padding, this->GetY() + 75 + (int) 2*this->padding);
        this->is_learning_text_p1 = new Text(learning_text_pre + "true",
                this->GetX() + this->padding, this->GetY() + 90 + (int) 2*this->padding);

        this->player2_text = new Text("Player 2",
                this->GetX() + this->padding, this->GetY() + 120 + (int) 2*this->padding);
        this->score_text_p2 = new Text(score_text_pre + std::to_string(0.0f),
                this->GetX() + this->padding, this->GetY() + 135 + (int) 2*this->padding);
        this->recent_text_p2 = new Text(recent_text_pre + std::to_string(0.0f),
                this->GetX() + this->padding, this->GetY() + 150 + (int) 2*this->padding);
        this->average_text_p2 = new Text(average_text_pre + std::to_string(0.0f),
                this->GetX() + this->padding, this->GetY() + 165 + (int) 2*this->padding);
        this->is_learning_text_p2 = new Text(learning_text_pre + "false",
                this->GetX() + this->padding, this->GetY() + 180 + (int) 2*this->padding);
    }

    void InfoList::Draw(const RECT *rect) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
        glColor4f(0, 0, 0, 0.5f);
        glVertex2f(this->GetX(), this->GetY());
        glVertex2f(this->GetX(), this->GetY() + this->padding * 2 + 195.0f);
        glVertex2f(this->GetX() + 9.0f * (11 + 20) + this->padding * 2,
                this->GetY() + this->padding * 2 + 195.0f);
        glVertex2f(this->GetX() + 9.0f * (11 + 20) + this->padding * 2, this->GetY());       
        glEnd();
        glDisable(GL_BLEND);
        this->episode_text->Draw(rect);

        this->player1_text->Draw(rect);
        this->score_text_p1->Draw(rect);
        this->average_text_p1->Draw(rect);
        this->recent_text_p1->Draw(rect);
        this->is_learning_text_p1->Draw(rect);

        this->player2_text->Draw(rect);
        this->score_text_p2->Draw(rect);
        this->average_text_p2->Draw(rect);
        this->recent_text_p2->Draw(rect);
        this->is_learning_text_p2->Draw(rect);
    }

    void InfoList::SetEpisode(int episode) {
        this->episode_text->SetText(this->episode_text_pre + std::to_string(episode));
    }

    void InfoList::SetScoreP1(float score) {
        this->score_text_p1->SetText(this->score_text_pre + std::to_string(score));
    }

    void InfoList::SetRecentP1(float event_score) {
        this->recent_text_p1->SetText(this->recent_text_pre + std::to_string(event_score));
    }

    void InfoList::SetAverageP1(float average) {
        this->average_text_p1->SetText(this->average_text_pre + std::to_string(average));
    }

    void InfoList::SetLearningP1(bool learning) {
        this->is_learning_text_p1->SetText(this->learning_text_pre + 
                (learning ? "true" : "false"));
    }

    void InfoList::SetScoreP2(float score) {
        this->score_text_p2->SetText(this->score_text_pre + std::to_string(score));
    }

    void InfoList::SetRecentP2(float event_score) {
        this->recent_text_p2->SetText(this->recent_text_pre + std::to_string(event_score));
    }

    void InfoList::SetAverageP2(float average) {
        this->average_text_p2->SetText(this->average_text_pre + std::to_string(average));
    }

    void InfoList::SetLearningP2(bool learning) {
        this->is_learning_text_p2->SetText(this->learning_text_pre + 
                (learning ? "true" : "false"));
    }
}
