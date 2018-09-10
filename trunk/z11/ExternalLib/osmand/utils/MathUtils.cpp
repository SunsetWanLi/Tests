#include "MathUtils.h"

const double MathUtils::PI=3.141592653589793;
MathUtils::MathUtils(void)
{
}


MathUtils::~MathUtils(void)
{
}

int MathUtils::abs( int i )
{
	if (i<0)
	{
		i=-i;
	}
	return i;
}

double MathUtils::abs( double d )
{
	if (d<0)
	{
		d=-d;
	}
	return d;
}

int MathUtils::max( int a,int b )
{
	return a>b?a:b;
}

int MathUtils::min( int a,int b )
{
	return a<b?a:b;
}