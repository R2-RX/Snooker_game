#pragma once 
#include <SFML/Graphics.hpp>


namespace Biliard {

    class Table;  // Forward declaration

    class Ball {
        public:
            Ball(float x, float y, float vx, float vy, Table* table, sf::Color, int radius);
            void Move(float dt);
            float x() { return x_; }
            float y() { return y_; }
            float vx() { return vx_; }
            float vy() { return vy_; }
            float radius() {return radius_;}
            void Set_location(float _x, float _y);
            void set_speed(float _vx, float _vy);
            bool ball_collision(Ball*, float scale);
            void Set_position(float x , float y) { Cir_.setPosition(x, y); }
            void Draw(sf::RenderWindow& window) { window.draw(Cir_); };
            void ResolveCollision(Ball* b);
            void ApplyFriction(float dt, float friction_coefficient);
        private:
            float x_;
            float y_;
            float vx_;
            float vy_;
            float radius_;
            Table* table_;
            sf::CircleShape Cir_;
    };
}