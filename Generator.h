#pragma once

#include "IT.h"
#include "LexAnalysis.h"
#include "LT.h"
#include "Parm.h"
#include "Log.h"
#include <string>

#define SEPSTREMP "\n;------------------------------\n"
#define SEPSTR(x) ("\n;----------- " + std::string(x) + " ------------\n")

#define BEGIN \
".586\n"\
".model flat, stdcall\n"\
"includelib libucrt.lib\n"\
"includelib kernel32.lib\n"\
"includelib ..\\Debug\\Static_Library.lib\n"\
"ExitProcess PROTO :DWORD\n"\
"\n"

#define EXTERN \
"\noutlich PROTO :DWORD\n"\
"outrad  PROTO :DWORD\n"\
"slength PROTO :DWORD, :DWORD\n"\
"itoa_func PROTO :DWORD, :DWORD\n"\
"\n.stack 4096\n"\
"\n"

#define ITENTRY(x) tables.idtable.table[tables.lextable.table[x].idxTI]
#define LEXEMA(x)  tables.lextable.table[x].lexema

#define CONSTA \
".const\n"\
"\tnewline byte 13, 10, 0\n"\
"\terror_negative byte 'ERROR: Negative value for unsigned type!', 13, 10, 0\n"

#define DATA \
".data\n"\
"\ttemp sdword ?\n"\
"\tbuffer  byte 256 dup(0)\n"

#define CODE ".code\n"

#define END  "push 0\ncall ExitProcess\nmain ENDP\nend main\n"

namespace Gener
{
    void CodeGeneration(Lexer::LEX& tables, Parm::PARM& parm, Log::LOG& log);
}
