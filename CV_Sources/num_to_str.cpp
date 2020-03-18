#include "num_to_str.h"

string convert_number_to_string (int number)
{
	char numstr[21]; // enough to hold all numbers up to 64-bits
	return string(itoa(number, numstr, 10));
}