#include <iostream>
#include "GridHash.h"

int main()
{
	std::cout << GridHash<int>::CoordinateHash()(GridHash<int>::Coordinate(1,1,1)) << "\n";
	return 0;
}
