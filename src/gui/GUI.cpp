#include "GUI.h"

#include "../gui.h"

using namespace L;

Ref<GUI::Base> GUI::from(const XML& xml) {
  Map<String,Base*> nothing;
  return from(xml,nothing);
}
Ref<GUI::Base> GUI::from(const XML& xml, Map<String,GUI::Base*>& ids) {
  if(xml.text) // Text node
    return ref<Text>(xml.name);
  else {
    Ref<Base> wtr;
    if(xml.name == "background") {
      if(!xml.attributes.has("color"))
        L_ERROR("GUI: A background must have a color.");
      if(!xml.children.size())
        L_ERROR("GUI: A background must have a child.");
      wtr = ref<Background>(GUI::from(xml.children[0],ids),Color(xml.attributes["color"]));
    } else if(xml.name == "border") {
      wtr = ref<Border>(GUI::from(xml.children[0],ids),
                        (xml.attributes.has("size")) ? String::to<size_t>(xml.attributes["size"]) : 1,
                        Color(xml.attributes["color"]));
    } else if(xml.name == "image") {
      if(!xml.attributes.has("src"))
        L_ERROR("GUI: An image needs the src attribute");
      wtr = ref<Image>(xml.attributes["src"]);
    } else if(xml.name == "rectangle") {
      wtr = ref<Rectangle>(point(xml.attributes["size"]),
                           Color(xml.attributes["color"]));
    } else if(xml.name == "line") {
      wtr = ref<Line>(point(xml.attributes["size"]),
                      Color(xml.attributes["color"]));
    } else if(xml.name == "list") {
      Ref<ListContainer> wtr;
      if(xml.attributes.has("spacing"))
        wtr.make(String::to<int>(xml.attributes["spacing"]));
      else wtr.make();
      for(auto&& child : xml.children)
        wtr->push_back(GUI::from(child,ids));
      wtr = wtr;
    } else if(xml.name == "text") {
      wtr = ref<Text>(xml.children[0].name,(xml.attributes.has("font"))
                      ? xml.attributes["font"]
                      : "");
    } else if(xml.name == "textinput") {
      wtr = ref<TextInput>((xml.attributes.has("size"))           ? GUI::point(xml.attributes["size"])   : Vector<2,int>(100,20),
                           (xml.attributes.has("font"))           ? xml.attributes["font"]               : "",
                           (xml.attributes.has("placeholder"))    ? xml.attributes["placeholder"]        : "",
                           xml.attributes.has("password"));
    } else L_ERROR("GUI: Unknown node \""+xml.name+"\".");
    if(xml.attributes.has("id"))
      ids[xml.attributes["id"]] = wtr;
    return wtr;
  }
}

Vector2i GUI::point(const String& str) {
  Array<String> coords(str.explode(' '));
  if(coords.size()==2)
    return Vector<2,int>(String::to<int>(coords[0]),String::to<int>(coords[1]));
  else L_ERROR("GUI: Vector must be \"x y\", not \""+str+"\".");
}
