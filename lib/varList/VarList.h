#pragma once
#include <Arduino.h>
#include <avr/pgmspace.h>


template <typename T>
class VarList {
public:
  struct Node {
    const char* name_P;   // име в PROGMEM
    T value;
    T minVal;
    T maxVal;
    bool updated;
    Node* next;
  };

  VarList() : head_(nullptr) {};

  bool add(const char* name_P, T initial, T minVal, T maxVal) {
    if (!name_P) return false;
    if (findNode(name_P)) return false;

    Node* n = new (std::nothrow) Node;
    if (!n) return false;

    n->name_P  = name_P;
    n->minVal = minVal;
    n->maxVal = maxVal;
    n->value  = clamp(initial, minVal, maxVal);
    n->updated = true;
    n->next = head_;
    head_ = n;
    return true;
  }
  
  const char * get_name()const{
    return this->name_P;
  }
  

  T get(const char* name_P)const{
        Node* n = findNode(name_P);
        if (!n) return false;
        //out = n->value;
        return n->value;
    }

  bool set(const char* name_P, T v){
    Node* n = findNode(name_P);
    if (!n) return false;
    T nv = clamp(v, n->minVal, n->maxVal);
    if (nv != n->value) {
      n->value = nv;
      n->updated = true;
    }
    return true;
  }

  bool isUpdated(const char* name_P)const{
    Node* n = findNode(name_P);
    return n ? n->updated : false;
  }

  void clearUpdated(const char* name_P) {
    Node* n = findNode(name_P);
    if (n) n->updated = false;
  }

  Node* head() const { return head_; }

private:
  Node* head_;

  static T clamp(T v, T lo, T hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
  }

  Node* findNode(const char* name_P)const{
    Node* cur = head_;
    while (cur) {
      if (strcmp_P(name_P, cur->name_P) == 0)
        return cur;
      cur = cur->next;
    }
    return nullptr;
  }
};