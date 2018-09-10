//////////////////////////////////////////////////////////////////////////
///算法：牛顿下山法。
//////////////////////////////////////////////////////////////////////////
#ifndef __CRT_MATH_H__
#define __CRT_MATH_H__

#include<iostream>
#include<cmath>

//using namespace std;
namespace GIS_Embedded 
{

	class CCRTMath
	{
	public:
		CCRTMath();
		virtual ~CCRTMath();

	public:
		//牛顿法求根
		//如果没有求出根或者出错返回false；
		//y为输入值，
		//init_x 为初始的x值，一般为min_x, 
		//min_x和max_x 为-1.0+1e-04和1.0-1e-04, 
		//*output为输出的根
		static bool newtonCalRoot(double e, double y, double init_x, double min_x, 
				   double max_x, double *output);
	protected:
		// 函数为:
		// f(x) = ((1+e*x)/(1-e*x))^e * (1-x)/(1+x) - y;
		// 计算f(x)函数值
		static double calFunValue(double e, double x, double y);
		//f '(x) =  e * [((1+e*x)/(1-e*x))^(e-1)] * [2*e/((1-e*x)^2)] * [(1-x)/(1+x)]
		//		 + [((1+e*x)/(1-e*x))^e] * [-2/((1+x)^2)];
		// 计算f(x)一导数的值：为了利用已经计算出来的函数值，公式已经化解过
		static double calDFunValue(double e, double x, double y, double fun_val);
	};

//
////test...
//void main()
//{
//	double e, input_y;
//	double min_x, max_x;
//	double output_x;
//
//	min_x = -1.0 + 1e-04;
//	max_x = 1.0 - 1e-04;
//
//	while(cin >> e >> input_y)
//	{
//		if(e <= 0.0 || e >= 1.0) break;
//		
//		if(!newtonCalRoot(e, input_y, min_x, min_x, max_x, &output_x))
//			break;
//
//		cout.setf(ios::fixed);
//		cout.precision(10);
//		cout << output_x << endl;
//	}
//}
}

#endif// __CRT_MATH_H__
