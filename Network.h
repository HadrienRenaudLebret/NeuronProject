#ifndef NETWORK_H
#define NETWORK_H

#include "config.h"

using namespace std;

class Layer; //on dit qu'on va utiliser la classe layer


//! Classe Reseau de neurones
/*! Contient sa premiere couche, les methodes et tous les parametres necessaire */
class Network {
    public:
	//! Constructeur par defaut (vide)
	Network();
	//! Constructeur utilise
	/*!
	   initialise tous les attributs :
	   \param lettre_testee lettre dont s'occupera le reseau
	   \param nom_fichier string qui servira a stocke le nom du fichier pendant les recuperations, sauvegardes
 	   \param maximal_distance initialisation de l'attribut [maximal_distance_](@maximal_distance)
	*/
	Network(char lettre_testee, string nom_fichier = "", double maximal_distance = MAXIMAL_DISTANCE, double momentum = ALPHA);

	//! Destructeur
	~Network();


	//! Changer la première couche
	void setFirstLayer(Layer* layer = 0);

	//! Retourne la premiere couche du reseau
	Layer* getFirstLayer() const;

	//! Retourne la derniere couche du reseau
	Layer* getLastLayer() const;

	//! Retourne la taille de la premiere couche = taille de l'entree a fournir
	int  getFirstLayerSize() const;

	//! Initialiser pour la propagation normale
	void initNetwork(double* inputs);

	//! Initialiser pour la rétropropagation
	void initNetworkGradient(double* expectedOutputs);

	//! Nombre de liaisons dans le réseau
	int  getTotalBindingsNumber() const;

	//! Nombre de couches dans le réseau
	int  getTotalLayerNumber();

	//! Augmenter le nombre de bindings de n (par defaut de 1)
	void increaseTotalBindingsNumber(int n = 1);

	//! Tester si le réseau boucle sur lui mm
	// bool isALoop() const;

	//! Lancer la propagation
	void launch(double output[]);

	//! Lancer la rétropropagation du gradient
	bool launchGradient();

	//! Algorithme d'apprentissage (tout initialise)
	bool learn();

	//! Sauver l'etat du réseau
	//void save();

	//! Fixe le moment d'inertie à momentum
	void setMomentum(double momentum);

	//! Retourne le moment d'inertie du reseau
	double getMomentum();

	//! Fonction appelée pour récupérer à partir d'un fichier
	void recuperateur();

	//! Retourne la distance maximale en fin d'apprentissage productif
	double getMaximalDistance();

	//! Fixe la distance maximale en fin d'apprentissage productif
	void setMaximalDistance(double maximal_distance);

	//! Retourne le nombre maximal de boucles d'apprentissage a effectuer par le reseau
	int  getMaxLimitLoop();

	//! Fixe le nombre maximal de boucles d'apprentissage a effectuer par le reseau
	void setMaxLimitLoop(int maxLimitLoop);

protected:

	//! Pointeur vers la premiere couche
	/*! Seule la premiere couche suffit, le reseau fonctionne comme une liste chainee */
	Layer* firstLayer_;

	//! Nombre total de liaison dans le reseau
	int totalBindingsNumber_;

	//! On procede à la propagation seulement si initialized_ est vrai
	bool initialized_;

	//! On procede à la retropropagation seulement si gradientInitialized_ est vrai
	bool gradientInitialized_;

	//! Facteur d'inertie, par defaut define -> ALPHA
	double momentum_;

	//! Distance maximale en fin d'apprentissage productif
	double maximal_distance_;

	//! Nombre maximal de boucles d'apprentissage a effectuer
	int maxLimitLoop_;

	//! Lettre testee par le neurone
	char testedLetter_;

	//! Nom du fichier dans lequel le reseau est sauvegarde/ recupere
	char* nameFile_;
};

//! Calcul d'écart
double distance(double* data1, double* data2, int length);

//! Calcul d'écart, modulo 26 : la casse n'est pas prise en compte
double distanceMod(double* data1, double* data2, int length);


#endif
