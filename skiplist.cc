#include "skiplist.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <optional>
#include <utility>
#include <vector>
namespace skiplist {
using std::cout;
using std::endl;
void skiplist_type::put(key_type key, const value_type &val) {
  vector<Node *> prev = find_prev(key);
  // if the key already exist, simply replace the val
  // NOTE: height start at 0
  for (int i = height; i >= 0; --i) {
    if (prev.at(i)->next != tail.at(i) && prev.at(i)->next->key == key) {
      auto p_equal = prev.at(i)->next;
      p_equal->value = val;
      while (p_equal->down != nullptr) {
        p_equal = p_equal->down;
        p_equal->value = val;
      }
      return;
    }
  }

  size_t layer = roll_size();
  vector<Node *> new_node_vec(height + 1, nullptr);
  for (int i = 0; i <= layer; i++) {
    if (i > height) {
      // NOTE: create a new layer
      // cout << "create new layer: " << i << endl;
      height = i;
      head.push_back(new Node(nullptr, nullptr));
      tail.push_back(new Node(nullptr, nullptr));
      head.at(i)->down = head.at(i - 1);
      tail.at(i)->down = tail.at(i - 1);
      new_node_vec.push_back(
          new Node(key, val, tail.at(i), head.at(i),
                   i == 0 ? nullptr : new_node_vec.at(i - 1)));
      // when i > height, prev[i] is head[i]
      head.at(i)->next = new_node_vec.at(i);
      break; // height += 1
    }
    Node *new_top = new Node(key, val, nullptr, nullptr,
                             i == 0 ? nullptr : new_node_vec.at(i - 1));
    new_top->next = prev.at(i)->next;
    prev.at(i)->next = new_top;
    new_node_vec.at(i) = new_top;
  }
  ele_number++;
  // Now new_node_vec[i] is node in layer i
}
std::string skiplist_type::get(key_type key) const {
  vector<Node *> prev = find_prev(key);
  Node *node = prev.at(0)->next;
  if (node != nullptr && node->key == key) {
    return node->value;
  }
  return "";
}
void skiplist_type::print() const {
  for (int i = height; i >= 0; i--) {
    Node *cur = head.at(i)->next;
    cout << "Layer " << i << ": ";
    while (cur->next != tail.at(i)) {
      cout << cur->key << " ";
      cur = cur->next;
    }
    cout << endl;
  }
}

std::list<key_type> skiplist_type::get_keylist() const {
  Node *cur = head.at(0)->next;
  std::list<key_type> keys;
  while (cur != tail.at(0)) {
    keys.push_back(cur->key);
    cur = cur->next;
  }
  // std::cerr << "The skiplist should has " << ele_number << "elements"
  //           << std::endl;
  // std::cerr << "But it has " << keys.size() << "elements: " << std::endl;
  // for (auto &k : keys) {
  //   std::cerr << k << " ";
  // }
  return keys;
}
std::list<kvpair> skiplist_type::scan(key_type start, key_type end) const {
  // NOTE: [k1, k2]

  Node *cur = head.at(0)->next;
  std::list<kvpair> pairs;
  if (end < start) {
    return pairs;
  }
  while (cur != tail.at(0)) {
    if (cur->key >= start && cur->key <= end) {
      pairs.emplace_back(std::make_pair(cur->key, cur->value));
    }
    cur = cur->next;
  }
  return pairs;
}
size_t skiplist_type::size() const { return ele_number; }

std::list<kvpair> skiplist_type::get_kvplist() const {
  Node *cur = head.at(0)->next;
  std::list<kvpair> kvps;
  while (cur != tail.at(0)) {
    kvps.emplace_back(make_pair(cur->key, cur->value));
    cur = cur->next;
  }
  // if (ele_number != kvps.size()) {
  //   std::cerr << "The skiplist should has " << ele_number << "elements"
  //             << std::endl;
  //   std::cerr << "But it has " << kvps.size() << "elements: " << std::endl;
  //   for (auto &kv : kvps) {
  //     std::cerr << kv.first << " " << kv.second << std::endl;
  //   }
  // }
  return kvps;
}
} // namespace skiplist
