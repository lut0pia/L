#pragma once

#include "../container/Array.h"
#include "../stream/Stream.h"
#include "../system/System.h"

namespace L {
  class Assembly {
    public:
      static uint8_t *_write, *_limit;
      static const size_t _page = 64*1024;
      Array<uint8_t> _assembly;

    public:
      enum Register : uint8_t { eax = 0, ecx, edx, ebx, esp, ebp, esi, edi };

      void load(void*);
      inline void clear() {_assembly.clear();}
      inline uint32_t label() const { return uint32_t(_assembly.size()); }

      template <typename... Args> inline void emit(uint8_t b,Args&&... args) {_assembly.push(b); emit(args...);}
      inline void emit() {}

      // Assembly shortcuts
      inline void instruction(uint8_t opcode, uint8_t reg, uint8_t rm) {emit(opcode); modregrm(3,reg,rm);}
      inline void instruction(uint8_t opcode, uint8_t reg, uint8_t rm, int disp) {
        emit(opcode);
        if(disp) {
          bool byteDisp(disp>=-128 && disp<=127);
          modregrm((byteDisp)?1:2,reg,rm);
          if(rm==esp) sib(0,rm,rm);
          (byteDisp) ? emit(uint8_t(disp)) : imm(disp);
        } else modregrm(0,reg,rm);
      }
      inline void modregrm(uint8_t mod, uint8_t reg, uint8_t rm) {emit((mod<<6)|((reg&0x7)<<3)|(rm&0x7));}
      inline void sib(uint8_t scale, uint8_t index, uint8_t base) {emit((scale<<6)|((index&0x7)<<3)|(base&0x7));}
      inline void imm(uint32_t i) { emit(uint8_t(i), uint8_t(i >> 8), uint8_t(i >> 16), uint8_t(i >> 24)); }

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

      inline void push(uint32_t i) {emit(0x68u); imm(i);}
      inline void push(Register r) {emit(0x50u+r);}

      inline void pop(Register r) {emit(0x58+r);}
      inline void pop(uint32_t n=1) {add(esp,4*n);}

      // Subroutines
      inline void call(void* f) { mov(Assembly::eax, (uint32_t)(uintptr_t)f); emit(uint8_t(0xff), uint8_t(0xd0)); }
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
        imm(i-(uint32_t(_assembly.size())+4));
      }
      inline void jcc(uint8_t opcode,uint32_t i) {
        int rel(i-int(_assembly.size()));
        if(rel >= -128 && rel <= 127)
          emit(opcode, uint8_t(rel - 2));
        else {
          emit(0x0f, uint8_t(opcode + 0x10));
          imm(rel-6);
        }
      }
      inline void jle(uint32_t i) {jcc(0x7e,i);}
      inline void jg(uint32_t i) {jcc(0x7f,i);}

      inline void cmp(Register r, uint32_t i) {instruction(0x81,r,r); imm(i);}
      inline void cmp(Register r1, Register r2) {instruction(0x39,r2,r1);}

      void* commit() const;

      friend inline Stream& operator<<(Stream& s, const Assembly& v) { return s << v._assembly; }
  };
}
