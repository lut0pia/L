#include "debug.h"

#include "../macros.h"

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
  char symbol_info_data[sizeof(SYMBOL_INFO)+max_symbol_size];
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
