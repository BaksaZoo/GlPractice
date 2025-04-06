#include "practice.h"
#include "hello_triangle_excercise.h"

int main() {
	int programFlag = 1;

	if (programFlag == 0)
	{
		practice::main();
	} else if (programFlag == 1)
	{
		hello_triangle::main();
	}

	return 0;
}