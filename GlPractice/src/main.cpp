#include "practice.h"
#include "hello_triangle_excercise.h"
#include "uniforms.h"
#include "shader_data.h"
#include "texture.h"

int main() {
	int programFlag = 4;

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
	else if (programFlag == 4)
	{
		texture::main();
	}

	return 0;
}