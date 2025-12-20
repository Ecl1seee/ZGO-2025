#include "LexAnalysis.h"
#include "Graph.h"
#include "Error.h"
#include "Log.h"
#include <stack>
#include <string>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>

#pragma warning(disable : 4996)

#define MAX_FUNC_PARAMS 3

int DecimicalNotation(std::string input, int scaleofnot) {
	try {
		bool isNegative = !input.empty() && input[0] == '-';
		size_t startIdx = input.size() > 2 && (input[1] == 'x' || input[1] == 'X') ? 2 : 0;
		int value = std::stoi(input.substr(isNegative ? 1 + startIdx : startIdx), nullptr, scaleofnot);
		return isNegative ? -value : value;
	}
	catch (const std::out_of_range&) {
		return input[0] == '-' ? INT_MINSIZE : INT_MAXSIZE;
	}
}

namespace Lexer
{
	Graph graphs[N_GRAPHS] =
	{
		{ LEX_SEPARATORS,   FST::FST(GRAPH_SEPARATORS) },
		{ LEX_LITERAL,      FST::FST(GRAPH_TRUE) },
		{ LEX_LITERAL,      FST::FST(GRAPH_FALSE) },

		{ LEX_LITERAL,      FST::FST(GRAPH_INT_LITERAL) },
		{ LEX_LITERAL,      FST::FST(GRAPH_STRING_LITERAL) },
		{ LEX_LITERAL,      FST::FST(GRAPH_CHAR_LITERAL) },
		{ LEX_LITERAL_HEX,  FST::FST(GRAPH_HEX_LITERAL) },

		{ LEX_VAR,          FST::FST(GRAPH_VAR) },
		{ LEX_MAIN,         FST::FST(GRAPH_MAIN) },

		{ LEX_ID_TYPE,      FST::FST(GRAPH_NUMBER) },
		{ LEX_ID_TYPE,      FST::FST(GRAPH_STRING) },
		{ LEX_ID_TYPE,      FST::FST(GRAPH_CHAR) },
		{ LEX_ID_TYPE,      FST::FST(GRAPH_BOOL) },

		{ LEX_FUNCTION,     FST::FST(GRAPH_FUNCTION) },
		{ LEX_RETURN,       FST::FST(GRAPH_RETURN) },
		{ LEX_OUTPUT,       FST::FST(GRAPH_OUTPUT) },
		{ LEX_NEWLINE,      FST::FST(GRAPH_NEWLINE) },

		{ LEX_SWITCH,       FST::FST(GRAPH_SWITCH) },
		{ LEX_CASE,         FST::FST(GRAPH_CASE) },
		{ LEX_DEFAULT,      FST::FST(GRAPH_DEFAULT) },
		{ LEX_BREAK,        FST::FST(GRAPH_BREAK) },

		{ LEX_ID,           FST::FST(GRAPH_ID) }
	};

	int getLiteralIndex(IT::IdTable ittable, char* value, IT::IDDATATYPE type, Log::LOG log, int line)
	{
		for (int i = 0; i < ittable.size; i++)
		{
			if (ittable.table[i].idtype == IT::IDTYPE::L && ittable.table[i].iddatatype == type)
			{
				switch (type)
				{
				case IT::IDDATATYPE::INT: {
					int valToCheck = 0;
					if (strlen(value) > 2 && (value[1] == 'x' || value[1] == 'X'))
						valToCheck = DecimicalNotation(value, 16);
					else
						valToCheck = atoi(value);
					if (ittable.table[i].value.vint == valToCheck) return i;
					break;
				}
				case IT::IDDATATYPE::STR: {
					char buf[STR_MAXSIZE];
					size_t len = strlen(value);
					if (len > 2) {
						for (unsigned j = 1; j < len - 1; j++) buf[j - 1] = value[j];
						buf[len - 2] = '\0';
					}
					else buf[0] = '\0';
					if (strcmp(ittable.table[i].value.vstr.str, buf) == 0) return i;
					break;
				}
				case IT::IDDATATYPE::CHAR: {
					char buk[STR_MAXSIZE];
					size_t len = strlen(value);
					if (len > 2) {
						for (unsigned j = 1; j < len - 1; j++) buk[j - 1] = value[j];
						buk[len - 2] = '\0';
					}
					else buk[0] = '\0';
					if (strcmp(ittable.table[i].value.vstr.str, buk) == 0) return i;
					break;
				}
				case IT::IDDATATYPE::BOOL: {
					int valToCheck = (strcmp(value, "true") == 0) ? 1 : 0;
					if (ittable.table[i].value.vint == valToCheck) return i;
					break;
				}
				}
			}
		}
		return NULLIDX_TI;
	}

	IT::IDDATATYPE getType(char* curword, char* idtype)
	{
		if (idtype) {
			if (!strcmp("line", idtype)) return IT::IDDATATYPE::STR;
			if (!strcmp("char", idtype)) return IT::IDDATATYPE::CHAR;
			if (!strcmp("number", idtype)) return IT::IDDATATYPE::INT;
			if (!strcmp("boolean", idtype)) return IT::IDDATATYPE::BOOL;
		}

		// проверка булевых литералов 
		if (strcmp(curword, "true") == 0 || strcmp(curword, "false") == 0)
			return IT::IDDATATYPE::BOOL;

		if (isdigit(*curword) || *curword == LEX_MINUS) return IT::IDDATATYPE::INT;
		else if (*curword == IN_CODE_QUOTE) return IT::IDDATATYPE::STR;
		else if (*curword == IN_CODE_QUOTE2) return IT::IDDATATYPE::CHAR;

		return IT::IDDATATYPE::UNDEF;
	}

	int getIndexInLT(LT::LexTable& lextable, int itTableIndex)
	{
		if (itTableIndex == NULLIDX_TI) return lextable.size;
		for (int i = 0; i < lextable.size; i++)
			if (itTableIndex == lextable.table[i].idxTI) return i;
		return NULLIDX_TI;
	}

	IT::STDFNC getStandFunction(char* id)
	{
		if (!strcmp("slength", id)) return IT::STDFNC::F_LENGHT;
		if (!strcmp("itoa", id)) return IT::STDFNC::F_ITOA;
		return IT::STDFNC::F_NOT_STD;
	}

	char* getNextLiteralName()
	{
		static int literaldigit = 1;
		char* buf = new char[SCOPED_ID_MAXSIZE];
		char lich[10];
		strcpy_s(buf, MAXSIZE_ID, "LTRL");
		_itoa_s(literaldigit++, lich, 10);
		strcat_s(buf, SCOPED_ID_MAXSIZE, lich);
		return buf;
	}

	IT::Entry* getEntry(LEX& tables, char lex, char* id, char* idtype, bool isParam, bool isFunc, Log::LOG log, int line, int col, bool& lex_ok) {
		IT::IDDATATYPE type = getType(id, idtype);
		int index = IT::isId(tables.idtable, id);

		if (lex == LEX_LITERAL)
			index = getLiteralIndex(tables.idtable, id, type, log, line);

		if (index != NULLIDX_TI) 
			return nullptr;

		IT::Entry* itentry = new IT::Entry;
		itentry->iddatatype = type;
		itentry->idxfirstLE = getIndexInLT(tables.lextable, index);
		itentry->parmQuantity = 0;

		if (lex == LEX_LITERAL) {
			if (type == IT::IDDATATYPE::BOOL) {
				itentry->value.vint = (strcmp(id, "true") == 0) ? 1 : 0;
			}
			else if (type == IT::IDDATATYPE::INT) {
				unsigned long long temp_value = 0;
				bool is_hex = (strlen(id) > 2 && (id[1] == 'x' || id[1] == 'X'));

				try {
					if (is_hex) {
						temp_value = std::stoull(id, nullptr, 16);
					}
					else {
						temp_value = std::stoull(id, nullptr, 10);
					}

					if (temp_value > INT_MAXSIZE) {
						Log::writeError(log.stream, Error::GetError(313, line, col));
						lex_ok = false;
						delete itentry;
						return nullptr;
					}

					itentry->value.vint = static_cast<int>(temp_value);
				}
				catch (const std::exception&) {
					Log::writeError(log.stream, Error::GetError(313, line, col));
					lex_ok = false;
					delete itentry;
					return nullptr;
				}
			}
			else if (type == IT::IDDATATYPE::STR || type == IT::IDDATATYPE::CHAR) {
				bool str_ok = IT::SetValue(itentry, id);
				if (!str_ok) {
					Log::writeError(log.stream, Error::GetError(312, line, col));
					lex_ok = false;
					delete itentry;
					return nullptr;
				}
			}

			strcpy_s(itentry->id, getNextLiteralName());
			itentry->idtype = IT::IDTYPE::L;
		}
		else
		{
			if (type == IT::IDDATATYPE::INT || type == IT::IDDATATYPE::BOOL) itentry->value.vint = INT_DEFAULT;
			else
			{
				strcpy_s(itentry->value.vstr.str, "");
				itentry->value.vstr.len = STR_DEFAULT;
			}

			if (isFunc)
			{
				IT::STDFNC stdf = getStandFunction(id);
				if (stdf != IT::STDFNC::F_NOT_STD) {
					itentry->idtype = IT::IDTYPE::S;
					itentry->value.params.count = 0;

					if (stdf == IT::STDFNC::F_LENGHT) {
						itentry->iddatatype = IT::IDDATATYPE::INT;
						itentry->parmQuantity = 1;
					}
					else if (stdf == IT::STDFNC::F_ITOA) {
						itentry->iddatatype = IT::IDDATATYPE::STR;
						itentry->parmQuantity = 1;
					}
					else itentry->iddatatype = IT::IDDATATYPE::UNDEF;
				}
				else itentry->idtype = IT::IDTYPE::F;
			}
			else if (isParam) itentry->idtype = IT::IDTYPE::P;
			else itentry->idtype = IT::IDTYPE::V;

			strncpy_s(itentry->id, id, SCOPED_ID_MAXSIZE);
		}

		if (lex != LEX_LITERAL && !isFunc && !isParam && idtype != nullptr)
		{
			if (tables.lextable.size >= 2)
			{
				if (tables.lextable.table[tables.lextable.size - 2].lexema != LEX_VAR)
				{
					Log::writeError(log.stream, Error::GetError(304, line, col));
					lex_ok = false;
				}
			}
		}
		else if (lex != LEX_LITERAL && !isFunc && !isParam && idtype == nullptr)
		{
			Log::writeError(log.stream, Error::GetError(303, line, col));
			lex_ok = false;
		}

		return itentry;
	}

	bool analyze(LEX& tables, In::IN& in, Log::LOG& log, Parm::PARM& parm)
	{
		static bool lex_ok = true;
		tables.lextable = LT::Create(MAXSIZE_LT);
		tables.idtable = IT::Create(MAXSIZE_TI);

		int enterPoint = 0;
		char curword[STR_MAXSIZE], nextword[STR_MAXSIZE];
		int curline, curcol;

		std::stack<std::string> scopes;
		int braceLevel = 0;

		bool state_VarDecl = false;
		bool state_FuncDecl = false;
		bool state_FuncParam = false;
		char* state_LastType = nullptr;
		std::string currentFunctionName = "";

		for (int i = 0; i < In::InWord::size; i++)
		{
			strcpy_s(curword, in.words[i].word);
			if (i < In::InWord::size - 1) strcpy_s(nextword, in.words[i + 1].word);
			else nextword[0] = '\0';
			curline = in.words[i].line;
			curcol = in.words[i].col;

			if (curword[0] == IN_CODE_QUOTE || curword[0] == IN_CODE_QUOTE2) {
				size_t len = strlen(curword);
				if (len == 2) {
					Log::writeError(log.stream, Error::GetError(310, curline, curcol));
					lex_ok = false;
					continue;
				}
			}

			int idxTI = NULLIDX_TI;
			bool matchFound = false;

			for (int j = 0; j < N_GRAPHS; j++)
			{
				FST::FST fst(curword, graphs[j].graph);
				if (FST::execute(fst))
				{
					matchFound = true;
					char lexema = graphs[j].lexema;

					if (lexema == LEX_SEPARATORS)
					{
						lexema = curword[0];
					}

					switch (lexema)
					{
					case LEX_VAR:
						state_VarDecl = true;
						state_FuncDecl = false;
						break;
					case LEX_FUNCTION:
						state_FuncDecl = true;
						break;
					case LEX_ID_TYPE:
						state_LastType = in.words[i].word;
						break;
					case LEX_SEMICOLON:
						state_VarDecl = false;
						state_LastType = nullptr;
						break;
					case LEX_LEFTHESIS:
						state_FuncParam = true;
						break;
					case LEX_RIGHTTHESIS:
						if (state_FuncParam)
						{
							state_FuncParam = false;
							state_FuncDecl = false;

							if (!currentFunctionName.empty())
							{
								int funcIdx = IT::isId(tables.idtable, (char*)currentFunctionName.c_str());

								if (funcIdx != NULLIDX_TI)
								{
									int actualParamCount = 0;
									for (int k = 0; k < tables.idtable.size; k++)
									{
										if (tables.idtable.table[k].idtype == IT::IDTYPE::P)
										{
											std::string paramId = tables.idtable.table[k].id;
											std::string expectedPrefix = currentFunctionName + ".";

											if (paramId.length() > expectedPrefix.length() &&
												paramId.substr(0, expectedPrefix.length()) == expectedPrefix)
											{
												actualParamCount++;
											}
										}
									}
									tables.idtable.table[funcIdx].parmQuantity = actualParamCount;
								}
							}
						}
						break;
					}

					switch (lexema)
					{
					case LEX_MAIN:
						enterPoint++;
						scopes.push("main");
						currentFunctionName = "main";
						{
							LT::Entry ltEntry(LEX_MAIN, curline, NULLIDX_TI);
							LT::Add(tables.lextable, ltEntry);
						}
						break;

					case LEX_LEFTSQUARE:
						braceLevel++;
						{
							LT::Entry ltEntry(lexema, curline, NULLIDX_TI);
							LT::Add(tables.lextable, ltEntry);
						}
						break;

					case LEX_RIGHTSQUARE:
						braceLevel--;
						if (braceLevel == 0 && !scopes.empty())
						{
							scopes.pop();
							currentFunctionName = scopes.empty() ? "" : scopes.top();
						}
						{
							LT::Entry ltEntry(lexema, curline, NULLIDX_TI);
							LT::Add(tables.lextable, ltEntry);
						}
						break;

					case LEX_LITERAL_HEX:
					{
						int val = DecimicalNotation(curword, 16);
						_itoa_s(val, curword, 10);
						lexema = LEX_LITERAL;
						IT::Entry* entry = getEntry(tables, lexema, curword, nullptr, false, false, log, curline, curcol, lex_ok);
						if (entry)
						{
							IT::Add(tables.idtable, *entry);
							idxTI = tables.idtable.size - 1;
						}
						else
						{
							IT::IDDATATYPE type = getType(curword, nullptr);
							idxTI = getLiteralIndex(tables.idtable, curword, type, log, curline);
						}
						LT::Entry ltEntry(lexema, curline, idxTI);
						LT::Add(tables.lextable, ltEntry);
						break;
					}

					case LEX_ID:
					{
						if (strlen(curword) > MAXSIZE_ID) {
							Log::writeError(log.stream, Error::GetError(204, curline, curcol));
							lex_ok = false;
						}

						char fullId[SCOPED_ID_MAXSIZE] = "";
						char cleanId[MAXSIZE_ID] = "";
						strcpy_s(cleanId, curword);

						bool isDecl = false, isFunc = false, isParam = false;
						char* currentType = nullptr;

						if (state_LastType != nullptr && !state_VarDecl && !state_FuncDecl && !state_FuncParam)
						{
							Log::writeError(log.stream, Error::GetError(300, curline, curcol));
							lex_ok = false;
							state_LastType = nullptr;
							break;
						}

						if (state_FuncDecl && !state_FuncParam && state_LastType != nullptr) {
							isDecl = true;
							isFunc = true;
							currentType = state_LastType;
							state_LastType = nullptr;
						}
						else if (state_FuncParam && state_LastType != nullptr) {
							isDecl = true;
							isParam = true;
							currentType = state_LastType;
							state_LastType = nullptr;
						}
						else if (state_VarDecl && state_LastType != nullptr) {
							isDecl = true;
							currentType = state_LastType;
							state_LastType = nullptr;
						}

						char localName[SCOPED_ID_MAXSIZE] = "";
						if (!scopes.empty()) {
							strcpy_s(localName, scopes.top().c_str());
							strcat_s(localName, ".");
							strcat_s(localName, cleanId);
						}
						else strcpy_s(localName, cleanId);

						if (isDecl) {
							if (isFunc) strcpy_s(fullId, cleanId);
							else strcpy_s(fullId, localName);
						}
						else {
							if (IT::isId(tables.idtable, localName) != NULLIDX_TI) strcpy_s(fullId, localName);
							else if (IT::isId(tables.idtable, cleanId) != NULLIDX_TI) strcpy_s(fullId, cleanId);
							else if (getStandFunction(cleanId) != IT::STDFNC::F_NOT_STD) {
								strcpy_s(fullId, cleanId);
								isFunc = true;
							}
							else strcpy_s(fullId, localName);
						}

						idxTI = IT::isId(tables.idtable, fullId);

						if (isDecl && idxTI != NULLIDX_TI) {
							Log::writeError(log.stream, Error::GetError(305, curline, curcol));
							lex_ok = false;
						}

						if (isParam)
						{
							if (idxTI == NULLIDX_TI) {
								IT::Entry* entry = getEntry(tables, lexema, fullId, currentType, isParam, isFunc, log, curline, curcol, lex_ok);
								if (entry)
								{
									if (!currentFunctionName.empty())
									{
										int parentIdx = IT::isId(tables.idtable, (char*)currentFunctionName.c_str());

										if (parentIdx != NULLIDX_TI &&
											(tables.idtable.table[parentIdx].idtype == IT::IDTYPE::F ||
												tables.idtable.table[parentIdx].idtype == IT::IDTYPE::S))
										{
											if (tables.idtable.table[parentIdx].parmQuantity >= MAX_FUNC_PARAMS)
											{
												Log::writeError(log.stream, Error::GetError(306, curline, curcol));
												lex_ok = false;
												delete entry;
												break;
											}
											else
											{
												tables.idtable.table[parentIdx].parmQuantity++;
											}
										}
									}

									IT::Add(tables.idtable, *entry);
									idxTI = tables.idtable.size - 1;
								}
							}
						}
						else if (idxTI == NULLIDX_TI)
						{
							IT::Entry* entry = getEntry(tables, lexema, fullId, currentType, isParam, isFunc, log, curline, curcol, lex_ok);
							if (entry)
							{
								IT::Add(tables.idtable, *entry);
								idxTI = tables.idtable.size - 1;

								if (isFunc && isDecl)
								{
									scopes.push(cleanId);
									currentFunctionName = cleanId;
								}
							}
						}

						LT::Entry ltEntry(lexema, curline, idxTI);
						LT::Add(tables.lextable, ltEntry);
						break;
					}

					case LEX_LITERAL:
					{
						IT::Entry* entry = getEntry(tables, lexema, curword, nullptr, false, false, log, curline, curcol, lex_ok);
						if (entry)
						{
							IT::Add(tables.idtable, *entry);
							idxTI = tables.idtable.size - 1;
						}
						else
						{
							IT::IDDATATYPE type = getType(curword, nullptr);
							idxTI = getLiteralIndex(tables.idtable, curword, type, log, curline);
						}
						LT::Entry ltEntry(lexema, curline, idxTI);
						LT::Add(tables.lextable, ltEntry);
						break;
					}

					default:
						LT::Entry ltEntry(lexema, curline, NULLIDX_TI);
						LT::Add(tables.lextable, ltEntry);
						break;
					}
					break;
				}
			}
			if (!matchFound)
			{
				Log::writeError(log.stream, Error::GetError(201, curline, curcol));
				lex_ok = false;
			}
		}

		if (enterPoint != 1)
		{
			Log::writeError(log.stream, Error::GetError(enterPoint == 0 ? 301 : 302, curline, curcol));
			lex_ok = false;
		}

		return lex_ok;
	}
}
