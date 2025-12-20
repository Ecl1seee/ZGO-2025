#pragma once
#include <fstream>

// Основные лексемы
#define LEX_SEPARATORS	 'S'	// разделители
#define	LEX_ID_TYPE    	 't'	// типы данных
#define	LEX_ID			 'i'	// идентификаторы
#define	LEX_LITERAL		 'l'	// литералы
#define	LEX_FUNCTION     'f'	// function
#define	LEX_MAIN		 'm'	// main
#define	LEX_SEMICOLON	 ';'	// ;
#define	LEX_COMMA		 ','	// ,
#define	LEX_COLON        ':'    // :
#define	LEX_LEFTSQUARE	 '['	// [
#define	LEX_RIGHTSQUARE	 ']'	// ]
#define	LEX_LEFTHESIS	 '('	// (
#define	LEX_RIGHTTHESIS	 ')'	// )
#define	LEX_PLUS		 '+'	// +
#define	LEX_MINUS		 '-'	// -
#define	LEX_STAR		 '*'	// *
#define LEX_DIRSLASH	 '/'	// /
#define	LEX_EQUAL		 '='	// =

#define LEX_PLUSPLUS		 '>'	// > инкремент
#define LEX_MINUSMINUS		 '<'	// < декремент

#define LEX_LITERAL_HEX	 'h'	// шестнадцатиричный литерал
#define LEX_OUTPUT		 'o'	// output
#define LEX_NEWLINE		 '^'	// writeline
#define LEX_RETURN		 'r'	// return
#define LEX_VAR			 'v'	// var

#define LEX_EXCLAMATION	 '!'	// !

// Switch-Case
#define LEX_SWITCH       's'    // switch
#define LEX_CASE         'k'    // case
#define LEX_DEFAULT      'd'    // default
#define LEX_BREAK        'b'    // break

#define	LEXEMA_FIXSIZE   1
#define	MAXSIZE_LT		 4096
#define	NULLDX_TI	 0xffffffff
#define SCOPED_ID_MAXSIZE 32
#define STR_MAXSIZE 256
#define INT_DEFAULT 0
#define STR_DEFAULT 0
#define NULLIDX_TI 0xffffffff

namespace LT
{
	struct Entry
	{
		char lexema;
		int sn;
		int idxTI;

		Entry();
		Entry(char lexema, int snn, int idxti = NULLDX_TI);
	};

	struct LexTable
	{
		int maxsize;
		int size;
		Entry* table;
	};

	LexTable Create(int size);
	void Add(LexTable& lextable, Entry entry);
	void writeLexTable(std::ostream* stream, LT::LexTable& lextable);
	void writeLexemsOnLines(std::ostream* stream, LT::LexTable& lextable);

};