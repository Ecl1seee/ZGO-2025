#include "Header.h"
#include "Poland.h"
#include "LexAnalysis.h"
#include "IT.h"
#include "LT.h"
#include "Error.h"
#include <stack>
#include <vector>
#include <cstring>
#include <iostream>
#include <iomanip>

using namespace std;

typedef std::vector<LT::Entry> ltvec;
typedef std::vector<int> intvec;

namespace Polish
{
    int getPriority(LT::Entry& e)
    {
        switch (e.lexema)
        {
        case LEX_LEFTHESIS:
        case LEX_RIGHTTHESIS:
            return 0;
        case LEX_PLUS:
        case LEX_MINUS:
            return 1;
        case LEX_STAR:
        case LEX_DIRSLASH:
            return 2;
        case LEX_EXCLAMATION:
        case LEX_PLUSPLUS:
        case LEX_MINUSMINUS:
            return 3;
        default:
            return -1;
        }
    }

    bool PolishNotation(Lexer::LEX& tbls, Log::LOG& log)
    {
        unsigned curExprBegin = 0;
        ltvec v;
        LT::LexTable new_table = LT::Create(tbls.lextable.maxsize);
        intvec vpositions = getExprPositions(tbls);

        *log.stream << std::endl << "--------------------------------- POLISH NOTATION ---------------------------------" << std::endl;

        for (int i = 0; i < tbls.lextable.size; i++)
        {
            if (curExprBegin < vpositions.size() && i == vpositions[curExprBegin])
            {
                int lexcount = fillVector(vpositions[curExprBegin], tbls.lextable, v);
                if (lexcount > 1)
                {
                    *log.stream << "ORIGINAL [" << std::setw(3) << std::right << v[0].sn << "]: ";
                    for (size_t k = 0; k < v.size(); k++)
                    {
                        if (v[k].lexema == LEX_ID)
                            *log.stream << tbls.idtable.table[v[k].idxTI].id;
                        else if (v[k].lexema == LEX_LITERAL && v[k].idxTI != NULLIDX_TI)
                        {
                            if (tbls.idtable.table[v[k].idxTI].iddatatype == IT::IDDATATYPE::STR)
                                *log.stream << "\"" << tbls.idtable.table[v[k].idxTI].value.vstr.str << "\"";
                            else
                                *log.stream << tbls.idtable.table[v[k].idxTI].value.vint;
                        }
                        else
                        {
                            *log.stream << (char)v[k].lexema;
                        }
                    }
                    *log.stream << std::endl;

                    bool rc = setPolishNotation(tbls.idtable, log, vpositions[curExprBegin], v);
                    if (!rc)
                        return false;

                    *log.stream << "POLISH   [" << std::setw(3) << std::right << v[0].sn << "]: ";
                    for (size_t k = 0; k < v.size(); k++)
                    {
                        if (v[k].lexema == LEX_ID)
                            *log.stream << tbls.idtable.table[v[k].idxTI].id;
                        else if (v[k].lexema == LEX_LITERAL && v[k].idxTI != NULLIDX_TI)
                        {
                            if (tbls.idtable.table[v[k].idxTI].iddatatype == IT::IDDATATYPE::STR)
                                *log.stream << "\"" << tbls.idtable.table[v[k].idxTI].value.vstr.str << "\"";
                            else
                                *log.stream << tbls.idtable.table[v[k].idxTI].value.vint;
                        }
                        else
                        {
                            if (v[k].lexema == '@') *log.stream << "@";
                            else *log.stream << (char)v[k].lexema;
                        }
                        *log.stream << " ";
                    }
                    *log.stream << std::endl;
                }

                addToTable(new_table, tbls.idtable, v);
                i += lexcount - 1;
                curExprBegin++;
                continue;
            }

            if (tbls.lextable.table[i].lexema == LEX_ID || tbls.lextable.table[i].lexema == LEX_LITERAL)
            {
                int firstind = Lexer::getIndexInLT(new_table, tbls.lextable.table[i].idxTI);
                if (firstind == -1)
                    firstind = new_table.size;
                tbls.idtable.table[tbls.lextable.table[i].idxTI].idxfirstLE = firstind;
            }
            LT::Add(new_table, tbls.lextable.table[i]);
        }

        if (tbls.lextable.table != NULL)
            delete[] tbls.lextable.table;
        tbls.lextable = new_table;
        return true;
    }

    int fillVector(int posExprBegin, LT::LexTable& lextable, ltvec& v)
    {
        v.clear();
        for (int i = posExprBegin; i < lextable.size; i++)
        {
            if (lextable.table[i].lexema == LEX_SEMICOLON)
                break;
            else
                v.push_back(lextable.table[i]);
        }
        return v.size();
    }

    void addToTable(LT::LexTable& new_table, IT::IdTable& idtable, ltvec& v)
    {
        for (size_t i = 0; i < v.size(); i++)
        {
            LT::Add(new_table, v[i]);
            if (v[i].lexema == LEX_ID || v[i].lexema == LEX_LITERAL)
            {
                int firstind = Lexer::getIndexInLT(new_table, v[i].idxTI);
                if (firstind == -1)
                    firstind = new_table.size - 1;
                idtable.table[v[i].idxTI].idxfirstLE = firstind;
            }
        }
    }

    intvec getExprPositions(Lexer::LEX& tbls)
    {
        intvec v;
        bool f_begin = false;
        int begin = 0;

        for (int i = 0; i < tbls.lextable.size; i++)
        {
            char l = tbls.lextable.table[i].lexema;

            if (l == LEX_VAR)
            {
                while (i < tbls.lextable.size && tbls.lextable.table[i].lexema != LEX_SEMICOLON)
                    i++;
                continue;
            }

            if (l == LEX_EQUAL && !f_begin)
            {
                if (i + 1 < tbls.lextable.size && tbls.lextable.table[i + 1].lexema != LEX_SEMICOLON)
                {
                    begin = i + 1;
                    f_begin = true;
                }
                continue;
            }

            if (l == LEX_RETURN && !f_begin)
            {
                if (i + 1 < tbls.lextable.size && tbls.lextable.table[i + 1].lexema != LEX_SEMICOLON)
                {
                    begin = i + 1;
                    f_begin = true;
                }
                continue;
            }

            if (f_begin && l == LEX_SEMICOLON)
            {
                v.push_back(begin);
                f_begin = false;
            }
        }
        return v;
    }

    bool setPolishNotation(IT::IdTable& idtable, Log::LOG& log, int lextable_pos, ltvec& v)
    {
        vector<LT::Entry> result;
        stack<LT::Entry> s;

        for (size_t i = 0; i < v.size(); i++)
        {
            char lx = v[i].lexema;
            int priority = getPriority(v[i]);

            if (lx == LEX_LEFTHESIS || lx == LEX_RIGHTTHESIS ||
                lx == LEX_PLUS || lx == LEX_MINUS ||
                lx == LEX_STAR || lx == LEX_DIRSLASH ||
                lx == LEX_PLUSPLUS || lx == LEX_MINUSMINUS ||
                lx == LEX_EXCLAMATION)
            {
                if (s.empty() || lx == LEX_LEFTHESIS)
                {
                    s.push(v[i]);
                    continue;
                }

                if (lx == LEX_RIGHTTHESIS)
                {
                    while (!s.empty() && s.top().lexema != LEX_LEFTHESIS)
                    {
                        result.push_back(s.top());
                        s.pop();
                    }
                    if (!s.empty() && s.top().lexema == LEX_LEFTHESIS)
                        s.pop();

                    // Проверяем - была ли функция перед скобкой
                    if (!s.empty() && s.top().lexema == LEX_ID && s.top().idxTI != NULLIDX_TI &&
                        (idtable.table[s.top().idxTI].idtype == IT::IDTYPE::F ||
                            idtable.table[s.top().idxTI].idtype == IT::IDTYPE::S))
                    {
                        result.push_back(s.top());
                        s.pop();

                        // Добавляем метку вызова
                        LT::Entry call_marker;
                        call_marker.lexema = '@';
                        call_marker.sn = v[i].sn;
                        call_marker.idxTI = NULLIDX_TI;
                        result.push_back(call_marker);
                    }
                    continue;
                }

                if (lx == LEX_EXCLAMATION || lx == LEX_PLUSPLUS || lx == LEX_MINUSMINUS)
                {
                    while (!s.empty() && getPriority(s.top()) > priority)
                    {
                        result.push_back(s.top());
                        s.pop();
                    }
                }
                else
                {
                    while (!s.empty() && getPriority(s.top()) >= priority)
                    {
                        result.push_back(s.top());
                        s.pop();
                    }
                }
                s.push(v[i]);
            }
            else if (lx == LEX_LITERAL || lx == LEX_ID)
            {
                if (v[i].idxTI != NULLIDX_TI &&
                    (idtable.table[v[i].idxTI].idtype == IT::IDTYPE::F ||
                        idtable.table[v[i].idxTI].idtype == IT::IDTYPE::S) &&
                    i + 1 < v.size() && v[i + 1].lexema == LEX_LEFTHESIS)
                {
                    s.push(v[i]);
                }
                else
                {
                    result.push_back(v[i]);
                }
            }
            else if (lx == LEX_COMMA)
            {
                while (!s.empty() && s.top().lexema != LEX_LEFTHESIS)
                {
                    result.push_back(s.top());
                    s.pop();
                }
            }
            else
            {
                Log::writeError(log.stream, Error::GetError(201, v[i].sn, 0));
                return false;
            }
        }

        while (!s.empty())
        {
            result.push_back(s.top());
            s.pop();
        }

        v = result;
        return true;
    }
}
