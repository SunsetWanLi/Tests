#ifndef _FRACTION_H_
#define _FRACTION_H_

#include <iostream>
#include <stdlib.h>
#include <assert.h>

using namespace std;

namespace GIS_Embedded
{
class Fraction
{
	friend istream& operator>>(istream&, Fraction&);
	friend ostream& operator<<(ostream&, Fraction&);
	friend bool operator==(const Fraction& lf, const Fraction& rf);
	friend bool operator!=(const Fraction& lf, const Fraction& rf);
	friend bool operator<(const Fraction& lf, const Fraction& rf);
	friend bool operator>(const Fraction& lf, const Fraction& rf);
	friend bool operator<=(const Fraction& lf, const Fraction& rf);
	friend bool operator>=(const Fraction& lf, const Fraction& rf);

public:
	int numerator; //分子
	int denominator;   //分母
	Fraction(){
		numerator = 0;
		denominator = 1;
	}
	Fraction(int a, int b){
		assert(b);
		if(b < 0)
		{
			numerator = - a;
			denominator = - b;
		}
		else
		{
			numerator = a;
			denominator = b;
		}
	}
	Fraction(const Fraction& f){
		numerator = f.numerator;
		denominator = f.denominator;
	}
	~Fraction(){}
	
	inline void reduct();	//约简

	inline void reciprocal();	//分子分母互换

	inline int getIntValue() const;

	inline double getDoubleValue() const;//lw

	inline Fraction getDecimalFraction() const;

	inline Fraction& operator+=(const Fraction&);
	inline Fraction& operator-=(const Fraction&);
	inline Fraction& operator*=(const Fraction&);
	inline Fraction& operator/=(const Fraction&);
	inline Fraction& operator=(const Fraction&);
};

Fraction operator+(const Fraction&,const  Fraction&);
Fraction operator-(const Fraction&, const Fraction&);
Fraction operator*(const Fraction&, const Fraction&);
Fraction operator/(const Fraction&, const Fraction&);

//inline void swap(int &a, int &b)
//{
//	int c = a;
//	a = b;
//	b = c;
//}

int gcd(int a, int b);

inline void Fraction::reduct()
{
	int a = denominator;
	int b = numerator;
	int c = gcd(a, b);
	a /= c;
	b /= c;
	denominator = a;
	numerator = b;
}

inline void Fraction::reciprocal()
{
	assert(numerator);
	int temp = denominator;
	denominator = numerator;
	numerator = temp;

	if(denominator < 0)
	{
		numerator = - numerator;
		denominator = - denominator;
	}
}

inline int Fraction::getIntValue() const
{
	return numerator / denominator;
}

inline double Fraction::getDoubleValue() const
{
	return numerator / denominator;
}

//zj 2010-1-10 返回小数的小数部分
inline Fraction Fraction::getDecimalFraction() const
{
	return Fraction(numerator % denominator, denominator);
}

inline Fraction& Fraction::operator+=(const Fraction& f)
{
	Fraction tmp(f);
	reduct();
	tmp.reduct();
	int ld = denominator;    
	int rd = tmp.denominator;    
	int cd = gcd(ld, rd);       
	int ll = rd / cd;          
	int rl = ld / cd;
	denominator *= ll;
	numerator = numerator * ll + tmp.numerator * rl;
	reduct();
	return *this;
}

inline Fraction& Fraction::operator-=(const Fraction& f)
{
	Fraction tmp(f);
	reduct();
	tmp.reduct();
	int ld = denominator;    
	int rd = tmp.denominator;    
	int cd = gcd(ld, rd);       
	int ll = rd / cd;          
	int rl = ld / cd;
	denominator *= ll;
	numerator = numerator * ll - tmp.numerator * rl;
	reduct();
	return *this;
}

inline Fraction& Fraction::operator*=(const Fraction& f)
{
	denominator *= f.denominator;
	numerator *= f.numerator;
	reduct();
	return *this;
}

inline Fraction& Fraction::operator/=(const Fraction& f)
{
	reciprocal();
	*this *= f;
	reciprocal();
	return *this;
}

inline Fraction& Fraction::operator=(const Fraction& f)
{
	denominator = f.denominator;
	numerator = f.numerator;
	return *this;
}

Fraction operator+(const Fraction& lo, const Fraction& ro);

Fraction operator-(const Fraction& lo, const Fraction& ro);

Fraction operator*(const Fraction& lo, const Fraction& ro);

Fraction operator/(const Fraction& lo, const Fraction& ro);

}

#endif