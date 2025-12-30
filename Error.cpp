#include "Error.h"

namespace Error
{
	ERROR errors[ERROR_MAX_ENTRY] =
	{
		ERROR_ENTRY(0, "Системная ошибка: Недопустимый код ошибки"),
		ERROR_ENTRY(1, "Системная ошибка: Системный сбой"),
		ERROR_ENTRY_NODEF(2), ERROR_ENTRY_NODEF(3), ERROR_ENTRY_NODEF(4), ERROR_ENTRY_NODEF(5),
		ERROR_ENTRY_NODEF(6), ERROR_ENTRY_NODEF(7), ERROR_ENTRY_NODEF(8), ERROR_ENTRY_NODEF(9),
		ERROR_ENTRY_NODEF10(10), ERROR_ENTRY_NODEF10(20), ERROR_ENTRY_NODEF10(30), ERROR_ENTRY_NODEF10(40),
		ERROR_ENTRY_NODEF10(50), ERROR_ENTRY_NODEF10(60), ERROR_ENTRY_NODEF10(70), ERROR_ENTRY_NODEF10(80), ERROR_ENTRY_NODEF10(90),

		ERROR_ENTRY(100, "Системная ошибка: Параметр -in должен быть задан"),
		ERROR_ENTRY(101, "Системная ошибка: Превышена длина входного параметра"),
		ERROR_ENTRY(102, "Системная ошибка: Ошибка при открытии файла с исходным кодом(-in)"),
		ERROR_ENTRY(103, "Системная ошибка: Ошибка при создании файла протокола(-log)"),
		ERROR_ENTRY_NODEF(104), ERROR_ENTRY_NODEF(105), ERROR_ENTRY_NODEF(106), ERROR_ENTRY_NODEF(107), ERROR_ENTRY_NODEF(108), ERROR_ENTRY_NODEF(109),
		ERROR_ENTRY_NODEF10(110), ERROR_ENTRY_NODEF10(120), ERROR_ENTRY_NODEF10(130), ERROR_ENTRY_NODEF10(140),
		ERROR_ENTRY_NODEF10(150), ERROR_ENTRY_NODEF10(160), ERROR_ENTRY_NODEF10(170), ERROR_ENTRY_NODEF10(180), ERROR_ENTRY_NODEF10(190),

		ERROR_ENTRY(200, "Лексическая ошибка: Недопустимый символ в исходном файле(-in)"),
		ERROR_ENTRY(201, "Лексическая ошибка: Неизвестная последовательность символов"),
		ERROR_ENTRY(202, "Лексическая ошибка: Превышен размер таблицы лексем"),
		ERROR_ENTRY(203, "Лексическая ошибка: Превышен размер таблицы идентификаторов"),
		ERROR_ENTRY(204, "Лексическая ошибка: Превышено число символов идентификатора"),
		ERROR_ENTRY_NODEF(205), ERROR_ENTRY_NODEF(206), ERROR_ENTRY_NODEF(207), ERROR_ENTRY_NODEF(208), ERROR_ENTRY_NODEF(209),
		ERROR_ENTRY_NODEF10(210), ERROR_ENTRY_NODEF10(220), ERROR_ENTRY_NODEF10(230), ERROR_ENTRY_NODEF10(240),
		ERROR_ENTRY_NODEF10(250), ERROR_ENTRY_NODEF10(260), ERROR_ENTRY_NODEF10(270), ERROR_ENTRY_NODEF10(280), ERROR_ENTRY_NODEF10(290),

		ERROR_ENTRY(300, "Лексическая ошибка: Необъявленный идентификатор"),
		ERROR_ENTRY(301, "Синтаксическая ошибка: Отсутствует точка входа main"),
		ERROR_ENTRY(302, "Синтаксическая ошибка: Обнаружено несколько точек входа main"),
		ERROR_ENTRY(303, "Лексическая ошибка: В объявлении не указан тип идентификатора"),
		ERROR_ENTRY(304, "Семантическая ошибка: В объявлении отсутствует ключевое слово"),
		ERROR_ENTRY(305, "Семантическая ошибка: Попытка переопределения идентификатора"),
		ERROR_ENTRY(306, "Семантическая ошибка: Превышено максимальное количество параметров функции"),
		ERROR_ENTRY(307, "Семантическая ошибка: Слишком много параметров в вызове"),
		ERROR_ENTRY(308, "Семантическая ошибка: Кол-во ожидаемых функцией и передаваемых параметров не совпадают"),
		ERROR_ENTRY(309, "Семантическая ошибка: Несовпадение типов передаваемых параметров"),
		ERROR_ENTRY(310, "Лексическая ошибка: Использование пустого строкового/символьного литерала недопустимо"),
		ERROR_ENTRY(311, "Синтаксическая ошибка: Обнаружен символ \'\"\'. Возможно, не закрыт строковый/символьный литерал"),
		ERROR_ENTRY(312, "Лексическая ошибка: Превышен размер строкового литерала или char "),
		ERROR_ENTRY(313, "Лексическая ошибка: Недопустимый number литерал"),
		ERROR_ENTRY(314, "Семантическая ошибка: Типы данных в выражении не совпадают"),
		ERROR_ENTRY(315, "Семантическая ошибка: Тип функции и возвращаемого значения не совпадают"),
		ERROR_ENTRY(316, "Семантическая ошибка: Недопустимое строковое выражение справа от знака \'=\'"),
		ERROR_ENTRY(317, "Семантическая ошибка: Деление на ноль"),
	    ERROR_ENTRY(318, "Семантическая ошибка: Инвертация number"),
		ERROR_ENTRY_NODEF(319),
		ERROR_ENTRY_NODEF10(320),ERROR_ENTRY_NODEF10(330),ERROR_ENTRY_NODEF10(340),ERROR_ENTRY_NODEF10(350),
		ERROR_ENTRY_NODEF10(360),ERROR_ENTRY_NODEF10(370),ERROR_ENTRY_NODEF10(380),ERROR_ENTRY_NODEF10(390),
		ERROR_ENTRY_NODEF100(400), ERROR_ENTRY_NODEF100(500),


		ERROR_ENTRY(600, "Синтаксическая ошибка: Неверная структура программы (ожидается function или main)"), 
		ERROR_ENTRY(601, "Синтаксическая ошибка: Ошибка в объявлении параметров функции"), 
		ERROR_ENTRY_NODEF(602),
		ERROR_ENTRY_NODEF(603),
		ERROR_ENTRY(604, "Синтаксическая ошибка: Ошибка внутри списка параметров (ожидается тип и id)"),
		ERROR_ENTRY(605, "Синтаксическая ошибка: Ошибка при вызове функции"), 
		ERROR_ENTRY(606, "Синтаксическая ошибка: Неверные аргументы вызова функции"),  
		ERROR_ENTRY(607, "Синтаксическая ошибка: Ожидается арифметический оператор (+, -, *, / )"), 
		ERROR_ENTRY_NODEF(608),
		ERROR_ENTRY(609, "Синтаксическая ошибка: Ошибка в арифметическом выражении (expr)"), 
		ERROR_ENTRY(610, "Синтаксическая ошибка: Ошибка в блоке операторов"),
		ERROR_ENTRY_NODEF(611),
		ERROR_ENTRY(612, "Синтаксическая ошибка: Ошибка в конструкции switch/case (ожидается case или default)"), 
		ERROR_ENTRY_NODEF(613),
		ERROR_ENTRY_NODEF(618),
		ERROR_ENTRY_NODEF(619),
		ERROR_ENTRY_NODEF10(620), ERROR_ENTRY_NODEF10(630), ERROR_ENTRY_NODEF10(640), ERROR_ENTRY_NODEF10(650),
		ERROR_ENTRY_NODEF10(660), ERROR_ENTRY_NODEF10(670), ERROR_ENTRY_NODEF10(680), ERROR_ENTRY_NODEF10(690),
		ERROR_ENTRY_NODEF100(700), ERROR_ENTRY_NODEF100(800), 
	};

	ERROR GetError(int id)
	{
		if (id < 0 || id > ERROR_MAX_ENTRY)
			return errors[0];
		else
			return errors[id];
	}

	ERROR GetError(int id, int line, int col)
	{
		ERROR err = GetError(id);
		err.position.line = line;
		return err;
	}
}