
#include <cmath>
#include "Fraction.h"

namespace GIS_Embedded
{
	int gcd(int a, int b)
	{
		a = abs(a);
		b = abs(b);
		if(0 == a){
			return b;
		}
		if(0 == b){
			return a;
		}
		if(a > b){
			swap(a, b);
		}
		int c;
		for(c = a % b ; c > 0 ; c = a % b){
			a = b;
			b = c;
		}
		return b;
	}

	ostream& operator<<(ostream& out, Fraction& f)
	{
		f.reduct();
		out << f.numerator << "/" << f.denominator;
		return out;
	}

	istream& operator>>(istream& in, Fraction& f)
	{
		in >> f.numerator >> f.denominator;
		return in;
	}

	bool operator==(const Fraction& _lf, const Fraction& _rf)
	{
		Fraction lf(_lf), rf(_rf);
		lf.reduct();
		rf.reduct();
		return (lf.denominator==rf.denominator && lf.numerator==rf.numerator);
	}

	bool operator!=(const Fraction& lf, const Fraction& rf)
	{
		return !(lf == rf);
	}

	bool operator<(const Fraction& _lf, const Fraction& _rf)
	{
		Fraction lf(_lf), rf(_rf);

		lf.reduct();
		rf.reduct();
		int ld = lf.denominator;     //左操作数的分母
		int rd = rf.denominator;     //右操作数的分母
		int cd = gcd(ld, rd);        //两个操作数分母的最大公因子
		int ll = rd / cd;            //左操作数分子需要扩大的倍数
		int rl = ld / cd;            //右操作数分子需要扩大的倍数
		return (lf.numerator*ll < rf.numerator*rl);
		//此函数的过程就是将两个分数假通分并比较分子
	}

	bool operator>(const Fraction& _lf, const Fraction& _rf)
	{
		Fraction lf(_lf), rf(_rf);

		lf.reduct();
		rf.reduct();
		int ld = lf.denominator;    
		int rd = rf.denominator;    
		int cd = gcd(ld, rd);       
		int ll = rd / cd;          
		int rl = ld / cd;           
		return (lf.numerator*ll > rf.numerator*rl);
	}

	bool operator<=(const Fraction& _lf, const Fraction& _rf)
	{
		Fraction lf(_lf), rf(_rf);

		lf.reduct();
		rf.reduct();
		int ld = lf.denominator;     //左操作数的分母
		int rd = rf.denominator;     //右操作数的分母
		int cd = gcd(ld, rd);        //两个操作数分母的最大公因子
		int ll = rd / cd;            //左操作数分子需要扩大的倍数
		int rl = ld / cd;            //右操作数分子需要扩大的倍数
		return (lf.numerator*ll <= rf.numerator*rl);
		//此函数的过程就是将两个分数假通分并比较分子
	}

	bool operator>=(const Fraction& _lf, const Fraction& _rf)
	{
		Fraction lf(_lf), rf(_rf);

		lf.reduct();
		rf.reduct();
		int ld = lf.denominator;    
		int rd = rf.denominator;    
		int cd = gcd(ld, rd);       
		int ll = rd / cd;          
		int rl = ld / cd;           
		return (lf.numerator*ll >= rf.numerator*rl);
	}

	Fraction operator+(const Fraction& lo, const Fraction& ro)
	{
		Fraction result;
		int ld = lo.denominator;    
		int rd = ro.denominator;    
		int cd = gcd(ld, rd);       
		int ll = rd / cd;          
		int rl = ld / cd;
		result.denominator = ld * ll;
		result.numerator = lo.numerator * ll + ro.numerator * rl;
		result.reduct();
		return result;
	}

	Fraction operator-(const Fraction& lo, const Fraction& ro)
	{
		Fraction result;
		int ld = lo.denominator;    
		int rd = ro.denominator;    
		int cd = gcd(ld, rd);       
		int ll = rd / cd;          
		int rl = ld / cd;
		result.denominator = ld * ll;
		result.numerator = lo.numerator * ll - ro.numerator * rl;
		result.reduct();
		return result;
	}

	Fraction operator*(const Fraction& lo, const Fraction& ro)
	{
		Fraction result;
		result.denominator = lo.denominator * ro.denominator;
		result.numerator = lo.numerator * ro.numerator;
		result.reduct();
		return result;
	}

	Fraction operator/(const Fraction& lo, const Fraction& ro)
	{
		Fraction result;
		result = lo;
		result.reciprocal();
		result *= ro;
		result.reciprocal();
		return result;
	}
}