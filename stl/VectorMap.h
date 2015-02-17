#ifndef DEF_L_VectorMap
#define DEF_L_VectorMap

#include "Set.h"
#include "Vector.h"

namespace L{
    template <class T>
    class VectorMap : private Vector<T>{
        private:
            Set<uint> erased;

        public:
            uint insert(const T& e){
                if(!erased.empty()){
                    uint i(*erased.begin());
                    erased.erase(erased.begin());
                    Vector<T>::operator[](i) = e;
                    return i;
                }
                else{
                    Vector<T>::push_back(e);
                    return size()-1;
                }
            }
            void erase(uint i){
                if(i<size())
                    erased.insert(i);
                else throw Exception("Element cannot be erased in VectorMap");
                shrink();
            }
            bool has(uint i) const{
                return i<size() && !erased.has(i);
            }
            size_t size() const{
                return Vector<T>::size();
            }
            void shrink(){
                while(true){
                    uint last(size()-1); // Last element's index
                    if(erased.has(last)){ // If last element is erased
                        erased.erase(last);
                        Vector<T>::pop_back();
                    }
                    else break;
                }
            }
            const T& operator[](uint i) const{
                if(erased.has(i))
                    throw Exception("Key not found in VectorMap");
                else return Vector<T>::operator[](i);
            }
            T& operator[](uint i){
                if(erased.has(i))
                    throw Exception("Key not found in VectorMap");
                else return Vector<T>::operator[](i);
            }

            /*
            class iterator{
                private:
                public:
                    typedef typename A::difference_type difference_type;
                    typedef typename A::value_type value_type;
                    typedef typename A::reference reference;
                    typedef typename A::pointer pointer;
                    typedef std::random_access_iterator_tag iterator_category; //or another tag

                    iterator();
                    iterator(const iterator&);
                    ~iterator();

                    iterator& operator=(const iterator&);
                    bool operator==(const iterator&) const;
                    bool operator!=(const iterator&) const;

                    iterator& operator++();
                    iterator operator++(int); //optional
                    iterator& operator--(); //optional
                    iterator operator--(int); //optional


                    T& operator*() const;
                    T* operator->() const;
            };
            */
    };
}

#endif


