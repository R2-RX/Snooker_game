#pragma once 
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>

namespace Biliard {
    class Renderer {
        public:
            Renderer(unsigned int width, unsigned int height, unsigned int frame);
            void update_scaling(unsigned int width, unsigned int height);
            void update_mouse_pos();
            void process_input(const sf::Event& event);
            void handle_collision(int i);
            void handle_window_events(const sf::Event& event);
            void draw_mouse_cursor(sf::RenderWindow& window); // draws the dot
            void init_balls();
            void run();
        private:
            sf::RenderWindow window_;
            sf::Vector2f mouse_pos_, strick_;
            float Cue_angle_ = 0;
            float Cue_spring_ = 0;
            inline static bool mouse_held_ = true;
            inline static bool collision_flag_ = false;
            Table table_;
            std::vector<Ball> balls_;
            Cue_stick cue_;
            float scale_x_, scale_y_;
            unsigned int frame_, Ball_number_;
    };
}