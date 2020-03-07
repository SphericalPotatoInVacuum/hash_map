#include <algorithm>
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
  using list_iterator = typename std::list<iterator>::iterator;

  explicit HashMap(Hash hash = Hash()) : hash_function_(hash) {
    data_.resize(kMinSize);
    dsize_ = kMinSize;
  }

  template <class T>
  HashMap(T begin, T end, Hash hash = Hash()) : hash_function_(hash) {
    dsize_ = std::distance(begin, end);
    data_.resize(dsize_);
    for (T it = begin; it != end; ++it) {
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
    insert({key, ValueType()});
    it = find(key);
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
    pdsize_ = 0;
    data_.clear();
    data_.resize(kMinSize);
    dsize_ = kMinSize;
  }

  const size_t size() const {
    return pdsize_;
  }

  const bool empty() const {
    return pdsize_ == 0;
  }

  const Hash hash_function() const {
    return hash_function_;
  }

  void insert(std::pair<KeyType, ValueType> element) {
    auto p = find_iterator(element.first);

    if (p.second == data_[p.first].end()) {
      iterator f = pdata_.insert(pdata_.end(),
                                 std::make_pair(element.first, element.second));
      data_[p.first].insert(data_[p.first].end(), f);
      ++pdsize_;
      if (dsize_ == pdsize_) {
        rebuild();
      }
    }
  }

  void erase(KeyType key) {
    auto p = find_iterator(key);
    if (p.second != data_[p.first].end()) {
      pdata_.erase(*p.second);
      data_[p.first].erase(p.second);
      --pdsize_;
    }
  }

  iterator find(KeyType key) {
    auto p = find_iterator(key);
    if (p.second != data_[p.first].end()) {
      return *p.second;
    }
    return pdata_.end();
  }

  const_iterator find(KeyType key) const {
    return const_cast<HashMap *>(this)->find(key);
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
    dsize_ *= 2;
    data_ = new_data_;
    for (auto it : old_data_) {
      for (auto it2 : it) {
        insert_existing(it2);
      }
    }
  }

  void insert_existing(iterator iter) {
    int index = hash_function_(iter->first) % data_.size();
    data_[index].insert(data_[index].end(), iter);
    if (pdsize_ == dsize_) {
      rebuild();
    }
  }

  std::pair<int, list_iterator> find_iterator(KeyType key) {
    int index = hash_function_(key) % dsize_;
    list_iterator it =
        std::find_if(data_[index].begin(), data_[index].end(),
                     [&key](auto it) { return it->first == key; });
    return std::make_pair(index, it);
  }

  Hash hash_function_;
  std::vector<std::list<iterator>> data_;
  std::list<std::pair<const KeyType, ValueType>> pdata_;
  size_t dsize_;
  size_t pdsize_ = 0;

  const size_t kMinSize = 16;
};
