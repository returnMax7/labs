#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <cstdio>

const int MAX_INPUT_LENGTH = 1023;
const int BUFFER_SIZE = MAX_INPUT_LENGTH * 2 + 1;

bool ProchitatStroku(FILE* file, char stroka[]);
bool Zadanie1(char stroka[], int& dlina, int vmestimost);
bool Zadanie2(char stroka[], int vmestimost);
bool Zadanie3(const char* imya_fayla1, const char* imya_fayla2);

#endif
