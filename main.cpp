#include "FixedPoint.hpp"

#include <iostream>
#include <string>
#include <stdexcept>

using std::cerr;
using std::endl;
using std::string;
using std::stoi;
using std::stoul;
using std::exception;
using std::invalid_argument;


int main(int argc, char** argv)
{
	try
	{
		if (argc != 4 && argc != 6)
			throw invalid_argument("Wrong format!");

		string fmt = argv[1];
		auto dot = fmt.find('.');
		if (dot == fmt.npos)
			throw invalid_argument("Wrong A.B format!");

		int wholePart = stoi(fmt.substr(0, dot));
		int fractPart = stoi(fmt.substr(dot + 1));

		int roundType = stoi(argv[2]);
		uint32_t num = stoul(argv[3], nullptr, 16);

		FixedPoint p(wholePart, fractPart, roundType, num);

		if (argc == 6)
		{
			char op = argv[4][0];
			uint32_t num2 = stoul(argv[5], nullptr, 16);

			FixedPoint p2(wholePart, fractPart, roundType, num2);

			switch (op)
			{
			case '+':
				p += p2;
				break;
			case '-':
				p -= p2;
				break;
			case '*':
				p *= p2;
				break;
			case '/':
				p /= p2;
				break;
			default:
				throw invalid_argument("Wrong operation!");
			}
		}

		p.print();
	}
	catch (const exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}

	return 0;
}
