#include "database.h"

#include <iostream>
#include<fstream>

#include <boost/filesystem.hpp>

//#include <QFileInfo>
//#include <QStringList>
//#include <QDateTime>
//#include <QCoreApplication>
//#include <QSqlQuery>
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
void cleMd5(QString pathname);
void addContentRecursively(path p, mode m);


int main()
{

    menu();

    return 0;
}




void update(){

    /** Sélection des dossiers à scanner à l'aide du fichier de configuration */
    ifstream config(CONFIGFILE); /// création d'un flux pour la manipulation du fichier de configuration

    if(!config)
        cerr<<"Erreur ouverture fichier de configuration"<<endl;

    string parcours_configfile, filesToSee;

    getline(config, parcours_configfile); /// récupère une ligne du fichier de configuration
    if(parcours_configfile.size() == 0)
    {
        cerr<<"Fichier de configuration vide !"<<endl;
    }

    DataBase::instance().ouvrirDB(); /// On ouvre la base de données principale pour pouvoir ensuite faire des requêtes dessus


    /** Scan des dossiers sélectionnés */
    while(parcours_configfile.size() != 0)
    {
        path p(parcours_configfile);
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

    DataBase::instance().fermerDB();
} /// ferme automatiquement tous les flux


/**
  @brief Affiche un dossier et son contenu récursivement
  */
void addContentRecursively(path p, mode m)
{
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
                    path dir(*it);
                    addContentRecursively(dir, recursive);
                }
            }

            if (is_regular_file(*it)) /// Si c'est un fichier régulier
                DataBase::instance().addNewFile( *it ); /// on ajoute ce fichier dans la base
       }
        catch (const filesystem_error& ex)
        {
            cerr << *it << " Permission Denied !\n";
        }

        ++it;
    }
}



/**
  @brief Calcul et affichage de la clé md5sum du fichier passé en paramètre
  utilisation de QByteArray et QCryptographicHasg pour le calcul
*/
void cleMd5(QString pathname)
{
    QFile fFichier(pathname);
    fFichier.open(QIODevice::ReadOnly);
    QByteArray contenuFichier = fFichier.readAll();
    QByteArray hashData = QCryptographicHash::hash(contenuFichier,QCryptographicHash::Md5);
    cout << QString(hashData.toHex()).toStdString();
}


/**
  @brief liste les éléments de la base de données
  utilise une requête select puis affiche les pathname de tous les fichiers
*/
void lister()
{
    if ( !DataBase::instance().ouvrirDB() )  { /// On ouvre la base de données pour pouvoir ensuite faire des requêtes dessus
        cout << "Erreur ouverture de la base de données";
        return;
    }

    QSqlQuery query;
    if(query.exec("SELECT filepath FROM " + QString( DataBase::instance().tableName() ) ) )
    {
        while(query.next())
        {
            cout << "    Nouvelle entrée" << std::endl;
            for(int x=0; x < query.record().count(); ++x) /// pour chaque ligne de résultat de la requête...
                cout << "        " << query.record().fieldName(x).toStdString() << " = " << query.value(x).toString().toStdString() << endl; /// affiche le pathname, et le numéro du champ sélectionné // 0?

        }
    }

    DataBase::instance().fermerDB();

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
            lister();
        break;
        case 'u' :
            update();
        case 'm' :
            afficherAide();
        break;
        case 'q' :
            return;
        break;
        default:
            cout<<choix<<" : commande inconnue"<<endl;

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


