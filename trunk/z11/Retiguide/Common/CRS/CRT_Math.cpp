
#include "CRT_Math.h"

namespace GIS_Embedded 
{

double CCRTMath::calFunValue(double e, double x, double y)
{
	double res;
	res = pow((1.0+e*x)/(1.0-e*x), e);
	res *= (1.0-x)/(1.0+x);
	res -= y;
	return res;
}
double CCRTMath::calDFunValue(double e, double x, double y, double fun_val)
{
	double res;
	res = (2.0*e*e-2) / (1-e*e*x*x) / (1-x*x) * (fun_val + y);
	return res;
}
bool CCRTMath::newtonCalRoot(double e, double y, double init_x, double min_x, double max_x, double *output)
{
	double x0, x1;
	double fx0,dfx0;
	double fx1,dfx1;
	double t, t_min;
	double x_eps, x_step;
	int count;
	const int MAX_NUM = 100;

	x_eps = 1e-06;
	x_step = (max_x - min_x)/16.0;
	t = 1.0;
	t_min = 1.0/64.0; 
	count = 0;

	x0 = init_x;
	fx0  = calFunValue(e, x0, y);
	dfx0 = calDFunValue(e, x0, y, fx0);
	
	if(fabs(fx0) < 1e-10)
	{
		*output = x0;
		return true;
	}

	if(fabs(dfx0) < 1e-10)
		return false;
	
	x1 = x0 - t * fx0 / dfx0;
	fx1  = calFunValue(e, x1, y);
	dfx1 = calDFunValue(e, x1, y, fx1);
	
	while(fabs(x1 - x0) >= x_eps)
	{
		if(fabs(fx1) < fabs(fx0))	
		{
			x0 = x1;
			t = 1.0;
			fx0 = fx1;
			dfx0 = dfx1;
			x1 = x0 - t * fx0 / dfx0;
			fx1  = calFunValue(e, x1, y);
			dfx1 = calDFunValue(e, x1, y, fx1);
		}
		else
		{
			t = t / 2;
			if(t > t_min)
			{
				x1 = x0 - t * fx0 / dfx0;
				fx1  = calFunValue(e, x1, y);
				dfx1 = calDFunValue(e, x1, y, fx1);
			}
			else
			{
				x0 = x1 + x_step;
				count++;
				
				if(count >= MAX_NUM)
					return false;
				else
				{
					t = 1.0;
					fx0  = calFunValue(e, x0, y);
					dfx0 = calDFunValue(e, x0, y, fx0);
					x1 = x0 - t * fx0 / dfx0;
					fx1  = calFunValue(e, x1, y);
					dfx1 = calDFunValue(e, x1, y, fx1);
				}
			}
		}
	}
	if(x1 < min_x)
	{
		*output = min_x;
		return false;
	}
	else if( x1 > max_x)
	{
		*output = max_x;
		return false;
	}

	*output = x1;
	return true;
}

}//GIS_Embedded
