#ifndef DEF_L_Assembly
#define DEF_L_Assembly

#include "../containers/Array.h"
#include "../streams/Stream.h"
#include "../system/System.h"
#include "../types.h"

namespace L {
  class Assembly {
    public:
      static byte *_write, *_limit;
      static const size_t _page = 64*1024;
      Array<byte> _assembly;

    public:
      typedef enum {eax = 0,ecx,edx,ebx,esp,ebp,esi,edi} Register;

      void load(void*);
      inline void clear() {_assembly.clear();}
      inline uint32_t label() const {return _assembly.size();}

      template <typename... Args> inline void emit(byte b,Args&&... args) {_assembly.push(b); emit(args...);}
      inline void emit() {}

      // Assembly shortcuts
      inline void instruction(byte opcode, byte reg, byte rm) {emit(opcode); modregrm(3,reg,rm);}
      inline void instruction(byte opcode, byte reg, byte rm, int disp) {
        emit(opcode);
        if(disp) {
          bool byteDisp(disp>=-128 && disp<=127);
          modregrm((byteDisp)?1:2,reg,rm);
          if(rm==esp) sib(0,rm,rm);
          (byteDisp)?emit(disp):imm(disp);
        } else modregrm(0,reg,rm);
      }
      inline void modregrm(byte mod, byte reg, byte rm) {emit((mod<<6)|((reg&0x7)<<3)|(rm&0x7));}
      inline void sib(byte scale, byte index, byte base) {emit((scale<<6)|((index&0x7)<<3)|(base&0x7));}
      inline void imm(uint32_t i) {emit(i,i>>8,i>>16,i>>24);}

      inline void add(Register dst, uint32_t i) {instruction(0x81,0,dst); imm(i);}
      inline void add(Register dst, Register src) {instruction(0x03,dst,src);}
      inline void inc(Register r) {emit(0x40+r);}

      inline void sub(Register dst, uint32_t i) {instruction(0x81,5,dst); imm(i);}
      inline void sub(Register dst, Register src) {instruction(0x2b,dst,src);}
      inline void dec(Register r) {emit(0x48+r);}

      inline void mov(Register r, uint32_t i) {emit(0xb8+r); imm(i);}
      inline void mov(Register dst, Register src, int disp) {instruction(0x8b,dst,src,disp);}
      inline void mov(Register dst, Register src) {instruction(0x8b,dst,src);}
      inline void mov(Register dst, int disp, uint32_t i) {instruction(0xc7,0,dst,disp); imm(i);}
      inline void mov(Register dst, int disp, Register src) {instruction(0x89,src,dst,disp);}

      inline void xor(Register dst, Register src) {instruction(0x33,dst,src);}
      inline void clr(Register r) {xor(r,r);}

      inline void push(uint32_t i) {emit(0x68); imm(i);}
      inline void push(Register r) {emit(0x50+r);}

      inline void pop(Register r) {emit(0x58+r);}
      inline void pop(uint32_t n=1) {add(esp,4*n);}

      // Subroutines
      inline void call(void* f) {mov(Assembly::eax,(uint32_t)f); emit(0xff,0xd0);}
      inline void include(void* f) {
        sub(esp,4);
        load(f);
        _assembly.pop();
        add(esp,4);
      }
      inline void retn() {emit(0xc3);}

      // Jumps
      inline void jmp(uint32_t i) {
        emit(0xe9);
        imm(i-(_assembly.size()+4));
      }
      inline void jcc(byte opcode,uint32_t i) {
        int rel(i-_assembly.size());
        if(rel>=-128&&rel<=127)
          emit(opcode,rel-2);
        else {
          emit(0x0f,opcode+0x10);
          imm(rel-6);
        }
      }
      inline void jle(uint32_t i) {jcc(0x7e,i);}
      inline void jg(uint32_t i) {jcc(0x7f,i);}

      inline void cmp(Register r, uint32_t i) {instruction(0x81,r,r); imm(i);}
      inline void cmp(Register r1, Register r2) {instruction(0x39,r2,r1);}

      void* commit() const;

      friend Stream& operator<<(Stream&, const Assembly&);
  };
  Stream& operator<<(Stream&, const Assembly&);
}

#endif
