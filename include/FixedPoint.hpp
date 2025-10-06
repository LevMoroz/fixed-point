#pragma once

#include <cstdint>

enum class RoundType { toZero = 0, toNearestEven = 1, toPosInf = 2, toNegInf = 3 };

class FixedPoint
{
	int wholePart;
	int fractPart;
	RoundType roundType;
	uint32_t num;

private:
	void roundRes(uint64_t r, uint32_t roundRange, int32_t sgn);

public:
	FixedPoint(int wp, int fp, int rt, uint32_t n);

	void print() const;

	void operator+=(const FixedPoint& other);

	void operator-=(const FixedPoint& other);

	void operator*=(const FixedPoint& other);

	void operator/=(const FixedPoint& other);
};
