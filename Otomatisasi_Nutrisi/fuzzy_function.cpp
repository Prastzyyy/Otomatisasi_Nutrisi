//fuzzy_function.cpp

#include "fuzzy_function.h"

//=========== fungsi-fungsi =====================
//pH
float p_FiN(){
    if (p_inputf < p_fuKecil[2]){
    p_outFiN = 1;
  }
  else if (p_inputf >= p_fuKecil[2] && p_inputf <= p_fuKecil[3]){
    p_outFiN = (p_fuKecil[3] - p_inputf)/(p_fuKecil[3]-p_fuKecil[2]);
  }
  else if (p_inputf > p_fuKecil[3]){
    p_outFiN = 0;
  }
  return p_outFiN;
}
//p
float p_FiZ(){
    if (p_inputf < p_fuSedang[0]){
    p_outFiZ = 0;
  }
    else if (p_inputf >= p_fuSedang[0] && p_inputf <= p_fuSedang[1]){
    p_outFiZ = (p_inputf - p_fuSedang[0])/(p_fuSedang[1]-p_fuSedang[0]);
  }
  else if (p_inputf == p_fuSedang[1]) {
    p_outFiZ = 1;
  }
    else if (p_inputf >= p_fuSedang[1] && p_inputf <= p_fuSedang[2]){
    p_outFiZ = (p_fuSedang[2] - p_inputf)/(p_fuSedang[2]-p_fuSedang[1]);
  }
    else if (p_inputf > p_fuSedang[2]){
    p_outFiZ = 0;
  }
  return p_outFiZ;
}
//p
float p_FiP(){
    if (p_inputf < p_fuBesar[0]){
    p_outFiP = 0;
  }
    else if (p_inputf >= p_fuBesar[0] && p_inputf <= p_fuBesar[1]){
    p_outFiP = (p_inputf - p_fuBesar[0])/(p_fuBesar[1]-p_fuBesar[0]);
  }
    else if (p_inputf > p_fuBesar[1]){
    p_outFiP = 1;
  }
    return p_outFiP;
}
//p
float p_FoN(){
    if (p_outputf < p_fySebentar[1]){
    p_outFoN = 1;
  }
    else if (p_outputf >= p_fySebentar[1] && p_outputf <= p_fySebentar[2]){
    p_outFoN = (p_fySebentar[2] - p_outputf)/(p_fySebentar[2]-p_fySebentar[1]);
  }
    else if (p_outputf > p_fySebentar[2]){
    p_outFoN = 0;
  }
  return p_outFoN;
}
//p
float p_FoZ(){
    if (p_outputf < p_fySedang[0]){
    p_outFoZ = 0;
  }
    else if (p_outputf >= p_fySedang[0] && p_outputf <= p_fySedang[1]){
    p_outFoZ = (p_outputf - p_fySedang[0])/(p_fySedang[1]-p_fySedang[0]);
  }
    else if (p_outputf >= p_fySedang[1] && p_outputf <= p_fySedang[2]){
    p_outFoZ = (p_fySedang[2] - p_outputf)/(p_fySedang[2]-p_fySedang[1]);
  }
    else if (p_outputf > p_fySedang[2]){
    p_outFoZ = 0;
  }
  return p_outFoZ;
}
//p
float p_FoP(){
    if (p_outputf < p_fyLama[0]){
    p_outFoP = 0;
  }
    else if (p_outputf >= p_fyLama[0] && p_outputf <= p_fyLama[1]){
    p_outFoP = (p_outputf - p_fyLama[0])/(p_fyLama[1]-p_fyLama[0]);
  }
    else if (p_outputf > p_fyLama[1]){
    p_outFoP = 1;
  }
    return p_outFoP;
}
//p
void p_implikasi (){
    p_a1 = p_fySebentar[2] - (p_FiN() * (p_fySebentar[2] - p_fySebentar[1]));
    p_b1a = p_fySedang[0] + (p_FiZ() * (p_fySedang[1] - p_fySedang[0]));
    p_b1b =  p_fySedang[2] - (p_FiZ() * (p_fySedang[2] - p_fySedang[1]));
    p_c1 = p_fyLama[0] + (p_FiP() * (p_fyLama[1] - p_fyLama[0]));
}
//p
void p_luas_deffuzzy(){
  p_implikasi ();
  p_A1 = ((p_fySebentar[2] - p_a1) * p_FiN()) / 2;
  p_A2 = (p_a1 - p_fySebentar[0]) * p_FiN();
  p_A3 = ((p_b1a - p_fySedang[0])* p_FiZ()) / 2;
  p_A4 = ((p_fySedang[2] - p_b1b)* p_FiZ()) / 2;
  p_A5 = (p_b1b - p_b1a)*p_FiZ();
  p_A6 = ((p_c1 - p_fyLama[0]) * p_FiP()) / 2;
  p_A7 = (p_fyLama[2] - p_c1)* p_FiP();
}
//p
float p_f(float p_x){
    if (p_B > 0 && p_sel_ == 0){
    return ((p_x-p_A)/p_B)*p_x;
    }
    else if (p_B > 0 && p_sel_ == 1){
    return ((p_A-p_x)/p_B)*p_x;
    }
    else {
    return p_A*p_x;
    }
}
//p
float p_simpsons(float p_f(float p_x), float p_a,float p_b,float p_n){
  float p_h,p_integral,p_x,p_sum=0;
  int p_i;
  p_h=fabs(p_b-p_a)/p_n;
  for(p_i=1;p_i<p_n;p_i++){
    p_x=p_a+p_i*p_h;
    if(p_i%2==0){
      p_sum=p_sum+2*p_f(p_x);
    }
    else{
      p_sum=p_sum+4*p_f(p_x);
    }
  }
  p_integral=(p_h/3)*(p_f(p_a)+p_f(p_b)+p_sum);
  return p_integral;
}
//p
float p_fx(float p_limd, float p_limu, float p_a, float p_b, int p_sel){
  int p_n,p_i=2;
    float p_h,p_x,p_integral,p_eps=0.1,p_integral_new;
    p_A = p_a;
    p_B = p_b;
    p_sel_ = p_sel;
    p_integral_new=p_simpsons(p_f,p_limd,p_limu,p_i);
    do{
        p_integral=p_integral_new;
        p_i=p_i+2;
        p_integral_new=p_simpsons(p_f,p_limd,p_limu,p_i);
    }while(fabs(p_integral_new-p_integral)>=p_eps);
    return p_integral_new;
}
//p
void p_moment(){
  p_luas_deffuzzy();
  p_M1 = p_fx(p_a1, p_fySebentar[2], p_fySebentar[2], (p_fySebentar[2]-p_fySebentar[0]), 1);
  p_M2 = p_fx(p_fySebentar[0], p_a1, p_FiN(), 0, 0);
  p_M3 = p_fx(p_fySedang[0], p_b1a, p_fySedang[0], (p_fySedang[1] - p_fySedang[0]), 0);
  p_M4 = p_fx (p_b1b, p_fySedang[2], p_fySedang[2], (p_fySedang[2]-p_fySedang[1]), 1);
  p_M6 = p_fx(p_fyLama[0], p_c1, p_fyLama[0], (p_fyLama[2]-p_fyLama[0]), 0);
  p_M7 = p_fx(p_c1, p_fyLama[2], p_FiP(), 0, 0);
}
//p
float p_deffuzzyfikasi(){
  return (p_M1+p_M2+p_M3+p_M4+p_M5+p_M6+p_M7)/(p_A1+p_A2+p_A3+p_A4+p_A5+p_A6+p_A7);
}

//==============================================================================
//Nutrisi
//n
float n_FiN(){
    if (n_inputf < n_fuKecil[2]){
    n_outFiN = 1;
  }
    else if (n_inputf >= n_fuKecil[2] && n_inputf <= n_fuKecil[3]){
    n_outFiN = (n_fuKecil[3] - n_inputf)/(n_fuKecil[3]-n_fuKecil[2]);
  }
    else if (n_inputf > n_fuKecil[3]){
    n_outFiN = 0;
  }
    return n_outFiN;
}
//n
float n_FiZ(){
    if (n_inputf < n_fuSedang[0]){
    n_outFiZ = 0;
  }
    else if (n_inputf >= n_fuSedang[0] && n_inputf <= n_fuSedang[1]){
    n_outFiZ = (n_inputf - n_fuSedang[0])/(n_fuSedang[1]-n_fuSedang[0]);
  }
  else if (n_inputf == n_fuSedang[1]) {
    n_outFiZ = 1;
  }
    else if (n_inputf >= n_fuSedang[1] && n_inputf <= n_fuSedang[2]){
    n_outFiZ = (n_fuSedang[2] - n_inputf)/(n_fuSedang[2]-n_fuSedang[1]);
  }
    else if (n_inputf > n_fuSedang[2]){
    n_outFiZ = 0;
  }
  return n_outFiZ;
}
//n
float n_FiP(){
    if (n_inputf < n_fuBesar[0]){
    n_outFiP = 0;
  }
    else if (n_inputf >= n_fuBesar[0] && n_inputf <= n_fuBesar[1]){
    n_outFiP = (n_inputf - n_fuBesar[0])/(n_fuBesar[1]-n_fuBesar[0]);
  }
    else if (n_inputf > n_fuBesar[1]){
    n_outFiP = 1;
  }
    return n_outFiP;
}
//n
float n_FoN(){
    if (n_outputf < n_fySebentar[1]){
    n_outFoN = 1;
  }
    else if (n_outputf >= n_fySebentar[1] && n_outputf <= n_fySebentar[2]){
    n_outFoN = (n_fySebentar[2] - n_outputf)/(n_fySebentar[2]-n_fySebentar[1]);
  }
    else if (n_outputf > n_fySebentar[2]){
    n_outFoN = 0;
  }
  return n_outFoN;
}
//n
float n_FoZ(){
    if (n_outputf < n_fySedang[0]){
    n_outFoZ = 0;
  }
    else if (n_outputf >= n_fySedang[0] && n_outputf <= n_fySedang[1]){
    n_outFoZ = (n_outputf - n_fySedang[0])/(n_fySedang[1]-n_fySedang[0]);
  }
    else if (n_outputf >= n_fySedang[1] && n_outputf <= n_fySedang[2]){
    n_outFoZ = (n_fySedang[2] - n_outputf)/(n_fySedang[2]-n_fySedang[1]);
  }
    else if (n_outputf > n_fySedang[2]){
    n_outFoZ = 0;
  }
  return n_outFoZ;
}
//n
float n_FoP(){
    if (n_outputf < n_fyLama[0]){
    n_outFoP = 0;
  }
    else if (n_outputf >= n_fyLama[0] && n_outputf <= n_fyLama[1]){
    n_outFoP = (n_outputf - n_fyLama[0])/(n_fyLama[1]-n_fyLama[0]);
  }
    else if (n_outputf > n_fyLama[1]){
    n_outFoP = 1;
  }
    return n_outFoP;
}
//n
void n_implikasi (){
  n_a1 = n_fySebentar[2] - (n_FiN() * (n_fySebentar[2] - n_fySebentar[1]));
  n_b1a = n_fySedang[0] + (n_FiZ() * (n_fySedang[1] - n_fySedang[0]));
  n_b1b =  n_fySedang[2] - (n_FiZ() * (n_fySedang[2] - n_fySedang[1]));
  n_c1 = n_fyLama[0] + (n_FiP() * (n_fyLama[1] - n_fyLama[0]));

}
//n
void n_luas_deffuzzy(){
  n_implikasi ();
  n_A1 = ((n_fySebentar[2] - n_a1) * n_FiN()) / 2;
  n_A2 = (n_a1 - n_fySebentar[0]) * n_FiN();
  n_A3 = ((n_b1a - n_fySedang[0])* n_FiZ()) / 2;
  n_A4 = ((n_fySedang[2] - n_b1b)* n_FiZ()) / 2;
  n_A5 = (n_b1b - n_b1a)*n_FiZ();
  n_A6 = ((n_c1 - n_fyLama[0]) * n_FiP()) / 2;
  n_A7 = (n_fyLama[2] - n_c1)* n_FiP();
}
//n
float n_f(float n_x){
    if (n_B > 0 && n_sel_ == 0){
    return ((n_x-n_A)/n_B)*n_x;
    }
    else if (n_B > 0 && n_sel_ == 1){
    return ((n_A-n_x)/n_B)*n_x;
    }
    else {
    return n_A*n_x;
    }
}
//n
float n_simpsons(float n_f(float n_x), float n_a,float n_b,float n_n){
  float n_h,n_integral,n_x,n_sum=0;
  int n_i;
  n_h=fabs(n_b-n_a)/n_n;
  for(n_i=1;n_i<n_n;n_i++){
    n_x=n_a+n_i*n_h;
    if(n_i%2==0){
      n_sum=n_sum+2*n_f(n_x);
    }
    else{
      n_sum=n_sum+4*n_f(n_x);
    }
  }
  n_integral=(n_h/3)*(n_f(n_a)+n_f(n_b)+n_sum);
  return n_integral;
}
//n
float n_fx(float n_limd, float n_limu, float n_a, float n_b, int n_sel){
    int n_n,n_i=2;
    float n_h,n_x,n_integral,n_eps=0.1,n_integral_new;
    n_A = n_a;
    n_B = n_b;
    n_sel_ = n_sel;
    n_integral_new=n_simpsons(n_f,n_limd,n_limu,n_i);
    do{
        n_integral=n_integral_new;
        n_i=n_i+2;
        n_integral_new=n_simpsons(n_f,n_limd,n_limu,n_i);
    }while(fabs(n_integral_new-n_integral)>=n_eps);
    return n_integral_new;
}
//n
void n_moment(){
  n_luas_deffuzzy();
  n_M1 = n_fx(n_a1, n_fySebentar[2], n_fySebentar[2], (n_fySebentar[2]-n_fySebentar[0]), 1);
  n_M2 = n_fx(n_fySebentar[0], n_a1, n_FiN(), 0, 0);
  n_M3 = n_fx(n_fySedang[0], n_b1a, n_fySedang[0], (n_fySedang[1] - n_fySedang[0]), 0);
  n_M4 = n_fx (n_b1b, n_fySedang[2], n_fySedang[2], (n_fySedang[2]-n_fySedang[1]), 1);
  n_M5 = n_fx (n_b1a, n_b1b, n_FiZ(), 0, 0);
  n_M6 = n_fx(n_fyLama[0], n_c1, n_fyLama[0], (n_fyLama[2]-n_fyLama[0]), 0);
  n_M7 = n_fx(n_c1, n_fyLama[2], n_FiP(), 0, 0);
}
//n
float n_deffuzzyfikasi(){
  return (n_M1+n_M2+n_M3+n_M4+n_M5+n_M6+n_M7)/(n_A1+n_A2+n_A3+n_A4+n_A5+n_A6+n_A7);
}
