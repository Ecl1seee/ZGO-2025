#pragma once
#include "Header.h"
#include "GRB.h"
#include "LexAnalysis.h"
#include <stack>
#include <iomanip>

extern int FST_TRACE_n;
extern char rbuf[205], sbuf[205], lbuf[1024];

// Класс-обертка для доступа к контейнеру стека
class my_stack_SHORT : public std::stack<short> {
public:
	using std::stack<short>::c;
};

#define MFST_DIAGN_MAXSIZE 2 * ERROR_MAXSIZE_MESSAGE
#define MFST_DIAGN_digit 3

typedef my_stack_SHORT MFSTSTSTACK;

// --- МАКРОСЫ ТРАССИРОВКИ  ---

// Вывод и в файл (log.stream), и в консоль (std::cout)
#define MFST_TRACE_START(LOG) \
    *log.stream << std::setw(4) << std::left << "Step" << ":" << std::setw(20) << std::left << "Rule" << std::setw(30) << std::left << "Entered Lenta" << std::setw(20) << std::left << "Stack" << std::endl; \
   /* std::cout   << std::setw(4) << std::left << "Шаг" << ":" << std::setw(20) << std::left << "Правило" << std::setw(30) << std::left << "Входная лента" << std::setw(20) << std::left << "Стек" << std::endl;*/

#define MFST_TRACE1(LOG) \
    *log.stream << std::setw(4) << std::left << ++FST_TRACE_n << ": " << std::setw(20) << std::left << rule.getCRule(rbuf, nrulechain) << std::setw(30) << std::left << getCLenta(lbuf, lenta_position) << std::setw(20) << std::left << getCSt(sbuf) << std::endl; \
    /*std::cout   << std::setw(4) << std::left << FST_TRACE_n   << ": " << std::setw(20) << std::left << rule.getCRule(rbuf, nrulechain) << std::setw(30) << std::left << getCLenta(lbuf, lenta_position) << std::setw(20) << std::left << getCSt(sbuf) << std::endl;*/

#define MFST_TRACE2(LOG) \
    *log.stream << std::setw(4) << std::left << FST_TRACE_n << ": " << std::setw(20) << std::left << " " << std::setw(30) << std::left << getCLenta(lbuf, lenta_position) << std::setw(20) << std::left << getCSt(sbuf) << std::endl; \
   /* std::cout   << std::setw(4) << std::left << FST_TRACE_n << ": " << std::setw(20) << std::left << " " << std::setw(30) << std::left << getCLenta(lbuf, lenta_position) << std::setw(20) << std::left << getCSt(sbuf) << std::endl;*/

#define MFST_TRACE3(LOG) \
    *log.stream << std::setw(4) << std::left << ++FST_TRACE_n << ": " << std::setw(20) << std::left << " " << std::setw(30) << std::left << getCLenta(lbuf, lenta_position) << std::setw(20) << std::left << getCSt(sbuf) << std::endl; \
    /*std::cout   << std::setw(4) << std::left << FST_TRACE_n   << ": " << std::setw(20) << std::left << " " << std::setw(30) << std::left << getCLenta(lbuf, lenta_position) << std::setw(20) << std::left << getCSt(sbuf) << std::endl;*/

#define MFST_TRACE4(LOG, c) \
    *log.stream << std::setw(4) << std::left << ++FST_TRACE_n << ": " << std::setw(20) << std::left << c << std::endl; \
    /*std::cout   << std::setw(4) << std::left << FST_TRACE_n   << ": " << std::setw(20) << std::left << c << std::endl;*/

#define MFST_TRACE5(LOG, c) \
    *log.stream << std::setw(4) << std::left << FST_TRACE_n << ": " << std::setw(20) << std::left << c << std::endl; \
    /*std::cout   << std::setw(4) << std::left << FST_TRACE_n << ": " << std::setw(20) << std::left << c << std::endl;*/

#define MFST_TRACE6(LOG, c, k) \
    *log.stream << std::setw(4) << std::left << ++FST_TRACE_n << ": " << std::setw(20) << std::left << c << k << std::endl; \
   /* std::cout   << std::setw(4) << std::left << FST_TRACE_n   << ": " << std::setw(20) << std::left << c << k << std::endl;*/

#define MFST_TRACE7(LOG) \
    *log.stream << std::setw(4) << std::left << state.lenta_position << ": " << std::setw(20) << std::left << rule.getCRule(rbuf, state.nrulechain) << std::endl; \
    /*std::cout   << std::setw(4) << std::left << state.lenta_position << ": " << std::setw(20) << std::left << rule.getCRule(rbuf, state.nrulechain) << std::endl;*/

namespace MFST
{
	struct MfstState // Состояние автомата
	{
		short lenta_position;	// позиция на ленте
		short nrule;			// номер текущего правила
		short nrulechain;		// номер текущей цепочки
		MFSTSTSTACK st;			// стек автомата

		MfstState();
		MfstState(short pposition, MFSTSTSTACK pst, short pnrulechain);
		MfstState(short pposition, MFSTSTSTACK pst, short pnrule, short pnrulechain);
	};

	struct Mfst // Магазинный автомат
	{
		enum RC_STEP // Код возврата шага
		{
			NS_OK,				// правило найдено, цепочка записана в стек
			NS_NORULE,			// правило не найдено
			NS_NORULECHAIN,		// цепочка не найдена
			NS_ERROR,			// ошибка
			TS_OK,				// вершина стека совпала с лентой (pop)
			TS_NOK,				// вершина стека не совпала (восстановление)
			LENTA_END,			// лента закончилась
			SURPRISE			// неожиданная ошибка
		};

		struct MfstDiagnosis // Диагностика ошибок
		{
			short lenta_position;
			RC_STEP rc_step;
			short nrule;
			short nrule_chain;
			MfstDiagnosis();
			MfstDiagnosis(short plenta_position, RC_STEP prc_step, short pnrule, short pnrule_chain);
		} diagnosis[MFST_DIAGN_digit];

		class my_stack_MfstState : public std::stack<MfstState> {
		public:
			using std::stack<MfstState>::c;
		};

		GRBALPHABET* lenta;				// лента (TS/NS)
		short lenta_position;			// текущая позиция
		short nrule;					// номер правила
		short nrulechain;				// номер цепочки
		short lenta_size;				// размер ленты
		GRB::Greibach grebach;			// грамматика
		Lexer::LEX lex;					// лекс. анализатор (хранит таблицы)
		MFSTSTSTACK st;					// стек автомата
		my_stack_MfstState storestate;	// стек состояний (для отката)

		Mfst();
		Mfst(Lexer::LEX plex, GRB::Greibach pgrebach);

		char* getCSt(char* buf);									// получить стек строкой
		char* getCLenta(char* buf, short pos, short n = 25);		// получить ленту строкой
		char* getDiagnosis(short n, char* buf);						// получить диагностику

		bool savestate(const Log::LOG& log);						// сохранить состояние
		bool reststate(const Log::LOG& log);						// восстановить состояние
		bool push_chain(GRB::Rule::Chain chain);					// положить цепочку в стек

		RC_STEP step(const Log::LOG& log);							// шаг автомата
		bool start(const Log::LOG& log);							// запуск
		bool savediagnois(RC_STEP pprc_step);						// сохранить ошибку

		void printrules(const Log::LOG& log);						// вывести правила

		struct Deducation // Дерево вывода
		{
			short size;
			short* nrules;
			short* nrulechains;
			Deducation() { size = 0; nrules = 0; nrulechains = 0; };
		} deducation;

		bool savededucation();
	};
};