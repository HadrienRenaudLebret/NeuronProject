#include "NetworkArray.h"

using namespace std;

NetworkArray::NetworkArray(int length_alphabet) :
	tablo_net_(new Network*[length_alphabet]),
	maximal_distance_(MAXIMAL_DISTANCE),
	maxLimitLoop_(MAX_LIMIT_LOOP * NB_LEARNING),
	length_alphabet_(length_alphabet),
	momentum_(ALPHA)
{
	cout << "Creation des reseaux ... " << flush;
	for (int i = 0; i < length_alphabet_; ++i)
	{
		tablo_net_[i] = new Network(CHARS[i]);                                                                                                                 //Le réseau
		//Layer* l = new Layer(adresse du réseau, nombre de neurones dans la couche, adresse de la couche précédente, adresse de la couche suivante, fonction de transfert des neuronres de la couche);
		Layer* l1  = new Layer(tablo_net_[i], FIRST_LAYER_SIZE,  0,  0);                                                                                                                 //première couche
		Layer* l2  = new Layer(tablo_net_[i], 100, l1, 0);                                                                                                                 //seconde
		Layer* l3  = new Layer(tablo_net_[i], 10, l2, 0);                                                                                                                 //troisieme
		Layer* lend = new Layer(tablo_net_[i], LAST_LAYER_SIZE, l3, 0);                                                                                                                 //couche de fin
	}
	cout << "Reseaux crees !" << endl;
}

NetworkArray::~NetworkArray()
{
	cout << "Destruction des reseaux ... " << flush;
	for (int i = 0; i < length_alphabet_; ++i)
	{
		delete tablo_net_[i];
	}
	delete[] tablo_net_;
	cout << "Reseaux detruits !" << endl;
}

void NetworkArray::learnAllNetworks()
{

	cout << "Bienvenue dans le gestionnaire d'apprentissage du reseau de neurones." << endl << endl;

	cout << "Initialisation des parametres." << endl;
	clock_t t0(clock());                                                         //temps de départ du programme

	//nombre d'exemples à traiter
	int const nb_exemples(countExemples());

	// Initialisation des tableaux contenant les donnees des exemples
	char**  tabloFichiers = new char*[nb_exemples];
	double** inputs   = new double*[nb_exemples];
	for (int i = 0; i < nb_exemples; ++i)
	{
		tabloFichiers[i] = new char[MAX_LENGTH_NAME_FILE];
		inputs[i]   = new double[FIRST_LAYER_SIZE];
	}

	setOptions();

	//Récupération des données des fichiers
	getArrayOfFileNames(tabloFichiers);                                                         //on récupère les noms des ficheirs d'exemples
	getArrayOfExemples(tabloFichiers, inputs, nb_exemples);                                                         //on récupère les donnees des exemples
	cout << "Initialisation effectuee en " << ((float)(clock() - t0) / CLOCKS_PER_SEC) << " secondes, l'apprentissage commence." << endl << endl;

	//Apprentissage
	for (int i = 0; i < length_alphabet_; ++i)
	{
		tablo_net_[i]->learnNetwork(nb_exemples, tabloFichiers, inputs);
		cout << endl << endl;
	}
}

char NetworkArray::testNetworks(double input[], bool verbose)
{
	//initialisation
	char lettre_trouvee('_');
	double maxi(LOWER_BOUND_DISTINCTION);
	double exp_output[length_alphabet_][LAST_LAYER_SIZE];

	//tests
	if (verbose)
		cout << "Resultats : ";
	for (int i = 0; i < length_alphabet_; ++i)                                                         //pour chaque réseau
	{                                                            //on récupère la réponse
		tablo_net_[i]->initNetwork(input);
		tablo_net_[i]->launch(exp_output[i]);
		//on l'affiche si elle est suffisement grande
		if (verbose && exp_output[i][0] > (LOWER_BOUND_DISTINCTION / 10))
			cout << CHARS[i] << " : " << exp_output[i][0] * 100 << "% -- ";

		//on trouve le maximum des réponses aux premiers réseaux
		if (maxi < exp_output[i][0])                                                                                                                 //On modifie maxi si elle est supérieure
		{
			maxi   = exp_output[i][0];
			lettre_trouvee = CHARS[i];
		}
	}
	if (verbose)
		cout << endl;
	return lettre_trouvee;
}

double NetworkArray::testAll(string directory)
{
	//nombre d'exemples à traiter
	int const nb_exemples(countExemples(directory));

	if ((double)nb_exemples==0) {
		return -3.14159; // on retourne une valeur négative
	}

	//compteur de succes
	int succes = 0;

	// Initialisation des tableaux contenant les donnees des exemples
	char** tabloFichiers = new char*[nb_exemples];

	for (int i = 0; i < nb_exemples; ++i)
		tabloFichiers[i] = new char[MAX_LENGTH_NAME_FILE];

	// contenu du fichier d'exemple
	double input[FIRST_LAYER_SIZE];

	//Récupération des données des fichiers
	getArrayOfFileNames(tabloFichiers, directory);

	//On compte le nombre de succes
	cout << "Test en cours de " << nb_exemples << " fichiers ..." << endl;
	for (int i = 0; i < nb_exemples; ++i)
	{
		readExemple(tabloFichiers[i], input, FIRST_LAYER_SIZE, directory);                                                                                                                 //on lit l'exemple
		if (testNetworks(input, false ) == tabloFichiers[i][0])                                                                                                                 // on le teste
			succes++;                                                                                                                                                                          //on incrémente succes, si c'est un succes
		if (nb_exemples > 100 && i % ( nb_exemples / 100 ) == 0 )
		{
			cout << "Progress : [";
			for (int j = 0; j < 51; ++j)
			{
				if (j <= i / (nb_exemples / 50))
					cout << '=';
				else if ( j == i / (nb_exemples / 50) + 1)
					cout << '>';
				else
					cout << ' ';
			}
			cout << "] : " << (int( (i+1) * 100 / nb_exemples)) << "% \r" << flush;
		}
	}
	cout << endl;
	cout << "Test effectué !" << endl;
	std::cout << (double)succes / (double)nb_exemples << std::endl;

	// On retourne la proportion de succes
	return (double)succes / (double)nb_exemples;
}


void NetworkArray::getMostRecent()
{
	for (int i = 0; i < length_alphabet_; ++i)
		tablo_net_[i]->getMostRecent();
}

double NetworkArray::getMaximalDistance()
{
	return maximal_distance_;
}
void NetworkArray::setMaximalDistance(double maximal_distance)
{
	cout << "Les reseaux changent pour une distance maximale apres apprentissage de " << maximal_distance << endl;
	for (int i = 0; i < length_alphabet_; ++i)
		tablo_net_[i]->setMaximalDistance(maximal_distance);
	maximal_distance_ = maximal_distance;
}

int NetworkArray::getMaxLimitLoop()
{
	return maxLimitLoop_;
}
void NetworkArray::setMaxLimitLoop(int maxLimitLoop)
{
	cout << "Les reseaux changent pour une limite de boucles d'apprentissage de " << maxLimitLoop << endl;
	for (int i = 0; i < length_alphabet_; ++i)
		tablo_net_[i]->setMaxLimitLoop(maxLimitLoop);
	maxLimitLoop_ = maxLimitLoop;
}

double NetworkArray::getMomentum()
{
	return momentum_;
}
void NetworkArray::setMomentum(double momentum)
{
	cout << "Les reseaux changent pour un moment d'inertie de " << momentum << endl;
	momentum_ = momentum;
	for (int i = 0; i < length_alphabet_; ++i)
		tablo_net_[i]->setMomentum(momentum);
}

void NetworkArray::setOptions()
{
	ifstream optionsFile(NAME_CONFIG_FILE);
	string line;
	string cmdName;
	string bin;
	string cmdValueStr;

	while (getline(optionsFile, line))
	{
		if (!(line.length() == 0 || line[0] == '#'))
		{
			istringstream line_stream(line);

			try
			{
				line_stream >> cmdName;
				line_stream >> bin;
				line_stream >> cmdValueStr;

				if (cmdName == "momentum" || cmdName == "Momentum")
				{
					double cmdValue;
					cmdValue = atof(cmdValueStr.c_str());
					setMomentum(cmdValue);
				}
				else if (cmdName == "maximal_distance")
				{
					double cmdValue;
					cmdValue = atof(cmdValueStr.c_str());
					if (cmdValue > 0)
						setMaximalDistance(cmdValue);
					else
						throw string("Valeur invalide : le reseau impose que maximal_distance > 0");
				}
				else if (cmdName == "max_limit_loop")
				{
					int cmdValue;
					cmdValue = atoi(cmdValueStr.c_str());
					setMaxLimitLoop(cmdValue);
				}
			}
			catch (string const erreur)
			{
				cout << erreur << endl;
			}
			catch (const invalid_argument &ia)
			{
				cout << "Valeur invalide : " << ia.what() << endl;
			}
		}
	}
}

void NetworkArray::save()
{
	cout<< "Sauvegarde des reseaux ... " << flush;
	for (int i = 0; i < length_alphabet_; ++i)
		tablo_net_[i]->save();
	cout << "Reseaux sauvegardes !" << endl;
}


// ######################### Hors de NetworkArray ##############################




template <class T>
void displayArray(T* data, int length)          //afficher un tableau de valeur
{
	cout << "[";                                                                //début de l'affichage
	for (int i = 0; i < length - 1; i++)                                                          // pour chaque valeur du tableau,
		cout << (data[i] >= 0 ? "+" : "") << data[i] << ",";                                                                                                                  // on l'affiche avec son signe
	cout << (data[length - 1] >= 0 ? "+" : "") << data[length - 1] << "]";                                                         //fin de l'affichage
}
