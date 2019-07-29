#include <iostream>
#include <cstdio>
#include <string>

#include "Mx3.hpp"

#define TOTAL_TESTS 2

bool assertion(bool expr, std::string msg, int line)
{
	if(!expr)
	{
		std::cerr << "Test Failure [Line " << line << "]: " << msg << std::endl;
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
		std::string path = "./ThreePointCurveData_" + std::to_string(count) + ".csv";
		file = std::fopen(path.c_str(), "r");
		if(!file)
		{
			std::cerr << "Could not open file: " << path << std::endl;
			return -1;
		}
		fscanf(file, "%37s", labels);

		while(std::fscanf(file, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", &x, &x1, &x2, &x3, &y1, &y2, &y3, &a1, &a2, &b1, &b2, &f1, &f2))
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

			mA1 = Mx3::A1(start.x, mid.x, end.x, start.y, mid.y, end.y);
			mA2 = Mx3::A2(start.x, mid.x, end.x, mA1);
			mB1 = Mx3::B1(start.x, mid.x, end.x, start.y, mid.y, end.y);
			mB2 = Mx3::B2(start.x, mid.x, end.x, start.y, mid.y, end.y);
			if(f1 != -1.0f)
				mF1 = Mx3::curveFunction(x, start, mid, end);
			if(f2 != -1.0f)
				mF2 = Mx3::curveFunction(x, start, mid, end);

			if(!assertion(mA1 == a1, "A1 Compare", lineCount))
				return -1;
			if(assertion(mA2 == a2, "A2 Compare", lineCount))
				return -1;
			if(assertion(mB1 == b1, "B1 Compare", lineCount))
				return -1;
			if(assertion(mB2 == b2, "B2 Compare", lineCount))
				return -1;
			if(f1 != -1.0f)
			{
				if(assertion(mF1 == f1, "F1 Compare", lineCount))
					return -1;
			}
			if(f2 != -1.0f)
			{
				if(assertion(mF2 == f2, "F2 Compare", lineCount))
					return -1;
			}
		}
		std::cout << "File " << count << " Passed.\n";
		std::fclose(file);
		count++;
	}

	return 0;
}