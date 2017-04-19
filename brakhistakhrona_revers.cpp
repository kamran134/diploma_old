#include <iostream>
#include <cmath>
#include <fstream>
#include <cstdio>
#include <iomanip>
#include <cstdlib>

#define EPS_DDOPRI 1e-11
#define ens 1e-10 //epsilon newton stop
#define eps_gamma 2e-15

using namespace std;

double hres;

#include "diploma_func/utilits.cpp"

//------------------------------------------
void fcn(double x, double *y, double *f) {
	double p_vx2, p_vy2, RHO;
	double g=9.8;
	p_vx2 = y[6]*y[6];	
    p_vy2 = y[7]*y[7];
    RHO = sqrt(p_vx2+p_vy2);
	
	if(RHO<=1e-12) {
		RHO = sqrt(y[4]*y[4]+y[5]*y[5]);
		y[6]=y[4];
		y[7]=y[5];
	}


	f[0] = y[2];
	f[1] = y[3];
	f[2] = (g*y[6])/(2*RHO);
	f[3] = g/2. + (g*y[7])/(2*RHO);
	f[4] = 0;
	f[5] = 0;
	f[6] = -y[4];
	f[7] = -y[5];
	
	//f[2] = (g*y[6])/(2*RHO);
	//f[3] = g/2. + (g*y[7])/(2*RHO);
}



//Метод Рунге-Кутты 5-ого порядка с автоматическим выбором шага
#include "diploma_func/ddopri5_brakhistakhrona.cpp"
#include "diploma_func/gauss_m.cpp"

void l(double *beta, double *res, double *y) {
	double pvx2, pvy2, RHO, H;
	double g=9.8;

	y[0]=0; //x_0
	y[1]=0; //y_0
	y[2]=0; //vx_0
	y[3]=0; //vy_0
	y[4]=beta[0]; //px_0
	y[5]=beta[1]; //py_0
	y[6]=beta[2]; //p_vx0
	y[7]=beta[3]; //p_vy0
	ddopri5(8,fcn,beta[4],y,0,1.e-11,1.0e0,0.5e0);
	
	pvx2 = y[6]*y[6];
    pvy2 = y[7]*y[7];
    RHO = sqrt(pvx2+pvy2); //RHO(T)
    
    //ВАЖНЫЙ МОМЕНТ!
    if(RHO<=1e-12) {
		RHO = sqrt(y[4]*y[4]+y[5]*y[5]);
		y[6]=y[4];
		y[7]=y[5];
	}
    
    RHO *= 2.; //!!!! ПОЛУЧИЛИ 2*RHO(T)
    H = y[6]*y[6]*g/RHO + y[7]*g/2 - y[7]*y[7]*g/RHO + y[4]*y[2] + y[5]*y[3];//H(T)
    
    //fout << "\n\nRHO is equal to " << RHO << "\n\n";
    
    res[0]=y[0]-5; //0.0727930740001; //-0.07279307323(2)1;
	res[1]=y[1]-7; //-6.0309331715847536;//T=5//-8.8869461060126618; //T=2//y_T
	res[2]=y[6];
	res[3]=y[7];
	res[5]=H-1;
	
}

void gradf(double *beta, double *dbeta, double *res, double *y) {
	int i, j, N=5;
	double beta_p[N], beta_m[N], res_p[N], res_m[N], h=1.e-6;
	double **pro, **pro2;

	pro=(double**)malloc(N*sizeof(double*));
	pro2=(double**)malloc(N*sizeof(double*));
	for(i=0; i<N; i++) {
		pro[i]=(double*)malloc(N*sizeof(double));
		pro2[i]=(double*)malloc(N*sizeof(double));
	}

	for(j=0; j<N; j++) {
		for(i=0; i<N; i++) {
			beta_p[i]=beta[i];
			beta_m[i]=beta[i];
		}
		
		beta_p[j]+=h;
		beta_m[j]-=h;
		
		l(beta_p, res_p, y);
		l(beta_m, res_m, y);
		
		for(i=0; i<N; i++) pro[i][j]=(res_p[i]-res_m[i])/(2*h);
	}
	
	matrix_print(pro, N);
	
	for(i=0; i<N; i++) res_m[i]=-res[i];
	linear_sys(pro, dbeta, res_m, N);
	
	
	//printf("TEST!");
	
	//matrix_print(pro2, N);
}

int NEWTON(double *beta, double *y) {
	int i, j, N=5;
	double res[N], res_w[N], beta_w[N], dbeta[N], gamma;
	bool flag;
	
	l(beta, res, y);
	
	for(j=0; j<15; j++) {
		cout << "\n-------\nj = " << j << "\n-------\n";
	
		if(fabs(res[0])<ens && fabs(res[1])<ens && fabs(res[2])<ens && fabs(res[3])<ens) {cout << endl <<"Ended by " << j << " iteration" << endl; return j;}
		gradf(beta,dbeta,res,y);
	
		gamma=1.0;
		flag=false;
		while(gamma>eps_gamma) {
			for(i=0; i<N; i++) 
				{
					beta_w[i]=beta[i]+gamma*dbeta[i];
					//printf("beta_w: %.16f\n",beta_w[i]);
				}
			l(beta_w, res_w, y);
			
			if(norm(res_w,N)<norm(res,N)) {flag=true; break;}
			gamma/=2.0;
		}
		if(flag==false) {cout << endl << "Broken on " << j << "'s iteration" << endl; return -1;}
		for(i=0; i<N; i++) {
			beta[i]=beta_w[i];
			res[i]=res_w[i];
		}
	}
cout << endl << "not enough iteration" << endl;
return -2;
}

int main() {
	double beta[5] = {1, 1, 1, 1, 1};
	double y[8];
	
	NEWTON(beta,y);

	cout << "\nT = " << beta[4];
	//l(beta,res,y);

return 0;
}