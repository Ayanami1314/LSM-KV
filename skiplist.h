#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <cstddef>
#include <cstdint>
// #include <optional>
#include <cassert>
#include <cmath>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <string>
#include <vector>
namespace skiplist {
using key_type = uint64_t;
using value_type = std::string;

using kvpair = std::pair<key_type, value_type>;
using std::vector;
class skiplist_type {
private:
  // add something here
  size_t height;
  double p;
  typedef struct node {
    key_type key;
    value_type value;
    node *next;
    node *prev;
    node *down;
    node(key_type key, value_type value, node *next, node *prev, node *down)
        : key(key), value(value), next(next), prev(prev), down(down) {}
    node(node *next, node *prev)
        : key(0), next(next), prev(prev), down(nullptr) {}
    node() : key(0), next(nullptr), prev(nullptr), down(nullptr) {}
  } Node;

  vector<Node *> head;
  vector<Node *> tail;
  size_t MAXHEIGHT;
  vector<Node *> find_prev(key_type key) const {
    // NOTE: return the prev node's array
    // NOTE: height is index, size = height + 1
    vector<Node *> node_vec(height + 1, new Node(nullptr, nullptr));
    size_t level = head.size() - 1;
    // assert(level == this->height);

    Node *cur = head.at(level);

    cur = head.at(level);
    for (int i = level; i >= 0; i--) {
      while (cur->next != tail.at(i) && cur->next->key < key) {
        cur = cur->next;
      }
      // cur->key < key <= cur -> next ->key
      // found
      node_vec.at(i) = cur;
      cur = cur->down;
    }
    // level 0
    return node_vec;
  }
  size_t roll_size() {
    // generate a random number in [1, MAXHEIGHT]
    // srand(time(NULL));
    size_t layer = 0;
    // NOTE: rand() generate a number in [0, RAND_MAX)
    while (static_cast<double>(rand()) / RAND_MAX < this->p &&
           layer < this->MAXHEIGHT) {
      layer++;
      //   cout << "rand: " << static_cast<double>(rand()) / RAND_MAX << endl;
      //   cout << "p: " << this->p << endl;
      //   cout << "layer: " << layer << endl;
    }
    return layer;
  }

public:
  explicit skiplist_type(double p = 0.5) {
    this->p = p;
    this->height = 0;
    this->MAXHEIGHT = 32;
    Node *h0 = new Node(nullptr, nullptr);
    Node *t0 = new Node(nullptr, nullptr);
    h0->next = t0;
    t0->prev = h0;
    this->head.push_back(h0);
    this->tail.push_back(t0);
  }
  void put(key_type key, const value_type &val);
  // std::optional<value_type> get(key_type key) const;
  std::string get(key_type key) const;
  void print() const;
  std::list<key_type> get_keyset() const;
  std::list<kvpair> scan(key_type start, key_type end) const;
};

} // namespace skiplist

#endif // SKIPLIST_H
