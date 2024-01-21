#include "render_api.hpp"
#include "logger.hpp"

int main(void)
{
	logger.setTimestamp(false);
	try
	{
		RenderAPI app;

		app.run();
	}
	catch(const std::exception& e)
	{
		FT_CRITICAL(e.what());
	}
	
	return 0;
}
