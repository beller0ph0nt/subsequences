#Подпоследовательности

Данное консольное клиент-серверное приложение, работает в среде Linux на 
пользовательском уровне. Серверная часть генерирует последовательность чисел,
состоящую из подпоследовательностей (подпоследовательность задаётся клиентом).
Каждая из подпоследовательностей представляет собой целочисленный
неотрицательный 64-х битный счетчик, который задаётся начальным значением и
шагом. Сгенерированная последовательность отправляется клиенту.

##Перечень команд клиента

seq1 <start_value> <step>
    Описание:
        Команда задает начальное значение первой подпоследовательности
    Параметры:
        seq1        - Команда
        start_value - Начальное значение
        step        - Шаг подпоследовательности

seq2 <start_value> <step>
    Описание:
        Команда задает начальное значение второй подпоследовательности
    Параметры:
        seq2        - Команда
        start_value - Начальное значение
        step        - Шаг подпоследовательности
    
seq3 <start_value> <step>
    Описание:
        Команда задает начальное значение третьей подпоследовательности
    Параметры:
        seq3        - Команда
        start_value - Начальное значение
        step        - Шаг подпоследовательности

export seq
    Описание:
        Команда выдает в сокет каждому клиенту сгенерированную
        последовательность.
    Параметры:
        export seq - Команда

##Примечания

    Если в командах seq1, seq2, seq3 любой из параметров (начальное значение
и/или шаг) будет указан как = 0, то программа не учитываtn данную
подпоследовательность.
    При переполнении счетчика, подпоследовательность начинается сначала.
    Формат передаваемых по сети данных – 64-х битные целые числа (binary data).
    Программа не завершает работу в случаях некорректно введенных параметров,
аварийного завершения работы клиента и т.д.
    Для создания/управления потоками, примитивами синхронизации используется
библиотека pthread.

##Примеры входных и выходных данных

seq1 1 2 – задает подпоследовательность 1, 3, 5 и т.д.
seq2 2 3 – задает подпоследовательность 2, 5, 8 и т.д.
seq3 3 4 – задает подпоследовательность 3, 7, 11 и т.д.
export seq – в сокет передается последовательность 1, 2, 3, 3, 5, 7, 5, 8, 11 и т.д. 