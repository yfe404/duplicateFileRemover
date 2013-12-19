#include <string>
#include <iostream>

#include <boost/filesystem.hpp>

#include <QSqlDatabase>
#include <QtSql>
#include <QtGlobal>

#include "database.h"


#define q2c(string) string.toStdString()


using namespace std;
using namespace boost::filesystem;



/** Définition des variables de classes */
string DataBase::dataBaseCreator = "./create_database.sh "; /// pour les scripts, l'espace avant les " est volontaire car les scripts prendront des arguments
string DataBase::m_name = (string(getenv("USER")) + ".db");
string DataBase::m_tableName = "files";



/**
  @brief Constructeur de la classe Singleton DataBase
  change les droits des scriptsqui vont etre executes et cree la base de données
  Grâce au pattern Singleton, il ne sera qu'une seule et unique fois (au premier appel d'une instance de DataBase)
  Ce pattern est utilisé car chaque utilisateur utilisant ce programme aura une seule et unique base de données contenant les informations sur ses fichiers
*/
DataBase::DataBase()
{
    father = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));

    /** Changement des droits sur les scripts afin que l'on puisse les exécuter */
    string cmd1 = string("chmod +x ") + dataBaseCreator;
    cout<< cmd1 <<endl;
    system(cmd1.c_str()); // CODERET À TESTER

    /** Appel du script pour créer la base de données sqlite */
    string cmd2 = dataBaseCreator + m_name + ' ' + m_tableName + string(" 2> /dev/null");
    cout<<cmd2<<endl;
    if(system(cmd2.c_str()) == -1)
    {
        cerr<<"Can't create database !"<<endl;
    }

    /** Paramétrage de la base de données */
    father->setHostName("localhost");
    father->setUserName( QString( getenv("USER")) );
    father->setPassword("");
    father->setDatabaseName( QString( m_name.c_str() ) );

    cout << "La base s'appelle : " << m_name << endl;
}


/**
  @brief accesseur du nom de la table

  @return m_tableName en const char*
*/
const char *DataBase::tableName() const{
    return m_tableName.c_str();
}


/**
  @brief ajoute un fichier dans la base de données
  Crée une requête insert into
*/
void DataBase::addNewFile(path p){
//    ouvrirDB();

    if (exists(p) && is_regular_file(p)) // A MODIF (un gros if c'est pas beau !)
    {

        // ici : méthode de DataBase => addFile(string filePath);
        QSqlQuery requete;

        requete.prepare("insert into " + QString(tableName()) + " values (?, ?, ?, ?)");

        requete.addBindValue( p.c_str() ); /// ajout du pathname
        requete.addBindValue( p.filename().c_str()); /// ajout du filename

        requete.addBindValue( quint64(last_write_time(p))  ); /// date de dernière modification

        requete.addBindValue(file_size(p)); /// taille
        //        requete.addBindValue( QString( md5Key.c_str() ) ); /// clé md5


        if (!requete.exec())
            std::cerr << "Erreur ajout base de données" << std::endl;
    }

//    fermerDB();
}



/**
  @brief ouverture de la base de données

  Ouvre la base de données et affiche un message en fonction du bon fonctionnement (ou non) de la méthode.

  @return true la base est ouverte
  @return false il y a eu un problème durant l'ouverture
*/
bool DataBase::ouvrirDB(){
    if( father->open() ) /// Appelle la méthode open de QSqlDatabase et retourne la valeur retournée par cette dernière.
    {
        std::cout << "Vous êtes maintenant connecté à " << q2c(father->hostName()) << std::endl;
        return true;
    }
    else {
        std::cout << "La connexion a échouée, désolé :(" << std::endl << q2c(father->lastError().text()) << std::endl;
        return false;
    }
}


/**
  @brief Fermeture de la base de données
    Ferme la connexion à la base de données, libérant toutes les ressources acquises, et rendant invalide tout objet QQuery utilisant la base.
*/
void DataBase::fermerDB(){
    father->close();
}



/**
  @brief Destructeur
  Détruit le père (QSqlDatabase)
*/
DataBase::~DataBase()
{
    delete father;
}

