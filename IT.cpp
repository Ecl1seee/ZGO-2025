#include "Header.h"
#include <iomanip>

#pragma warning(disable : 4996)
#define W(x, y)\
		<< std::setw(x) << (y) <<
#define STR(n, line, type, id)\
		"|" W(4,n) " |  " W(5,line) "    |" W(17,type) " |  " W(SCOPED_ID_MAXSIZE, id) " |"


namespace IT
{
	IdTable Create(int size)
	{
		if (size > MAXSIZE_TI)
			throw ERROR_THROW(203);
		IdTable idtable;
		idtable.table = new Entry[idtable.maxsize = size];
		idtable.size = NULL;
		return idtable;
	}

	void Add(IdTable& idtable, Entry entry)
	{
		if (idtable.size >= idtable.maxsize)
			throw ERROR_THROW(203);
		idtable.table[idtable.size++] = entry;
	}

	// возврат: номер строки(если есть), TI_NULLIDX(если нет)
	int isId(IdTable& idtable, char id[SCOPED_ID_MAXSIZE])
	{
		for (int i = 0; i < idtable.size; i++)
		{
			if (strcmp(idtable.table[i].id, id) == 0)
				return i;
		}
		return NULLIDX_TI;
	}

	bool SetValue(IT::IdTable& idtable, int index, char* value)
	{
		return SetValue(&(idtable.table[index]), value);
	}

	bool SetValue(IT::Entry* entry, char* value) // установка значения переменной
	{
		bool rc = true;
		if (entry->iddatatype == INT)
		{
			int temp = atoi(value);
			if (temp > INT_MAXSIZE || temp < INT_MINSIZE)
			{
				if (temp > INT_MAXSIZE)
					temp = INT_MAXSIZE;
				if (temp < INT_MINSIZE)
					temp = INT_MINSIZE;
				rc = false;
			}
			entry->value.vint = temp;
		}
		else
		{
			for (unsigned i = 1; i < strlen(value) - 1; i++)	// без кавычек
				entry->value.vstr.str[i - 1] = value[i];
			entry->value.vstr.str[strlen(value) - 2] = '\0';
			entry->value.vstr.len = strlen(value) - 2;
		}
		return rc;
	}
	void PrintIdTable(IT::IdTable& idtable, std::ostream* stream)
	{
		*stream << "\n=========================================== Id table ============================================\n" << std::endl;

		*stream << std::left
			<< "| " << std::setw(4) << "N"
			<< "| " << std::setw(6) << "LINE"
			<< "| " << std::setw(20) << "SCOPE"
			<< "| " << std::setw(20) << "NAME"
			<< "| " << std::setw(35) << "ID TYPE"
			<< "|" << std::endl;

		*stream << "|-----|-------|---------------------|---------------------|------------------------------------|" << std::endl;

		for (int i = 0; i < idtable.size; i++)
		{
			Entry* e = &idtable.table[i];
			if (e->idtype == IT::IDTYPE::L) continue;

			char scope[SCOPED_ID_MAXSIZE] = "";
			char cleanName[SCOPED_ID_MAXSIZE] = "";
			const char* dotPos = strchr(e->id, '.');

			if (dotPos != nullptr) {
				size_t len = dotPos - e->id;
				strncpy_s(scope, SCOPED_ID_MAXSIZE, e->id, len);
				strcpy_s(cleanName, SCOPED_ID_MAXSIZE, dotPos + 1);
			}
			else {
				strcpy_s(scope, SCOPED_ID_MAXSIZE, "GLOBAL");
				strcpy_s(cleanName, SCOPED_ID_MAXSIZE, e->id);
			}

			char typeStr[100] = "";
			switch (e->iddatatype)
			{
			case IT::IDDATATYPE::INT:  strcat_s(typeStr, "number "); break;
			case IT::IDDATATYPE::STR:  strcat_s(typeStr, "line   "); break;
			case IT::IDDATATYPE::CHAR: strcat_s(typeStr, "char   "); break;
			case IT::IDDATATYPE::BOOL: strcat_s(typeStr, "boolean   "); break;
			case IT::IDDATATYPE::PROC: strcat_s(typeStr, "void   "); break;
			default:                   strcat_s(typeStr, "-      "); break;
			}

			switch (e->idtype)
			{
			case IT::IDTYPE::V: strcat_s(typeStr, "variable"); break;
			case IT::IDTYPE::F: strcat_s(typeStr, "function"); break;
			case IT::IDTYPE::P: strcat_s(typeStr, "parameter"); break;
			case IT::IDTYPE::S: strcat_s(typeStr, "std func"); break;
			default:            strcat_s(typeStr, "unknown"); break;
			}

			*stream << "| " << std::setw(4) << i
				<< "| " << std::setw(6) << e->idxfirstLE
				<< "| " << std::setw(20) << scope
				<< "| " << std::setw(20) << cleanName
				<< "| " << std::setw(35) << typeStr
				<< "|" << std::endl;
		}
		*stream << "================================================================================================================\n\n";
	}

	void PrintLiteralTable(IT::IdTable& idtable, std::ostream* stream)
	{
		*stream << "\n============================================ Lteral Table =============================================\n" << std::endl;

		*stream << std::left
			<< "| " << std::setw(4) << "N"
			<< "| " << std::setw(6) << "LINE"
			<< "| " << std::setw(15) << "LOCAL NAME"
			<< "| " << std::setw(15) << "TYPE"
			<< "| " << std::setw(45) << "VALUE"
			<< "|" << std::endl;

		*stream << "|-----|-------|----------------|----------------|----------------------------------------------|" << std::endl;

		for (int i = 0; i < idtable.size; i++)
		{
			Entry* e = &idtable.table[i];
			if (e->idtype != IT::IDTYPE::L) continue;

			const char* typeStr = "unknown";
			switch (e->iddatatype)
			{
			case IT::IDDATATYPE::INT:  typeStr = "number"; break;
			case IT::IDDATATYPE::STR:  typeStr = "line"; break;
			case IT::IDDATATYPE::CHAR: typeStr = "char"; break;
			case IT::IDDATATYPE::BOOL: typeStr = "boolean"; break;
			}

			*stream << "| " << std::setw(4) << i
				<< "| " << std::setw(6) << e->idxfirstLE
				<< "| " << std::setw(15) << e->id
				<< "| " << std::setw(15) << typeStr;

			*stream << "| ";
			if (e->iddatatype == IT::IDDATATYPE::INT)
			{
				*stream << std::setw(45) << e->value.vint;
			}
			else if (e->iddatatype == IT::IDDATATYPE::BOOL) 
			{
				*stream << std::setw(45) << (e->value.vint ? "true" : "false");
			}
			else
			{
				char valBuf[60];
				if (e->iddatatype == IT::IDDATATYPE::STR)
					sprintf_s(valBuf, 60, "\"%s\"", e->value.vstr.str);
				else
					sprintf_s(valBuf, 60, "'%s'", e->value.vstr.str);

				if (strlen(valBuf) > 44) {
					valBuf[41] = '.'; valBuf[42] = '.'; valBuf[43] = '.'; valBuf[44] = '\0';
				}
				*stream << std::setw(45) << valBuf;
			}
			*stream << "|" << std::endl;
		}
		*stream << "============================================================================================================\n";
	}

	// Обертки для записи в файл
	void WriteIdTableToFile(IT::IdTable& idtable, const wchar_t* filename)
	{
		std::ofstream fout(filename);
		if (fout.is_open()) {
			PrintIdTable(idtable, &fout);
			fout.close();
		}
	}

	void WriteLiteralTableToFile(IT::IdTable& idtable, const wchar_t* filename)
	{
		std::ofstream fout(filename);
		if (fout.is_open()) {
			PrintLiteralTable(idtable, &fout);
			fout.close();
		}
	}

	
	void writeScopeTable(std::ostream* stream, IT::IdTable& idtable)
	{
		*stream << "\n\n================================ SCOPE TABLE ================================\n" << std::endl;

		*stream << std::left
			<< "| " << std::setw(3) << "N"
			<< " | " << std::setw(6) << "LINE"
			<< " | " << std::setw(25) << "SCOPE"
			<< " | " << std::setw(25) << "NAME"
			<< " | " << std::setw(15) << "TYPE"
			<< " |" << std::endl;

		*stream << "|-----|--------|---------------------------|---------------------------|-----------------|" << std::endl;

		int counter = 0; // Счетчик только для выводимых строк

		for (int i = 0; i < idtable.size; i++)
		{
			Entry* e = &idtable.table[i];

			//  ФИЛЬТРАЦИЯ 
			// Если это Литерал (L) или Стандартная функция (S) - пропускаем
			if (e->idtype == IT::IDTYPE::L || e->idtype == IT::IDTYPE::S)
			{
				continue;
			}

			// Буферы для области видимости и имени
			char scope[SCOPED_ID_MAXSIZE] = "";
			char cleanName[SCOPED_ID_MAXSIZE] = "";

			// ЛОГИКА ОПРЕДЕЛЕНИЯ ОБЛАСТИ 
			const char* dotPos = strchr(e->id, '.');

			if (dotPos != nullptr)
			{
				// --- Локальная переменная или параметр ---
				// Копируем имя функции (до точки) в scope
				size_t len = dotPos - e->id;
				strncpy_s(scope, SCOPED_ID_MAXSIZE, e->id, len);

				// Копируем чистое имя переменной (после точки)
				strcpy_s(cleanName, SCOPED_ID_MAXSIZE, dotPos + 1);
			}
			else
			{
				// --- Глобальная функция или переменная ---
				strcpy_s(scope, SCOPED_ID_MAXSIZE, "GLOBAL");
				strcpy_s(cleanName, SCOPED_ID_MAXSIZE, e->id);
			}

			// Красивое название типа
			const char* typeStr = "Unknown";
			switch (e->idtype)
			{
			case IT::IDTYPE::V: typeStr = "Variable"; break;
			case IT::IDTYPE::F: typeStr = "Function"; break;
			case IT::IDTYPE::P: typeStr = "Parameter"; break;
			}

			// Вывод строки
			*stream << "| " << std::setw(3) << counter++
				<< " | " << std::setw(6) << e->idxfirstLE
				<< " | " << std::setw(25) << scope
				<< " | " << std::setw(25) << cleanName
				<< " | " << std::setw(15) << typeStr
				<< " |" << std::endl;
		}
		*stream << "============================================================================================\n";
	}

	// Обертка для вызова по имени файла
	void PrintScopeTable(IT::IdTable& idtable, const wchar_t* outFile)
	{
		std::ofstream fout(outFile);
		if (fout.is_open())
		{
			writeScopeTable(&fout, idtable);
			fout.close();
		}
	}
};