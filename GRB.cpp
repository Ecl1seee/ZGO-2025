#include "Header.h"

#define GRB_ERROR_SERIES 600

typedef short GRBALPHABET;

namespace GRB
{

	Greibach greibach(NS('S'), TS('$'), 9,

		Rule(NS('S'), GRB_ERROR_SERIES + 0, 2,
			Rule::Chain(8, TS('t'), TS('f'), TS('i'), NS('P'), TS('['), NS('K'), TS(']'), NS('S')),
			Rule::Chain(4, TS('m'), TS('['), NS('K'), TS(']'))
		),

		Rule(NS('P'), GRB_ERROR_SERIES + 1, 2,
			Rule::Chain(3, TS('('), NS('E'), TS(')')),
			Rule::Chain(2, TS('('), TS(')'))
		),

		Rule(NS('E'), GRB_ERROR_SERIES + 4, 2,
			Rule::Chain(4, TS('t'), TS('i'), TS(','), NS('E')),
			Rule::Chain(2, TS('t'), TS('i'))
		),

		Rule(NS('F'), GRB_ERROR_SERIES + 5, 2,
			Rule::Chain(3, TS('('), NS('N'), TS(')')),
			Rule::Chain(2, TS('('), TS(')'))
		),

		Rule(NS('N'), GRB_ERROR_SERIES + 6, 4,
			Rule::Chain(3, TS('i'), TS(','), NS('N')),
			Rule::Chain(3, TS('l'), TS(','), NS('N')),
			Rule::Chain(1, TS('i')),
			Rule::Chain(1, TS('l'))
		),

		Rule(NS('K'), GRB_ERROR_SERIES + 10, 21,
			Rule::Chain(8, TS('s'), TS('('), TS('i'), TS(')'), TS('['), NS('C'), TS(']'), NS('K')),
			Rule::Chain(5, TS('i'), TS('='), NS('W'), TS(';'), NS('K')),
			Rule::Chain(4, TS('i'), NS('F'), TS(';'), NS('K')),
			Rule::Chain(4, TS('i'), TS('>'), TS(';'), NS('K')),
			Rule::Chain(4, TS('i'), TS('<'), TS(';'), NS('K')),
			Rule::Chain(7, TS('v'), TS('t'), TS('i'), TS('='), NS('W'), TS(';'), NS('K')),
			Rule::Chain(5, TS('v'), TS('t'), TS('i'), TS(';'), NS('K')),
			Rule::Chain(4, TS('o'), NS('W'), TS(';'), NS('K')),
			Rule::Chain(3, TS('^'), TS(';'), NS('K')),
			Rule::Chain(4, TS('r'), NS('W'), TS(';'), NS('K')),
			Rule::Chain(8, TS('s'), TS('('), TS('i'), TS(')'), TS('['), NS('C'), TS(']')),
			Rule::Chain(4, TS('i'), TS('='), NS('W'), TS(';')),
			Rule::Chain(3, TS('i'), NS('F'), TS(';')),
			Rule::Chain(3, TS('i'), TS('>'), TS(';')),
			Rule::Chain(3, TS('i'), TS('<'), TS(';')),
			Rule::Chain(6, TS('v'), TS('t'), TS('i'), TS('='), NS('W'), TS(';')),
			Rule::Chain(4, TS('v'), TS('t'), TS('i'), TS(';')),
			Rule::Chain(3, TS('o'), NS('W'), TS(';')),
			Rule::Chain(3, TS('r'), NS('W'), TS(';')),
			Rule::Chain(2, TS('^'), TS(';')),
			Rule::Chain(2, TS('b'), TS(';'))
		),

		Rule(NS('C'), GRB_ERROR_SERIES + 12, 2,
			Rule::Chain(5, TS('k'), TS('l'), TS(':'), NS('K'), NS('C')),
			Rule::Chain(3, TS('d'), TS(':'), NS('K'))
		),

		Rule(NS('A'), GRB_ERROR_SERIES + 7, 4,
			Rule::Chain(1, TS('+')),
			Rule::Chain(1, TS('-')),
			Rule::Chain(1, TS('*')),
			Rule::Chain(1, TS('/'))
		),

		Rule(NS('W'), GRB_ERROR_SERIES + 9, 12,
			Rule::Chain(4, TS('i'), NS('F'), NS('A'), NS('W')),
			Rule::Chain(3, TS('i'), NS('A'), NS('W')),
			Rule::Chain(2, TS('i'), NS('F')),
			Rule::Chain(2, TS('i'), TS('>')),
			Rule::Chain(2, TS('i'), TS('<')),
			Rule::Chain(1, TS('i')),
			Rule::Chain(3, TS('l'), NS('A'), NS('W')),
			Rule::Chain(1, TS('l')),
			Rule::Chain(5, TS('('), NS('W'), TS(')'), NS('A'), NS('W')),
			Rule::Chain(3, TS('('), NS('W'), TS(')')),
			Rule::Chain(4, TS('!'), NS('W'), NS('A'), NS('W')),
			Rule::Chain(2, TS('!'), NS('W'))
		)
	);

	Rule::Chain::Chain(short psize, GRBALPHABET s, ...)
	{
		nt = new GRBALPHABET[size = psize];
		int* p = (int*)&s;
		for (short i = 0; i < psize; ++i)
			nt[i] = (GRBALPHABET)p[i];
	};

	Rule::Rule(GRBALPHABET pnn, int piderror, short psize, Chain c, ...)
	{
		nn = pnn;
		iderror = piderror;
		chains = new Chain[size = psize];
		Chain* p = &c;
		for (int i = 0; i < size; ++i)
			chains[i] = p[i];
	};

	Greibach::Greibach(GRBALPHABET pstartN, GRBALPHABET pstbottom, short psize, Rule r, ...)
	{
		startN = pstartN;
		stbottomT = pstbottom;
		rules = new Rule[size = psize];
		Rule* p = &r;
		for (int i = 0; i < size; ++i)
			rules[i] = p[i];
	}

	Greibach getGreibach()
	{
		return greibach;
	}

	short Greibach::getRule(GRBALPHABET pnn, Rule& prule)
	{
		short rc = -1;
		short k = 0;
		while (k < size && rules[k].nn != pnn)
			++k;
		if (k < size)
			prule = rules[rc = k];
		return rc;
	}

	Rule Greibach::getRule(short n)
	{
		Rule rc;
		if (n < size)
			rc = rules[n];
		return rc;
	};

	char* Rule::getCRule(char* b, short nchain)
	{
		char bchain[200];
		b[0] = Chain::alphabet_to_char(nn);
		b[1] = '-';
		b[2] = '>';
		b[3] = 0x00;
		chains[nchain].getCChain(bchain);
		strcat_s(b, sizeof(bchain) + 5, bchain);
		return b;
	};

	short Rule::getNextChain(GRBALPHABET t, Rule::Chain& pchain, short j)
	{
		short rc = -1;
		while (j < size && chains[j].nt[0] != t)
			++j;
		rc = (j < size ? j : -1);
		if (rc >= 0)
			pchain = chains[rc];
		return rc;
	}

	char* Rule::Chain::getCChain(char* b)
	{
		for (int i = 0; i < size; ++i)
			b[i] = alphabet_to_char(nt[i]);
		b[size] = 0x00;
		return b;
	}
}
