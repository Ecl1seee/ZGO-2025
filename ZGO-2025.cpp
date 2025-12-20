#include <iostream>
#include "Header.h"

int wmain(int argc, wchar_t* argv[])
{
	
	char LEXERROR[] = "Лексический анализ завершен с ошибками";
	char SYNTERROR[] = "Синтаксический анализ завершен с ошибками";
	char SEMERROR[] = "Обнаружены семантические ошибки";
	char POLISHERROR[] = "Ошибка при попытке преобразования выражения";
	char LEXGOOD[] = "Лексический анализ завершен без ошибок";
	char SYNTGOOD[] = "Синтаксический анализ завершен без ошибок";
	char SEMGOOD[] = "Семантический анализ завершен без ошибок";
	char POLISHGOOD[] = "Преобразование выражений завершено без ошибок";
	char MESSAGE[] = "--------------------RESULTING LEXEMA AND ID TABLES-------------------";
	char STOP[] = "\nThe program execution has stopped";
	char ALLGOOD[] = "Программа завершена успешно!";

	setlocale(LC_ALL, "Russian");

	Log::LOG log;
	log.stream = nullptr;

	try
	{
		Parm::PARM parm = Parm::getparm(argc, argv);
		log = Log::getstream(parm.log);
		Log::writeLog(log);
		Log::writeParm(log, parm);
		In::IN in = In::getin(parm.in, log.stream);
		Log::writeIn(log.stream, in);

		in.words = In::getWordsTable(log.stream, in.text, in.code, in.size);
		Lexer::LEX tables;

		// --- ЛЕКСИЧЕСКИЙ АНАЛИЗ ---
		bool lex_ok = Lexer::analyze(tables, in, log, parm);

		// 1. Вывод в LOG
		LT::writeLexTable(log.stream, tables.lextable);
		IT::PrintIdTable(tables.idtable, log.stream);
		IT::PrintLiteralTable(tables.idtable, log.stream);
		LT::writeLexemsOnLines(log.stream, tables.lextable);

		// 2. Вывод в КОНСОЛЬ
		std::cout << "\n-------------------- Results of lexema analysis --------------------" << std::endl;
		IT::PrintIdTable(tables.idtable, &std::cout);
		IT::PrintLiteralTable(tables.idtable, &std::cout);
		LT::writeLexTable(&std::cout, tables.lextable);
		LT::writeLexemsOnLines(&std::cout, tables.lextable);
		std::cout << "------------------------------------------------------------------------\n" << std::endl;

		// 3. Вывод в ФАЙЛЫ
		IT::WriteIdTableToFile(tables.idtable, L"ID_Table.txt");
		IT::WriteLiteralTableToFile(tables.idtable, L"LIT_Table.txt");

		if (!lex_ok)
		{
			Log::writeLine(log.stream, LEXERROR, "");
			Log::writeLine(&std::cout, LEXERROR, STOP, "");
			return 0;
		}
		else
		{
			Log::writeLine(&std::cout, LEXGOOD, "");
		}

		// --- СИНТАКСИЧЕСКИЙ АНАЛИЗ ---
		MFST_TRACE_START(log);

		MFST::Mfst mfst(tables, GRB::getGreibach());

		bool synt_ok = mfst.start(log);

		mfst.savededucation();
		mfst.printrules(log);

		if (!synt_ok)
		{
			Log::writeLine(log.stream, SYNTERROR, "");
			Log::writeLine(&std::cout, SYNTERROR, STOP, "");
			return 0;
		}
		else Log::writeLine(&std::cout, SYNTGOOD, "");

		// --- СЕМАНТИЧЕСКИЙ АНАЛИЗ ---
		bool sem_ok = Semantic::semanticsCheck(tables, log);
		if (!sem_ok)
		{
			Log::writeLine(log.stream, SEMERROR, "");
			Log::writeLine(&std::cout, SEMERROR, STOP, "");
			return 0;
		}
		else Log::writeLine(&std::cout, SEMGOOD, "");

		//// --- ПОЛЬСКАЯ НОТАЦИЯ ---
		bool polish_ok = Polish::PolishNotation(tables, log);
		if (!polish_ok)
		{
			Log::writeLine(log.stream, POLISHERROR, "");
			Log::writeLine(&std::cout, POLISHERROR, STOP, "");
			return 0;
		}
		else Log::writeLine(&std::cout, POLISHGOOD, "");


		Log::writeLine(log.stream, MESSAGE, "");
		LT::writeLexTable(log.stream, tables.lextable);


		IT::PrintIdTable(tables.idtable, log.stream);
		IT::PrintLiteralTable(tables.idtable, log.stream);

		LT::writeLexemsOnLines(log.stream, tables.lextable);

		// --- ГЕНЕРАЦИЯ КОДА ---
		Gener::CodeGeneration(tables, parm, log);

		Log::writeLine(&std::cout, ALLGOOD, "");
		Log::Close(log);
	}
	catch (Error::ERROR e)
	{
		// Вывод ошибки в консоль
		std::cout << "\nОШИБКА " << e.id << ": " << e.message
			<< ", строка " << e.position.line << std::endl;

		system("pause");	
	}
	return 0;
}