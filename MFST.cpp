#include "Header.h"
#include "Error.h"
#include <stack>
#include <iostream>
#include <iomanip>
#include <cstdio>

extern "C" {
    int FST_TRACE_n = -1;
    char rbuf[205], sbuf[205], lbuf[1024];
}

namespace MFST
{
    // Состояние автомата
    MfstState::MfstState() : lenta_position(0), nrule(-1), nrulechain(-1) {}

    MfstState::MfstState(short pposition, MFSTSTSTACK pst, short pnrulechain)
        : lenta_position(pposition), st(pst), nrule(-1), nrulechain(pnrulechain) {}

    MfstState::MfstState(short pposition, MFSTSTSTACK pst, short pnrule, short pnrulechain)
        : lenta_position(pposition), st(pst), nrule(pnrule), nrulechain(pnrulechain) {}

    //Диагностика ошибки

    Mfst::MfstDiagnosis::MfstDiagnosis() : lenta_position(-1), rc_step(SURPRISE), nrule(-1), nrule_chain(-1) {}

    Mfst::MfstDiagnosis::MfstDiagnosis(short plenta_position, RC_STEP prc_step, short pnrule, short pnrule_chain)
        : lenta_position(plenta_position), rc_step(prc_step), nrule(pnrule), nrule_chain(pnrule_chain) {}


    Mfst::Mfst() : lenta(0), lenta_size(0), lenta_position(0) {}

    Mfst::Mfst(Lexer::LEX plex, GRB::Greibach pgrebach)
    {
        grebach = pgrebach;
        lex = plex;

        // 1. Инициализация ленты
        lenta_size = lex.lextable.size;
        lenta = new short[lenta_size];
        for (int k = 0; k < lenta_size; k++)
        {
            lenta[k] = TS(lex.lextable.table[k].lexema); // TS - преобразование лексемы в символ алфавита
        }

        lenta_position = 0;
        nrulechain = -1;

        // 2. Инициализация стека
        st.push(grebach.stbottomT); // Дно стека
        st.push(grebach.startN);   // Начальный нетерминал
    }

    // --- Основной Шаг Автомата ---
    Mfst::RC_STEP Mfst::step(const Log::LOG& log)
    {
        RC_STEP rc = SURPRISE;

        // 1. Проверка конца ленты
        if (lenta_position >= lenta_size)
        {
            rc = LENTA_END;
            MFST_TRACE4(log, "LENTA_END")
                return rc;
        }

        // 2. Если на вершине стека НЕ ТЕРМИНАЛ (НТ)
        if (ISNS(st.top()))
        {
            GRB::Rule rule;
            // Поиск правила для НТ
            if ((nrule = grebach.getRule(st.top(), rule)) >= 0)
            {
                GRB::Rule::Chain chain;
                // Поиск подходящей цепочки для текущего символа ленты
                if ((nrulechain = rule.getNextChain(lenta[lenta_position], chain, nrulechain + 1)) >= 0)
                {
                    // Найдена цепочка: СОХРАНЕНИЕ СОСТОЯНИЯ, РАСШИРЕНИЕ, NS_OK
                    MFST_TRACE1(log)
                        savestate(log);
                    st.pop();
                    push_chain(chain);
                    rc = NS_OK;
                    MFST_TRACE2(log)
                }
                else
                {
                    // Цепочка не найдена: ПОПЫТКА ОТКАТА (backtracking)
                    MFST_TRACE4(log, "TNS_NORULECHAIN/NS_NORULE")
                        savediagnois(NS_NORULECHAIN);

                    // Если откат успешен -> NS_NORULECHAIN (пробуем дальше), иначе NS_NORULE (ошибка)
                    rc = reststate(log) ? NS_NORULECHAIN : NS_NORULE;
                };
            }
            else
            {
                // Правило для НТ не найдено (серьезная ошибка грамматики)
                rc = NS_ERROR;
            }
        }
        // 3. Если на вершине стека ТЕРМИНАЛ (Т)
        else if (st.top() == lenta[lenta_position])
        {
            // Символы совпали: ПРОДВИЖЕНИЕ ПО ЛЕНТЕ
            lenta_position++;
            st.pop();
            nrulechain = -1;
            rc = TS_OK;
            MFST_TRACE3(log)
        }
        else
        {
            MFST_TRACE4(log, "TS_NOK/NS_NORULECHАIN")
                rc = reststate(log) ? TS_NOK : NS_NORULECHAIN;
        };

        return rc;
    };

    // --- Вспомогательные Функции ---

    // Расширение: заталкивает символы цепочки в стек (в обратном порядке)
    bool Mfst::push_chain(GRB::Rule::Chain chain)
    {
        for (int k = chain.size - 1; k >= 0; k--)
        {
            st.push(chain.nt[k]);
        }
        return true;
    };

    // Сохранение текущего состояния
    bool Mfst::savestate(const Log::LOG& log)
    {
        storestate.push(MfstState(lenta_position, st, nrule, nrulechain));
        MFST_TRACE6(log, "SAVESTATE:", storestate.size());
        return true;
    };

    // Восстановление предыдущего состояния (откат)
    bool Mfst::reststate(const Log::LOG& log)
    {
        bool rc = false;
        MfstState state;
        if (rc = (storestate.size() > 0))
        {
            state = storestate.top();
            lenta_position = state.lenta_position;
            st = state.st;
            nrule = state.nrule;
            nrulechain = state.nrulechain;
            storestate.pop();
            MFST_TRACE5(log, "RESSTATE")
                MFST_TRACE2(log)
        };
        return rc;
    };

    // Сохранение диагностической информации
    bool Mfst::savediagnois(RC_STEP prc_step)
    {
        bool rc = false;
        short k = 0;
        // Находим место в массиве diagnosis, где можно сохранить новое состояние
        while (k < MFST_DIAGN_digit && lenta_position <= diagnosis[k].lenta_position)
        {
            k++;
        }

        if (rc = (k < MFST_DIAGN_digit))
        {
            diagnosis[k] = MfstDiagnosis(lenta_position, prc_step, nrule, nrulechain);
            // Сбрасываем остальные, чтобы новая диагностика была самой приоритетной
            for (short j = k + 1; j < MFST_DIAGN_digit; j++)
            {
                diagnosis[j].lenta_position = -1;
            }
        };
        return rc;
    };

    // --- ГЛАВНЫЙ МЕТОД ЗАПУСКА ---
    bool Mfst::start(const Log::LOG& log)
    {
        bool rc = false;
        RC_STEP rc_step = SURPRISE;
        char buf[MFST_DIAGN_MAXSIZE];

        // 1. Первый шаг
        rc_step = step(log);

        // 2. Цикл автомата
        while (rc_step == NS_OK || rc_step == NS_NORULECHAIN || rc_step == TS_OK || rc_step == TS_NOK)
        {
            rc_step = step(log);
        }

        // 3. Обработка завершения
        switch (rc_step)
        {
        case LENTA_END:
            MFST_TRACE4(log, "------>LENTA_END\n ---------------------------------------------------------")
                * log.stream << "-------------------------------------------------------------------------------------" << std::endl;

            // Условие успешного завершения: лента закончилась, а в стеке только дно
            if (!st.empty() && st.top() != grebach.stbottomT)
            {
                *log.stream << "ERROR: Lenta end, but stack is not empty." << std::endl;

                // Очистка сохраненных состояний
                while (!storestate.empty()) storestate.pop();

                // Выбрасываем ошибку 600 (Неверная структура программы)
                int lastLine = 0;
                if (lex.lextable.size > 0)
                    lastLine = lex.lextable.table[lex.lextable.size - 1].sn;

                throw Error::GetError(600, lastLine, 0);
            }
            else
            {
                // Успешное завершение
                sprintf_s(buf, MFST_DIAGN_MAXSIZE, ":total number of lines %d, syntax analysis done without errors", lenta_size);
                *log.stream << std::setw(4) << std::left << 0 << buf << std::endl;
                rc = true;
            }
            break;

        case NS_NORULE:
            MFST_TRACE4(log, "------>NS_NORULE")
                * log.stream << "-------------------------------------------------------------------------------------" << std::endl;
            *log.stream << getDiagnosis(0, buf) << std::endl;
            *log.stream << getDiagnosis(1, buf) << std::endl;
            *log.stream << getDiagnosis(2, buf) << std::endl;

            // Выброс наиболее вероятной ошибки
            if (diagnosis[0].lenta_position >= 0 && diagnosis[0].lenta_position < lex.lextable.size)
            {
                int line = lex.lextable.table[diagnosis[0].lenta_position].sn;
                int errCode = grebach.getRule(diagnosis[0].nrule).iderror;
                throw Error::GetError(errCode, line, 0);
            }
            else
            {
                throw Error::GetError(600, 0, 0);
            }
            break;

        case NS_NORULECHAIN:
            MFST_TRACE4(log, "------>NS_NORULECHAIN")
                if (lenta_position < lex.lextable.size)
                    throw Error::GetError(600, lex.lextable.table[lenta_position].sn, 0);
                else
                    throw Error::GetError(600, -1, 0);
            break;

        case NS_ERROR:
        case SURPRISE:
            MFST_TRACE4(log, (rc_step == NS_ERROR ? "------>NS_ERROR" : "------>SURPRISE"))
                throw Error::GetError(600, 0, 0);
        };
        return rc;
    };

    // --- Диагностические и Трассировочные Функции ---

    char* Mfst::getCSt(char* buf)
    {
        short p;
        for (int k = (signed)st.size() - 1; k >= 0; --k)
        {
            p = st.c[k];
            buf[st.size() - 1 - k] = GRB::Rule::Chain::alphabet_to_char(p);
        };
        buf[st.size()] = 0x00;
        return buf;
    };

    char* Mfst::getCLenta(char* buf, short pos, short n)
    {
        short i, k = (pos + n < lenta_size) ? pos + n : lenta_size;
        for (i = pos; i < k; i++)
        {
            buf[i - pos] = GRB::Rule::Chain::alphabet_to_char(lenta[i]);
        }
        buf[i - pos] = 0x00;
        return buf;
    };

    char* Mfst::getDiagnosis(short n, char* buf)
    {
        char* rc = new char[2];
        int errid = 0;
        int lpos = -1;

        if (n < MFST_DIAGN_digit && (lpos = diagnosis[n].lenta_position) >= 0)
        {
            errid = grebach.getRule(diagnosis[n].nrule).iderror;
            Error::ERROR err = Error::GetError(errid);

            sprintf_s(buf, MFST_DIAGN_MAXSIZE, "Ошибка %d: строка %d, %s", err.id, lex.lextable.table[lpos].sn, err.message);
            rc = buf;
        }
        return rc; 
    };

    void Mfst::printrules(const Log::LOG& log)
    {
        MfstState state;
        GRB::Rule rule;
        for (unsigned short k = 0; k < storestate.size(); k++)
        {
            state = storestate.c[k];
            rule = grebach.getRule(state.nrule);
            MFST_TRACE7(log) // Использует state.nrule
        };
    };

    bool Mfst::savededucation()
    {
        MfstState state;
        GRB::Rule rule;
        deducation.nrules = new short[deducation.size = (short)storestate.size()];
        deducation.nrulechains = new short[deducation.size];

        for (unsigned short k = 0; k < storestate.size(); k++)
        {
            state = storestate.c[k];
            deducation.nrules[k] = state.nrule;
            deducation.nrulechains[k] = state.nrulechain;
        };
        return true;
    };
};