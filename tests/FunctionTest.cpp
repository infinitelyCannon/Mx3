#include <iostream>
#include <cstdio>
#include <string>

#include "Mx3.hpp"

#define TOTAL_TESTS 2

float A1(float x1, float x2, float x3, float y1, float y2, float y3)
{
	return ((x1 * (y2 - y3)) + (x2 * (y3 - y1)) + (x3 * (y1 - y2))) /
		((2.0f * std::pow(x1 - x2, 2.0f)) * (x1 - x3) * (x2 - x3));
}

float A2(float x1, float x2, float x3, float a1)
{
	return (x2 - x1) / (x2 - x3) * a1;
}

float B1(float x1, float x2, float x3, float y1, float y2, float y3)
{
	return (std::pow(x1, 2.0f) * (y3 - y2) - x1 * (x2 * (-3.0f * y1 + y2 + 2.0f * y3) + 3.0f * x3 * (y1 - y2)) + std::pow(x2, 2.0f) * (y3 - y1) + x2 * x3 * (y2 - y1) + 2.0f * std::pow(x3, 2.0f) * (y1 - y2))
		/ (2.0f * (x1 - x2) * (x1 - x3) * (x2 - x3));
}

float B2(float x1, float x2, float x3, float y1, float y2, float y3)
{
	return (2.0f * std::pow(x1, 2.0f) * (y2 - y3) + x2 * (x1 * (y3 - y2) + x3 * (2.0f * y1 + y2 - 3.0f * y3)) + 3.0f * x1 * x3 * (y3 - y2) + std::pow(x2, 2.0f) * (y3 - y1) + std::pow(x3, 2.0f) * (y2 - y1))
		/ (2.0f * (x1 - x2) * (x1 - x3) * (x2 - x3));
}

float curveFunction(float x, Point start, Point mid, Point end)
{
	if(x >= start.x && x <= mid.x)
	{
		float a1 = A1(start.x, mid.x, end.x, start.y, mid.y, end.y);
		float b1 = B1(start.x, mid.x, end.x, start.y, mid.y, end.y);

		return (a1 * std::pow(x - start.x, 3.0f)) + (b1 * (x - start.x)) + start.y;
	}
	else if(x >= mid.x && x <= end.x)
	{
		float a1 = A1(start.x, mid.x, end.x, start.y, mid.y, end.y);
		float a2 = A2(start.x, mid.x, end.x, a1);
		float b2 = B2(start.x, mid.x, end.x, start.y, mid.y, end.y);

		return (a2 * std::pow(x - end.x, 3.0f)) + (b2 * (x - end.x)) + end.y;
	}
}

bool approxEqual(float a, float b, float absEpsilon, float relEpsilon)
{
	float diff = std::fabs(a - b);
	if(diff <= absEpsilon)
		return true;

	return diff <= ((std::fabs(a) < std::fabs(b) ? std::fabs(b) : std::fabs(a)) * relEpsilon);
}

bool assertion(float a, float b, std::string msg, int line)
{
	if(!approxEqual(a, b, 1e-12, 1e-4))
	{
		std::cerr << "Test Failure [Line " << line << "]: " << msg << std::endl
			<< a << " != " << b << std::endl;
		return false;
	}

	return true;
}

int main(int argc, char **argv)
{
	float x, x1, x2, x3, y1, y2, y3, a1, a2, b1, b2, f1, f2;
	int count = 1;
	int lineCount = 2;
	std::FILE *file;
	char labels[39];

	while(count <= TOTAL_TESTS)
	{
		std::string path = "../../tests/data/ThreePointCurveData_" + std::to_string(count) + ".csv";
		file = std::fopen(path.c_str(), "r");
		if(!file)
		{
			std::cerr << "Could not open file: " << path << std::endl;
			return -1;
		}
		fscanf(file, "%37s", labels);

		while(EOF != std::fscanf(file, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", &x, &x1, &x2, &x3, &y1, &y2, &y3, &a1, &a2, &b1, &b2, &f1, &f2))
		{
			lineCount++;
			float mA1, mA2, mB1, mB2, mF1, mF2;
			Point start, mid, end;

			start.x = x1;
			start.y = y1;
			mid.x = x2;
			mid.y = y2;
			end.x = x3;
			end.y = y3;

			mA1 = A1(start.x, mid.x, end.x, start.y, mid.y, end.y);
			mA2 = A2(start.x, mid.x, end.x, mA1);
			mB1 = B1(start.x, mid.x, end.x, start.y, mid.y, end.y);
			mB2 = B2(start.x, mid.x, end.x, start.y, mid.y, end.y);
			if(f1 != -1.0f)
				mF1 = curveFunction(x, start, mid, end);
			if(f2 != -1.0f)
				mF2 = curveFunction(x, start, mid, end);

			if(!assertion(a1, mA1, "A1 Compare", lineCount))
				return -1;
			if(!assertion(a2, mA2, "A2 Compare", lineCount))
				return -1;
			if(!assertion(b1, mB1, "B1 Compare", lineCount))
				return -1;
			if(!assertion(b2, mB2, "B2 Compare", lineCount))
				return -1;
			if(f1 != -1.0f)
			{
				if(!assertion(f1, mF1, "F1 Compare", lineCount))
					return -1;
			}
			if(f2 != -1.0f)
			{
				if(!assertion(f2, mF2, "F2 Compare", lineCount))
					return -1;
			}
		}
		std::cout << "File " << count << " Passed.\n";
		std::fclose(file);
		count++;
	}

	return 0;
}