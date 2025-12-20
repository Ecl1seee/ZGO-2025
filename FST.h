#pragma once
#include <iostream>

namespace FST
{
	struct RELATION		
	{
		char  symbol;	// символ перехода
		short nnode;	// номер смежной вершины


		RELATION(char c = 0, short ns = 0);
	};

	struct NODE					// вершина графа переходов
	{
		short n_relation;		
		RELATION* relations;	
		NODE();					
		NODE(short n, RELATION rel, ...);  
	};

	struct FST   // недетерминированный конечный автомат
	{
		char* string;				// цепочка(строка, завершается 0х00)
		short position;				
		short nstates;				
		NODE* node;					
		short* rstates;			


		FST();

		FST(short ns, NODE n, ...); 
		FST(char* s, FST& fst);		
	};

	bool execute(FST& fst); 
};