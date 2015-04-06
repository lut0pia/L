#ifndef DEF_L_String
#define DEF_L_String

#include <sstream>
#include <string>
#include "List.h"
#include "../types.h"

namespace L {
  template <class T> class Set;
  class String : public std::string, public Serializable {
    public:
      String();
      String(const std::string& str);
      String(const std::string& str, size_t pos, size_t len = npos);
      String(const char* s);
      String(const char* s, size_t n);
      String(size_t n, char c);
      template <class InputIterator>
      String(InputIterator first, InputIterator last) : std::string(first,last) {}

      using std::string::replace;

      static const byte quotations;
      static const byte parentheses;
      static const byte curlybrackets;
      static const byte squarebrackets;
      static const byte allbrackets;

      String substr(size_t pos, size_t len = npos) const;
      List<String> escapedExplode(char c, byte escapeBrackets, size_t limit = 0) const; // Splits and returns an array of each part (limit is the max number of parts)
      List<String> explode(char c, size_t limit = 0) const;
      String replaceAll(const String& search, const String& replace) const; // Replaces search by replace
      void trimLeft(size_t=1);
      void trimRight(size_t=1);
      String trim(char c = ' ') const;
      String trim(const String&) const;
      String padLeft(size_t,char) const;
      String toLower() const; // To lowercase
      String capitalize() const; // Capitalize first letter of all words
      size_t endOf(size_t start, bool dquotesEscape = true) const; // Returns end of bracket position

      void write(std::ostream&) const;
      void read(std::istream&);
  };
  // Override concatenate operators
  String operator+(const String& lhs, const String& rhs);
  String operator+(const String& lhs, const char* rhs);
  String operator+(const char* lhs, const String& rhs);
  String operator+(const String& lhs, char rhs);
  String operator+(char lhs, const String& rhs);

  template <class T>
  String ToString(const T& t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
  }
  template <class T>
  T FromString(const String& str) {
    T wtr;
    std::stringstream ss;
    ss << str;
    ss >> wtr;
    return wtr;
  }
}

#endif

