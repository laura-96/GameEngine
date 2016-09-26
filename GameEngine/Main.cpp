#include <stdlib.h>
#include "Application.h"
#include "Globals.h"

#include "SDL/include/SDL.h"

#pragma comment( lib, "SDL/libx86/x86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/x86/SDL2main.lib" )


enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

int main(int argc, char* argv[])
{
	LOG("Starting '%s'...", TITLE);

	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;
	Application* App = NULL;

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_CREATION:
		{
			LOG("___Application Creation___");
			App = new Application();
			state = MAIN_START;

			break;
		}

		case MAIN_START:
		{
			LOG("___Application Init___");

			if (App->Init() == false)
			{
				LOG("Application inits with ERROR");
				state = MAIN_EXIT;
			}

			else
			{
				state = MAIN_UPDATE;
			}

			break;
		}

		case MAIN_UPDATE:
		{
			LOG("___Application Update___");

			int update_return = App->Update();

			if (update_return == UPDATE_ERROR)
			{
				LOG("Application Update exits with ERROR");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
			{
				state = MAIN_FINISH;
			}
		}
			break;

		case MAIN_FINISH:
		{
			LOG("__Application CleanUp__");

			if (App->CleanUp() == false)
			{
				LOG("Application CleanUp exits with ERROR");
			}
			else
			{
				main_return = EXIT_SUCCESS;
			}

			state = MAIN_EXIT;

			break;
		}

		}
	}

	delete App;
	LOG("Exiting '%s'...\n", TITLE);

	return main_return;
}