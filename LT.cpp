#include <iostream>
#include <iomanip>
#include "Header.h"
#include "Error.h"


namespace LT
{
	Entry::Entry()
	{
		lexema = 0;
		sn = 0;
		idxTI = NULLDX_TI;
	}

	Entry::Entry(char lexema, int snn, int idxti)
	{
		this->lexema = lexema;
		this->sn = snn;
		this->idxTI = idxti;
	}

	LexTable Create(int size)
	{
		if (size > MAXSIZE_LT)
			throw ERROR_THROW(202);
		LexTable lextable;
		lextable.table = new Entry[size]; // size присваиваем сразу
		lextable.maxsize = size;
		lextable.size = 0;
		return  lextable;
	}

	void Add(LexTable& lextable, Entry entry)
	{
		if (lextable.size >= lextable.maxsize)
			throw ERROR_THROW(202);
		lextable.table[lextable.size++] = entry;
	}

	void writeLexTable(std::ostream* stream, LT::LexTable& lextable)
	{
		*stream << "------------------------------ LEXEME TABLE ------------------------\n" << std::endl;
		*stream << "|  N | LEX | LINE | ID INDEX |" << std::endl;
		for (int i = 0; i < lextable.size; i++)
		{
			*stream << "|" << std::setw(3) << i << " | " << std::setw(3) << lextable.table[i].lexema << " | " << std::setw(4)
				<< lextable.table[i].sn << " |";
			if (lextable.table[i].idxTI == NULLDX_TI)
				*stream << "          |" << std::endl;
			else
				*stream << std::setw(8) << lextable.table[i].idxTI << "  |" << std::endl;
		}
	}

	void writeLexemsOnLines(std::ostream* stream, LexTable& lextable)
	{
		*stream << "\n-------------------- LEXEMES --------------------" << std::endl;

		int currentSourceLine = -1;
		int sequentialCounter = 0;

		for (int i = 0; i < lextable.size; i++)
		{
			if (lextable.table[i].sn != currentSourceLine)
			{
				if (currentSourceLine != -1) *stream << std::endl;

				currentSourceLine = lextable.table[i].sn;
				sequentialCounter++;

				*stream << std::setw(3) << sequentialCounter << " | ";
			}
			*stream << lextable.table[i].lexema;
		}

		*stream << std::endl;
		*stream << "------------------------------------------------------------" << std::endl;
	}
}