#include <cstring>
#include "functions.h"



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

bool Zadanie1(char stroka[], int& dlina, int vmestimost)
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
                stroka[zapis++] = stroka[chtenie];
            }
        }
        dlina = zapis;
    }
    else if (kolichestvo > 5)
    {
        if (kolichestvo > vmestimost - dlina)
        {
            return false;
        }

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

bool Zadanie2(char stroka[], int vmestimost)
{
    int dlina = static_cast<int>(strlen(stroka));
    bool rezultat = Zadanie1(stroka, dlina, vmestimost - 1);
    stroka[dlina] = '\0';
    return rezultat;
}

bool ProchitatStroku(FILE* file, char stroka[])
{
    if (fgets(stroka, MAX_INPUT_LENGTH + 1, file) == nullptr)
    {
        stroka[0] = '\0';
        return feof(file) != 0 && ferror(file) == 0;
    }

    int dlina = 0;
    while (stroka[dlina] != '\0')
    {
        dlina++;
    }

    if (dlina > 0 && stroka[dlina - 1] == '\n')
    {
        dlina--;
        stroka[dlina] = '\0';
    }
    else
    {
        int c = fgetc(file);
        if ((c != '\n' && c != EOF) || ferror(file))
        {
            return false;
        }
    }

    return true;
}

bool Zadanie3(const char* imya_fayla1, const char* imya_fayla2)
{
    FILE* vhod = fopen(imya_fayla1, "r");
    if (vhod == nullptr)
    {
        return false;
    }

    char stroka[BUFFER_SIZE];
    bool prochitano = ProchitatStroku(vhod, stroka);
    int zakrytVhod = fclose(vhod);
    if (!prochitano || zakrytVhod != 0 || !Zadanie2(stroka, BUFFER_SIZE))
    {
        return false;
    }

    FILE* vyhod = fopen(imya_fayla2, "w");
    if (vyhod == nullptr)
    {
        return false;
    }

    bool zapisano = fputs(stroka, vyhod) != EOF;
    int zakrytVyhod = fclose(vyhod);
    return zapisano && zakrytVyhod == 0;
}
