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
    buckets_.resize(kMinSize);
    buckets_size_ = kMinSize;
  }

  template <class T>
  HashMap(T begin, T end, Hash hash = Hash()) : hash_function_(hash) {
    buckets_size_ = std::distance(begin, end);
    buckets_.resize(buckets_size_);
    for (T it = begin; it != end; ++it) {
      insert({it->first, it->second});
    }
  }

  HashMap &operator=(HashMap hm) {
    clear();
    for (auto it : hm.dict_) {
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
    if (it == dict_.end()) {
      throw std::out_of_range("blah");
    }
    return it->second;
  }

  void clear() {
    dict_.clear();
    dict_size_ = 0;
    buckets_.clear();
    buckets_.resize(kMinSize);
    buckets_size_ = kMinSize;
  }

  const size_t size() const {
    return dict_size_;
  }

  const bool empty() const {
    return dict_size_ == 0;
  }

  const Hash hash_function() const {
    return hash_function_;
  }

  void insert(std::pair<KeyType, ValueType> element) {
    auto p = find_iterator(element.first);

    if (p.second == buckets_[p.first].end()) {
      iterator f = dict_.insert(dict_.end(),
                                std::make_pair(element.first, element.second));
      buckets_[p.first].insert(buckets_[p.first].end(), f);
      ++dict_size_;
      if (buckets_size_ == dict_size_) {
        rebuild();
      }
    }
  }

  void erase(KeyType key) {
    auto p = find_iterator(key);
    if (p.second != buckets_[p.first].end()) {
      dict_.erase(*p.second);
      buckets_[p.first].erase(p.second);
      --dict_size_;
    }
  }

  iterator find(KeyType key) {
    auto p = find_iterator(key);
    if (p.second != buckets_[p.first].end()) {
      return *p.second;
    }
    return dict_.end();
  }

  const_iterator find(KeyType key) const {
    return const_cast<HashMap *>(this)->find(key);
  }

  iterator begin() {
    return dict_.begin();
  }

  iterator end() {
    return dict_.end();
  }

  const_iterator begin() const {
    return dict_.begin();
  }

  const_iterator end() const {
    return dict_.end();
  }

 private:
  void rebuild() {
    std::vector<std::list<iterator>> new_buckets_(buckets_.size() * 2),
        old_buckets_(buckets_);
    buckets_size_ *= 2;
    buckets_ = new_buckets_;
    for (auto it : old_buckets_) {
      for (auto it2 : it) {
        insert_existing(it2);
      }
    }
  }

  void insert_existing(iterator iter) {
    int index = hash_function_(iter->first) % buckets_.size();
    buckets_[index].insert(buckets_[index].end(), iter);
    if (dict_size_ == buckets_size_) {
      rebuild();
    }
  }

  std::pair<int, list_iterator> find_iterator(KeyType key) {
    int index = hash_function_(key) % buckets_size_;
    list_iterator it =
        std::find_if(buckets_[index].begin(), buckets_[index].end(),
                     [&key](auto it) { return it->first == key; });
    return std::make_pair(index, it);
  }

  Hash hash_function_;
  std::vector<std::list<iterator>> buckets_;
  std::list<std::pair<const KeyType, ValueType>> dict_;
  size_t buckets_size_;
  size_t dict_size_ = 0;

  const size_t kMinSize = 16;
};
