#ifndef DEF_L_Interface_json
#define DEF_L_Interface_json

#include <L/L.h>
#include <iostream>
#include <iomanip>

namespace L {
  class JSON : public Interface<Dynamic::Var> {
    public:
      JSON() : Interface("json") {}

      bool to(const Dynamic::Var& v, std::ostream& stream) {
        if(v.is<String>())
          stream << '"' << v.as<String>() << '"';
        else if(v.is<bool>())
          stream << std::boolalpha << v.as<bool>();
        else if(v.is<Dynamic::Node>()) {
          stream << '{';
          L_Iter(v.as<Dynamic::Node>(),it) {
            if(it != v.as<Dynamic::Node>().begin())
              stream << ',';
            stream << '"' << (*it).first << "\":";
            to((*it).second,stream);
          }
          stream << '}';
        } else if(v.is<Dynamic::Array>()) {
          stream << '[';
          L_Iter(v.as<Dynamic::Array>(),it) {
            if(it != v.as<Dynamic::Array>().begin())
              stream << ',';
            to((*it),stream);
          }
          stream << ']';
        } else if(v.is<float>()) {
          float tmp(v.as<float>());
          stream << std::fixed << std::noshowpoint << std::setprecision(tmp == std::floor(tmp) ? 0 : 5) << v.as<float>();
        } else L_Error("Bad JSON output.");
        return true;
      }
      bool from(Dynamic::Var& v, std::istream& stream) {
        char c;
        size_t i;
        String str;
        nospace(stream);
        switch(c = stream.get()) {
          case '"': // String
            v = String();
            while((c = stream.get()) != '"') {
              if(c == '\\' && stream.peek()=='"')
                v.as<String>() += stream.get();
              else v.as<String>() += c;
            }
            break;
          case 't':
          case 'f': // Bool
            if(c=='t') { // true
              v = true;
              stream.ignore(3);
            } else { // false
              v = false;
              stream.ignore(4);
            }
            break;
          case '{':
            v = Dynamic::Node();
            while(stream.peek()!='}') {
              nospace(stream);
              if(stream.get()!='"') L_Error("Bad JSON input.");
              str.clear();
              while((c = stream.get()) != '"') { // Get name of attribute
                if(c == '\\' && stream.peek()=='"')
                  str += stream.get();
                else str += c;
              }
              nospace(stream);
              if(stream.get()!=':') L_Error("Bad JSON input.");
              from(v[str],stream);
              if(stream.peek()==',')stream.ignore();
            }
            stream.ignore();
            break;
          case '[': // Array
            i = 0;
            v = Dynamic::Array();
            while(stream.peek()!=']') {
              from(v[i],stream);
              if(stream.peek()==',')stream.ignore();
              i++;
            }
            stream.ignore();
            break;
          default: // Probably a number
            v.get<float>();
            if(1) {
              bool negative(c=='-'), afterpoint(false);
              int point(0);
              if(!negative) v.get<float>() = c - '0';
              while(c = stream.peek()) {
                if(c>='0'&&c<='9') {
                  if(afterpoint)point++;
                  (v.as<float>() *= 10) += c - '0';
                } else if(c=='.')
                  afterpoint = true;
                else break;
                stream.ignore();
              }
              while(point--)
                v.as<float>() /= 10;
              if(negative)
                v = -v.as<float>();
            }
            break;
        }
        nospace(stream);
        return true;
      }
  };
}

#endif

