#pragma once 
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include "Ball.h"
#include "Table.h"

namespace Biliard {
    class Cue_stick {
        public:
            Cue_stick() {} //= default; 

            Cue_stick(float x, float y, float vx, float vy, Table* table, Ball* ball, sf::Vector2f Rect_size_, sf::Color color) {
                init(x, y, vx, vy, table, ball, Rect_size_, color);
            }
            void init(float x, float y, float vx, float vy, Table* table, Ball* ball, sf::Vector2f Rect_size, sf::Color color);
            float x() { return x_; }
            float y() { return y_; }
            float vx() { return vx_; }
            float vy() { return vy_; }
            sf::Vector2f Rect_size() { return Rect_size_; }
            sf::Vector2f scale_Rect_size(float scale) { return Rect_size_ * scale; }
            void set_location(float x, float y);
            void set_speed(float vx, float vy);
            void Set_position(float x , float y) { Rect_.setPosition(x, y); Dot_.setPosition(x,y); }
            void Draw(sf::RenderWindow& window) { window.draw(Dot_); window.draw(Rect_); }
            bool collision(Ball*, float scale);
            void ResolveCollision(Ball* b);
            void Rotation (float Degree) { Rect_.setRotation(Degree); Dot_.setRotation(Degree); }
            void Origin(Ball *b) { Rect_.setOrigin(Rect_size_.x/2, - Rect_size_.x/2-b->radius()); Dot_.setOrigin(Rect_size_.x/2, -Rect_size_.x-b->radius()/2); }
            void Origin(float x , float y) {Rect_.setOrigin(x, y); Dot_.setOrigin(x, x);}
            void Cue_angle_calculator(float vx, float vy);
            float wrapAngle(float angle);
        private:
            float x_;
            float y_;
            float vx_;
            float vy_;
            Ball* ball_;
            Table* table_;
            sf::RectangleShape Rect_;
            sf::CircleShape Dot_;
            sf::Vector2f Rect_size_;
    };
}
