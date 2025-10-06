#include "FixedPoint.hpp"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <stdexcept>

using std::cout;
using std::endl;
using std::setw;
using std::setfill;
using std::invalid_argument;
using std::llabs;


inline static int64_t deg2(uint32_t s)
{
	return 1LL << s;
}

inline static uint64_t modOfDeg2(uint64_t r, int64_t d)
{
	return r & (d - 1);
}

FixedPoint::FixedPoint(int wp, int fp, int rt, uint32_t n)
{
	if (wp + fp > 32 || wp < 1 || fp < 0)
		throw invalid_argument("Wrong A.B format!");

	if (rt > 3 || rt < 0)
		throw invalid_argument("Wrong round type!");

	wholePart = wp; fractPart = fp;
	roundType = (RoundType)rt;
	num = n << (32 - wholePart - fractPart);
}

void FixedPoint::print() const
{
	int64_t n = (int32_t)num;
	int64_t denom = deg2(32 - wholePart);
	int32_t whole = n / denom;

	int divisor = roundType == RoundType::toNearestEven ? 10000 : 1000;
	int32_t fract = llabs(n * divisor / denom % divisor);

	switch (roundType)
	{
	case RoundType::toZero:
		break;

	case RoundType::toNearestEven:
		if (fract % 10 > 5 || fract % 10 == 5 && ((fract / 10 & 1) == 1 || modOfDeg2(n * divisor, denom) != 0))
		{
			fract = fract / 10 + 1;
			if (fract == 1000)
			{
				whole = n < 0 ? --whole : ++whole;
				fract = 0;
			}
		}
		else
		{
			fract /= 10;
		}
		break;

	case RoundType::toPosInf:
		if (n > 0 && modOfDeg2(n * divisor, denom) != 0)
		{
			++fract;
			if (fract == 1000)
			{
				++whole;
				fract = 0;
			}
		}
		break;

	case RoundType::toNegInf:
		if (n < 0 && modOfDeg2(n * divisor, denom) != 0)
		{
			++fract;
			if (fract == 1000)
			{
				--whole;
				fract = 0;
			}
		}
		break;
	}

	cout << (n < 0 && whole == 0 && fract != 0 ? "-" : "") << whole << '.' << setw(3) << setfill('0') << fract << endl;
}

void FixedPoint::operator+=(const FixedPoint& other)
{
	num += other.num;
}

void FixedPoint::operator-=(const FixedPoint& other)
{
	num -= other.num;
}

void FixedPoint::roundRes(uint64_t r, uint32_t roundRange, int32_t sgn)
{
	int rnd = 0;
	switch (roundType)
	{
	case RoundType::toZero:
		break;

	case RoundType::toNearestEven:
		if (roundRange != 0 && (r & deg2(roundRange - 1)) != 0 &&
			((r & deg2(roundRange)) != 0 || modOfDeg2(r, deg2(roundRange - 1)) != 0))
		{
			rnd = 1;
		}
		break;

	case RoundType::toPosInf:
		if (sgn >= 0 && modOfDeg2(r, deg2(roundRange)) != 0)
		{
			rnd = 1;
		}
		break;

	case RoundType::toNegInf:
		if (sgn < 0 && modOfDeg2(r, deg2(roundRange)) != 0)
		{
			rnd = 1;
		}
		break;
	}

	r >>= roundRange;
	r += rnd;
	r <<= 32 - wholePart - fractPart;

	if (sgn < 0)
		r = ~r + 1;

	num = (uint32_t)r;
}

void FixedPoint::operator*=(const FixedPoint& other)
{
	uint64_t n1 = llabs((int32_t)num);
	uint64_t n2 = llabs((int32_t)other.num);

	uint64_t r = n1 * n2;
	r >>= 64 - 2 * (fractPart + wholePart);

	roundRes(r, fractPart, num ^ other.num);
}

void FixedPoint::operator/=(const FixedPoint& other)
{
	if (other.num == 0)
	{
		cout << "div_by_0" << endl;
		exit(0);
	}

	uint64_t n1 = llabs((int32_t)num);
	uint64_t n2 = llabs((int32_t)other.num);

	n1 <<= 32;

	uint64_t r = n1 / n2;

	roundRes(r, 32 - fractPart, num ^ other.num);
}
