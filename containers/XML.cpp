#include "XML.h"

#include <fstream>
#include "../macros.h"
#include "../general.h"

using namespace L;
using namespace std;

XML::XML() {}
XML::XML(const File& f) {
  ifstream s(f.path());
  read(s);
}
XML::XML(std::istream& s) {
  read(s);
}

void XML::write(std::ostream& s) const {
  if(text)
    s << name;
  else {
    s << '<' << name;
    L_Iter(attributes,it)
    s << ' ' << (*it).first << "=\"" << (*it).second << '"';
    if(children.empty())
      s << "/>";
    else {
      s << '>';
      children.foreach([&s](const XML& child) {
        child.write(s);
      });
      s << "</" << name << '>';
    }
  }
}
void XML::read(std::istream& s) {
  name.clear();
  children.clear();
  attributes.clear();
  text = false;
  nospace(s);
  if(s.peek()=='<') { // Node
    s.ignore();
    if(s.peek()=='/') return; // It's an end tag
    else if(s.peek()=='!') { // It's a comment
      uint n(0);
      char c;
      while((c = s.get()))
        if(c=='-') n++;
        else if(c=='>'&&n>1) break;
        else n=0;
      return;
    } else if(s.peek()=='?') { // Declaration (skipping it for now)
      while(s.get()!='?'||s.peek()!='>') {}
      s.ignore();
      read(s);
      return;
    }
    while(!isspace(s.peek()) && s.peek()!='/' && s.peek()!='>') // Get the name of the tag
      name.push_back(s.get());
    if(s.peek()=='/') { // It's a single tag with no attributes
      s.ignore(2);
      return;
    }
    nospace(s);
    while(s.peek()!='>'&&s.peek()!='/') { // It has attributes
      String attrName, attrValue;
      while(s.peek()!='=')
        attrName.push_back(s.get());
      s.ignore(2);
      while(s.peek()!='"')
        attrValue.push_back(s.get());
      s.ignore();
      attributes[attrName] = attrValue;
      nospace(s);
    }
    if(s.peek()=='/') { // It's a single tag
      s.ignore(2);
      return;
    }
    if(s.peek()=='>')
      s.ignore();
    while(true) { // It has children
      children.push(XML());
      children.back().read(s);
      if(s.peek()=='/') {
        children.pop();
        break;
      }
    }
    while(s.peek()!='>') // End tag
      s.ignore();
    s.ignore();
  } else { // Text node
    text = true;
    while(s.peek()!='<')
      name.push_back(s.get());
    name = name.trim(" \t\n\r");
  }
}
