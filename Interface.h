#ifndef DEF_L_Interface
#define DEF_L_Interface

#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include "macros.h"
#include "Exception.h"
#include "stl/Map.h"
#include "stl/String.h"
#include "stl/Vector.h"
#include "system/File.h"

namespace L{
    /*
    template <class T>
    class Interface{
        public:
            static Map<String,void (*)(const String&,T&)> stringRead;
            static Map<String,void (*)(String&,const T&)> stringWrite;
            static Map<String,void (*)(std::istream&,T&)> streamRead;
            static Map<String,void (*)(std::ostream&,const T&)> streamWrite;
            static Map<String,void (*)(const String&,T&)> filePathRead;
            static Map<String,void (*)(const String&,const T&)> filePathWrite;

            static void fromString(const String& str, T& t, const String& format = ""){
                if(stringRead.has(format))
                    stringRead[format](str,t);
                else throw Exception("Tried to read an unhandled format: " +format);
            }
            static T fromString(const String& str, const String& format = ""){
                T wtr;
                fromString(str,wtr,format);
                return wtr;
            }
            static void toString(String& str, T& t, const String& format = ""){
                if(stringWrite.has(format))
                    stringWrite[format](str,t);
                else throw Exception("Tried to write an unhandled format: " +format);
            }

            static void fromStream(std::istream& stream, T& t, const String& format = ""){
                if(streamRead.has(format))
                    streamRead[format](stream,t);
                else throw Exception("Tried to read an unhandled format: " +format);
            }
            static void toStream(std::ostream& stream, T& t, const String& format = ""){
                if(streamWrite.has(format))
                    streamWrite[format](stream,t);
                else throw Exception("Tried to write an unhandled format: " +format);
            }

            static void fromFile(String filePath, T& t){
                String format = filePath.explode('.').back().toLower();
                t = T();
                if(filePathRead.has(format))
                    filePathRead[format](filePath,t);
                else if(streamRead.has(format)){
                    std::ifstream file(filePath.c_str(),std::ios::in);
                    if(file)
                        streamRead[format](file,t);
                    else throw Exception("Could not open the file: " + filePath);
                }
                else throw Exception("Tried to load an unhandled format: " + format);
            }
            static void toFile(String filePath, T& t){
                String format = filePath.explode('.').back().toLower();
                if(filePathWrite.has(format))
                    filePathWrite[format](filePath,t);
                else if(streamWrite.has(format)){
                    std::ofstream file(filePath.c_str());
                    streamWrite[format](file,t);
                }
                else throw Exception("Tried to save as an unhandled format: " + format);
            }

            static void fromBytes(const Vector<byte>& bytes, T& t, const String& format = ""){
                if(streamRead.has(format)){
                    t = T();
                    std::stringstream ioss;
                    copy(bytes.begin(),bytes.end(),std::ostream_iterator<byte>(ioss));
                    streamRead[format](ioss,t);
                }
                else throw Exception("Tried to read an unhandled format: " + format);
            }
            static void toBytes(Vector<byte>& bytes, const T& t, const String& format = ""){
                if(streamWrite.has(format)){
                    std::stringbuf sb;
                    std::iostream stream(&sb); // Create stream
                    streamWrite[format](stream,t); // Write in stream

                    // Copy data in vector
                    bytes.resize(1);
                    stream.read((char*)&bytes.front(),1);
                    if(stream.gcount()){
                        do{
                            bytes.resize(bytes.size()*2);
                            stream.read((char*)&bytes[bytes.size()/2],bytes.size()/2);
                        }while(stream.gcount()==bytes.size()/2);
                        //bytes.resize(bytes.size()-((bytes.size()/2)-stream.gcount()));
                        bytes.resize((2*stream.gcount()+bytes.size())/2);
                    }
                    else bytes.clear();
                }
                else throw Exception("Tried to write an unhandled format: " + format);
            }
    };

    template <class T> Map<String,void (*)(const String&,T&)> Interface<T>::stringRead;
    template <class T> Map<String,void (*)(String&,const T&)> Interface<T>::stringWrite;
    template <class T> Map<String,void (*)(std::istream&,T&)> Interface<T>::streamRead;
    template <class T> Map<String,void (*)(std::ostream&,const T&)> Interface<T>::streamWrite;
    template <class T> Map<String,void (*)(const String&,T&)> Interface<T>::filePathRead;
    template <class T> Map<String,void (*)(const String&,const T&)> Interface<T>::filePathWrite;
    */
    template <class T>
    class Interface{
        protected:
            static Map<String,Interface*> instance;
            String format;

            Interface(const String& format) : format(format){
                instance[format] = this;
            }

        public:
            virtual void from(T& v, const File& file){
                std::ifstream is(file.gPath().c_str(),std::ios::binary);
                from(v,is);
            }
            virtual void from(T& v, const String& str){
                throw Exception("Cannot interface "+format+" from string.");
            }
            virtual void from(T& v, std::istream& is){
                throw Exception("Cannot interface "+format+" from stream.");
            }
            virtual void from(T& v, const Vector<byte>& bytes){
                throw Exception("Cannot interface "+format+" from bytes.");
            }

            virtual void to(const T& v, const File& file){
                std::ofstream os(file.gPath().c_str(),std::ios::binary);
                to(v,os);
            }
            virtual void to(const T& v, String& str){
                throw Exception("Cannot interface "+format+" to string.");
            }
            virtual void to(const T& v, std::ostream& os){
                throw Exception("Cannot interface "+format+" to stream.");
            }
            virtual void to(const T& v, Vector<byte>& bytes){
                std::stringbuf sb;
                std::iostream stream(&sb); // Create stream
                to(v,stream); // Write in stream

                // Copy data in vector
                bytes.resize(1);
                stream.read((char*)&bytes.front(),1);
                if(stream.gcount()){
                    do{
                        bytes.resize(bytes.size()*2);
                        stream.read((char*)&bytes[bytes.size()/2],bytes.size()/2);
                    }while(stream.gcount()==bytes.size()/2);
                    //bytes.resize(bytes.size()-((bytes.size()/2)-stream.gcount()));
                    bytes.resize((2*stream.gcount()+bytes.size())/2);
                }
                else bytes.clear();
            }

            static Interface& in(const String& format){
                if(instance.has(format)) return *instance[format];
                else throw Exception("Unhandled format "+format);
            }
            static void fromFile(T& v, const String& path){
                in(path.explode('.').back().toLower()).from(v,File(path));
            }
            static void toFile(const T& v, const String& path){
                in(path.explode('.').back().toLower()).to(v,File(path));
            }
    };
    template <class T> Map<String,Interface<T>*> Interface<T>::instance;
}

#endif

