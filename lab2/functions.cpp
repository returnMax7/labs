#include <cstring>
#include <climits>
#include "functions.h"
#include <stdlib.h>


static char Strochnaya(char bukva)
{
    if ((bukva >= 'A' && bukva <= 'Z') ||
        (bukva >= 'А' && bukva <= 'Я'))
    {
        return char(bukva + 32);
    }
    if (bukva == 'Ё')
    {
        return 'ё';
    }
    return bukva;
}

static char Zaglavnaya(char bukva)
{
    if ((bukva >= 'a' && bukva <= 'z') ||
        (bukva >= 'а' && bukva <= 'я'))
    {
        return char(bukva - 32);
    }
    if (bukva == 'ё')
    {
        return 'Ё';
    }
    return bukva;
}

static bool Glasnaya(char bukva)
{
    bukva = Strochnaya(bukva);

    return bukva == 'a' || bukva == 'e' || bukva == 'i' ||
           bukva == 'o' || bukva == 'u' || bukva == 'y' ||
           bukva == 'а' || bukva == 'е' || bukva == 'ё' ||
           bukva == 'и' || bukva == 'о' || bukva == 'у' ||
           bukva == 'ы' || bukva == 'э' || bukva == 'ю' || bukva == 'я';
}

bool Zadanie1(char*& stroka, int& dlina)
{
    int kolichestvo = 0;
    for (int i = 0; i < dlina; i++)
    {
        if (Glasnaya(stroka[i]))
        {
            kolichestvo++;
        }
    }

    if (kolichestvo < 5)
    {
        int zapis = 0;
        for (int chtenie = 0; chtenie < dlina; chtenie++)
        {
            if (!Glasnaya(stroka[chtenie]))
            {
                stroka[zapis] = stroka[chtenie];
                zapis++;
            }
        }
        dlina = zapis;
    }
    else if (kolichestvo > 5)
    {
        if (kolichestvo > INT_MAX - dlina)
            return false;

        char* novaya = (char*)realloc(stroka, static_cast<size_t>(dlina) + kolichestvo + 1);
        if (novaya == nullptr)
            return false;

        stroka = novaya;

        for (int i = 0; i < dlina; )
        {
            if (!Glasnaya(stroka[i]))
            {
                i++;
                continue;
            }

            for (int j = dlina; j > i + 1; j--)
            {
                stroka[j] = stroka[j - 1];
            }

            stroka[i] = Zaglavnaya(stroka[i]);
            stroka[i + 1] = Strochnaya(stroka[i]);
            dlina++;
            i += 2;
        }
    }
    return true;
}

bool Zadanie2(char*& stroka)
{
    int dlina = static_cast<int>(strlen(stroka));
    if (!Zadanie1(stroka, dlina))
        return false;

    stroka[dlina] = '\0';
    return true;
}

bool ProchitatStroku(FILE* file, char*& stroka, bool sKlaviatury)
{
    int dlina = 0;
    int capacity = 5;
    stroka = (char*)calloc(capacity, sizeof(char));
    if (stroka == nullptr)
        return false;

    int c;
    while ((c = fgetc(file)) != EOF)
    {
        if (sKlaviatury && c == '0' && dlina > 0 &&
            stroka[dlina - 1] == '\\')
        {
            --dlina;
            // Пропускаем остаток введённой строки до Enter.
            while ((c = fgetc(file)) != '\n' && c != EOF)
            {
            }
            stroka[dlina] = '\0';
            return ferror(file) == 0;
        }
        if (dlina + 1 == capacity)
        {
            if (capacity > INT_MAX - 5)
                return false;

            char* novaya = (char*)realloc(stroka, capacity + 5);
            if (novaya == nullptr)
                return false;

            stroka = novaya;
            capacity += 5;
        }

        stroka[dlina++] = (char)c;
    }

    stroka[dlina] = '\0';
    return ferror(file) == 0 && (!sKlaviatury || c != EOF || dlina > 0);
}

static bool ZapisatStroku(const char* imya_fayla, const char* stroka)
{
    FILE* file = fopen(imya_fayla, "w");
    if (file == nullptr)
        return false;

    bool zapisano = fputs(stroka, file) != EOF;
    return fclose(file) == 0 && zapisano;
}

bool Zadanie3(const char* imya_fayla1, const char* imya_fayla2)
{
    char* vvod = nullptr;
    bool zapisano = ProchitatStroku(stdin, vvod, true) && ZapisatStroku(imya_fayla1, vvod);
    free(vvod);
    if (!zapisano)
        return false;

    FILE* vhod = fopen(imya_fayla1, "r");
    if (vhod == nullptr)
        return false;

    char* stroka = nullptr;
    bool prochitano = ProchitatStroku(vhod, stroka, false);
    int zakrytVhod = fclose(vhod);
    bool rezultat = prochitano && zakrytVhod == 0 && Zadanie2(stroka) && ZapisatStroku(imya_fayla2, stroka);
    free(stroka);
    return rezultat;
}
