#pragma once
#include <iostream>

#define MAXSIZE_ID	16						
#define SCOPED_ID_MAXSIZE   MAXSIZE_ID*2	
#define MAXSIZE_TI		4096				
#define INT_DEFAULT	0x00000000				
#define STR_DEFAULT	0x00					
#define NULLIDX_TI		0xffffffff			
#define STR_MAXSIZE	255						
#define CHAR_MAXSIZE 1						
#define INT_MAXSIZE   4294967295			
#define INT_MINSIZE   0			
#define MAX_PARAMS_COUNT 3													
#define LENGHT_PARAMS_CNT 1												
#define COPY_PARAMS_CNT 1		

#define LENGHT_TYPE IT::IDDATATYPE::INT
#define ITOA_TYPE IT::IDDATATYPE::INT
#define COPY_TYPE IT::IDDATATYPE::STR

namespace IT
{
	enum IDDATATYPE { INT = 1, STR = 2, PROC = 3, CHAR = 4, BOOL = 5, UNDEF };

	enum IDTYPE { V = 1, F = 2, P = 3, L = 4, S = 5 };
	enum STDFNC { F_LENGHT, F_NOT_STD, F_ITOA };

	static const IDDATATYPE LENGHT_PARAMS[] = { IT::IDDATATYPE::STR };
	static const IDDATATYPE ITOA_PARAMS[] = { IT::IDDATATYPE::INT };
	static const IDDATATYPE COPY_PARAMS[] = { IT::IDDATATYPE::STR };

	struct Entry
	{
		union
		{
			int	vint;
			struct
			{
				int len;
				char str[STR_MAXSIZE - 1];
			} vstr;
			struct
			{
				int count;
				IDDATATYPE* types;
			} params;
		} value;
		int			idxfirstLE;
		char		id[SCOPED_ID_MAXSIZE];
		IDDATATYPE	iddatatype;
		IDTYPE		idtype;
		int			parmQuantity;  

		Entry()
		{
			this->value.vint = INT_DEFAULT;
			this->value.vstr.len = NULL;
			this->value.params.count = NULL;
			this->parmQuantity = 0; 
		};
		Entry(char* id, int idxLT, IDDATATYPE datatype, IDTYPE idtype)
		{
			strncpy_s(this->id, id, SCOPED_ID_MAXSIZE - 1);
			this->idxfirstLE = idxLT;
			this->iddatatype = datatype;
			this->idtype = idtype;
			this->parmQuantity = 0; 
		};
	};

	struct IdTable
	{
		int maxsize;
		int size;
		Entry* table;
	};

	IdTable Create(int size = NULL);
	void Add(IdTable& idtable, Entry entry);
	int isId(IdTable& idtable, char id[SCOPED_ID_MAXSIZE]);
	bool SetValue(IT::Entry* entry, char* value);
	bool SetValue(IT::IdTable& idtable, int index, char* value);

	// Функции вывода
	void PrintIdTable(IT::IdTable& idtable, std::ostream* stream);
	void PrintLiteralTable(IT::IdTable& idtable, std::ostream* stream);
	void WriteIdTableToFile(IT::IdTable& idtable, const wchar_t* filename);
	void WriteLiteralTableToFile(IT::IdTable& idtable, const wchar_t* filename);
	void writeScopeTable(std::ostream* stream, IT::IdTable& idtable);
	void PrintScopeTable(IT::IdTable& idtable, const wchar_t* outFile);
};
