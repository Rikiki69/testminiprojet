//Romain Julian et LAURIER Alexis

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <ctype.h>
#include <sstream>
#include <algorithm>
#include <time.h>

#include "Menu.h"
#include "Diagramme.h"
#include "filedialog.h"
#include "CImg.h"

#pragma warning(disable:4996)

using namespace std;
using namespace cimg_library;

// fonctione retournant un bool utilis� pour trier les vector dans l'ordre d�croissant des occurences
bool comparerMot(const Mot *elem1, const Mot *elem2)
{
	return elem1->getOccurence() > elem2->getOccurence();
}

// constructeur par d�faut
Diagramme::Diagramme() {
	listeMot_.clear();
	nombreAffiche_ = 20;  //20 mots par d�faut
	police_ = "testPolice"; // Police par d�faut � d�finir
	courbe_ = cercle; // courbe de g�n�ration de base
	orientation_ = 45.0; // Inclinaison par d�faut de +/- 45�
	menu_ = new MenuDiagramme(*this);
	origine_ = new MenuPrincipal(*this);
	CImg<unsigned char> grid(600, 600, 1, 3, 255);
	scene_ = grid; // definition de l'espace cimg du diagramme
}

//constructeur de recopie

Diagramme::Diagramme(const Diagramme & diagramme) { 
	listeMot_.clear();
	for (int count = 0; count < diagramme.getListeMot().size();count++) {
		listeMot_.push_back(diagramme.getListeMot().at(count));
	}
	nombreAffiche_ = diagramme.getNombre();
	police_ = diagramme.getPolice();
	courbe_ = diagramme.getCourbe();
	orientation_ = diagramme.getOrientation();
	menu_ = diagramme.getMenu();
	origine_ = diagramme.getOrigine();
	CImg<unsigned char> grid(600, 600, 1, 3, 255);
	scene_ = grid;
}

// importation de la liste des mots, traitements des donn�es et s�paration selon les caract�res d�finis dans les sp�cification

void Diagramme::creerListe() 
{
	char reponse;
	cout << "Attention, les donnees non enregistrees seront effacees" << endl;
	cout << "Souhaitez-vous poursuivre ? (o/n)" << endl;
	cin >> reponse;
	if ((reponse == 'n') || (reponse == 'N'))
	{
		return;
	}
	listeMot_.clear();
	string nomOpen = getOpenFileName("Nom du fichier a charger :", "Fichiers txt (*.txt )");

	if (nomOpen != "")
	{
		cout << "Nom du fichier charge : " << nomOpen << endl;
		ifstream fich(nomOpen.c_str());
		if (!fich.is_open())
		{

			cout << "Erreur d'ouverture, verifier le chemin du fichier" << endl;
		}
		else
		{
			string line;

				while (getline(fich, line))
				{
					string str = line;
					const string& delimiters = " ;,.()!?";
						// Skip delimiters at beginning.
						string::size_type lastPos = str.find_first_not_of(delimiters, 0);
					// Find first "non-delimiter".
					string::size_type pos = str.find_first_of(delimiters, lastPos);

					while (string::npos != pos || string::npos != lastPos)
					{
						// Found a token, add it to the vector list if word doesn't already exist.
						string motExtrait = str.substr(lastPos, pos - lastPos);
						vector<Mot*>::iterator it; // D�claration de l'it�rateur
						Mot *motAAjouter = new Mot;
						it = listeMot_.begin();
						int trouve = 0;
						while ((it != listeMot_.end()) && trouve == 0)
						{
							if (*(*it)->getText() == motExtrait)
							{
								trouve = 1;
								(*it)->apparu();
							}
							it++;
						}
						if (trouve == 0)
						{
							motAAjouter->setText(motExtrait);
							listeMot_.push_back(motAAjouter);
						}
						// Skip delimiters.  Note the "not_of"
						lastPos = str.find_first_not_of(delimiters, pos);
						// Find next "non-delimiter"
						pos = str.find_first_of(delimiters, lastPos);
					}

				}

				sort(listeMot_.begin(), listeMot_.end(), comparerMot);
				//recherche des extr�maux en nombre d'occurence
				int min = 1;
				int max = 1;
				int value = 0;
				vector<Mot*>::iterator it; // D�claration de l'it�rateur
				for (it = listeMot_.begin(); it != listeMot_.end(); it++)
				{
					value = (*it)->getOccurence();
					if (value > max) { max = value; }
					if (value < min) { min = value; }

				}

				if (max != 0) {
					for (it = listeMot_.begin(); it != listeMot_.end(); it++)
					{
						double occurence;
						occurence = (*it)->getOccurence();
						occurence = (occurence - min) / (max - min);
						(*it)->setOccurenceNormalisee(occurence);

					}

				}
		}
	}
}

//methode permettant d'ajouter un nouveau mot � la liste

void Diagramme::ajouterMot(Mot * mot) { 

	listeMot_.push_back(mot);

}

//methode permettant d'effectuer le choix des mots � afficher en proposant une interface � l'utilisateur

void Diagramme::choixMot() {
	system("cls");
	int page = 0; int i = 0;
	vector<Mot*>::iterator it; 
	string erreur ="";
	string choix;
	while(choix != "exit") {
		cout << left << setw(14) << "N�ligne" << setw(25) << "Mot : " << setw(22) << "Nombre d'apparition" << setw(14) << "L'afficher ?" << endl;
		for (it = listeMot_.begin() + (19 * page); (it != listeMot_.end()) && i<19 ; it++)
		{
			string affiche;
			if ((*it)->getChoisi()) { affiche = "oui"; }
			else { affiche = "non"; };
			cout << left << setw(14) << i+1 << setw(25) << *(*it)->getText() << setw(22) << (*it)->getOccurence() << setw(14) << affiche << endl;
			i += 1;
		}
		i = 0;
		cout << "Affichage de la page " << page + 1 << endl;
		cout << "Changer statut mot : numero ligne ; Changer numero page : p+, p-, px ;retourner menu principal : exit" << endl;
		cout << endl << erreur;
		cin >> choix;
		if (isdigit(choix[0]))
		{
			int indice = stoi(choix);
			if (indice + 19 * page <= listeMot_.size()) {
				(listeMot_[(indice - 1) + 19*page])->changeChoisi();
				erreur = "";
			}
			else {
				erreur = "Erreur d'indice";
			}
			
		}
		else {
			if(choix[0] == 'p')
			{ 
				switch (choix[1])
				{
				case '+':
					if (listeMot_.size() > (page+1) * 19) {
						page = page + 1; 
						erreur = "";
					}
					else {
						erreur = "Plus d'autres mots";
					}
					break;
				case '-':
					if (page >= 1)
					{
						page = page - 1;
						erreur = "";
					}
					else
						erreur= "Vous etes deja a la premiere page !";
				
					break;
				default :
				{
					int pageCible = stoi(choix.erase(0,1));
					if(listeMot_.size() > 19*(pageCible-1))
					{
						page = pageCible-1;
						erreur = "";
					}
					else {
						erreur = "choix incorrect";
					}

				}

			}
			
			}
			
			

		}
		system("cls");
	}

}

//G�n�re le nuage de mot � partir d'un ordre al�atoire des mots, puis affiche les mots en suivant la courbe dont les points sont d�finies par la fonction prochainPoint

void Diagramme::afficher(MenuPrincipal &origine, bool reload) {
	// Cr�ation et affichage du nuage � ajouter
	// Usefull colors
	if (reload)
		disp_.close();

	vector<Mot*> listeMotAleatoire;
	listeMotAleatoire.clear();

	// M�lange des mots
	for (vector<Mot*>::iterator it = listeMot_.begin();it != listeMot_.end(); it++) {
		int hasard = rand() % 5;
		int actuelle = (*it)->getOccurence();
		it++;
		if (it != listeMot_.end()) {
			int futur = (*it)->getOccurence();
			if (actuelle < futur + hasard) {
				listeMotAleatoire.push_back(*it);
				it--;
				listeMotAleatoire.push_back(*it);
				it++;
			}
			else {
				it--;
				listeMotAleatoire.push_back(*it);
			}
		}
		else {
			it--;
			listeMotAleatoire.push_back(*it);
		}
	}
		

	

	unsigned char grid_color[3] = { 0,0,255 };

	// Declare an image to draw the grid
	CImg<unsigned char> grid(600, 600, 1, 3, 255);

	// Declare a display to draw the scene
	disp_ = CImgDisplay(grid, "Generateur de mot", 0, false, false);

	// Center the window on the screen
	disp_.move((CImgDisplay::screen_width() - disp_.width()) / 2,
		(CImgDisplay::screen_height() - disp_.height()) / 2);



	// Main loop, exit if the display window is closed or if ESC or Q key is hit


		// Display the scene
		// Declare an image to display the scene
		scene_ = grid;
		Point point{ 300,300,0 };
		int nombreMotAffiche = 0;
		bool utilise = false;
		for (vector<Mot*>::iterator it = listeMotAleatoire.begin(); it != listeMotAleatoire.end() && nombreMotAffiche<nombreAffiche_; it++) {
			if ((*it)->getChoisi()) {
				unsigned char color[] = { rand() % 256, rand() % 256,rand() % 256 };

				if (utilise)
					it--;
				utilise = false;
				string text = *(*it)->getText();

				int hauteur = 15 + 100 * (*it)->getOccurenceNormalisee();
				int longueur = text.size()*hauteur;

				for (int i = point.x; i < point.x + longueur + 10 && i<600;i++) {
					for (int j = point.y;j < point.y + hauteur + 10 && j<600;j++) {

						int r = (int)scene_(i, j, 0, 0);
						int g = (int)scene_(i, j, 0, 1);
						int b = (int)scene_(i, j, 0, 2);
						if (r + g + b != 765 || i == 599 || j == 599) {
							utilise = true;
						}


					}
				}
				if (!utilise) {
					scene_.draw_text(point.x, point.y, text.c_str(), color, 0, 1, hauteur);
					nombreMotAffiche++;
				}

				point = prochainPoint(cercle, point);
			}
		}
		


		
		// Usefull variables

		scene_.display(disp_);

		disp_.wait();

		// Handle window resize
		if (disp_.is_resized()) disp_.resize();







	if (!reload) {
		origine_ = &origine;
		menu_->setDiagramme(*this);
		menu_->getDiagramme()->setOrigine(&origine);
		menu_->executer();

		
	}
	
}

// m�thode permettant de v�rifier si le chemin du fichier fourni contient d�j� l'extension .genmots et l'ajoute si necessaire
//http://stackoverflow.com/questions/20446201/how-to-check-if-string-ends-with-txt
bool has_suffix(const std::string &str)
{
	string suffix = "genmots"; 
	return str.size() >= suffix.size() &&
		str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}


//methode permettant de sauvegarder les param�tres entr�s par l'utilisateur
void Diagramme::sauvegarde() {
	cout << "Merci de choisir dans quel fichier effectuer la sauvegarde" << endl;
	string nomSave = getSaveFileName("Nom du fichier a sauvegarder :", "Fichiers genmots (*.genmots )");
	if (!has_suffix(nomSave))
		nomSave += ".genmots";
	if (nomSave != "")
	{
		cout << "Nom du fichier sauvegarde : " << nomSave << endl;
		ofstream fich(nomSave.c_str());
		if (!fich.is_open())
			cout << "Impossible d'enregistrer ici, verifier le chemin du fichier" << endl;
		else
		{
			fich << "&&listemot&&" << endl;
			vector<Mot*>::iterator it; // D�claration de l'it�rateur
			for (it = listeMot_.begin(); it != listeMot_.end(); it++)
			{
				fich << *(*it)->getText() << endl;
				fich << (*it)->getOccurence() << endl;
				fich << (*it)->getChoisi() << endl;
			}
			fich << "/listemot&&" << endl;
			fich << "&&settings&&" << endl;
			fich << nombreOccurenceChoisi_ << endl;
			fich << nombreAffiche_ << endl;
			fich << police_ << endl;
			fich << courbe_ << endl;
			fich << orientation_ << endl;
			fich << "/settings&&" << endl;
			cout << "Sauvegarde effectuee !" << endl;
			system("pause");

		}
	}
	else
	{
		cout << "Abandon" << endl;
	}
}

//m�thode permettant de charger une sauvegarde effectu�e pr�c�damment, de la v�rifier, puis de la charger en m�moire si celle ci est valide
void Diagramme::charger() {

	Diagramme diag;
	char reponse;
	cout << "Attention, les donnees non enregistrees seront effacees" << endl;
	cout << "Souhaitez-vous poursuivre ? (o/n)" << endl;
	cin >> reponse;
	if ((reponse == 'N') || (reponse == 'n')) 
	{
		exit;
	}

	string nomOpen = getOpenFileName("Nom du fichier a sauvegarder :", "Fichiers genmots (*.genmots )");

	if (nomOpen != "")
	{
		cout << "Nom du fichier charge : " << nomOpen << endl;
		ifstream fich(nomOpen.c_str());
		if (!fich.is_open())
		{

			cout << "Erreur d'ouverture, verifier le chemin du fichier" << endl;
		}

		else
		{
			string ligne;
			do {
				getline(fich, ligne);
			} while (ligne != "&&listemot&&");
			getline(fich, ligne);
			do {
				Mot *mot = new Mot;
				mot->setText(ligne);
				getline(fich, ligne);
				mot->setOccurence(stoi(ligne));
				getline(fich, ligne);
				mot->setChoisi(ligne == "1");
				diag.ajouterMot(mot);
				getline(fich, ligne);

			} while (ligne != "/listemot&&");

			do {
				getline(fich, ligne);

			} while (ligne != "&&settings&&");
			getline(fich, ligne);
			diag.setnombreOccurenceChoisi(stoi(ligne));
			getline(fich, ligne);
			diag.setnombreAffiche(stoi(ligne));
			getline(fich, ligne);
			diag.setPolice(ligne);
			getline(fich, ligne);
			diag.setCourbe(static_cast<Courbe>(stoi(ligne)));
			getline(fich, ligne);
			diag.setOrientation(stoi(ligne));
			getline(fich, ligne);
			fich.close();
			if (ligne != "/settings&&") {
				cerr << "sauvegarde endommagee, les donnees n'ont pas �t� importees" << endl;
				system("pause");
				exit;

			}
	
			*this = diag;
			//recherche des extr�maux en nombre d'occurence
			int min = 0;
			int max = 0;
			int value = 0;
			vector<Mot*>::iterator it; // D�claration de l'it�rateur
			for (it = listeMot_.begin(); it != listeMot_.end(); it++)
			{
				value = (*it)->getOccurence();
				if (value > max) { max = value; }
				if (value < min) { min = value; }

			}

			if (max != 0) {
				for (it = listeMot_.begin(); it != listeMot_.end(); it++)
				{
					double occurence;
					occurence = (*it)->getOccurenceNormalisee();
					occurence = (occurence - min) / (max - min);
					(*it)->setOccurenceNormalisee(occurence);

				}

			}
			cout << "Chargement effectuee !" << endl;
			system("pause");
			return;

		}

	}

}

// fonction permettant d'exporter le diagramme g�n�r� au format .ppm

void Diagramme::exporter() {
	string nomSave = getSaveFileName("Nom du fichier a sauvegarder :", "Fichiers image (*.ppm )");
	scene_.save(nomSave.c_str());

}

//fonction permettant de choisir la plage d'orientation des mots dans le diagramme // pas encore impl�ment� � l'affichage
void Diagramme::choixOrientation() {
	system("cls");
	cout << "Actuellement, le nuage affiche les mots avec une orientation maximale de +/- " << getOrientation() << " degres, quelle orientation souhaitez vous avoir ?" << endl;
	cin >> orientation_;
}
// fonction permettant de d�finir un autre filtre que celui du choix manuel des mots : le nombre de mots � afficher simultan�ment,
// dont l'odre est pris de facon d�croissante dans la liste al�atoire r�alis� � chaque affichage
void Diagramme::choixNombre() {
	system("cls");
	cout << "Actuellement, le nuage affiche " << getNombre() << " mots, combien souhaitez vous en afficher ?" << endl;
	cin >> nombreAffiche_;
}


// pas encore impl�ment� - permet de choisir la police � utiliser pour r�aliser le diagramme
void Diagramme::choixPolice() {

}
// pas encore impl�ment� � l'affichage - permet de choisir plusieurs courbes de g�n�ration de base
void Diagramme::choixCourbe() {
	int courbe;
	system("cls");
	cout << "Actuellement, le nuage affiche le nuage autour d'une courbe : " << nomCourbe(getCourbe()) << endl << "Quelle courbe souhaitez-vous utiliser ?" << endl;
	for (int courbe = 0; courbe < NOMBRE_DE_COURBE ; courbe++) {
		cout << courbe + 1 << ") " << nomCourbe(courbe) << endl;
	}
	cin >> courbe;
	courbe_ = (Courbe)(courbe-1);
}
// Fonction retournant le prochain point � tester pour placer le mot suivant en fonction de la courbe choisie
Point Diagramme::prochainPoint(Courbe courbe, Point pointActuelle) {
	int t = pointActuelle.posCourbe;
	Point prochainPoint = { 0,0,t + 1 };
	switch (courbe) {
	case cercle:
		prochainPoint.x = 300+10*t*sin(36000*t);
		prochainPoint.y = 300+10*t*cos(36000*t);
		break;
	case rectangle:
		break;
	}
	return prochainPoint;

}

// d�finition des diff�rentes courbes disponibles
string nomCourbe(int idCourbe) {
	switch (idCourbe) {
	case 0:
		return "Cercle";
	case 1:
		return "Rectangle";
	default:
		return "Erreur de courbe";
	}
}



