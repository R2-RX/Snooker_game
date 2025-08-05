#include "../include/Game_components/Config.h"
#include "../include/Game_components/Cue.h"
#include "../include/Game_components/Ball.h"
#include "../include/Game_components/Table.h"
#include "../include/Graphic/Renderer.h"

using namespace std;
using namespace Biliard;

Renderer::Renderer(unsigned int width, unsigned int height, unsigned int frame)
    : window_(sf::VideoMode(width, height), "Ball Simulation"),
      table_(900, 400),
      cue_() // default-constructed
{   
    frame_ = frame;
    window_.setMouseCursorVisible(false);
    window_.setFramerateLimit(frame);

    update_scaling(width, height); 

    init_balls();

    if (!balls_.empty()) {
    cue_.init(0, 0, 0, 0, &table_, &balls_[0], sf::Vector2f(4.f, height * 2 / 3), Cue_Color);
    table_.Scale_play_area(scale_x_, scale_y_); 
    }
}

void Renderer::init_balls() {
    const float r = 10.0f;
    table_.SetTableMargin(2 * r);

    float w = table_.width();
    float h = table_.height();

    balls_.clear();

    // Cue ball (20% from left, 50% height)
    balls_.emplace_back(w * 0.2f, h * 0.5f, 0, 0, &table_, sf::Color::White, r);

    // Colors
    balls_.emplace_back(w * 0.05f, h * 0.1f, 0, 0, &table_, sf::Color(255, 255, 0), r);    // Yellow
    balls_.emplace_back(w * 0.05f, h * 0.9f, 0, 0, &table_, sf::Color(0, 128, 0), r);      // Green
    balls_.emplace_back(w * 0.05f, h * 0.5f, 0, 0, &table_, sf::Color(111, 46, 0), r);    // Brown
    balls_.emplace_back(w * 0.5f, h * 0.5f, 0, 0, &table_, sf::Color(0, 0, 255), r);       // Blue
    balls_.emplace_back(w * 0.70f, h * 0.5f, 0, 0, &table_, sf::Color(255, 200, 203), r);  // Pink
    balls_.emplace_back(w * 0.95f, h * 0.5f, 0, 0, &table_, sf::Color::Black, r);          // Black

    // Red triangle near pink
    float base_x = w * 0.72f;
    float base_y = h * 0.5f;
    int redCount = 0;

    for (int row = 0; row < 5; ++row) {
        for (int i = 0; i <= row; ++i) {
            float x = base_x + row * r * std::sqrt(3);
            float y = base_y - row * r + i * 2 * r;
            balls_.emplace_back(x, y, 0, 0, &table_, sf::Color(255, 20, 20), r);
            if (++redCount == 15) break;
        }
        if (redCount == 15) break;
    }
}

void Renderer::update_scaling(unsigned int width, unsigned int height) {
    scale_x_ = static_cast<float>(width) / table_.width();
    scale_y_ = static_cast<float>(height) / table_.height();
}

void Renderer::update_mouse_pos() {
    mouse_pos_ = window_.mapPixelToCoords(sf::Mouse::getPosition(window_));
}

void Renderer::process_input(const sf::Event& event) {
    constexpr float pi = 3.14159f;
    constexpr float speed_multiplier = 50.0f;
    static    float temp_angle = 0; 

    float sin_a = std::sin(temp_angle);
    float cos_a = std::cos(temp_angle);

    if (event.type == sf::Event::MouseMoved) {
        mouse_held_ = true;
    }
    
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
        mouse_held_ = true;
        if (collision_flag_) {
            balls_[Ball_number_].set_speed(
                -Cue_spring_ * sin_a * speed_multiplier,
                -Cue_spring_ * -cos_a * speed_multiplier
            );
        }
        collision_flag_ = false;
        Cue_spring_ = 0;
        Cue_angle_ = 0;
        temp_angle = 0;
        cue_.set_location(-5,0); // cheating
        cue_.Set_position(-5,0); // cheating
    }

    if (collision_flag_) {
        cue_.set_location(
            balls_[Ball_number_].x() + Cue_spring_ * sin_a,
            balls_[Ball_number_].y() - Cue_spring_ * cos_a
        );
        cue_.Set_position(
            balls_[Ball_number_].x() * scale_x_ + Cue_spring_ * sin_a,
            balls_[Ball_number_].y() * scale_y_ - Cue_spring_ * cos_a
        );
    }

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        collision_flag_ = false;
        Cue_spring_ = 0;
        Cue_angle_ = 0;
        temp_angle = 0;
        cue_.set_location(-5,0); // cheating
        cue_.Set_position(-5,0); // cheating

        strick_ = mouse_pos_;  // update the strick_

        cue_.Origin(cue_.Rect_size().x / 2, 0);
        cue_.set_location(strick_.x / scale_x_, strick_.y / scale_y_);
        cue_.Set_position(strick_.x, strick_.y);
    }

    if (event.type == sf::Event::MouseWheelScrolled) {
        if ((event.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel &&
            event.mouseWheelScroll.wheel != sf::Mouse::VerticalWheel)) {  // should call both events otherwise it won't work properly (|| !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))

            Cue_angle_ = std::fmod((Cue_angle_ - event.mouseWheelScroll.delta + 360.0f), 360.0f);
            temp_angle = cue_.wrapAngle(Cue_angle_) * pi / 180.0f;
        }
        else if ((event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel &&
                 event.mouseWheelScroll.wheel != sf::Mouse::HorizontalWheel)) { //(|| sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                  
            Cue_spring_ -= event.mouseWheelScroll.delta;
            Cue_spring_ = std::clamp(Cue_spring_, -25.0f, 0.0f);
        }
    }
}

void Renderer::handle_collision(int i) {
    cue_.Origin(&balls_[i]); 
    //cue_.Cue_angle_calculator(cue_.vx(), cue_.vy());
}

void Renderer::handle_window_events(const sf::Event& event) {
    if (event.type == sf::Event::Closed)
        window_.close();
}

void Renderer::draw_mouse_cursor(sf::RenderWindow& window) {
    if (!mouse_held_) return;

    sf::CircleShape C(cue_.scale_Rect_size(0.5).x);
    C.setFillColor(sf::Color::White);
    C.setOrigin(cue_.scale_Rect_size(0.5).x, cue_.scale_Rect_size(0.5).x);
    C.setPosition(mouse_pos_);
    window.draw(C);
}


void Renderer::run() {
    sf::Event event;

    while (window_.isOpen()) {  
      window_.clear(BackGround_Color);
      update_mouse_pos();

      while (window_.pollEvent(event)) {
        handle_window_events(event);
        process_input(event);
      }
      // Move all balls
      for (auto& ball : balls_)
        ball.Move(1.0 / frame_);
      
      for (size_t i = 0; i < balls_.size(); ++i) {
        if (cue_.collision(&balls_[i], scale_x_) && collision_flag_ == false) {
           Ball_number_ = i;
           collision_flag_ = true;
           handle_collision(i);
        }
        // Check collisions between all unique pairs
        for (size_t j = i + 1; j < balls_.size(); ++j) {
          if (balls_[i].ball_collision(&balls_[j],scale_x_)) {
            balls_[i].ResolveCollision(&balls_[j]);
          }
        }
      }
      //cue_.Rotation(float(Cue_angle_));

      for (auto& ball : balls_) {
        ball.Set_position(static_cast<float>(ball.x() * scale_x_), static_cast<float>(ball.y() * scale_y_));
        ball.Draw(window_);
      }

      draw_mouse_cursor(window_);
      // Draw table boundaries
      table_.Draw(window_);

      cue_.Draw(window_);

      window_.display();

    }
}
