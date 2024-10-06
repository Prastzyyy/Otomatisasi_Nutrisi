//fuzzy_function.h

#ifndef FUZZY_FUNCTION_H
#define FUZZY_FUNCTION_H
#include <Arduino.h>

//Deklarasi Input dan Output
//Input pH
float p_fuKecil[4] = {0, 0, 0, 33};
float p_fuSedang[3] = {30, 50, 70};
float p_fuBesar[4] = {66, 100, 100, 100};
//output pH
float p_fySebentar[3] = {0, 0, 33};
float p_fySedang[3] = {33, 50, 66};
float p_fyLama[3] = {66, 100, 100};
//input Nutrisi
float n_fuKecil[4] = {0, 0, 0, 33};
float n_fuSedang[3] = {30, 50, 70};
float n_fuBesar[4] = {66, 100, 100, 100};
//output Nutrisi
float n_fySebentar[3] = {0, 0, 33};
float n_fySedang[3] = {33, 50, 66};
float n_fyLama[3] = {66, 100, 100};

//Deklarasi Komponen pH =======================================
extern float p_inputf, p_outputf;

float p_A, p_B, p_hasil;
int p_sel_;
float p_a1, p_b1a, p_b1b, p_c1;
float p_A1, p_A2, p_A3, p_A4, p_A5, p_A6, p_A7;
float p_M1, p_M2, p_M3, p_M4, p_M5, p_M6, p_M7;
float p_outFiN, p_outFiZ, p_outFiP, p_outFoN, p_outFoZ, p_outFoP;

float p_FiN();
float p_FiZ();
float p_FiP();
float p_FoN();
float p_FoZ();
float p_FoP();
void p_implikasi();
void p_luas_deffuzzy();
float p_f(float p_x);
float p_simpsons(float p_f(float p_x), float p_a,float p_b,float p_n);
float p_fx(float p_limd, float p_limu, float p_a, float p_b, int p_sel);
void p_moment();
float p_deffuzzyfikasi();

//Deklarasi komponen Nutrisi ====================================
extern float n_inputf, n_outputf;

float n_A, n_B, n_hasil;
int n_sel_;
float n_a1, n_b1a, n_b1b, n_c1;
float n_A1, n_A2, n_A3, n_A4, n_A5, n_A6, n_A7;
float n_M1, n_M2, n_M3, n_M4, n_M5, n_M6, n_M7;
float n_outFiN, n_outFiZ, n_outFiP, n_outFoN, n_outFoZ, n_outFoP;

float n_FiN();
float n_FiZ();
float n_FiP();
float n_FoN();
float n_FoZ();
float n_FoP();
void n_implikasi();
void n_luas_deffuzzy();
float n_f(float n_x);
float n_simpsons(float n_f(float n_x), float n_a,float n_b,float n_n);
float n_fx(float n_limd, float n_limu, float n_a, float n_b, int n_sel);
void n_moment();
float n_deffuzzyfikasi();

#endif
