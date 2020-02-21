#include <fstream>
#include <iostream>
#include <list>
#include <stdexcept>
#include <vector>

template <class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
 public:
  using iterator =
      typename std::list<std::pair<const KeyType, ValueType>>::iterator;
  using const_iterator =
      typename std::list<std::pair<const KeyType, ValueType>>::const_iterator;

  explicit HashMap(Hash hash = Hash()) : hash_function_(hash) {
    data_.resize(16);
    dsize = 16;
  }

  template <class T>
  HashMap(T begin_, T end_, Hash hash = Hash()) : hash_function_(hash) {
    dsize = std::distance(begin_, end_);
    data_.resize(dsize);
    for (T it = begin_; it != end_; ++it) {
      insert({it->first, it->second});
    }
  }

  HashMap &operator=(HashMap hm) {
    clear();
    for (auto it : hm.pdata_) {
      insert(std::make_pair(it.first, it.second));
    }
    return *this;
  }

  HashMap(std::initializer_list<std::pair<KeyType, ValueType>> init_list,
          Hash hash = Hash())
      : HashMap(init_list.begin(), init_list.end(), hash) {}

  ValueType &operator[](const KeyType &key) {
    iterator it = find(key);
    if (it == pdata_.end()) {
      insert({key, ValueType()});
      it = find(key);
    }
    return it->second;
  }

  const ValueType &at(KeyType key) const {
    const_iterator it = find(key);
    if (it == pdata_.end()) {
      throw std::out_of_range("blah");
    }
    return it->second;
  }

  void clear() {
    pdata_.clear();
    pdsize = 0;
    data_.clear();
    data_.resize(16);
    dsize = 16;
  }

  const size_t size() const {
    return pdsize;
  }

  const bool empty() const {
    return pdsize == 0;
  }

  const Hash hash_function() const {
    return hash_function_;
  }

  void insert(std::pair<KeyType, ValueType> element) {
    int index = hash_function_(element.first) % data_.size();
    auto it = data_[index].begin();
    while (it != data_[index].end() && !((*it)->first == element.first)) {
      ++it;
    }
    if (it == data_[index].end() || !((*it)->first == element.first)) {
      iterator f = pdata_.insert(pdata_.end(),
                                 std::make_pair(element.first, element.second));
      data_[index].insert(data_[index].end(), f);
      ++pdsize;
      if (dsize == pdsize) {
        rebuild();
      }
    }
  }

  void insert_existing(iterator iter) {
    int index = hash_function_(iter->first) % data_.size();
    data_[index].insert(data_[index].end(), iter);
    if (pdsize == dsize) {
      rebuild();
    }
  }

  void erase(KeyType key) {
    int index = hash_function_(key) % dsize;
    auto it = data_[index].begin();
    while (it != data_[index].end() && !((*it)->first == key)) {
      ++it;
    }
    if (it != data_[index].end()) {
      pdata_.erase(*it);
      data_[index].erase(it);
      --pdsize;
    }
  }

  iterator find(KeyType key) {
    int index = hash_function_(key) % dsize;
    auto it = data_[index].begin();
    while (it != data_[index].end() && !((*it)->first == key)) {
      ++it;
    }
    if (it != data_[index].end()) {
      return *it;
    }
    return pdata_.end();
  }

  const_iterator find(KeyType key) const {
    int index = hash_function_(key) % dsize;
    auto it = data_[index].begin();
    while (it != data_[index].end() && (*it)->first != key) {
      ++it;
    }
    if (it != data_[index].end()) {
      return *it;
    }
    return pdata_.end();
  }

  iterator begin() {
    return pdata_.begin();
  }

  iterator end() {
    return pdata_.end();
  }

  const_iterator begin() const {
    return pdata_.begin();
  }

  const_iterator end() const {
    return pdata_.end();
  }

 private:
  void rebuild() {
    std::vector<std::list<iterator>> new_data_(data_.size() * 2),
        old_data_(data_);
    dsize *= 2;
    data_ = new_data_;
    for (auto it : old_data_) {
      for (auto it2 : it) {
        insert_existing(it2);
      }
    }
  }

  Hash hash_function_;
  std::vector<std::list<iterator>> data_;
  std::list<std::pair<const KeyType, ValueType>> pdata_;
  size_t dsize;
  size_t pdsize = 0;
};
