#include <cstdio>
#include "functions.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main()
{
#ifdef _WIN32
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif

    printf("Лабораторная работа №1. Обработка гласных букв.\n");
    printf("Меньше 5 — удалить; больше 5 — удвоить; ровно 5 — не изменять.\n");

    char stroka1[BUFFER_SIZE];
    int dlina = 0;
    int c;

    printf("\nЗадание 1. Введите строку:\n");
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (dlina == MAX_INPUT_LENGTH)
        {
            printf("Ошибка: строка длиннее %d байт.\n", MAX_INPUT_LENGTH);
            return 1;
        }
        stroka1[dlina] = static_cast<char>(c);
        dlina++;
    }

    if (ferror(stdin) || !Zadanie1(stroka1, dlina, BUFFER_SIZE))
    {
        printf("Ошибка чтения или недостаточно места в массиве.\n");
        return 1;
    }

    printf("Результат: ");
    for (int i = 0; i < dlina; i++)
    {
        printf("%c", stroka1[i]);
    }
    printf("\n");

    char stroka2[BUFFER_SIZE];
    printf("\nЗадание 2. Введите строку:\n");

    if (!ProchitatStroku(stdin, stroka2))
    {
        printf("Ошибка чтения: допустимо не более %d байт.\n", MAX_INPUT_LENGTH);
        return 1;
    }

    if (!Zadanie2(stroka2, BUFFER_SIZE))
    {
        printf("Недостаточно места для удвоения гласных.\n");
        return 1;
    }

    printf("Результат: %s\n", stroka2);

    printf("\nЗадание 3. Чтение из input.txt.\n");
    if (!Zadanie3("input.txt", "output.txt"))
    {
        printf("Ошибка работы с файлами или слишком длинная строка.\n");
        return 1;
    }

    printf("Результат записан в output.txt.\n");
    return 0;
}
