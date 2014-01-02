#include "database.h"

#include <iostream>
#include<fstream>
#include <console.h>
#include <QApplication>


using namespace std;
using namespace boost::filesystem;



// PROTOTYPES ET CORPS DE MÉTHODES A BOUGER (dans une classe ?)
void menu();
void update();
void afficherAide();
void lister();
void listerDoublons();
void testMD5();

void test(void);




int main(int argc, char* argv[])
{

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    QApplication app(argc, argv);

    QTranslator translator;

    int choice;
    cout<<"Choose a language : 0 for english, 1 for french (default english)."<<endl;
    cin >> choice;

    switch(choice)
    {
        case 0 :
            translator.load(QString("test_database_en"));
        break;
        default :
            translator.load(QString("test_database_fr"));

    }

    app.installTranslator(&translator);

    DataBase::instance().ouvrirDB(); /// On ouvre la base de données principale pour pouvoir ensuite faire des requêtes dessus

    Console* afficheur = new Console();
    afficheur->menuPrincipal();

    delete afficheur;



   return app.exec();
}








