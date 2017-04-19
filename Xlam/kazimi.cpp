#include <iostream>
#include <cmath>
using namespace std;

void fcn(double x, double *y,double *f) {
    f[0]=y[1];
	f[1]=-x*y[0]*y[2];
	f[2]=0;
}

double dsign(double a, double b) {
	if (b<0) { a=fabs(a)*(-1.0); return a; }
	if (b>0) return fabs(a);
	return 0.0;
}

//Метод Рунге-Кутты 5-ого порядка с автоматическим выбором шага
void ddopri5(int n,void (*fcn)(double, double*,double*),double x,double *y,double xend, double eps, double hmax,double h) {	
    double k1[11],k2[11],k3[11],k4[11],k5[11],y1[11];
    bool reject;
    double xph,err,denom,fac,hnew,posneg;
    int nmax=30000,i;
    double uround=2.2205e-16;
    posneg=dsign(1.e0,xend-x);
    hmax=fabs(hmax);
    h=min(max(1.e-4,fabs(h)),hmax);
    h=dsign(h,posneg);
    reject=false;
    int naccpt=0;
    int nrejct=0;
    int nfcn=1;
    int nstep=0;
    
    fcn(x,y,k1);
    while (1) {
		if ( nstep > nmax ) break;
		if ( (x-xend)*posneg+uround > 0.e0) break;
		if ( (x+h-xend)*posneg > 0.e0) h=xend-x;
		nstep++;
		for (i=0; i<n; i++) y1[i]=y[i]+h*.2e0*k1[i];
		fcn(x+h*.2e0,y1,k2);
		for (i=0; i<n; i++) y1[i]=y[i]+h*((3.e0/40.e0)*k1[i]+(9.e0/40.e0)*k2[i]);
		fcn(x+h*.3e0,y1,k3);
		for (i=0; i<n; i++) y1[i]=y[i]+h*((44.e0/45.e0)*k1[i]-(56.e0/15.e0)*k2[i]+(32.e0/9.e0)*k3[i]);
		fcn(x+h*.8e0,y1,k4);
		for (i=0; i<n; i++) y1[i]=y[i]+h*((19372.e0/6561.e0)*k1[i]-(25360.e0/2187.e0)*k2[i]+(64448.e0/6561.e0)*k3[i]-(212.e0/729.e0)*k4[i]);
		fcn(x+h*(8.e0/9.e0),y1,k5);
		for (i=0; i<n; i++) y1[i]=y[i]+h*((9017.e0/3168.e0)*k1[i]-(355.e0/33.e0)*k2[i]+(46732.e0/5247.e0)*k3[i]+(49.e0/176.e0)*k4[i]-(5103.e0/18656.e0)*k5[i]);
		xph=x+h;
		fcn(xph,y1,k2);
		for (i=0; i<n; i++) y1[i]=y[i]+h*((35.e0/384.e0)*k1[i]+(500.e0/1113.e0)*k3[i]+(125.e0/192.e0)*k4[i]-(2187.e0/6784.e0)*k5[i]+(11.e0/84.e0)*k2[i]);
		for (i=0; i<n; i++) k2[i]=(71.e0/57600.e0)*k1[i]-(71.e0/16695.e0)*k3[i]+(71.e0/1920.e0)*k4[i]-(17253.e0/339200.e0)*k5[i]+(22.e0/525.e0)*k2[i];
		fcn(xph,y1,k3);
		for (i=0; i<n; i++) k4[i]=(k2[i]-(1.e0/40.e0)*k3[i])*h;
		nfcn+=6;
		err=0;
		for (i=0; i<n; i++) {
			denom=max(1.e-5,max( fabs(y1[i]) , max(fabs(y[i]),2.e0*uround/eps)));
			err+=pow(k4[i]/denom,2);
		}
		err=sqrt(err/double(n));
		fac=max( .1e0, min( 5.e0, pow( err/eps,0.2e0 )/.9e0) );
		hnew=h/fac;
		if(err <= eps) {
			naccpt++;
			for (i=0; i<n; i++) {
				k1[i]=k3[i];
				y[i]=y1[i];
			}
			x=xph;
			if(fabs(hnew)>hmax) hnew=posneg*hmax;
			if(reject) hnew=posneg*min(fabs(hnew),fabs(h)),reject=false;
			else reject=true;
			if(naccpt >= 1) nrejct++;
		}
		h=hnew;
		//cout << x << "\t\t" << y[0] << "\t" << y[1] << "\t" << y[2] << "\t" << endl;
		if(x<=h || fabs(x-0.5)<=h || (x>=1 && x<=1)) {
			//cout << x << "\t\t" << y[0] << "\t" << y[1] << "\t" << y[2] << endl;
			printf("%.16f: %.16f %.16f %.16f\n", x, y[0], y[1], y[2]);
		}
    }
    //cout << endl << "-------------------------" << endl << "" << "local error: " << err << endl << "-------------------------" << endl;
    //cout << "Global error: "<< eps*(1/nstep) << endl;
}

//Поиск минимальной лямбды
double lambdaSearch(double eps, double *y, void (*ddopri5)(int, void (*fcn)(double, double*, double*), double, double*, double, double, double, double)) {
	double hl;
	hl = 1.0;
		
	while(fabs(y[0])>1.e-10) {
		if(y[0]>0) y[2]+=hl;
		else if(y[0]<0) {hl = hl/2; y[2]-=hl;}
		
		y[0]=0;
		y[1]=1;
		
		ddopri5(3,fcn,0,y,1.0e0,eps,1.0e0,0.5e0);
		cout << endl << "<------------------------------>" << endl;
	}
	
	return y[2];
}


int main()
{
	double lambda;
	double y[3];
	double y1, y2, y3, eps1, eps2, eps3;
	
	lambda = 0.0;
	eps1=1e-7;
	eps2=1e-9;
	eps3 = 1e-11;

	
	//Для eps3  --------------------------------
	y[0]=0;
	y[1]=1;
	y[2]=lambda;
	
	ddopri5(3,fcn,0,y,1.0e0,eps3,1.0e0,0.5e0);
	lambda = lambdaSearch(eps3, y, ddopri5);
	printf("\nMinimalnaya lambda: %.40f\n", lambda);
	//============================================
	
	//В точке t = 0.0200000000000000
	y[0]=0;
	y[1]=1;
	y[2]=lambda;
	ddopri5(3,fcn,0,y,0.026,eps1,1.0e0,0.5e0);
	//cout << endl << "eps = 1e-7 : \t" << y[0] << "\t" << y[1] << "\t" << y[2] << "\t" << endl;
	y1 = y[0];
	y[0]=0;
	y[1]=1;
	y[2]=lambda;
	ddopri5(3,fcn,0,y,0.026,eps2,1.0e0,0.5e0);
	//cout << endl << "eps = 1e-9 : \t" << y[0] << "\t" << y[1] << "\t" << y[2] << "\t" << endl;
	y2 = y[0];
	y[0]=0;
	y[1]=1;
	y[2]=lambda;
	ddopri5(3,fcn,0,y,0.026,eps3,1.0e0,0.5e0);
	//cout << endl << "eps = 1e-11 : \t" << y[0] << "\t" << y[1] << "\t" << y[2] << "\t" << endl;
	y3 = y[0];
	cout << "lambda: " << lambda << endl;
	cout << "**********************" << endl;
	cout << "eps = 1e-7 \t : \t y1 = " << y1 << endl;
	cout << "eps = 1e-9 \t : \t y2 = " << y2 << endl;
	cout << "eps = 1e-11 \t : \t y3 = " << y3 << endl;
	cout << "**********************" << endl;
	printf("t = 0.026\ny1 - y2 = %.16f\ny2 - y3 = %.16f\nGlobal error: %.16f\n",y1-y2, y2-y3, (y1-y2)/(y2-y3));
	//cout << "t = 0.02\n" << "y1 - y2 = " << y1-y2 << "\ny2 - y3 = " << y2-y3 << "\nGlobal error: " << (y1-y2)/(y2-y3) << endl;
	cout << "============================================================" << endl << endl;
	
	
	//В точке t = 0.5
	ddopri5(3,fcn,0,y,0.5000128147791552,eps1,1.0e0,0.5e0);
	//cout << endl << "eps = 1e-7 : \t" << y[0] << "\t" << y[1] << "\t" << y[2] << "\t" << endl;
	y1 = y[0];
	y[0]=0;
	y[1]=1;
	y[2]=lambda;
	ddopri5(3,fcn,0,y,0.5000128147791552,eps2,1.0e0,0.5e0);
	//cout << endl << "eps = 1e-9 : \t" << y[0] << "\t" << y[1] << "\t" << y[2] << "\t" << endl;
	y2 = y[0];
	y[0]=0;
	y[1]=1;
	y[2]=lambda;
	ddopri5(3,fcn,0,y,0.5000128147791552,eps3,1.0e0,0.5e0);
	//cout << endl << "eps = 1e-11 : \t" << y[0] << "\t" << y[1] << "\t" << y[2] << "\t" << endl;
	y3 = y[0];
	y[0]=0;
	y[1]=1;
	y[2]=lambda;
	cout << "lambda: " << lambda << endl;
	cout << "**********************" << endl;
	cout << "eps = 1e-7 \t : \t y1 = " << y1 << endl;
	cout << "eps = 1e-9 \t : \t y2 = " << y2 << endl;
	cout << "eps = 1e-11 \t : \t y3 = " << y3 << endl;
	cout << "**********************" << endl;
	printf("t = 0.5000128147791552\ny1 - y2 = %.16f\ny2 - y3 = %.16f\nGlobal error: %.16f\n",y1-y2, y2-y3, (y1-y2)/(y2-y3));
	//cout << "t = 0.5\n" << "y1 - y2 = " << y1-y2 << "\ny2 - y3 = " << y2-y3 << "\nGlobal error: " << (y1-y2)/(y2-y3) << endl;
	cout << "============================================================" << endl << endl;
	
	
	//В точке t = 1
	ddopri5(3,fcn,0,y,1.0e0,eps1,1.0e0,0.5e0);
	//cout << endl << "eps = 1e-7 : \t" << y[0] << "\t" << y[1] << "\t" << y[2] << "\t" << endl;
	y1 = y[0];
	y[0]=0;
	y[1]=1;
	y[2]=lambda;
	ddopri5(3,fcn,0,y,1.0e0,eps2,1.0e0,0.5e0);
	//cout << endl << "eps = 1e-9 : \t" << y[0] << "\t" << y[1] << "\t" << y[2] << "\t" << endl;
	y2 = y[0];
	y[0]=0;
	y[1]=1;
	y[2]=lambda;
	ddopri5(3,fcn,0,y,1.0e0,eps3,1.0e0,0.5e0);
	//cout << endl << "eps = 1e-11 : \t" << y[0] << "\t" << y[1] << "\t" << y[2] << "\t" << endl;
	y3 = y[0];
	y[0]=0;
	y[1]=1;
	y[2]=lambda;
	cout << "lambda: " << lambda << endl;
	cout << "**********************" << endl;
	cout << "eps = 1e-7 \t : \t y1 = " << y1 << endl;
	cout << "eps = 1e-9 \t : \t y2 = " << y2 << endl;
	cout << "eps = 1e-11 \t : \t y3 = " << y3 << endl;
	cout << "**********************" << endl;
	printf("t = 1\ny1 - y2 = %.16f\ny2 - y3 = %.16f\nGlobal error: %.16f\n",y1-y2, y2-y3, (y1-y2)/(y2-y3));
	//cout << "t = 1\n" << "y1 - y2 = " << y1-y2 << "\ny2 - y3 = " << y2-y3 << "\nGlobal error: " << (y1-y2)/(y2-y3) << endl;
	cout << "============================================================" << endl << endl;

	return 0;
}

