#ifndef DEF_L_Interface_xml
#define DEF_L_Interface_xml

#include <L/L.h>
#include <iostream>
#include <iomanip>
/*
namespace L{
    class XML : public Interface<Dynamic::Var>{
        private:
            Set<char> esc;
        public:
            XML() : Interface("xml"){
                esc.insert(' ');
                esc.insert('\t');
                esc.insert('\n');
                esc.insert('\r');
            }
            void to(const Dynamic::Var& v, std::ostream& s){
                if(v.is<String>())
                    s << v.as<String>();
                else if(v.is<Dynamic::Node>()){
                    if(!v["name"].is<String>()
                    || !v["attributes"].is<Dynamic::Node>()
                    || !v["children"].is<Dynamic::Array>())
                        throw Exception("In an XML node, name has to be a string, attributes a node and children an array.");

                    s << '<' << v["name"].as<String>();
                    L_Iter(v["attributes"].as<Dynamic::Node>(),it)
                        s << ' ' << (*it).first << "=\"" << (*it).second << '"';
                    if(v["children"].as<Dynamic::Array>().empty())
                        s << "/>";
                    else{
                        s << '>';
                        L_Iter(v["children"].as<Dynamic::Array>(),it)
                            to((*it),s);
                        s << "</" << v["name"].as<String>() << '>';
                    }
                }
                else throw Exception("Bad XML output.");
            }
            void from(Dynamic::Var& v, std::istream& s){
                nospace(s);
                if(s.peek()=='<'){ // Node
                    s.ignore();
                    if(s.peek()=='/') return; // It's an end tag
                    else if(s.peek()=='!'){ // It's a comment
                        uint n(0);
                        char c;
                        while((c = s.get()))
                            if(c=='-') n++;
                            else if(c=='>'&&n>1) break;
                            else n=0;
                        return;
                    }
                    else if(s.peek()=='?'){ // Declaration (skipping it for now)
                        while(s.get()!='?'||s.peek()!='>'){}
                        s.ignore();
                        return from(v,s);
                    }

                    String& name(v["name"].get<String>());
                    Dynamic::Node& attributes(v["attributes"].get<Dynamic::Node>());
                    Dynamic::Array& children(v["children"].get<Dynamic::Array>());

                    while(!isspace(s.peek()) && s.peek()!='/' && s.peek()!='>') // Get the name of the tag
                        name.push_back(s.get());
                    if(s.peek()=='/'){ // It's a single tag with no attributes
                        s.ignore(2);
                        return;
                    }
                    nospace(s);
                    while(s.peek()!='>'&&s.peek()!='/'){ // It has attributes
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
                    if(s.peek()=='/'){ // It's a single tag
                        s.ignore(2);
                        return;
                    }
                    if(s.peek()=='>')
                        s.ignore();
                    while(true){ // It has children
                        Dynamic::Var tmp;
                        from(tmp,s);
                        if(tmp.is<Dynamic::Node>() || tmp.is<String>())
                            children.push_back(tmp);
                        else break;
                    }
                    while(s.peek()!='>') // End tag
                        s.ignore();
                    s.ignore();
                }
                else{ // Text node
                    String text;
                    while(s.peek()!='<')
                        text.push_back(s.get());
                    v = (String)text.trim(esc);
                }
            }
    };
}
*/

#endif

