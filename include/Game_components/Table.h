#pragma once 
#include <SFML/Graphics.hpp>
#include "Ball.h"

namespace Biliard {

    class Ball; // Forward declaration

    class Table {
        public:
            Table(float width, float height);
            bool IsWithinBounds(float x, float y);
            void Reflect(Ball* b);
            float width() { return width_; }
            float height() { return height_; }
            void Draw(sf::RenderWindow& window) { window.draw(innerBoundary_); };
            void Scale_play_area(float scale_x, float scale_y);
            void SetTableMargin(float M) { margin_ = M; }
        private:
            float width_;
            float height_;
            float margin_;
            Ball* Ball_;
            sf::RectangleShape innerBoundary_;
    };
}