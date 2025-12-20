#pragma once
#include "LT.h"
#include "IT.h"
#include "In.h"
#include "Log.h"
#include "FST.h"
#include "Parm.h"


#define TYPE_DIGIT		"number" 
#define TYPE_STRING		"line"
#define TYPE_CHAR		"char"
#define TYPE_BOOL		"boolean"

#define ITOA            "itoa"     
#define LENGHT			"slength"

#define MAIN			"main"

// Макрос проверки, является ли строка типом данных
#define ISTYPE(str) ( !strcmp(str, TYPE_DIGIT) || !strcmp(str, TYPE_STRING)|| !strcmp(str, TYPE_CHAR) )

namespace Lexer
{
	struct LEX
	{
		LT::LexTable lextable;
		IT::IdTable	idtable;
		LEX() {}
	};

	struct Graph
	{
		char lexema;
		FST::FST graph;
	};

	IT::Entry* getEntry		// формирует и возвращает строку ТИ
	(
		Lexer::LEX& tables,						// ТЛ + ТИ
		char lex,								// лексема
		char* id,								// идентификатор
		char* idtype,							// предыдущая (тип)
		bool isParam,							// признак параметра функции
		bool isFunc,							// признак функции
		Log::LOG log,							// протокол
		int line,								// строка в исходном тексте
		bool& rc_err							// флаг ошибки(по ссылке)
	);

	bool analyze(LEX& tables, In::IN& in, Log::LOG& log, Parm::PARM& parm);

	int	getIndexInLT(LT::LexTable& lextable, int itTableIndex); // индекс первой встречи в таблице лексем
};