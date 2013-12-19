#include <iostream>
#include<fstream>

#include <QFileInfo>
#include <QStringList>
#include <QDateTime>
#include <QCoreApplication>
#include <QtSql>

#include "database.h"

#define DB_SEP ':'  // A SUPPRIMER ? (comme on utilise sqlite3 désormais)      // séparateur dans le fichier qui servira de base de donnée en attendant sqlite
#define CONFIGFILE ".config"        // path du fichier de configuration; sera bien entendu placé ailleur sur la version finale

#define USER string(getenv("USER"))

//La macro-définition q2c est simplement faite pour faciliter la conversion de QString vers std::string
#define q2c(string) string.toStdString()



using namespace std;


void menu();
void update();
void afficherAide();
void lister();



int main(int countArg, char **listArg)
{

    menu();

    return 0;
}




void update() // MÉTHODE À RAPETISSIR !!; AVEC TOUS LES TESTS : EXCEPTIONS
{

    /** Sélection des dossiers à scanner à l'aide du fichier de configuration */ // MÉTHODE  string configure();
    ifstream config(CONFIGFILE); /// création d'un flux pour la manipulation du fichier de configuration

    if(!config)
        cerr<<"Erreur ouverture fichier de configuration"<<endl;

    string parcours_configfile;
    string update = DataBase::instance().updater();   /// commande qui sera lancée pour appeler le script avec le nom des dossiers à parcourir


    getline(config, parcours_configfile); /// récupère une ligne du fichier de configuration
    if(parcours_configfile.size() == 0)
    {
        cerr<<"Fichier de configuration vide !"<<endl;
    }


    /** parcours du fichier de configuration pour connaître les autres dossiers à scanner */
    while(parcours_configfile.size() != 0)
    {
        update += parcours_configfile + ' ';
        getline(config, parcours_configfile);
    }


    /**
    Appel du script update.sh pour les dossiers sélectionnés dans le fichier de configuration
    Update
*/
    if(system(update.c_str())==-1)
    {
        cerr<<"Erreur execution fichier : update.sh"<<endl;
    }



    /** Parcours des bases pathnames.db et md5.db */
    ifstream pathnames("pathnames.db");
    if(!pathnames)
    {
        cerr<<"Erreur ouverture fichier : pathnames.db"<<endl;
    }


    ifstream md5sum("md5.db");
    if(!md5sum)
    {
        cerr<<"Erreur ouverture fichier : md5.db"<<endl;
    }

    string filepath;
    string md5Key;
    getline(pathnames, filepath);
    getline(md5sum, md5Key);


    DataBase::instance().ouvrirDB(); /// On ouvre la base de données pour pouvoir ensuite faire des requêtes dessus


    while(filepath.size() != 0) // && md5key.size() != 0
    {
// ici : méthode de DataBase => addFile(string filePath);
        QSqlQuery requete;
        QFileInfo fichier(filepath.c_str());


        requete.prepare("insert into " + QString(DataBase::instance().tableName() ) + " values (?, ?, ?, ?)");

        requete.addBindValue( QString( filepath.c_str() ) );
        requete.addBindValue(fichier.baseName());
        requete.addBindValue(fichier.lastModified().toTime_t());
        requete.addBindValue( QString( md5Key.c_str() ) );

        getline(pathnames, filepath);
        getline(md5sum, md5Key);

        if (requete.exec())
        {
            //std::cout << "Ça marche ! :)" << std::endl;
        }
        else
        {
            std::cerr << "Erreur ajout base de données" << std::endl;
        }


    }

    system("rm pathnames.db 2> /dev/null"); // WARNING VALEUR DE RETOUR AUX DEUX APPELS
    system("rm md5.db 2> /dev/null");

    DataBase::instance().fermerDB();

} /// ferme automatiquement tous les flux



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







