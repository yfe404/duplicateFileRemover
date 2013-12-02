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

#define UPDATER_EXECUTABLE "./update.sh " // l'espace est volontaire est important (il y aura des arguments !)
#define DATABASE_CREATOR_EXECUTABLE "./create_database.sh "

#define DATABASENAME string(string(getenv("USER")) + ".db") // N.B : pour utiliser getenv en-dehors du define, il faut inclure cstdlib
#define TABLENAME string("files")
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
    QCoreApplication app(countArg, listArg);

    menu();

    return app.exec();
}




void update() // MÉTHODE À RAPETISSIR !!
{

    ifstream config(CONFIGFILE);
    if(!config)
    {
        cerr<<"Erreur ouverture fichier de configuration"<<endl;
    }
    string parcours_configfile;
    string cmd = UPDATER_EXECUTABLE ;   // commande qui sera lancée pour appeler le script avec le nom des dossiers à parcourir


    getline(config, parcours_configfile);
    if(parcours_configfile.size() == 0)
    {
        cerr<<"Fichier de configuration vide !"<<endl;
    }


    while(parcours_configfile.size() != 0)
    {
        cmd += parcours_configfile + ' ';
        getline(config, parcours_configfile);
    }


    if(system(cmd.c_str())==-1)
    {
        cerr<<"Erreur execution fichier : update.sh"<<endl;
    }

    string filepath;
    string md5Key;
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

    getline(pathnames, filepath);
    getline(md5sum, md5Key);



    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE"); // CREATION DE LA BD A VIRER (car singleton)
    db.setHostName("localhost");
    db.setUserName(QString(USER.c_str()));
    db.setPassword("");
    db.setDatabaseName(QString(DATABASENAME.c_str()));
    if(db.open())
    {
        std::cout << "Vous êtes maintenant connecté à " << q2c(db.hostName()) << std::endl;
    }
    else
    {
        std::cout << "La connexion a échouée, désolé :(" << std::endl << q2c(db.lastError().text()) << std::endl;
    }


    while(filepath.size() != 0)
    {
// ici : méthode de DataBase
        QSqlQuery q;
        QFileInfo fichier(filepath.c_str());


        q.prepare("insert into " + QString(TABLENAME.c_str()) + " values (?, ?, ?, ?)");
        q.addBindValue(QString(filepath.c_str()));
        q.addBindValue(fichier.baseName());
        q.addBindValue(fichier.lastModified().toString("dd MMMM yyyy hh-mm-ss"));
        q.addBindValue(QString(md5Key.c_str()));

        getline(pathnames, filepath);
        getline(md5sum, md5Key);

        if (q.exec())
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

    db.close();


} // ferme automatiquement tous les flux



void lister()
{

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE"); // CREATION DANS LE SINGLETON
    db.setHostName("localhost");
    db.setUserName(QString(USER.c_str()));
    db.setPassword("");
    db.setDatabaseName(QString(DATABASENAME.c_str()));
    if(db.open())
    {
        std::cout << "Vous êtes maintenant connecté à " << q2c(db.hostName()) << std::endl;
    }
    else
    {
        std::cout << "La connexion a échouée, désolé :(" << std::endl << q2c(db.lastError().text()) << std::endl;
    }

    QSqlQuery query;
    if(query.exec("SELECT filepath FROM " + QString(TABLENAME.c_str())))
    {
        while(query.next())
        {
            std::cout << "    Nouvelle entrée" << std::endl;
            for(int x=0; x < query.record().count(); ++x)
            {
                std::cout << "        " << query.record().fieldName(x).toStdString() << " = " << query.value(x).toString().toStdString() << std::endl;
            }
        }
    }

    db.close();

}




void menu()
{
    char choix = 'm';


    while(choix!='q')
    {
        cout<<"Commande (m pour l'aide): ";
        scanf("%c%*c", &choix); // CODERET A GERER (réglerait le problème qui suit ?)  // Problème : si l'utilisateur entre une chaine de caractères, pas géré !
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
            // quitter
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







