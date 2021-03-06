#include "ReadNetwork.h"

template <class T>
void displayArray(T* data, int length) //afficher un tableau de valeurs
{
	cout << "[";
	for ( int i = 0; i < length - 1; i++)
		cout << (data[i] >= 0 ? "+" : "") << data[i] << ",";
	cout << (data[length - 1] >= 0 ? "+" : "") << data[length - 1] << "]";
}


ReadNetwork::ReadNetwork() : Network('b', "", MAXIMAL_DISTANCE){
	m_alphabet = (char*) CHARS;
	m_length_alphabet = LENGTH_ALPHABET;
}

//ReadNetwork::ReadNetwork(const ReadNetwork rdnk){}//constructeur de copie

ReadNetwork::ReadNetwork(int layerNb, int* layerSizes, char* alphabet, transfert trsf, double maximal_distance, double momentum, double mu, short verbose):
	Network('b', "", maximal_distance, momentum),
	m_maxLimitLoop(0),
	m_alphabet(alphabet),
	m_length_alphabet(layerSizes[layerNb - 1])
{
	if (verbose >= VERBOSE_NORMAL)
		cout << endl << "Creation d'un reseau a " << layerNb << " couches : ";
	Layer* layer = NULL;
	for(int i = 0; i<layerNb; i++) {
		layer = new Layer(this, layerSizes[i], layer, 0, trsf, mu);
		if (verbose >= VERBOSE_NORMAL)
			cout << layerSizes[i] << " ";
	}
	if (verbose >= VERBOSE_NORMAL)
		cout << endl;
}

ReadNetwork::~ReadNetwork(){    // le destructeur parent est déjà appelé, donc rien à delete
}


int ReadNetwork::getLenghtAlphabet(){
	return m_length_alphabet;
}
void ReadNetwork::setLengthAlphabet(int length){
	m_length_alphabet = length;
}

char* ReadNetwork::getAlphabet(){
	return m_alphabet;
}
void ReadNetwork::setAlphabet(char* alphabet){
	if(alphabet)
		m_alphabet = alphabet;
	else
		err("ReadNetwork::setAlphabet : alphabet NULL",1);
}

void ReadNetwork::train(double maximal_distance, short verbose){

	short lastLayerSize = getLastLayer()->getSize();
	if(m_length_alphabet!=lastLayerSize)
		err("ReadNetwork::train : certaines lettres ne seront pas prises en compte car m_length_alphabet!=lastLayerSize",1);

	short firstLayerSize = firstLayer_->getSize();
	short const nb_exemples(countExemples());
	if (verbose >= VERBOSE_NORMAL)
		cout << endl << nb_exemples << " exemples d'entrainement trouves, debut de l'apprentissage." << endl;

	if (verbose == VERBOSE_DETAILS)
		cout << "Distance maximale : " << maximal_distance << endl << endl;

	short ignoredExemples = 0;
	char**  tabloFichiers = new char*[nb_exemples];
	double** inputs   = new double*[nb_exemples];
	double** outputExpected = new double*[nb_exemples];
	char* lettres = new char[nb_exemples];
	double outputExperimental[lastLayerSize]; //valeur prototypale en sortie du réseau

	for (int i = 0; i < nb_exemples; ++i)
	{
		tabloFichiers[i] = new char[MAX_LENGTH_NAME_FILE];
		inputs[i]   = new double[firstLayerSize];
		outputExpected[i] = new double[lastLayerSize];
		for(int j = 0; j<lastLayerSize; j++)
			outputExpected[i][j] = 0;
	}
	//Récupération des données des fichiers
	if(nb_exemples != getArrayOfFileNames(tabloFichiers))
		err("ReadNetwork::train() : compteur!=nb_exemples",1);

	for(int i = 0; i<nb_exemples; i++)
		lettres[i] = tabloFichiers[i][0];

	getArrayOfExemples(tabloFichiers, inputs, nb_exemples);

	for(int i = 0; i < nb_exemples; i++) {
		int ca = findChar(lettres[i], m_alphabet, m_length_alphabet);
		if(ca == -1)
		{   //échec
			outputExpected[i] = NULL;
			ignoredExemples++;
		}
		else
			outputExpected[i][ca] = 1;
	}

	// Initialisations
	short exemple   = 0; //exemple actuellement selectionné pour l'apprentissage, cette variable est incrémentée à chaque fois qu'il réussit un exemple
	short successes  = 0; //le réseau doit enchainer nb_exemples - ignoredExemples succès pour avoir fini l'apprentissage, cela ne devra pas être le cas pour les caractères manuscrits, parce qu'il y un risque de surapprentissage
	short maxSuccesses = 0;
	int count   = 0; //nombre de passage dans la boucle
	short successDisplay =0;

	clock_t t0(clock()); //temps de départ du programme
	short trueNb_exemples = nb_exemples - ignoredExemples;

	//APPRENTISSAGE
	while ((successes < trueNb_exemples) && (count < MAX_LIMIT_LOOP))
	//tant qu'on a pas enchaîné nb_exemples succès
	{
		if (exemple == 0)
			count++;

		if(outputExpected[exemple] != NULL) { //
			initNetwork(inputs[exemple]);  //on initialise avec les valeurs inputs
			launch(outputExperimental);  //on lance et on récupère les outputs

			//On apprend, ou pas en fonction du résultat
			if (isSuccess(outputExperimental, outputExpected[exemple], lastLayerSize, maximal_distance)) {
				//si c'est assez petit, c'est un succès
				successes++;
				successDisplay++;
			}
			else
			{
				successes = 0;  //on réinitialise aussi le nombre de succès enchaînés
				initNetworkGradient(outputExpected[exemple]);
				learn();
			}
			//cout << distance(outputExpected[exemple], outputExperimental, lastLayerSize) << ", " << flush;
			if(successes > maxSuccesses) {
				maxSuccesses = successes;
				if (verbose == VERBOSE_NORMAL)
					cout << 100*maxSuccesses/trueNb_exemples << "%\n";
			}
			if (exemple == 0 && count % NB_LEARNING == 0)
			{
				if (verbose == VERBOSE_DETAILS)
					cout << "Iteration " << count << " , " << (double)successDisplay / (NB_LEARNING * trueNb_exemples) << endl;
				successDisplay = 0;
			}
		}
		exemple++;
		exemple %= nb_exemples;  //On ne dépasse pas nb_exemples
	}
	if (verbose == VERBOSE_DETAILS)
		cout << "Iteration "<< count << " : 1.0" << endl;
	if (verbose >= VERBOSE_MINIMAL)
		cout << "Temps : " << ((float)(clock() - t0) / CLOCKS_PER_SEC) << " secondes, apprentissage termine." << endl;

	for(int i = 0; i<nb_exemples; i++) {
		delete tabloFichiers[i];
		delete inputs[i];
		if(outputExpected[i])
			delete outputExpected[i];
	}
	delete tabloFichiers;
	delete inputs;
	delete outputExpected;
	delete lettres;
}

void ReadNetwork::save(string name)
{
	//on écrit dans le fichier
	ofstream file(name);    // flux sortant dans le fichier
	file << maximal_distance_ << ' ';
	file << m_alphabet << '\n';
	file << getTotalLayerNumber() << ' ';  // on entre le nombre total de couches
	Layer *  layer(getFirstLayer());   // on initialise la premiere couche
	file << layer->getSize() << ' ';   // en donnant sa longueur
	Neuron * neurone;
	while (layer->getNextLayer() != 0)   //pour toute couche
	{
		layer = layer->getNextLayer();  // on prend la suivante
		file << endl << layer->getSize() << ' '; // on donne sa taille
		for (int i(0); i < layer->getSize(); i++) // pour tout neurone de la couche
		{
			neurone = layer->getNeuron(i);  // on récupère le neurone
			for (int j(0); j < neurone->getBindingsNumber(); j++) // pour toute liaison de la couche précédente vers ce neurone
				file << neurone->getBinding(j)->getWeight() << ' ';    // on ajoute au fichier le poids de la liaison
			file << ',';  //séparateur
		}
	}
}

char ReadNetwork::test(char* name, string directory){

	double* input = new double[FIRST_LAYER_SIZE];
	double* output = new double[m_length_alphabet];
	char result = '_';

	if (readExemple(name, input, FIRST_LAYER_SIZE, directory))
	{
		initNetwork(input);
		launch(output);

		short imax = 0;
		for(short k = 0; k < m_length_alphabet; k++)
			if(output[k] > output [imax])
				imax = k;

		result = m_alphabet[imax];
	}
	else
		err("L'exemple " + directory + string(name) + " n'a pas pu être ouvert.", 1);

	delete input;
	delete output;
	return result;
}


double ReadNetwork::testAllExamples(short verbose, string directory)
{
	//nombre d'exemples à traiter
	int const nb_exemples(countExemples(directory));

	//Affichage
	if (verbose >= VERBOSE_NORMAL)
		cout << endl << "Test en cours de " << nb_exemples << " fichiers ..." << endl;

	if ((double)nb_exemples==0) {
		return 1; // Plus logique de retourner une proportion égale à 1 : on a réussi à identifier toutes les images tests ("pour tout" dans un ensemble vide = vrai)
		//return -3.14159265358979323846264338327969399375105084281971; // on retourne une valeur négative
	}


	// Initialisation des tableaux contenant les noms de fichiers des exemples
	char** tabloFichiers = new char*[nb_exemples];

	for (int i = 0; i < nb_exemples; ++i)
		tabloFichiers[i] = new char[MAX_LENGTH_NAME_FILE];

	//Récupération des données des fichiers
	getArrayOfFileNames(tabloFichiers, directory);


	//compteur de succes
	int succes = 0;

	for (int i = 0; i < nb_exemples; ++i)
	{
		// on le teste
		if (test(tabloFichiers[i], directory.c_str()) == tabloFichiers[i][0])
			succes++;  //on incrémente succes, si c'est un succes

		//Affichage de la progression
		if (verbose >= VERBOSE_NORMAL && nb_exemples > 100 && i % ( nb_exemples / 100 ) == 0 )
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
	if (verbose >= VERBOSE_NORMAL)
	{
		cout << endl;
		cout << "Test effectue !" << endl;
	}
	//std::cout << (double)succes / (double)nb_exemples << std::endl;   inutile d'afficher le résultat : la fonction qui appelle celle-ci s'en chargera ...

	// On retourne la proportion de succes
	return (double)succes / (double)nb_exemples;
}

ReadNetwork* load(string name, bool treat_error)
{
	// On ouvre le fichier
	ifstream file(name);
	if (!file) {

		if (treat_error)
			err("Impossible d'ouvrir le fichier" + name,1);

		return 0;
	}

	// Declarations
	int nbTotalLayer, lengthLayer, lengthPreviousLayer,lab;
	double weight, mdist;

	//Variables inutilisées
	//char*  pEnd(0);
	//char tamp;
	//Neuron*  neurone;

	string ligne, weight_str, neurone_str,alpha;
	Layer *  layer;
	ReadNetwork* rdnk = new ReadNetwork();

	file >> mdist;
	file >> alpha;

	rdnk->setMaximalDistance(mdist);
	lab = alpha.length();
	char* alphab = new char[lab]; //on ne peut pas le delete à la fin de la fonction car le réseau doit garder son alphabet, mais on ne peut pas le delete dans le constructeur non plus, donc c'est une fuite de mémoire
	strncpy(alphab,alpha.c_str(),lab);

	rdnk->setAlphabet(alphab);

	file >> nbTotalLayer;
	file >> lengthLayer;

	layer = new Layer(rdnk,lengthLayer,0,0,0);

	// pour chaque couche
	for(int k = 1; k<=nbTotalLayer; k++)
	{
		lengthPreviousLayer = lengthLayer;
		file >> lengthLayer;  // on lit la longueur de la prochaine couche
		// cout << "lengthLayer = " << lengthLayer << endl;
		layer = new Layer(rdnk,lengthLayer,layer,0,0);
		// pour tout neurone de la couche
		for(int i = 0; i<lengthLayer; i++)
		{
			for(int j = 0; j<lengthPreviousLayer; j++) {
				file >> weight;  //on récupère le poids de la liaison
				// cout << "weight[" << k << "][" << i << "][" << j << "] : " << weight <<endl;
				layer->getNeuron(i)->getBinding(j)->setWeight(weight);    //on change le poids de la liaison
			}
			if(i!=lengthPreviousLayer-1)
				file.seekg(2,ios::cur);  //on saute l'espace et la virgule à la fin d'un neurone
		}
	}
	rdnk->setLengthAlphabet(lengthLayer);

	return rdnk;
}

bool isSuccess(double* tab1, double* tab2, int length, double dist){
	for(int i = 0; i<length; i++) {
		if(abso(tab1[i]-tab2[i])>dist)
			return false;
	}
	return true;
}


bool readExemple(char* nom_fichier, double entrees[], int taille_entree, string directory)
{
	char* temp = new char[MAX_LENGTH_NAME_FILE];
	strncpy(temp, nom_fichier, MAX_LENGTH_NAME_FILE);
	ifstream file((directory + string(temp)).c_str());                                                         //ouverture du fichier

	if (!file)
	{
		//ERREUR -> message dans erreur.txt, affiché dans la console
		err("Apprentissage - Bug 1 : Exemple " + directory + string(temp) + " non lu.",1);
		delete temp;
		return false;
	}
	else
	{
		// On lit le fichier
		for (int i(0); i < taille_entree; i++)
			file >> entrees[i];

		delete temp;                                                                                                                                                                        // on insère les valeurs dans entrees
		return true;
	}
}
