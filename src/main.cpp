#include "database.h"

#include <iostream>
#include<fstream>
#include <console.h>
#include <QApplication>


using namespace std;

/*
  BUGS :
  *quit 'q' ne marche plus
  *CHAQUE fois que la macro DEBUG est appelée, il y a le warning 'database.cpp: avertissement : format not a string literal and no format arguments [-Wformat-security]'
        => 37 warnings !!

  *A NE PAS FAIRE : utiliser 2 lignes 'using namespace ___'
  A FAIRE : 'using namespace boost::filesystem;
                using std::___;
                using std::___;
*/


int main(int argc, char* argv[])
{

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    QApplication app(argc, argv);

    QTranslator translator;

    int choice;
    cout << "Choose a language : 0 for english, 1 for french (default english). ";
    cin >> choice;

    switch(choice)
    {
        case 0 :
            translator.load(QString("test_database_en")); // PROBLÈME => PAS PRIS EN COMPTE (reste en français)
        break;
        default :
            translator.load(QString("test_database_fr"));

    }


    app.installTranslator(&translator);

    DataBase::instance().ouvrirDB(); /// On ouvre la base de données principale pour pouvoir ensuite faire des requêtes dessus

    Console* afficheur = new Console();
    afficheur->menuPrincipal();

    delete afficheur;
    DataBase::instance().fermerDB(); /// comme on a fini de s'en servir, on ferme la base // À BOUGER QUAND ON AURA LA PARTIE GRAPHIQUE (qui s'éxecute après)

    return app.exec();
}
