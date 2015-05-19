#ifndef DEF_L_STL_Set
#define DEF_L_STL_Set

#include <set>

#include "../Exception.h"
#include "../macros.h"
#include "../Rand.h"
#include "../general.h"
#include "../tmp.h"

namespace L {
  template <class T>
  class Set : public std::set<T> {
    public:
      Set() : std::set<T>() {}
      Set(const std::set<T>& other) : std::set<T>(other) {}

      Set& operator+=(const Set& other) {
        std::set<T>::insert(other.begin(),other.end());
        return *this;
      }
      Set& operator+=(const T& e) {
        insert(e);
        return *this;
      }
      Set operator+(const Set& other) const {
        Set wtr(*this);
        return wtr += other;
      }
      Set operator+(const T& e) const {
        Set wtr(*this);
        return wtr += e;
      }
      Set& operator *=(const Set& other) {
        typename std::set<T>::iterator it;
        it = std::set<T>::begin();
        while(it!=std::set<T>::end()) {
          if(other.find(*it)==other.end()) {
            std::set<T>::erase(it);
            it = std::set<T>::begin();
          } else it++;
        }
        return *this;
      }
      Set operator*(const Set& other) {
        Set wtr(*this);
        return wtr *= other;
      }
      const T& random() const {
        if(std::set<T>::empty())
          throw Exception("Empty set");
        size_t i(Rand::next(0,std::set<T>::size()-1));
        typename std::set<T>::iterator it(std::set<T>::begin());
        while(i--) it++;
        return *it;
      }
      bool has(const T& e) const {
        return std::set<T>::find(e) != std::set<T>::end();
      }
      Set map(T(*f)(const T&)) const {
        Set wtr;
        typename std::set<T>::iterator it(std::set<T>::begin());
        while(it!=std::set<T>::end()) {
          wtr.insert(f(*it));
          it++;
        }
        return wtr;
      }
      Set filter(bool (*f)(const T&)) const {
        Set wtr;
        typename std::set<T>::iterator it(std::set<T>::begin());
        while(it!=std::set<T>::end()) {
          if(f(*it))
            wtr.insert(*it);
          it++;
        }
        return wtr;
      }
  };
}

#endif

