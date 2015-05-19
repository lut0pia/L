#ifndef DEF_L_ECS_Entity
#define DEF_L_ECS_Entity

/*
#include "../dynamic/Type.h"
#include "../stl.h"
#include "Engine.h"

namespace L{
    namespace ECS{
        class Entity{
            private:
                Multimap<const Dynamic::TypeDescription*,uint> components;
                static VectorMap<Entity> entities;

            public:
                template <class CompType>
                Set<uint> gComponents(){
                    return components[Dynamic::Type<CompType>::description()];
                }
                template <class CompType>
                CompType& gComponent(){
                    Set<uint> results(gComponents<CompType>());
                    if(!results.empty())
                        return CompType::get(*results.begin());
                    else throw Exception("No such component in entity");
                }

                static uint add(const Entity& e = Entity());
                static Entity& get(uint ei);

                template <class CompType>
                static void addComponent(const CompType& c){
                    Engine::addUpdate(CompType::updateAll); // Make sure the component's update is in the engine
                    uint ci(CompType::add(c)); // Actually add the component
                    c.gEntity().components.insert(std::make_pair(Dynamic::Type<CompType>::description(),ci)); // Link the entity to the component
                }

        };
    }
}
*/

#endif

