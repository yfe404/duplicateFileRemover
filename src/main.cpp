#include "database.h"

#include <iostream>
#include<fstream>
#include <QtSql>
#include <QApplication>



//La macro-définition q2c est simplement faite pour faciliter la conversion de QString vers std::string
#define q2c(string) string.toStdString()




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

    while(choix!='q')
    {
        cout<<"Commande (m pour l'aide): ";
        scanf("%c%*c", &choix); // CODERET A GERER (réglerait le problème qui suit ?)  // Problème : si l'utilisateur entre une chaine de caractères, pas géré ! => prend 1car/2 de la chaine (alors qu'il faudrait mettre un message d'erreur !)
        switch(choix)
        {
        case 'l' :
            DataBase::instance().listerFichiers();       /// Liste les fichiers présents dans la BDD
        break;
        case 'd' :
            DataBase::instance().listerDoublonsTaille();    /// Liste les fichiers qui on des doublons de taille ( 1 seul fichier par taille )
        break;
        case 'u' :
            DataBase::instance().update();       /// Met à jour la liste des fichiers de la BDD
        break;
        case 's' :
            DataBase::instance().updateMD5( DataBase::instance().getListSizeDuplicate() ); /// Met à jour les clés md5 pour les fichiers de même taille
            DataBase::instance().listerDoublons(); /// liste les doublons en fonctions des clés md5
        break;
        case 'm' :
            afficherAide(); /// Affiche ce menu
        break;
        case 'q' :
            return;         /// Quitte le programme
        break;
        default:
            cout<<choix<<" : commande inconnue"<<endl;  /// Par défaut affiche commande inconnue

        }
    }
}


void afficherAide()
{
    cout << "Commande d'action" << endl <<
    "   l   lister les fichiers (sera remplacé par doublons) du dossier passé en paramètre"<< endl <<
    "   d   lister les fichiers qui ont des doublons de taille"<< endl <<
    "   s   lister les doublons parfaits" << endl <<
    "   m   afficher ce menu" << endl <<
    "   u   mettre à jour la base de données" << endl <<
    "   q   quitter sans enrengistrer les changements" << endl;
}

