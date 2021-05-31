#include "../macros.h"
#include "debug.h"

L_PUSH_NO_WARNINGS
#include <Windows.h>
#include <DbgHelp.h>
L_POP_NO_WARNINGS

using namespace L;

#if !L_RLS
void L::dump_stack(FILE* stream) {
  HANDLE current_process(GetCurrentProcess());
  HANDLE current_thread(GetCurrentThread());
  CONTEXT context;
  RtlCaptureContext(&context);
  SymInitialize(current_process, nullptr, true);

  DWORD machine_type;
  STACKFRAME64 stackframe;
  {
    memset(&stackframe, 0, sizeof(stackframe));
#if _M_X64
    machine_type = IMAGE_FILE_MACHINE_AMD64;
    stackframe.AddrPC.Offset = context.Rip;
    stackframe.AddrPC.Mode = AddrModeFlat;
    stackframe.AddrFrame.Offset = context.Rsp;
    stackframe.AddrFrame.Mode = AddrModeFlat;
    stackframe.AddrStack.Offset = context.Rsp;
    stackframe.AddrStack.Mode = AddrModeFlat;
#else
#error "Unsupported platform"
#endif
  }

  const size_t max_symbol_size(256);
  char symbol_info_data[sizeof(SYMBOL_INFO) + max_symbol_size];
  PSYMBOL_INFO symbol_info((PSYMBOL_INFO)&symbol_info_data);
  {
    memset(symbol_info, 0, sizeof(*symbol_info));
    symbol_info->SizeOfStruct = sizeof(*symbol_info);
    symbol_info->MaxNameLen = max_symbol_size;
  }
  IMAGEHLP_LINE line;
  {
    line.SizeOfStruct = sizeof(line);
    line.FileName = nullptr;
  }

  while(StackWalk64(machine_type, current_process, current_thread, &stackframe,
    &context, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr)) {
    DWORD64 disp;
    DWORD disp_line;
    SymFromAddr(current_process, stackframe.AddrPC.Offset, &disp, symbol_info);
    SymGetLineFromAddr(current_process, stackframe.AddrPC.Offset, &disp_line, &line);
    fprintf(stream, "%s@%s:%d\n", symbol_info->Name, (strrchr(line.FileName, '\\') + 1), (line.LineNumber - 1));
  }
}
#else
void L::dump_stack(FILE*) {}
#endif

void L::debugbreak() {
  __debugbreak();
}

bool L::set_data_breakpoint(const void* ptr, size_t size, L::DataBreakpointType type) {
  CONTEXT ctx{0};
  ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
  if(::GetThreadContext(::GetCurrentThread(), &ctx) == FALSE) {
    return false;
  }

  DWORD64 size_bits;
  switch(size) {
    case 1: size_bits = 0; break;
    case 2: size_bits = 1; break;
    case 4: size_bits = 3; break;
    case 8: size_bits = 2; break;
    default: return false;
  }

  DWORD64 type_bits;
  switch(type) {
    case DataBreakpointType::ReadOrWritten: type_bits = 3; break;
    case DataBreakpointType::Written: type_bits = 1; break;
    case DataBreakpointType::Executed: type_bits = 0; break;
    default: return false;
  }

  for(uintptr_t i = 0; i < 4; i++) {
    const DWORD64 used_bit = 1ull << (i * 2ull);
    const DWORD64 meta_bit_pos = 16ull + i * 4ull;
    const bool is_used = ctx.Dr7 & used_bit;
    if(!is_used) {
      *(&ctx.Dr0 + i) = DWORD64(ptr); // Set breakpoint pointer
      ctx.Dr7 |= used_bit; // Enable breakpoint
      ctx.Dr7 &= ~(0xfull << meta_bit_pos); // Zero the breakpoint metadata
      ctx.Dr7 |= type_bits << meta_bit_pos;
      ctx.Dr7 |= size_bits << (meta_bit_pos + 2);
      return ::SetThreadContext(::GetCurrentThread(), &ctx) != FALSE;
    }
  }

  return false;
}
bool L::unset_data_breakpoint(const void* ptr) {
  CONTEXT ctx{0};
  ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
  if(::GetThreadContext(::GetCurrentThread(), &ctx) == FALSE) {
    return false;
  }

  for(uintptr_t i = 0; i < 4; i++) {
    const DWORD64 used_bit = 1ull << (i * 2ull);
    const bool is_used = ctx.Dr7 & used_bit;
    if(is_used && *(&ctx.Dr0 + i) == DWORD64(ptr)) {
      ctx.Dr7 &= ~used_bit; // Disable breakpoint
      return ::SetThreadContext(::GetCurrentThread(), &ctx) != FALSE;
    }
  }

  return false;
}