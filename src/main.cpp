#include "database.h"

#include <iostream>
#include<fstream>
#include <boost/filesystem.hpp>
#include <QtSql>

#include "path.h"
#include "vmd5.h"
#include "vdisplayer.h"
#include "vadddatabase.h"


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
void addContentRecursively(Path p, mode m);


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
  @brief Affiche un dossier et son contenu récursivement
  */
void addContentRecursively(Path p, mode m)
{
    VAddDatabase * v = new VAddDatabase();              /// Crée un visiteur du type "Ajoueur à la BDD
    directory_iterator end;
    directory_iterator it(p);

    while(it != end)
    {
       try
       {
            if(m == recursive)
            {
                if(is_directory(*it) && !is_symlink(*it)) /// Si c'est un dossier et non un lien symbolique
                {
                    Path dir(*it);
                    addContentRecursively(dir, recursive);
                }
            }

            if (is_regular_file(*it) && file_size(*it) > 0) /// Si c'est un fichier régulier
                Path(*it).Accept(v);                    /// Accepte un visiteur pour se faire ajouter à la base de données
       }
        catch (const filesystem_error& ex)
        {
            cerr << *it << " Permission Denied !\n";
        }

        ++it;
    }
}





/**
  @brief liste les éléments de la base de données
  utilise une requête select puis affiche les pathname de tous les fichiers
*/
void lister()
{
    VDisplayer * v = new VDisplayer();          /// Crée un visiteur de type "Afficheur"

    QSqlQuery query;
    if(query.exec("SELECT filepath FROM " + QString( DataBase::instance().tableName() ) ) )
    {
        while(query.next())
        {

            for(int x=0; x < query.record().count(); ++x) /// pour chaque ligne de résultat de la requête...
                Path(query.value(x).toString().toStdString()).Accept(v);    /// Accepte le visiteur pour se faire afficher

        }
    }
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
    cout<<"Commande d'action"<<endl<<
    "   l   lister les fichiers (sera remplacé par doublons) du dossier passé en paramètre"<<endl<<
    "   m   afficher ce menu"<<endl<<
    "   u   mettre à jour la base de données"<<endl<<
    "   q   quitter sans enrengistrer les changements"<<endl;
}


