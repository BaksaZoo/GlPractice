#include "practice.h"
#include "hello_triangle_excercise.h"
#include "uniforms.h"
#include "shader_data.h"

int main() {
	int programFlag = 3;

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
	else if (programFlag == 3)
	{
		shader_data::main();
	}

	return 0;
}