#include "database.h"
#include "path.h"
#include "vmd5.h"
#include "vdisplayer.h"
#include "vadddatabase.h"

#include <iostream>
#include<fstream>
#include <boost/filesystem.hpp>
#include <QtSql>

#define CONFIGFILE ".config"        // path du fichier de configuration; sera bien entendu placé ailleur sur la version finale

//La macro-définition q2c est simplement faite pour faciliter la conversion de QString vers std::string
#define q2c(string) string.toStdString()



using namespace std;
using namespace boost::filesystem;

typedef enum {normal, recursive} mode;

// PROTOTYPES ET CORPS DE MÉTHODES A BOUGER (dans une classe ?)
void menu();
void update();
void afficherAide();
void lister();
void addContentRecursively(Path &p, mode m);


int main()
{
    DataBase::instance().ouvrirDB(); /// On ouvre la base de données principale pour pouvoir ensuite faire des requêtes dessus

    menu();

    return 0;
}




void update(){

    /** Sélection des dossiers à scanner à l'aide du fichier de configuration */
    ifstream config(CONFIGFILE); /// création d'un flux pour la manipulation du fichier de configuration

    if(!config)
        cerr<<"Erreur ouverture fichier de configuration"<<endl;

    string parcours_configfile;

    getline(config, parcours_configfile); /// récupère une ligne du fichier de configuration
    if(parcours_configfile.size() == 0)
    {
        cerr<<"Fichier de configuration vide !"<<endl;
    }


    /** Scan des dossiers sélectionnés */
    while(parcours_configfile.size() != 0)
    {
        Path p(parcours_configfile);
        try
        {
            if (exists(p))    /// does p actually exist?
                addContentRecursively(p, recursive);

            else
                cout << p << " does not exist\n";
        }
        catch(const filesystem_error& ex)
        {
            cerr << p << " Permission Denied !\n";
        }


        getline(config, parcours_configfile); /// continue le parcours du fichier
    }

} /// ferme automatiquement tous les flux


/**
  @brief ajoute récursivement un dossier et son contenu à notre base de données

  @param p le fichier courant
  @param m le mode choisit pour le scan, peut être récursif ou normal (on ne parcoure pas les dossiers)
*/
void addContentRecursively(Path &p, mode m)
{
    VAddDatabase addDB;   // new enlevé pour risques de fuites de mémoire           /// Crée un visiteur du type "Ajoueur à la BDD
    directory_iterator end;
    directory_iterator file(p);

    while(file != end)
    {
       try
       {
            if(m == recursive)
            {
                if(is_directory(*file) && !is_symlink(*file) && !p.isHidden(*file) && !p.isForbidden(*file)) /// Si c'est un dossier, non un lien symbolique, qu'il n'est ni caché, ni interdit..
                {
                    Path dir(*file);
                    addContentRecursively(dir, recursive); /// on relance la fonction récursivement
                }
            }

            if (is_regular_file(*file) && file_size(*file) > 0 && !p.isHidden(*file)) { /// Si c'est un fichier régulier, de taille supérieure à 0 et qui n'est pas caché...
                Path(*file).Accept(&addDB); /// Accepte un visiteur pour se faire ajouter à la base de données
                cout << *file << " ajouté " << endl;
            }
        }
        catch (const filesystem_error& ex)
        {
            cerr << *file << " Permission Denied !\n";
        }

        ++file;
    }
}





/**
  @brief liste les éléments de la base de données
  utilise une requête select puis affiche les pathname de tous les fichiers
*/
void lister()
{
    VDisplayer *afficheur = new VDisplayer();

    QSqlQuery selectQuery;

    if( selectQuery.exec("SELECT filepath FROM " + QString( DataBase::instance().tableName()) ) ) {
        while(selectQuery.next()) { /// tant qu'il y a encore des résultats à la requête...
            for(int x=0; x < selectQuery.record().count(); ++x) /// pour chaque ligne de résultat de la requête...
                Path(selectQuery.value(x).toString().toStdString()).Accept(afficheur);    /// Accepte le visiteur pour se faire afficher
        }
    }

    delete afficheur; /// une fois utilisée, on supprime le visiteur
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
            lister();       /// Liste les fichiers présents dans la BDD
        break;
        case 'u' :
            update();       /// Met à jour la liste des fichiers de la BDD
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
    cout<< endl <<"Commande d'action"<<endl<<
    "   l   lister les fichiers (sera remplacé par doublons) du dossier passé en paramètre"<<endl<<
    "   m   afficher ce menu"<<endl<<
    "   u   mettre à jour la base de données"<<endl<<
    "   q   quitter sans enrengistrer les changements"<<endl;
}
