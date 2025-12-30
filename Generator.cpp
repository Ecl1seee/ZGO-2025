#include "Generator.h"
#include "Parm.h"
#include "LexAnalysis.h"
#include "IT.h"
#include "LT.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <iostream>

using namespace std;

#define LEXEMA(i)  tables.lextable.table[i].lexema
#define ITENTRY(i) tables.idtable.table[tables.lextable.table[i].idxTI]

namespace Gener
{
    static int labelCounter = 0;

    string itoS(int x) { stringstream ss; ss << x; return ss.str(); }
    string makeLabel() { return "L" + itoS(labelCounter++); }

    string mangleId(const char* id)
    {
        string s = id;
        for (auto& c : s)
            if (c == '.') c = '_';
        return s;
    }

    void Head(ofstream& stream, IT::IdTable& idtable)
    {
        stream << BEGIN;

        stream << EXTERN;

        stream << CONSTA;

        // Ћитералы
        for (int i = 0; i < idtable.size; i++)
        {
            IT::Entry e = idtable.table[i];
            if (e.idtype != IT::IDTYPE::L) continue;

            stream << "\t" << mangleId(e.id) << " ";
            switch (e.iddatatype)
            {
            case IT::IDDATATYPE::INT:
            case IT::IDDATATYPE::BOOL:
                stream << "dword " << e.value.vint;
                break;
            case IT::IDDATATYPE::STR:
            case IT::IDDATATYPE::CHAR:
                stream << "byte '" << e.value.vstr.str << "', 0";
                break;
            }
            stream << "\n";
        }

        stream << "\tdiv_zero_msg byte 'Division by zero',0\n";

        stream << DATA;

        for (int i = 0; i < idtable.size; i++)
        {
            IT::Entry e = idtable.table[i];
            if (e.idtype == IT::IDTYPE::V)
            {
                if (e.iddatatype == IT::IDDATATYPE::STR || e.iddatatype == IT::IDDATATYPE::CHAR)
                {
                    stream << "\t" << mangleId(e.id) << "_buf byte 256 dup(0)\n";
                    stream << "\t" << mangleId(e.id) << " dword 0\n";
                }
                else
                {
                    stream << "\t" << mangleId(e.id) << " dword 0\n";
                }
            }
        }

        // временна€ переменна€ дл€ switch
        stream << "\ttmp_switch_value dword 0\n";

        stream << CODE;

        stream << "\n; ----------- ERROR HANDLERS ------------\n";
        stream << "handle_negative PROC\n";
        stream << "\txor eax, eax\n";
        stream << "\tret\n";
        stream << "handle_negative ENDP\n";

        // обработчик делени€ на ноль: выводит сообщение и завершает программу
        stream << "handle_div_zero PROC\n";
        stream << "\tpush offset div_zero_msg\n";
        stream << "\tcall outrad\n";
        stream << "\tpush 0\n";
        stream << "\tcall ExitProcess\n";
        stream << "\tret\n";
        stream << "handle_div_zero ENDP\n";
    }

    string genEqualCode(Lexer::LEX& tables, int i)
    {
        string str;
        IT::Entry lval = ITENTRY(i - 1);

        bool isNumericType = (lval.iddatatype == IT::IDDATATYPE::INT);

        for (int j = i + 1; LEXEMA(j) != LEX_SEMICOLON; j++)
        {
            char lex = LEXEMA(j);

            if (lex == LEX_ID || lex == LEX_LITERAL)
            {
                IT::Entry entry = ITENTRY(j);

                if (entry.idtype != IT::IDTYPE::F && entry.idtype != IT::IDTYPE::S)
                {
                    if (entry.iddatatype == IT::IDDATATYPE::STR || entry.iddatatype == IT::IDDATATYPE::CHAR)
                    {
                        if (entry.idtype == IT::IDTYPE::L)
                            str += "\tlea eax, " + mangleId(entry.id) + "\n\tpush eax\n";
                        else
                            str += "\tpush " + mangleId(entry.id) + "\n";
                    }
                    else
                    {
                        str += "\tpush " + mangleId(entry.id) + "\n";
                    }
                }
            }
            else if (lex == '@')
            {
                int funcIdx = j - 1;
                if (funcIdx > i && LEXEMA(funcIdx) == LEX_ID)
                {
                    IT::Entry func = ITENTRY(funcIdx);
                    string funcName = mangleId(func.id);

                    if (funcName == "itoa")
                    {
                        funcName = "itoa_func";
                        str += "\tpop ebx\n";
                        str += "\tpush ebx\n";

                        string bufName = mangleId(lval.id) + "_buf";
                        str += "\tpush offset " + bufName + "\n";
                        str += "\tcall " + funcName + "\n";
                        str += "\tpush eax\n";
                    }
                    else if (funcName == "slength")
                    {
                        str += "\tpop ebx\n";
                        str += "\tpush ebx\n";
                        str += "\tpush offset buffer\n";
                        str += "\tcall " + funcName + "\n";
                        str += "\tpush eax\n";
                    }
                    else if (func.idtype == IT::IDTYPE::S)
                    {
                        str += "\tpop ebx\n";
                        str += "\tpush ebx\n";
                        str += "\tpush offset buffer\n";
                        str += "\tcall " + funcName + "\n";
                        str += "\tpush eax\n";
                    }
                    else
                    {
                        str += "\tcall " + funcName + "\n";
                        str += "\tpush eax\n";
                    }
                }
            }
            else if (lex == LEX_PLUS)
            {
                str += "\tpop ebx\n\tpop eax\n\tadd eax, ebx\n\tpush eax\n";
            }
            else if (lex == LEX_MINUS)
            {
                str += "\tpop ebx\n\tpop eax\n\tsub eax, ebx\n";
                if (isNumericType)
                {
                    str += "\tjns _positive_" + itoS(labelCounter) + "\n";
                    str += "\tcall handle_negative\n";
                    str += "_positive_" + itoS(labelCounter++) + ":\n";
                }
                str += "\tpush eax\n";
            }
            else if (lex == LEX_STAR)
            {
                str += "\tpop ebx\n\tpop eax\n\tmul ebx\n\tpush eax\n";
            }
            else if (lex == LEX_DIRSLASH)
            {
                // деление с проверкой делител€ на 0
                str += "\tpop ebx\n";
                str += "\tcmp ebx, 0\n";
                str += "\tje handle_div_zero\n";
                str += "\tpop eax\n";
                str += "\txor edx, edx\n";
                str += "\tdiv ebx\n";
                str += "\tpush eax\n";
            }
            else if (lex == LEX_EXCLAMATION)
            {
                str += "\tpop eax\n\txor eax, 1\n\tpush eax\n";
            }
            else if (lex == LEX_PLUSPLUS)
            {
                str += "\tpop eax\n\tinc eax\n\tpush eax\n";
            }
            else if (lex == LEX_MINUSMINUS)
            {
                str += "\tpop eax\n\tdec eax\n";
                if (isNumericType)
                {
                    str += "\tjns _positive_dec_" + itoS(labelCounter) + "\n";
                    str += "\tcall handle_negative\n";
                    str += "_positive_dec_" + itoS(labelCounter++) + ":\n";
                }
                str += "\tpush eax\n";
            }
        }

        str += "\tpop eax\n\tmov " + mangleId(lval.id) + ", eax\n";

        return str;
    }

    void CodeGeneration(Lexer::LEX& tables, Parm::PARM& parm, Log::LOG& log)
    {
        ofstream stream(parm.out);
        if (!stream.is_open()) return;

        Head(stream, tables.idtable);

        string funcName;
        bool inFunction = false;
        stack<string> switchEndLabels;
        stack<string> nextCaseLabels;

        for (int i = 0; i < tables.lextable.size; i++)
        {
            char lex = LEXEMA(i);

            switch (lex)
            {
            case LEX_FUNCTION:
            {
                IT::Entry func = ITENTRY(i + 1);
                funcName = mangleId(func.id);
                inFunction = true;

                stream << "\n; ----------- " << funcName << " ------------\n";
                stream << funcName << " PROC";

                {
                    vector<string> params;
                    for (int j = i + 3; LEXEMA(j) != LEX_RIGHTTHESIS; j++)
                    {
                        if (LEXEMA(j) == LEX_ID)
                        {
                            IT::Entry param = ITENTRY(j);
                            if (param.idtype == IT::IDTYPE::P)
                                params.push_back(mangleId(param.id) + ":DWORD");
                        }
                    }

                    for (size_t p = 0; p < params.size(); p++)
                    {
                        if (p == 0) stream << " ";
                        stream << params[p];
                        if (p < params.size() - 1) stream << ", ";
                    }
                    stream << "\n";
                }
                break;
            }

            case LEX_MAIN:
            {
                stream << "\n; ----------- MAIN ------------\n";
                stream << "main PROC\n";
                inFunction = false;
                break;
            }

            case LEX_VAR:
            {
                while (i < tables.lextable.size && LEXEMA(i) != LEX_SEMICOLON)
                {
                    if (LEXEMA(i) == LEX_EQUAL && i > 0 && i + 1 < tables.lextable.size)
                    {
                        int varIdx = tables.lextable.table[i - 1].idxTI;
                        int valIdx = tables.lextable.table[i + 1].idxTI;

                        bool isSimpleLiteral = false;

                        if (varIdx != NULLIDX_TI && valIdx != NULLIDX_TI)
                        {
                            if (i + 2 < tables.lextable.size && LEXEMA(i + 2) == LEX_SEMICOLON)
                            {
                                IT::Entry val = tables.idtable.table[valIdx];
                                if (val.idtype == IT::IDTYPE::L)
                                {
                                    isSimpleLiteral = true;
                                    string varName = mangleId(tables.idtable.table[varIdx].id);
                                    string litName = mangleId(val.id);

                                    if (val.iddatatype == IT::IDDATATYPE::STR || val.iddatatype == IT::IDDATATYPE::CHAR)
                                    {
                                        stream << "\tlea eax, " << litName << "\n";
                                        stream << "\tmov " << varName << ", eax\n";
                                    }
                                    else
                                    {
                                        stream << "\tmov eax, " << litName << "\n";
                                        stream << "\tmov " << varName << ", eax\n";
                                    }
                                }
                            }
                        }

                        if (!isSimpleLiteral && varIdx != NULLIDX_TI)
                        {
                            stream << genEqualCode(tables, i);
                            while (i < tables.lextable.size && LEXEMA(i) != LEX_SEMICOLON) i++;
                        }
                    }
                    i++;
                }
                break;
            }

            case LEX_EQUAL:
            {
                if (i > 0 && LEXEMA(i - 1) == LEX_ID)
                {
                    stream << genEqualCode(tables, i);
                    while (i < tables.lextable.size && LEXEMA(i) != LEX_SEMICOLON) i++;
                }
                break;
            }

            case LEX_PLUSPLUS:
            {
                if (i > 0 && LEXEMA(i - 1) == LEX_ID)
                {
                    IT::Entry var = ITENTRY(i - 1);
                    stream << "\tinc " << mangleId(var.id) << "\n";
                }
                break;
            }

            case LEX_MINUSMINUS:
            {
                if (i > 0 && LEXEMA(i - 1) == LEX_ID)
                {
                    IT::Entry var = ITENTRY(i - 1);
                    string varName = mangleId(var.id);
                    if (var.iddatatype == IT::IDDATATYPE::INT)
                    {
                        string checkLabel = makeLabel();
                        stream << "\tmov eax, " << varName << "\n";
                        stream << "\ttest eax, eax\n";
                        stream << "\tjz " << checkLabel << "\n";
                        stream << "\tdec " << varName << "\n";
                        stream << "\tjmp " << checkLabel << "_end\n";
                        stream << checkLabel << ":\n";
                        stream << "\tcall handle_negative\n";
                        stream << "\tmov " << varName << ", eax\n";
                        stream << checkLabel << "_end:\n";
                    }
                    else
                    {
                        stream << "\tdec " << varName << "\n";
                    }
                }
                break;
            }

            case LEX_SWITCH:
            {
                string endLabel = makeLabel();
                switchEndLabels.push(endLabel);

                int exprStart = i + 2;
                stream << "\tmov eax, " << mangleId(ITENTRY(exprStart).id) << "\n";
                stream << "\tmov tmp_switch_value, eax\n";
                break;
            }

            case LEX_CASE:
            {
                if (!nextCaseLabels.empty())
                {
                    stream << nextCaseLabels.top() << ":\n";
                    nextCaseLabels.pop();
                }

                string caseCodeLabel = makeLabel();
                string nextLabel = makeLabel();

                if (i + 1 < tables.lextable.size)
                {
                    IT::Entry caseVal = ITENTRY(i + 1);
                    stream << "\tmov eax, tmp_switch_value\n";
                    stream << "\tcmp eax, " << mangleId(caseVal.id) << "\n";
                    stream << "\tje " << caseCodeLabel << "\n";
                    stream << "\tjmp " << nextLabel << "\n";
                    stream << caseCodeLabel << ":\n";
                }

                nextCaseLabels.push(nextLabel);

                while (i < tables.lextable.size && LEXEMA(i) != ':') i++;
                break;
            }

            case LEX_DEFAULT:
            {
                if (!nextCaseLabels.empty())
                {
                    stream << nextCaseLabels.top() << ":\n";
                    nextCaseLabels.pop();
                }

                while (i < tables.lextable.size && LEXEMA(i) != ':') i++;
                break;
            }

            case LEX_BREAK:
            {
                if (!switchEndLabels.empty())
                {
                    stream << "\tjmp " << switchEndLabels.top() << "\n";
                }
                break;
            }

            case LEX_OUTPUT:
            {
                if (i + 1 < tables.lextable.size && tables.lextable.table[i + 1].idxTI != NULLIDX_TI)
                {
                    IT::Entry e = ITENTRY(i + 1);
                    string ename = mangleId(e.id);

                    if (e.iddatatype == IT::IDDATATYPE::INT || e.iddatatype == IT::IDDATATYPE::BOOL)
                    {
                        stream << "\tpush " << ename << "\n\tcall outlich\n";
                    }
                    else
                    {
                        if (e.idtype == IT::IDTYPE::L)
                            stream << "\tpush offset " << ename << "\n";
                        else
                            stream << "\tpush " << ename << "\n";
                        stream << "\tcall outrad\n";
                    }
                    i++;
                }
                break;
            }

            case LEX_NEWLINE:
                stream << "\tpush offset newline\n\tcall outrad\n";
                break;

            case LEX_RETURN:
            {
                if (i + 1 < tables.lextable.size && LEXEMA(i + 1) != LEX_SEMICOLON)
                {
                    stream << "\tmov eax, " << mangleId(ITENTRY(i + 1).id) << "\n";
                }
                stream << "\tret\n";
                stream << funcName << " ENDP\n";

                while (i < tables.lextable.size && LEXEMA(i) != LEX_RIGHTSQUARE) i++;
                break;
            }

            case LEX_RIGHTSQUARE:
            {
                if (!switchEndLabels.empty())
                {
                    int bracketCount = 1;
                    bool foundSwitch = false;

                    for (int j = i - 1; j >= 0; j--)
                    {
                        if (LEXEMA(j) == LEX_RIGHTSQUARE)
                            bracketCount++;
                        else if (LEXEMA(j) == LEX_LEFTSQUARE)
                            bracketCount--;

                        if (bracketCount == 0)
                        {
                            for (int k = j - 1; k >= 0 && k >= j - 10; k--)
                            {
                                if (LEXEMA(k) == LEX_SWITCH)
                                {
                                    foundSwitch = true;
                                    break;
                                }
                                if (LEXEMA(k) == LEX_SEMICOLON || LEXEMA(k) == LEX_LEFTSQUARE)
                                    break;
                            }
                            break;
                        }
                    }

                    if (foundSwitch)
                    {
                        while (!nextCaseLabels.empty())
                            nextCaseLabels.pop();

                        stream << switchEndLabels.top() << ":\n";
                        switchEndLabels.pop();
                        break;
                    }
                }

                if (!inFunction)
                {
                    stream << END;
                }
                break;
            }
            }
        }

        stream.close();
    }
}
