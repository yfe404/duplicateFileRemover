#include "database.h"

#include <iostream>
#include<fstream>
#include <QtSql>
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

   menu();

   return app.exec();
}



void menu()
{
    char choix = 'm';
    multimap<string, boost::filesystem::path*> map;


    while(choix!='q')
    {
        cout << QObject::trUtf8("Commande (m pour l'aide) : ").toStdString();
        cin >> choix; // CODERET A GERER (réglerait le problème qui suit ?)  // Problème : si l'utilisateur entre une chaine de caractères, pas géré ! => prend 1car/2 de la chaine (alors qu'il faudrait mettre un message d'erreur !)
        switch(choix)
        {
        case 'd' :
            DataBase::instance().rechercherDoublons(map);
        break;
        case 'u' :
            DataBase::instance().update();       /// Met à jour la liste des fichiers de la BDD
        break;
        case 'm' :
            afficherAide(); /// Affiche ce menu
        break;
        case 'q' :
            return;         /// Quitte le programme
        break;
        default:
            cout<<choix<<QObject::trUtf8(" : commande inconnue").toStdString()<<endl;  /// Par défaut affiche commande inconnue

        }
    }
}


void afficherAide()
{
    cout << QObject::trUtf8("Commande d'action").toStdString() << endl <<
    QObject::trUtf8("   d   lister les fichiers qui ont des doublons de taille").toStdString()<< endl <<
    QObject::trUtf8("   m   afficher ce menu").toStdString() << endl <<
    QObject::trUtf8("   u   mettre à jour la base de données").toStdString() << endl <<
    QObject::trUtf8("   q   quitter").toStdString() << endl;
}

