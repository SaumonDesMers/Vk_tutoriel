#include "application.hpp"
#include "logger.hpp"

int main(void)
{
	log.setTimestamp(false);
	try
	{
		Application app;

		app.run();
	}
	catch(const std::exception& e)
	{
		FT_CRITICAL(e.what());
	}
	
	return 0;
}
