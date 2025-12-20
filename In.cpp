#include "Error.h"
#include "Parm.h"
#include "In.h"
#include "Log.h"
#define	LEX_MINUS		 '-'	// вычитание
#define	MAXSIZE_LT		 4096	    //максимальное количество строк в ТЛ


int In::InWord::size = NULL;

namespace In
{
	IN getin(wchar_t infile[], std::ostream* stream)			// ввести и проверить входной поток
	{
		unsigned char* text = new unsigned char[IN_MAX_LEN_TEXT];
		std::ifstream instream(infile);
		if (!instream.is_open())
			throw ERROR_THROW(102);								//"Системная ошибка: Ошибка при открытии файла с исходным кодом(-in)"
		IN in;
		int pos = 0;
		bool isLiteral = false;
		while (true)
		{
			unsigned char uch = instream.get();
			if (instream.eof())
				break;
			if (in.code[uch] == IN::Q)
				isLiteral = !isLiteral;
			switch (in.code[uch])
			{
			case IN::N:
				text[in.size++] = uch;
				in.lines++;
				pos = 0;
				break;
			case IN::T:
			case IN::P:
			case IN::S:
			case IN::Q:
				text[in.size++] = uch;
				pos++;
				break;
			case IN::F:
				Log::writeError(stream, Error::GetError(200, in.lines, pos));
				in.ignor++;
				pos++;
				break;
			case IN::I:
				in.ignor++;
				pos++;
				break;
			default:
				text[in.size++] = in.code[uch];
				pos++;
			}
		}
		text[in.size] = IN_CODE_NULL;
		in.text = text;
		instream.close();
		return in;
	}

	void addWord(InWord* words, char* word, int line, int col)
	{
		if (*word == IN_CODE_NULL)
			return;
		words[InWord::size].line = line;
		words[InWord::size].col = col;
		strcpy_s(words[InWord::size].word, strlen(word) + 1, word);
		InWord::size++;
	}

	InWord* getWordsTable(std::ostream* stream, unsigned char* text, int* code, int textSize)
	{
		InWord* words = new InWord[MAXSIZE_LT];
		int bufpos = 0;
		int line = 1;
		int col = 0;
		int wordStartCol = 0;
		char buffer[MAX_LEN_BUFFER] = "";

		for (int i = 0; i < textSize; i++)
		{
			switch (code[text[i]])
			{
			case IN::S:
			{
				if (text[i] == LEX_MINUS && isdigit(text[i + 1]))
				{
					if (bufpos == 0) wordStartCol = col;
					buffer[bufpos++] = text[i];
					buffer[bufpos] = IN_CODE_NULL;
					col++;
					break;
				}
				addWord(words, buffer, line, wordStartCol);
				*buffer = IN_CODE_NULL;
				bufpos = 0;

				char letter[] = { (char)text[i], IN_CODE_NULL };
				addWord(words, letter, line, col);
				col++;
				wordStartCol = col;  // Обновляем начальную позицию для следующего слова
				break;
			}
			case IN::N:
				addWord(words, buffer, line, wordStartCol);
				*buffer = IN_CODE_NULL;
				bufpos = 0;
				line++;
				col = 0;
				wordStartCol = 0;  // Сбрасываем позицию для новой строки
				break;

			case IN::P:
				addWord(words, buffer, line, wordStartCol);
				*buffer = IN_CODE_NULL;
				bufpos = 0;
				col++;
				wordStartCol = col;  // Обновляем начальную позицию после пробела
				break;

			case IN::Q:
			{
				addWord(words, buffer, line, wordStartCol);
				*buffer = IN_CODE_NULL;
				bufpos = 0;
				wordStartCol = col;

				bool isltrlgood = true;
				bool isChar = false;

				for (int j = i + 1; text[j] != IN_CODE_QUOTE; j++)
				{
					if (text[j] == IN_CODE_QUOTE2) {
						isChar = true;
						break;
					}
					if (code[text[j]] == IN::N)
					{
						Log::writeError(stream, Error::GetError(311, line, col));
						isltrlgood = false;
						break;
					}
				}

				if (isltrlgood && !isChar)
				{
					buffer[bufpos++] = IN_CODE_QUOTE;
					col++;
					for (int j = 1; ; j++)
					{
						if (j == 256 || i + j == textSize)
						{
							Log::writeError(stream, Error::GetError(312, line, wordStartCol));
							break;
						}
						buffer[bufpos++] = text[i + j];
						col++;
						if (text[i + j] == IN_CODE_QUOTE)
						{
							buffer[bufpos] = IN_CODE_NULL;
							addWord(words, buffer, line, wordStartCol);
							i = i + j;
							break;
						}
					}
				}
				else if (isChar)
				{
					buffer[bufpos++] = IN_CODE_QUOTE2;
					col++;
					for (int j = 1; ; j++)
					{
						if (j == 3 || i + j == textSize)
						{
							Log::writeError(stream, Error::GetError(312, line, wordStartCol));
							break;
						}
						buffer[bufpos++] = text[i + j];
						col++;
						if (text[i + j] == IN_CODE_QUOTE2)
						{
							buffer[bufpos] = IN_CODE_NULL;
							addWord(words, buffer, line, wordStartCol);
							i = i + j;
							break;
						}
					}
				}
				*buffer = IN_CODE_NULL;
				bufpos = 0;
				wordStartCol = col;  // Обновляем позицию после строкового литерала
				break;
			}
			default:
				if (bufpos == 0) wordStartCol = col;
				buffer[bufpos++] = text[i];
				buffer[bufpos] = IN_CODE_NULL;
				col++;
			}
		}
		return words;
	}


	void printTable(InWord* table)
	{
		std::cout << " ------------------ ТАБЛИЦА СЛОВ: ------------------" << std::endl;
		for (int i = 0; i < table->size; i++)
			std::cout << std::setw(2) << i
			<< std::setw(3) << table[i].line
			<< ":" << std::setw(3) << table[i].col
			<< " |  " << table[i].word << std::endl;
	}
};
