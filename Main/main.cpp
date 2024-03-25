#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include <graphics_engine.h>

int main()
{
	shiny::GraphicsEngine app;

	try
	{
		app.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}