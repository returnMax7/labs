#include <climits>
#include <cstdio>
#include "functions.h"
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif

int main()
{
#ifdef _WIN32
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif

    printf("Лабораторная работа №2. Обработка гласных букв.\n");
    printf("Меньше 5 — удалить; больше 5 — удвоить; ровно 5 — не изменять.\n");

    int dlina = 0;
    int c;
    char* stroka1 = nullptr;

    printf("\nЗадание 1. Введите строку:\n");
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (dlina == INT_MAX)
        {
            free(stroka1);
            printf("Ошибка: строка слишком длинная.\n");
            return 1;
        }

        char* novaya = (char*)malloc((dlina + 1) * sizeof(char));
        if (novaya == nullptr)
        {
            free(stroka1);
            printf("Ошибка выделения памяти.\n");
            return 1;
        }

        for (int i = 0; i < dlina; i++)
        {
            novaya[i] = stroka1[i];
        }
        novaya[dlina] = (char)c;
        free(stroka1);
        stroka1 = novaya;
        dlina++;
    }

    if (ferror(stdin) || !Zadanie1(stroka1, dlina))
    {
        free(stroka1);
        printf("Ошибка ввода или выделения памяти.\n");
        return 1;
    }

    printf("Результат: ");
    for (int i = 0; i < dlina; i++)
    {
        printf("%c", stroka1[i]);
    }
    printf("\n");
    free(stroka1);

    char* stroka2 = nullptr;
    printf("\nЗадание 2. Введите строку и завершите её символами \\0, затем нажмите Enter.\n");
    if (!ProchitatStroku(stdin, stroka2, true) || !Zadanie2(stroka2))
    {
        free(stroka2);
        printf("Ошибка ввода или выделения памяти.\n");
        return 1;
    }

    printf("Результат: %s\n", stroka2);
    free(stroka2);

    printf("\nЗадание 3. Введите строку и завершите её символами \\0, затем нажмите Enter.\n");
    if (!Zadanie3("input.txt", "output.txt"))
    {
        printf("Ошибка ввода, работы с файлами или выделения памяти.\n");
        return 1;
    }

    printf("Результат записан в output.txt.\n");
    return 0;
}
