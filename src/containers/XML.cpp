#include "XML.h"

#include "../types.h"
#include "../streams/FileStream.h"

using namespace L;

XML::XML() {}
XML::XML(const char* filepath) {
  FileStream fs(filepath,"rb");
  read(fs);
}
XML::XML(Stream& s) {
  read(s);
}

void XML::write(Stream& s) const {
  if(text)
    s << name;
  else {
    s << '<' << name;
    for(auto&& attribute : attributes)
      s << ' ' << attribute.key() << "=\"" << attribute.value() << '"';
    if(children.empty())
      s << "/>";
    else {
      s << '>';
      for(auto&& child : children)
        child.write(s);
      s << "</" << name << '>';
    }
  }
}
void XML::read(Stream& s) {
  name.clear();
  children.clear();
  attributes.clear();
  text = false;
  s.nospace();
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
    while(!Stream::isspace(s.peek()) && s.peek()!='/' && s.peek()!='>') // Get the name of the tag
      name.push(s.get());
    if(s.peek()=='/') { // It's a single tag with no attributes
      s.ignore(2);
      return;
    }
    s.nospace();
    while(s.peek()!='>'&&s.peek()!='/') { // It has attributes
      String attrName, attrValue;
      while(s.peek()!='=')
        attrName.push(s.get());
      s.ignore(2);
      while(s.peek()!='"')
        attrValue.push(s.get());
      s.ignore();
      attributes[attrName] = attrValue;
      s.nospace();
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
      name.push(s.get());
    name.trim(" \t\n\r");
  }
}
