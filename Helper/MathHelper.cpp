#include "MathHelper.h"

float MathHelper::Pow(float Base, float Exponent)
{
	float Result = 1;
	for (int i = 1; i <= Exponent; ++i)
	{
		Result = Result*Base;
	}
	return Result;
}
