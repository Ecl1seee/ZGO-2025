#include "FST.h"
#include <iostream>
#include <cstring>
#include <cstdarg>

namespace FST
{
	RELATION::RELATION(char c, short ns)
	{
		this->symbol = c;
		this->nnode = ns;
	};

	NODE::NODE()
	{
		this->n_relation = 0;
		this->relations = nullptr;
	};

	NODE::NODE(short n, RELATION rel, ...)
	{
		this->n_relation = n;
		this->relations = new RELATION[n];

		this->relations[0] = rel; // Первый аргумент берем явно

		va_list args;
		va_start(args, rel); // Запускаем перебор остальных аргументов
		for (short i = 1; i < n; i++)
		{
			this->relations[i] = va_arg(args, RELATION);
		}
		va_end(args);
	};

	FST::FST()
	{
		this->string = nullptr;
		this->position = 0;
		this->nstates = 0;
		this->node = nullptr;
		this->rstates = nullptr;
	}

	FST::FST(short ns, NODE n, ...)
	{
		this->nstates = ns;
		this->node = new NODE[ns];
		this->rstates = new short[ns];

		this->node[0] = n; // Первый аргумент берем явно

		va_list args;
		va_start(args, n); // Запускаем перебор остальных аргументов
		for (short i = 1; i < ns; i++)
		{
			this->node[i] = va_arg(args, NODE);
		}
		va_end(args);
	}

	FST::FST(char* s, FST& fst)
	{
		this->node = new NODE[fst.nstates];
		NODE* temp = fst.node;
		this->string = s;
		this->nstates = fst.nstates;
		this->rstates = new short[this->nstates];
		for (short i = 0; i < this->nstates; i++)
			this->node[i] = *(temp + i);
	}

	bool execute(FST& fst)
	{
		if (!fst.string || !fst.node) return false;

		short* rstates = fst.rstates;
		memset(rstates, -1, fst.nstates * sizeof(short));
		short lstring = strlen(fst.string);
		bool rc = true;

		for (short i = 0; i < lstring && rc; i++)
		{
			fst.position = i;
			rc = false;
			if (i == 0) rstates[0] = 0;

			// Создаем временный массив для следующих состояний
			short* next_states = new short[fst.nstates];
			memset(next_states, -1, fst.nstates * sizeof(short));

			for (int j = 0; j < fst.nstates; j++)
			{
				if (rstates[j] == i) // Если мы в состоянии j
				{
					for (int k = 0; k < fst.node[j].n_relation; k++)
					{
						if (fst.node[j].relations[k].symbol == fst.string[i])
						{
							next_states[fst.node[j].relations[k].nnode] = i + 1;
							rc = true;
						}
					}
				}
			}
			// Копируем найденные переходы обратно в rstates
			memcpy(rstates, next_states, fst.nstates * sizeof(short));
			delete[] next_states;
		}
		return (rc ? (rstates[fst.nstates - 1] == lstring) : false);
	};
}