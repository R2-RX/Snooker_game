#include "../include/Game_components/Ball.h"
#include "../include/Game_components/Table.h"
#include "../include/Game_components/Config.h"

using namespace std;
using namespace Biliard;

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
