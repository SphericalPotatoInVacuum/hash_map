#pragma once
#include <iostream>
#include <stdexcept>

namespace f {

template <typename T>
class vector {
 public:
  using iterator = T *;
  using const_iterator = const T *;
  using value_type = T;
  using size_type = size_t;

  vector(size_type size = 0, value_type value = {}) {
    const size_type capacity = size + kMinSize;
    data_ = new value_type[capacity];
    for (int i = 0; i < size; ++i) {
      data_[i] = value;
    }
    end_ = data_ + size;
    end_of_memory_ = end_ + capacity;
  }
  ~vector() {
    delete[] data_;
  }

  void push_back(value_type value) {
    if (end_of_memory_ == end_) {
      reallocate();
    }
    *end_ = value;
    ++end_;
  }

  value_type &operator[](size_type index) {
    if (index > std::distance(begin(), end())) {
      throw std::out_of_range("fvector out of range");
    }
    return data_[index];
  }

  iterator begin() noexcept {
    return data_;
  }

  iterator end() noexcept {
    return end_;
  }

  const_iterator begin() const noexcept {
    return data_;
  }

  const_iterator end() const noexcept {
    return end_;
  }

  friend std::ostream &operator<<(std::ostream &out, const vector<T> &v) {
    for (const auto &value : v) {
      out << value << " ";
    }
    return out;
  }

 private:
  void reallocate() {
    size_type dist = std::distance(data_, end_of_memory_);
    size_type new_size = 2 * dist;
    value_type *new_data = new T[new_size];
    end_ = std::copy(begin(), end(), new_data);
    delete[] data_;
    data_ = new_data;
    end_of_memory_ = data_ + new_size;
  }

  value_type *data_;
  value_type *end_;
  value_type *end_of_memory_;

  static constexpr size_type kMinSize = 10;
};
}  // namespace f