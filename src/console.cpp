#include "console.h"
#include "command.h"

using std::cout;
using std::cin;
using std::endl;



Console::Console(QWidget* parent) : Displayer(parent){
}

/**
  @brief reçoit la commande de l'utilisateur et appelle les fonctions nécessaires pour répondre à sa demande
  */
void Console::menuPrincipal()
{

    char choix = 'm';
    multimap<string, boost::filesystem::path*> map;


    while(choix != 'q') /// tant qu'on a pas décidé de quitter...
    {
        cout << QObject::trUtf8("Commande (m pour l'aide) : ").toStdString();
        cin >> choix; // Problème : si l'utilisateur entre une chaine de caractères, pas géré ! => prend 1car/2 de la chaine (alors qu'il faudrait mettre un message d'erreur !)
        // VIDER LE BUFFER ICI (sinon, teste le caractère qui a été rentré après)

        switch(choix)
        {
        case 'd' :
            DataBase::instance().rechercherDoublons(map);
        break;
        case 'u' :
            DataBase::instance().update();       /// Met à jour la liste des fichiers de la BDD
        break;
        case 'm' :
            menuAide(); /// Affiche une brève description sur chaque commande de ce menu
        break;
        case 'q' :
            return;         /// Quitte le programme
        break;
        default:
            cout << choix << QObject::trUtf8(" : commande inconnue").toStdString() << endl;
        }
    }
 }

/**
  @brief affiche une description rapide des commandes possibles
  */
void Console::menuAide()
{
    cout << QObject::trUtf8("Commande d'action").toStdString() << endl <<
    QObject::trUtf8("   d   lister les fichiers qui ont des doublons de taille").toStdString()<< endl <<
    QObject::trUtf8("   m   afficher ce menu").toStdString() << endl <<
    QObject::trUtf8("   u   mettre à jour la base de données").toStdString() << endl <<
    QObject::trUtf8("   q   quitter").toStdString() << endl;
}


