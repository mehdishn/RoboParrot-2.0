#include "utility_functions.h"

 bool is_even(int num)
{
    if(num > -1)
        if(num%2 == 0)
            return true;
    return false;
}