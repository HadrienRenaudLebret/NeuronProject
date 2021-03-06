#include "config.h"
#include "ReadNetwork.h"

using namespace std;



#ifndef NO_GRAPHIC
void graphics()
{
	SDL_Window * window(0);
	SDL_Renderer * ren = init(window);

	menu(ren);
	SDL_DestroyRenderer(ren);
	quit(window);
}
#endif //NO_GRAPHIC

void help()
{
	cout << "Welcome to NeuronProject help page !" << endl;
	cout << "NeuronProject is a little software created by french students for their school project." << endl;
	cout << endl;
	cout << "Usage :" <<endl;
	cout << "	./NeuronProject"<< endl;
	cout << "	./NeuronProject [-h/--help]"<< endl;
	cout << "	./NeuronProject [-f/--file] (FILE)"<< endl;
	cout << "	./NeuronProject [-c/--commands] COMMANDS"<< endl;
	cout << endl;
	cout << "Options :"<< endl;
	cout << "	[None] : default, graphics utilisation"<<endl;
	cout << "	-h, --help : display this help file"<< endl;
	cout << "	-f, --file [FILE]: call script with FILE, default in config.h"<< endl;
	cout << "	-c, --commands [COMMANDS] : call script with COMMANDS"<< endl;
	cout << endl;
}

int main(int argc, char** argv)
{
	srand(time(0));//ne pas enlever

	if (argc < 2)
		#ifndef NO_GRAPHIC
		graphics();
		#else
		help();
		#endif //NO_GRAPHIC

	else
	{
		for (int i = 1; i < argc; ++i)
		{
			if ((string)argv[i] == "-h" || (string)argv[i] == "--help")
				help();

			else if ((string)argv[i] == "-f" || (string)argv[i] == "--file")
			{
				ifstream file(NAME_SCRIPT_FILE);
				if (i + 1 < argc)
					ifstream file(argv[i+1]);

				if (file)
					scriptFile(file);
				else
					cout << "Pas de script a executer." << endl;
			}

			else if ((string)argv[i] == "-c" || (string) argv[i] == "--commands")
			{
				if (i + 1 < argc)
				{
					string cmds[argc - i -1];
					for (int j = 0; j < argc-i-1; ++j)
						cmds[j] = (string)argv[i+j+1];
					commands(argc - i -1,cmds);
				}
				else
					help();
			}
		}
	}


	return 0;
}
