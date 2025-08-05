#include "../include/Game_components/Table.h"
#include "../include/Game_components/Ball.h"


using namespace std;
using namespace Biliard;

Table::Table(float width, float height) {
  if (width - margin_ <= 0 || height - margin_ <= 0)
    throw invalid_argument("Table dimensions must be positive");
  width_ = width;
  height_ = height;
  margin_ = 0;  using namespace std; //Biliard

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
