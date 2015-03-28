#ifndef DEF_L_Set
#define DEF_L_Set

#include <set>

#include "../Exception.h"
#include "../macros.h"
#include "../Rand.h"
#include "../Serializable.h"
#include "../stream.h"
#include "../tmp.h"

namespace L {
  template <class T>
  class Set : public std::set<T>, public Serializable {
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

      void write(std::ostream& s) const {writeT<T>(s);}
      void read(std::istream& s) {readT<T>(s);}

      template <typename R>
      typename enable_if<is_serializable<R> >::type writeT(std::ostream& s) const {
        s << "{ ";
        L_Iter(((std::set<T>&)*this),it) {
          Serializable::write(s,(*it));
          s << " ";
        }
        s << "}";
      }
      template <typename R>
      typename disable_if<is_serializable<R> >::type writeT(std::ostream& s) const {}

      template <typename R>
      typename enable_if<is_serializable<R> >::type readT(std::istream& s) {
        nospace(s);
        if(s.peek()=='{') {
          std::set<T>::clear();
          s.ignore();
          nospace(s);
          while(s.peek()!='}') {
            T tmp;
            Serializable::read(s,tmp);
            insert(tmp);
            nospace(s);
          }
          s.ignore();
        }
      }
      template <typename R>
      typename disable_if<is_serializable<R> >::type readT(std::istream& s) const {}
  };
  /*
  template <class T>
  std::set<T>& operator +=(std::set<T>& a, const std::set<T>& b){
      a.insert(b.begin(),b.end());
      return a;
  }
  template <class T>
  std::set<T> operator +(const std::set<T>& a, const std::set<T>& b){
      std::set<T> wtr(a);
      wtr += b;
      return wtr;
  }
  template <class T>
  std::set<T> operator +(const std::set<T>& a, const T& b){
      std::set<T> wtr(a);
      wtr.insert(b);
      return wtr;
  }

  template <class T>
  std::set<T>& operator *=(std::set<T>& a, const std::set<T>& b){
      typename std::set<T>::iterator it;
      it = a.begin();
      while(it!=a.end()){
          if(b.find(*it)==b.end()){
              a.erase(it);
              it = a.begin();
          }
          else it++;
      }
      return a;
  }
  template <class T>
  std::set<T> operator *(const std::set<T>& a, const std::set<T>& b){
      std::set<T> wtr(a);
      return wtr *= b;
  }

  template <class T>
  std::set<T> Map(T (*f)(T), const std::set<T>& s){
      std::set<T> wtr;
      typename std::set<T>::iterator it;
      for(it=s.begin();it!=s.end();it++)
          wtr.insert(f(*it));
      return wtr;
  }
  */
}

#endif

