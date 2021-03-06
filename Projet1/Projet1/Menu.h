#ifndef __MENU__
#define __MENU__

#include <string>
#include <vector>
#include "Diagramme.h"

using namespace std;

class Menu;

class OptionMenu
{
	string nom_;
	string description_;
public:
	OptionMenu(const string &nom, const string &description);
	string getNom() const { return nom_; }
	string getDescription() const { return description_; }
};


class Menu
{
	string titre_;
	vector<OptionMenu> listeOptions_;
	bool fin_;
	Diagramme* diagramme_;
public:
	Menu(const string & titre);
	void ajouterOption(const string &nom, const string &description);
	void afficherMenu();
	int demanderChoix();
	void executer();
	virtual void executerOption(const string &nom);
	void quitter();
};


class MenuPrincipal : public Menu
{
public:
	MenuPrincipal(const string & titre);
	void executerOption();
};

class MenuDiagramme : public Menu
{
public:
	MenuDiagramme(const string & titre);
	void executerOption();
};

#endif