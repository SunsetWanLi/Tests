#pragma once
#undef max
#undef min
class MathUtils
{
public:
	static const double PI;
	static int abs(int i);
	static double abs(double d);
	static int max(int a,int b);
	static int min(int a,int b);
public:
	MathUtils(void);
	~MathUtils(void);
};

