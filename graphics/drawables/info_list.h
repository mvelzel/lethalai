#pragma once
#include "../drawable.h"
#include "text.h"

namespace graphics {
    class InfoList : public Drawable {
        private:
            bool hide = true;
            Text* episode_text;
            Text* score_text_p1;
            Text* score_text_p2;
            Text* recent_text_p1;
            Text* recent_text_p2;
            Text* average_text_p1;
            Text* average_text_p2;
            Text* is_learning_text_p1;
            Text* is_learning_text_p2;
            Text* player1_text;
            Text* player2_text;

            const std::string score_text_pre = "Current Score: ";
            const std::string episode_text_pre = "Current Episode: ";
            const std::string recent_text_pre = "Most Recent Reward: ";
            const std::string average_text_pre = "Average Score: ";
            const std::string learning_text_pre = "Is Learning: ";

            const float padding = 10.0f;
        public:
            InfoList(int x, int y);

            void Draw(RECT const *rect) override;
            void ToggleHide();
            void SetEpisode(int episode);

            void SetScoreP1(float score);
            void SetRecentP1(float event_score);
            void SetAverageP1(float average);
            void SetLearningP1(bool learning);

            void SetScoreP2(float score);
            void SetRecentP2(float event_score);
            void SetAverageP2(float average);
            void SetLearningP2(bool learning);
    };
}
