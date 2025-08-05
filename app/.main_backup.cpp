#include <SFML/Graphics.hpp>
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

#define Cue_Color sf::Color(150, 75, 0)
#define BackGround_Color sf::Color(31, 78, 47)
#define Friction 0.5

class Ball;

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

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

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

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

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

Table::Table(float width, float height) {
  if (width - margin_ <= 0 || height - margin_ <= 0)
    throw invalid_argument("Table dimensions must be positive");
  width_ = width;
  height_ = height;
  margin_ = 0;  

  // boarder table line
  innerBoundary_.setFillColor(sf::Color::Transparent); // No fill
  innerBoundary_.setOutlineThickness(1.0f); 
  innerBoundary_.setOutlineColor(sf::Color::White);  
}

bool Table::IsWithinBounds(float x, float y) {
    return x >= margin_ && x <= width_ - margin_ &&
          y >= margin_ && y <= height_ - margin_;
}
  
void Table::Scale_play_area(float scale_x, float scale_y) {
  innerBoundary_.setPosition(margin_/2* scale_x, margin_/2 * scale_y ); 
  innerBoundary_.setSize(sf::Vector2f((width_ - margin_ )* scale_x, (height_ - margin_) * scale_y ));
}

void Table::Reflect(Ball* b) {
  float x = b->x();
  float y = b->y();
  float vx = b->vx();
  float vy = b->vy();
  
  if (!IsWithinBounds(x, y)) {
    if (x < margin_) {
      x = 2 * margin_ - x;
      vx = -vx;
    }
    if (x >= width_ - margin_) {
      x = 2 * (width_ - margin_) - x;
      vx = -vx;
    }
    if (y < margin_) {
      y = 2 * margin_ - y;
      vy = -vy;
    }
    if (y >= height_ - margin_) {
      y = 2 * (height_ - margin_) - y;
      vy = -vy;
    }
  }
  b->Set_location(x, y);
  b->set_speed(vx, vy);
}

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

Ball::Ball(float x, float y, float vx, float vy, Table* table, sf::Color color, int radius) : Cir_(radius) {
  radius_ = radius;
  table_ = table;
  Set_location(x, y);
  set_speed(vx, vy);
  Cir_.setFillColor(color);
  Cir_.setOrigin(radius_, radius_); // center it
}

void Ball::Set_location(float x, float y) {
  if (!table_->IsWithinBounds(x, y))
    throw invalid_argument("Ball position out of bounds");
  x_ = x;
  y_ = y;
}

void Ball::set_speed(float vx, float vy) {
  vx_ = vx;
  vy_ = vy;
}

bool Ball::ball_collision(Ball* b, float scale) {
  float delta_x = b->x_ - x_ ;
  float delta_y = b->y_ - y_ ;
  float Two_radius = (this->radius_+ b->radius_)/scale;
  return (delta_x*delta_x) + (delta_y*delta_y) <= Two_radius*Two_radius;
}

void Ball::ResolveCollision(Ball* b) {

    // Velocity difference
  float dvx = b->vx_ - this->vx_;
  float dvy = b->vy_ - this->vy_;
  float dx = b->x() - this->x();
  float dy = b->y() - this->y();

  // Dot product of velocity difference and position difference
  float dot = dx * dvx + dy * dvy;
  if (dot >= 0) return; // Balls are moving apart — no need to resolve

  // Compute collision normal
  float dist2 = dx * dx + dy * dy;
  if (dist2 == 0.0) return; // Prevent division by zero (exact overlap)

  // Compute collision impulse (assuming equal mass and perfectly elastic)
  float collisionScale = dot / dist2;
  float impulseX = dx * collisionScale;
  float impulseY = dy * collisionScale;

  // Update velocities
  this->vx_ += impulseX;
  this->vy_ += impulseY;
  b->vx_ -= impulseX;
  b->vy_ -= impulseY;

}

void Ball::ApplyFriction(float dt, float friction_coefficient) {
  vx_ *= (1 - friction_coefficient * dt);
  vy_ *= (1 - friction_coefficient * dt);
  
  if (std::abs(vx_) < 0.1) vx_ = 0;
  if (std::abs(vy_) < 0.1) vy_ = 0;
}

void Ball::Move(float dt) {
  ApplyFriction(dt, Friction);

  x_ += vx_ * dt;
  y_ += vy_ * dt;

  if (!table_->IsWithinBounds(x_, y_))
    table_->Reflect(this);
}

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

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

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

void Cue_stick::init(float x, float y, float vx, float vy, Table* table, Ball* ball, sf::Vector2f Rect_size, sf::Color color) {
  set_location(x, y);
  set_speed(vx, vy);
  table_ = table;
  ball_ = ball;
  Rect_size_= Rect_size;
  Rect_.setSize(Rect_size_);
  Rect_.setFillColor(color);
  

  Dot_.setRadius (Rect_size_.x/2);
  Dot_.setFillColor(sf::Color::White);

  Origin(Rect_size.x/2 , 0);
}

void Cue_stick::set_location(float x, float y) {
  x_ = x;
  y_ = y;
}

void Cue_stick::set_speed(float vx, float vy) {
  vx_ = vx;
  vy_ = vy;
}

bool Cue_stick::collision(Ball* b, float scale) {
  float scaled_radius = (Dot_.getRadius()+ b->radius())/scale;
  float delta_x = b->x() - x_ ;
  float delta_y = b->y() - y_ ;
  return (delta_x*delta_x) + (delta_y*delta_y) <= scaled_radius*scaled_radius;
}

void Cue_stick::ResolveCollision(Ball* b) {
  b->set_speed(b->vx() + this->vx_, b->vy()+this->vy_);
}

void Cue_stick::Cue_angle_calculator(float vx, float vy) {
  return Rotation((atan2(vx,vy)* 180 / 3.14159265)); // the Y-axis is reversed atan2(vy,vx) => atan2(vx,vy)
}

float Cue_stick::wrapAngle(float angle) {
    angle = std::fmod(angle, 720.0f);  // wrap to [-720, 720)
    if (angle > 360.0f)
        angle -= 720.0f;
    else if (angle < -360.0f)
        angle += 720.0f;
    return angle;
}

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

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

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

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
        //Cue_angle_ = 0;
        //temp_angle = 0;
        cue_.set_location(-5,0); // put it out of the table
        cue_.Set_position(-5,0); // put it out of the table
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
        //Cue_angle_ = 0;
        //temp_angle = 0;
        cue_.set_location(-5,0); // put it out of the table
        cue_.Set_position(-5,0); // put it out of the table

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
      cue_.Rotation(float(Cue_angle_));

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

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

int main() {
    Renderer renderer(1000, 500, 60);
    renderer.run();
    return 0;
}


