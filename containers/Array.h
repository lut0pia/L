#ifndef DEF_L_Array
#define DEF_L_Array

#include <cstdarg>
#include <cstring>
#include "../stl/Vector.h"

namespace L{
    /*
    template <size_t d,class T>
    class Array : private Vector<T>{
        protected:
            size_t sizes[d];
        public:
            Array() : Vector<T>(){
                for(size_t i=0;i<d;i++)
                    sizes[i] = 0;
            }

            #define RESIZE  size_t i(0);\
                            sizes[0] = newsize;\
                            va_list vl;\
                            va_start(vl,newsize);\
                            while(++i<d) newsize *= (sizes[i] = va_arg(vl,size_t));\
                            va_end(vl);
            Array(size_t newsize,...) : Vector<T>(){
                RESIZE
                Vector<T>::resize(newsize);
            }
            void resizeFast(size_t newsize,...){
                RESIZE
                Vector<T>::resize(newsize);
            }

            void resize(size_t newsize,...){
                size_t oldSizes[d];
                memcpy(oldSizes,sizes,d*sizeof(size_t));

                RESIZE

                Vector<T>::reserve(newsize);
                for(int i=d-1;i>=0;i--){ // For every dimension starting from last
                    typename Vector<T>::iterator it(Vector<T>::end());
                    size_t rowSize(1);
                    for(int j=0;j<i;j++)
                        rowSize *= oldSizes[j];

                    while(it!=Vector<T>::begin()){
                        if(sizes[i]>=oldSizes[i])
                            Vector<T>::insert(it,(sizes[i]-oldSizes[i])*(rowSize/oldSizes[i]),T());
                        else
                            Vector<T>::erase(it+(sizes[i]-oldSizes[i])*(rowSize/oldSizes[i]),it);
                        it += (sizes[i]-oldSizes[i])*(rowSize/oldSizes[i]);
                        it -= rowSize;
                    }
                }
            }


            void resize(size_t newsize,...){
                Array<d,T> old(*this);
                size_t pos[d] = {0}, oldpos, newpos, zeroCount;

                RESIZE

                Vector<T>::clear();
                Vector<T>::resize(newsize);

                // Restore old elements
                for(i=0;i<d;i++)
                    if(!old.size(i))
                        return;
                do{
                    // Replace element
                    oldpos = newpos = pos[0];
                    for(i=1;i<d;i++){

                        oldpos *= old.size(i);
                        newpos *= size(i);
                        oldpos += pos[i];
                        newpos += pos[i];
                    }

                    (*this)[newpos] = old[oldpos];

                    // Change position
                    zeroCount = 0;
                    for(i=0;i<d;i++){
                        pos[i]++;
                        if(pos[i]<std::min(size(i),old.size(i))) break;
                        else{
                            pos[i] = 0;
                            zeroCount++;
                        }
                    }
                }while(zeroCount<d); // All positions have been done
            }

            #undef RESIZE

            #define ACCESS  size_t i(0);\
                            va_list vl;\
                            va_start(vl,pos);\
                            while(++i<d){\
                                pos *= sizes[i];\
                                pos += va_arg(vl,size_t);\
                            }\
                            va_end(vl);\
                            return (*this)[pos];
            T& operator()(size_t pos,...){ACCESS}
            const T& operator()(size_t pos,...) const{ACCESS}
            #undef ACCESS

            inline size_t size(size_t i) const{return sizes[i];}
    };
    */
    #define GETPARAMS   size_t i(0);\
                        size_t sizes[d];\
                        sizes[0] = first;\
                        va_list vl;\
                        va_start(vl,first);\
                        while(++i<d) sizes[i] = va_arg(vl,size_t);\
                        va_end(vl);
    template <size_t d,class T>
    class Array{
        private:
            Vector<Array<d-1,T> > array;
            T& access(size_t* pos){
                return array[*pos].access(pos+1);
            }
            const T& access(size_t* pos) const{
                return array[*pos].access(pos+1);
            }
            void resize(size_t* size){
                array.resize(*size);
                L_Iter(array,it)
                    it->resize(size+1);
            }
        public:
            Array(){}
            Array(size_t first,...){
                GETPARAMS
                resize(sizes);
            }
            inline bool operator==(const Array& other) const{return array == other.array;}
            inline bool operator!=(const Array& other) const{return array != other.array;}
            void resize(size_t first,...){
                GETPARAMS
                resize(sizes);
            }
            T& operator()(size_t first,...){
                GETPARAMS
                return access(sizes);
            }
            const T& operator()(size_t first,...) const{
                GETPARAMS
                return access(sizes);
            }
            inline size_t size(size_t i) const{
                return (i) ? array[0].size(i-1) : array.size();
            }
        friend class Array<d+1,T>;
    };
    template <class T>
    class Array<1,T>{
        private:
            Vector<T> array;
            static const size_t d = 1;
            T& access(size_t* pos){
                return array[*pos];
            }
            const T& access(size_t* pos) const{
                return array[*pos];
            }
            void resize(size_t* size){
                array.resize(*size);
            }
        public:
            Array(){}
            Array(size_t first,...){
                GETPARAMS
                resize(sizes);
            }
            inline bool operator==(const Array& other) const{return array == other.array;}
            inline bool operator!=(const Array& other) const{return array != other.array;}
            void resize(size_t first,...){
                GETPARAMS
                resize(sizes);
            }
            T& operator()(size_t first,...){
                GETPARAMS
                return access(sizes);
            }
            const T& operator()(size_t first,...) const{
                GETPARAMS
                return access(sizes);
            }
            inline size_t size(size_t i) const{return array.size();}
        friend class Array<2,T>;
    };
    #undef GETPARAMS
}

#endif


