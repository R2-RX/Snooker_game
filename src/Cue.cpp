#include "../include/Game_components/Config.h"
#include "../include/Game_components/Cue.h"
#include "../include/Game_components/Ball.h"
#include "../include/Game_components/Table.h"

using namespace std;
using namespace Biliard;

void Cue_stick::init(float x, float y, float vx, float vy, Table* table, Ball* ball, sf::Vector2f Rect_size, sf::Color color) {
  set_location(x, y);
  set_speed(vx, vy);
  table_ = table;
  ball_ = ball;
  Rect_size_= Rect_size;
  Rect_.setSize(Rect_size_);
  Rect_.setFillColor(color);
  
  Dot_.setRadius(Rect_size_.x/2);
  Dot_.setFillColor(sf::Color::White);

  Origin(Rect_size.x/2, 0);
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
  Rotation((atan2(vx,vy)* 180 / 3.14159265)); // the Y-axis is reversed atan2(vy,vx) => atan2(vx,vy)
}

float Cue_stick::wrapAngle(float angle) {
    angle = std::fmod(angle, 720.0f);  // wrap to [-720, 720)
    if (angle > 360.0f)
        angle -= 720.0f;
    else if (angle < -360.0f)
        angle += 720.0f;
    return angle;
}