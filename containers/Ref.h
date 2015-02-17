#ifndef DEF_L_Ref
#define DEF_L_Ref

#include <cstdlib>
#include <iostream>
//#include <typeinfo>

namespace L{
    void increment(void*);
    bool decrement(void*); // Returns true if last instance
    size_t ReferencedObjects(); // Returns the number of referenced objects
    /*
    template <class T>
    class Ref{
        private:
            T* pointer;     // Actual pointer -> returned to the user
            void* id;       // Id pointer (useful when multiple/virtual inheritance) -> used for reference counting

            void free(){
                if(pointer && decrement(id)){ // If it was the last instance
                    //cout << "it is " << typeid(*pointer).name() << endl;
                    delete pointer; // Delete the object
                    //cout << "ok" << endl;
                }
            }
            void copy(T* p, void* i){
                // Take the values
                pointer = p;
                id = i;
                if(pointer) // If there's an object pointed
                    increment(id);
            }

        public:
            Ref(){
               id = pointer =  NULL;
            }
            Ref(const Ref& other){
                copy(other.pointer,other.id);
            }
            template <class R>
            Ref(const Ref<R>& other){
                copy(other.gPointer(),other.gId());
            }
            template <class R>
            Ref(R* p){
                copy(p,p);
            }
            ~Ref(){
                free();
            }
            Ref& operator=(const Ref& other){
                if(pointer != other.pointer){
                    free();
                    copy(other.pointer,other.id);
                }
                return *this;
            }
            template <class R>
            Ref& operator=(const Ref<R>& other){
                if(pointer != other.gPointer()){
                    free();
                    copy(other.gPointer(),other.gId());
                }
                return *this;
            }
            bool operator==(const Ref& other){
                return (id == other.id);
            }
            const T& operator*() const{
                return *pointer;
            }
            T& operator*(){
                return *pointer;
            }
            operator T*() const{
                return pointer;
            }
            T* operator->() const{
                return pointer;
            }
            T* gPointer() const{
                return pointer;
            }
            void* gId() const{
                return id;
            }
            inline bool null() const{
                return (pointer==NULL);
            }
    };
    */

    template<class T>
    class Ref{
        private:
            T* p;
            size_t* c; // Reference counter

            template <class R>
            void copy(const Ref<R>& other){
                p = other.p;
                c = other.c;
                if(c) (*c)++;
            }
            void free(){
                if(p){
                    (*c)--;
                    if(*c==0){
                        delete p;
                        delete c;
                    }
                }
            }
        public:
            Ref() : p(NULL), c(NULL){}
            Ref(const Ref& other){
                copy(other);
            }
            template <class R>
            Ref(const Ref<R>& other){
                copy(other);
            }
            template <class R>
            Ref(R* p): p(p), c(new size_t(1)){}
            Ref& operator=(const Ref& other){
                if(p != other.p){
                    free();
                    copy(other);
                }
                return *this;
            }
            template <class R>
            Ref& operator=(const Ref<R>& other){
                if(p != other.p){
                    free();
                    p = other.p;
                    c = other.c;
                    if(c) *c++;
                }
                return *this;
            }
            ~Ref(){
                free();
            }
            bool operator==(const Ref& other){
                return (p == other.p);
            }
            const T& operator*() const{
                return *p;
            }
            T& operator*(){
                return *p;
            }
            operator T*() const{
                return p;
            }
            T* operator->() const{
                return p;
            }
            inline bool null() const{
                return (p==NULL);
            }
            inline void clear(){
                *this = Ref();
            }
            template <class R>
            friend class Ref;
    };

}

#endif
