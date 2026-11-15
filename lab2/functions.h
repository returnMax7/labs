#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <cstdio>

bool Zadanie1(char*& stroka, int& dlina);
bool Zadanie2(char*& stroka);
bool Zadanie3(const char* imya_fayla1, const char* imya_fayla2, FILE* istochnik = stdin);
bool ProchitatStroku(FILE* file, char*& stroka, bool sKlaviatury);

#endif
