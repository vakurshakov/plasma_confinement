#include "point.hpp"

void reflective_x(Point& point, double left, double right) {
  if (point.x() < left) {
    point.x() = left;
    point.px() *= -1.0;
  }
  else if (point.x() > right) {
    point.x() = right;
    point.px() *= -1.0;
  }
}

void reflective_y(Point& point, double bottom, double top) {
  if (point.y() < bottom) {
    point.y() = bottom;
    point.py() *= -1.0;
  }
  else if (point.y() > top) {
    point.y() = top;
    point.py() *= -1.0;
  }
}

void periodic_x(Point& point, double left, double right) {
  if (point.x() < left) {
    point.x() = right - (left - point.x());
  }
  else if (point.x() > right) {
    point.x() = left + (point.x() - right);
  }
}

void periodic_y(Point& point, double bottom, double top) {
  if (point.y() < bottom) {
    point.y() = top - (bottom - point.y());
  }
  else if (point.y() > top) {
    point.y() = bottom + (point.y() - top);
  }
}