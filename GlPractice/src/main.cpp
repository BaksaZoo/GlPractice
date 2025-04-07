#include "practice.h"
#include "hello_triangle_excercise.h"
#include "uniforms.h"

int main() {
	int programFlag = 2;

	if (programFlag == 0)
	{
		practice::main();
	}
	else if (programFlag == 1)
	{
		hello_triangle::main();
	}
	else if (programFlag == 2)
	{
		uniforms::main();
	}

	return 0;
}