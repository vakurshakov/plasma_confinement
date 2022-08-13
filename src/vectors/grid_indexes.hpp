#ifndef SRC_VECTORS_GRID_INDEXES_HPP
#define SRC_VECTORS_GRID_INDEXES_HPP

#include "src/utils/assertions.hpp"

/**
 * @brief Set of grid indexes. Describes the rectangle,
 *  represented by first and last indexes.
 *
 * @details A two-dimensional grid is assumed, so internal
 *  structure of the index consists of only `x` and `y`. 
 *  OpenMP parallelization can be done with the for
 *  loop iterations.
 * 
 * @example
 *  #pragma omp parallel for
 *  for (auto& g : grid_indexes({0, 2}, {10, 6})) { ... }
 */
class grid_indexes {
 public:
  struct index {
    int x, y;
  };

  grid_indexes(const index& first, const index& last)
    : first_{first}, last_{last} {
      ASSERT(first.x < last.x && first.y < last.y,
        "Undefined behaviour for index iterations.");
    }

  grid_indexes(index&& first, index&& last)
    : first_{std::move(first)}, last_{std::move(last)} {
      ASSERT(first.x < last.x && first.y < last.y,
        "Undefined behaviour for index iterations.");
    }

  class iterator {
   public:
    iterator(const index& current,
      const index& first, const index& last)
      : current_{current}, first_{first}, last_{last} {}

    // Input iterator requirements
    index& operator*() {
      return current_;
    }

    index* operator->() {
      return &current_;
    }

    iterator& operator++() {
      current_.y++;
      
      if (current_.y == last_.y) {
        current_.y = first_.y;
        current_.x++;
      }

      return *this;
    }

    bool operator!=(const iterator& other) const {
      return this->current_.x != other.current_.x
          && this->current_.y != other.current_.y;
    }

    // Random access iterator requirements for OpenMP
    iterator& operator+=(int other_plain) { 
      int plain_shift = (current_.y - first_.y) + other_plain;
      int y_width = last_.y - first_.y;
      current_.y = first_.y + plain_shift % y_width;
      current_.x = current_.x + plain_shift / y_width;
      return *this;
    }

    int operator-(const iterator& other) const {
      int cur_plain = (this->current_.x - this->first_.x) *
        (this->last_.y - this->first_.y) + (this->current_.y - this->first_.y);
      
      int oth_plain = (other.current_.x - other.first_.x) *
        (other.last_.y - other.first_.y) + (other.current_.y - other.first_.y);
      
      return cur_plain - oth_plain;
    }

   private:
    index current_;
    index first_;
    index last_;
  };

  iterator begin() {
    return iterator{first_, first_, last_};
  }

  iterator end() {
    return iterator{{last_.x - 1, last_.y}, first_, last_};
  }

 private:
  index first_;
  index last_;
};

#endif  // SRC_VECTORS_GRID_INDEXES_HPP
