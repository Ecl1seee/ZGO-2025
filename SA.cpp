#include "IT.h"
#include "LT.h"
#include "Error.h"
#include "LexAnalysis.h"
#include "SA.h"
#include <set>
#include <vector>
#include <iostream>
#include <string>

namespace Semantic
{
    bool semanticsCheck(Lexer::LEX& tables, Log::LOG& log)
    {
        bool sem_ok = true;

        for (int i = 0; i < tables.lextable.size; i++)
        {
            switch (tables.lextable.table[i].lexema)
            {
            case LEX_VAR:
            {
                if (i + 1 >= tables.lextable.size || tables.lextable.table[i + 1].lexema != LEX_ID_TYPE)
                {
                    sem_ok = false;
                    Log::writeError(log.stream, Error::GetError(303, tables.lextable.table[i].sn, 0));
                }
                break;
            }
            case LEX_DIRSLASH:
            {
                int k = i;
                if (i + 1 < tables.lextable.size && tables.lextable.table[i + 1].lexema == LEX_ID)
                {
                    for (k; k > 0; k--)
                    {
                        if (tables.lextable.table[k].lexema == LEX_ID)
                        {
                            if (tables.idtable.table[tables.lextable.table[k].idxTI].id == tables.idtable.table[tables.lextable.table[i + 1].idxTI].id)
                            {
                                if (k + 2 < tables.lextable.size && tables.lextable.table[k + 2].lexema == LEX_LITERAL && tables.idtable.table[tables.lextable.table[k + 2].idxTI].value.vint == 0)
                                {
                                    sem_ok = false;
                                    Log::writeError(log.stream, Error::GetError(317, tables.lextable.table[k].sn, 0));
                                }
                            }
                        }
                    }
                }
                if (i + 1 < tables.lextable.size && tables.lextable.table[i + 1].lexema == LEX_LITERAL)
                {
                    if (tables.idtable.table[tables.lextable.table[i + 1].idxTI].value.vint == 0)
                    {
                        sem_ok = false;
                        Log::writeError(log.stream, Error::GetError(317, tables.lextable.table[i].sn, 1));
                    }
                }

                // Простая проверка для ( LITERAL OP LITERAL )
                if (i + 1 < tables.lextable.size && tables.lextable.table[i + 1].lexema == LEX_LEFTHESIS)
                {
                    // Паттерн: / ( число оператор число )
                    if (i + 5 < tables.lextable.size &&
                        tables.lextable.table[i + 2].lexema == LEX_LITERAL &&
                        tables.lextable.table[i + 4].lexema == LEX_LITERAL &&
                        tables.lextable.table[i + 5].lexema == LEX_RIGHTTHESIS)
                    {
                        int val1 = tables.idtable.table[tables.lextable.table[i + 2].idxTI].value.vint;
                        int val2 = tables.idtable.table[tables.lextable.table[i + 4].idxTI].value.vint;
                        char op = tables.lextable.table[i + 3].lexema;

                        int result = 0;
                        if (op == LEX_PLUS) result = val1 + val2;
                        else if (op == LEX_MINUS) result = val1 - val2;
                        else if (op == LEX_STAR) result = val1 * val2;
                        else if (op == LEX_DIRSLASH && val2 != 0) result = val1 / val2;

                        if (result == 0)
                        {
                            sem_ok = false;
                            Log::writeError(log.stream, Error::GetError(317, tables.lextable.table[i].sn, 0));
                        }
                    }
                }

                break;
            }
            case LEX_EQUAL:
            {
                if (i > 0 && tables.lextable.table[i - 1].idxTI != NULLIDX_TI)
                {
                    IT::IDDATATYPE lefttype = tables.idtable.table[tables.lextable.table[i - 1].idxTI].iddatatype;
                    bool ignore = false;
                    bool insideFunctionCall = false;
                    for (int k = i + 1; k < tables.lextable.size && tables.lextable.table[k].lexema != LEX_SEMICOLON; k++)
                    {
                        if (tables.lextable.table[k].lexema == LEX_LEFTHESIS)
                        {
                            if (k > 0 && tables.lextable.table[k - 1].lexema == LEX_ID)
                            {
                                int idIdx = tables.lextable.table[k - 1].idxTI;
                                if (idIdx != NULLIDX_TI)
                                {
                                    IT::IDTYPE idtype = tables.idtable.table[idIdx].idtype;
                                    if (idtype == IT::IDTYPE::F || idtype == IT::IDTYPE::S)
                                    {
                                        insideFunctionCall = true;
                                        ignore = true;
                                    }
                                }
                            }
                        }
                        else if (tables.lextable.table[k].lexema == LEX_RIGHTTHESIS && insideFunctionCall)
                        {
                            insideFunctionCall = false;
                            ignore = false;
                            continue;
                        }

                        if (tables.lextable.table[k].idxTI != NULLIDX_TI && !ignore)
                        {
                            IT::IDDATATYPE righttype = tables.idtable.table[tables.lextable.table[k].idxTI].iddatatype;

                            int idIdx = tables.lextable.table[k].idxTI;
                            IT::IDTYPE idtype = tables.idtable.table[idIdx].idtype;

                            if (idtype != IT::IDTYPE::F && idtype != IT::IDTYPE::S)
                            {
                                if (lefttype != righttype)
                                {
                                    Log::writeError(log.stream, Error::GetError(314, tables.lextable.table[k].sn, 0));
                                    sem_ok = false;
                                    break;
                                }
                            }
                        }

                        if (lefttype == IT::IDDATATYPE::STR && !ignore)
                        {
                            char l = tables.lextable.table[k].lexema;
                            if (l == LEX_MINUS || l == LEX_STAR || l == LEX_DIRSLASH || l == LEX_PLUS)
                            {
                                Log::writeError(log.stream, Error::GetError(316, tables.lextable.table[k].sn, 0));
                                sem_ok = false;
                                break;
                            }
                        }
                    }
                }
                break;
            }

            case LEX_ID:
            {
                if (tables.lextable.table[i].idxTI == NULLIDX_TI)
                    break;

                IT::Entry& e = tables.idtable.table[tables.lextable.table[i].idxTI];

                if (i > 0 && tables.lextable.table[i - 1].lexema == ' ')
                {
                    if (e.idtype == IT::IDTYPE::F)
                    {
                        for (int k = i + 1; k < tables.lextable.size; k++)
                        {
                            char l = tables.lextable.table[k].lexema;
                            if (l == LEX_RETURN)
                            {
                                if (k + 1 < tables.lextable.size)
                                {
                                    int next = tables.lextable.table[k + 1].idxTI;
                                    if (next != NULLIDX_TI)
                                    {
                                        if (tables.idtable.table[next].iddatatype != e.iddatatype)
                                        {
                                            Log::writeError(log.stream, Error::GetError(315, tables.lextable.table[k].sn, 0));
                                            sem_ok = false;
                                            break;
                                        }
                                    }
                                }
                                break;
                            }
                        }
                    }
                }

                // ПРОВЕРКА ВЫЗОВА ФУНКЦИИ
                if (i + 1 < tables.lextable.size &&
                    tables.lextable.table[i + 1].lexema == LEX_LEFTHESIS &&
                    (i == 0 || tables.lextable.table[i - 1].lexema != LEX_FUNCTION))
                {
                    if (e.idtype == IT::IDTYPE::F || e.idtype == IT::IDTYPE::S)
                    {
                        int expectedParamsCount = e.parmQuantity;

                        int paramscount = 0;
                        std::vector<int> argIndices;
                        int j = i + 2;
                        int nesting_level = 1;

                        bool isEmpty = (j < tables.lextable.size &&
                            tables.lextable.table[j].lexema == LEX_RIGHTTHESIS);

                        if (!isEmpty)
                        {
                            paramscount = 1;
                            bool found_current = false;

                            while (j < tables.lextable.size)
                            {
                                char currentLex = tables.lextable.table[j].lexema;

                                if (currentLex == LEX_LEFTHESIS)
                                {
                                    nesting_level++;
                                }
                                else if (currentLex == LEX_RIGHTTHESIS)
                                {
                                    nesting_level--;
                                    if (nesting_level == 0)
                                        break;
                                }

                                if (currentLex == LEX_COMMA && nesting_level == 1)
                                {
                                    paramscount++;
                                    found_current = false;
                                }

                                if (!found_current && (currentLex == LEX_ID || currentLex == LEX_LITERAL))
                                {
                                    argIndices.push_back(j);
                                    found_current = true;
                                }

                                j++;
                            }
                        }

                        // Ошибка 307: Превышено максимальное количество параметров
                        if (paramscount > 3)
                        {
                            Log::writeError(log.stream, Error::GetError(307, tables.lextable.table[i].sn, 0));
                            sem_ok = false;
                        }
                        // Ошибка 308: Несовпадение количества параметров
                        else if (paramscount != expectedParamsCount)
                        {
                            Log::writeError(log.stream, Error::GetError(308, tables.lextable.table[i].sn, 0));
                            sem_ok = false;
                        }
                        // ошибка 309: несовпадение типов параметров 
                        else if (paramscount > 0)
                        {
                            std::vector<IT::Entry*> functionParams;
                            std::string funcName = e.id;
                            std::string prefix = funcName + ".";

                            for (int idx = 0; idx < tables.idtable.size; idx++)
                            {
                                if (tables.idtable.table[idx].idtype == IT::IDTYPE::P)
                                {
                                    std::string paramId = tables.idtable.table[idx].id;
                                    if (paramId.length() > prefix.length() &&
                                        paramId.substr(0, prefix.length()) == prefix)
                                    {
                                        functionParams.push_back(&tables.idtable.table[idx]);
                                    }
                                }
                            }

                            for (int idx = 0; idx < (int)argIndices.size() && idx < (int)functionParams.size(); idx++)
                            {
                                int argPos = argIndices[idx];
                                if (argPos < tables.lextable.size &&
                                    tables.lextable.table[argPos].idxTI != NULLIDX_TI)
                                {
                                    IT::IDDATATYPE argType = tables.idtable.table[tables.lextable.table[argPos].idxTI].iddatatype;
                                    IT::IDDATATYPE paramType = functionParams[idx]->iddatatype;

                                    if (argType != paramType)
                                    {
                                        Log::writeError(log.stream, Error::GetError(309, tables.lextable.table[argPos].sn, 0));
                                        sem_ok = false;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            }
            }
        }
        return sem_ok;
    }
};
