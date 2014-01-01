#include <string>
#include <iostream>
#include<fstream>

//#include <QtGlobal>

#include "database.h"
#include "extended_filesystem.h"

#define TFILES string("TFILES")
#define DATABASE_NAME string(string(getenv("USER")) + string(".db"))
#define CONFIGFILE ".config"        // path du fichier de configuration; sera bien entendu placé ailleur sur la version finale
#define CREATE_FILE_DB string("echo  \"CREATE TABLE IF NOT EXISTS " + TFILES  + " (filepath varchar(255) primary key, parentpath varchar(255), filename varchar(255), lastmodified varchar(255), size integer, md5sum varchar(255));\" | /usr/bin/sqlite3" +" " + DATABASE_NAME)

using namespace std;
using namespace boost::filesystem;


/**
  @brief Constructeur de la classe Singleton DataBase
  change les droits des scriptsqui vont etre executes et cree la base de données
  Grâce au pattern Singleton, il ne sera qu'une seule et unique fois (au premier appel d'une instance de DataBase)
  Ce pattern est utilisé car chaque utilisateur utilisant ce programme aura une seule et unique base de données contenant les informations sur ses fichiers
*/
DataBase::DataBase()
{
    father = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));


    /** Appel du script pour créer la base de données sqlite */

    if(system(CREATE_FILE_DB.c_str()) == -1)
    {
        cerr<<"Can't create database !"<<endl;
    }

    /** Paramétrage de la base de données */
    father->setHostName("localhost");
    father->setUserName( QString( getenv("USER")) );
    father->setPassword("");
    father->setDatabaseName( QString(DATABASE_NAME.c_str()) );

    //cout << "La base s'appelle : " << m_name << endl;
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
        //std::cout << "Vous êtes maintenant connecté à " << q2c(father->hostName()) << std::endl;
        return true;
    }
    else {
        std::cout << "La connexion a échouée, désolé :(" << std::endl << father->lastError().text().toStdString() << std::endl;
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

bool DataBase::commit()
{
    return father->commit();
}

bool DataBase::rollback()
{
    return father->rollback();
}

bool DataBase::transaction()
{
    return father->transaction();
}



/**
  @brief Met à jour la base de données des fichiers.
*/
void DataBase::update(){

    /** Sélection des dossiers à scanner à l'aide du fichier de configuration */
    ifstream config(CONFIGFILE); /// création d'un flux pour la manipulation du fichier de configuration

    if(!config)
    {
        cerr<<"Erreur ouverture fichier de configuration"<<endl;
    }

    string parcours_configfile;

    getline(config, parcours_configfile); /// récupère une ligne du fichier de configuration
    if(parcours_configfile.size() == 0)
    {
        cerr<<"Fichier de configuration vide !"<<endl;
    }

    list<boost::filesystem::path*> *liste = new list<boost::filesystem::path*> ;

    /** Scan des dossiers sélectionnés */
    while(parcours_configfile.size() != 0)
    {

        boost::filesystem::path rep(parcours_configfile);
        try
        {
            if (exists(rep))    /// Si le chemin existe
            {
               addContentRecursively(rep, liste, recursive); /// ajout des fichiers de manière récursive ou pas à la liste
            }

            else
              cout << rep << " does not exist\n";
        }
        catch(const filesystem_error& ex)
        {
            cerr << rep << " Permission Denied !\n";
        }


        getline(config, parcours_configfile); /// continue le parcours du fichier


    }
    DataBase::instance().transaction();       /// Début de transaction

    QSqlQuery requete;

    /// Insert ou met à jour dans la base si la clé existe déjà
    requete.prepare("INSERT OR REPLACE INTO " + QString(TFILES.c_str()) + " values (?, ?, ?, ?, ?, ?)");

    for(list<boost::filesystem::path*>::iterator it = liste->begin(); it != liste->end(); ++it)
    {
            requete.addBindValue( (*it)->c_str() ); /// ajout du pathname
            requete.addBindValue( (*it)->parent_path().c_str() ); /// ajout du pathname du parent
            requete.addBindValue((*it)->filename().c_str()); /// ajout du filename
            requete.addBindValue( quint64(last_write_time(*(*it)))  ); /// date de dernière modification
            requete.addBindValue(quint64(file_size(*(*it)))); /// taille
            requete.addBindValue("");                   /// Vide pour le moment
            requete.exec();
    }

    if(!DataBase::instance().commit())                          /// Commit (seul accès à la base)
    {
        std::cerr << "Erreur mise à jour " << requete.lastError().text().toStdString()<< std::endl;
        DataBase::instance().rollback();

    }


    delete liste;       /// @todo deleter tous les pointeurs de la liste
    config.close();
}





/**
  @brief liste les éléments de la base de données
  utilise une requête select puis affiche les pathname de tous les fichiers
*/
void DataBase::listerFichiers()
{

    QSqlQuery query;
    if(query.exec("SELECT filepath FROM " + QString(TFILES.c_str()) ) )
    {
        while(query.next())
        {

            for(int x=0; x < query.record().count(); ++x) /// pour chaque ligne de résultat de la requête...
                cout<<boost::filesystem::path(query.value(x).toString().toStdString())<<endl;    /// Accepte le visiteur pour se faire afficher

        }
    }
}


/**
  @brief liste les éléments de la base de données qui on des doublons de taille ( 1 fichier par taille )
*/
void DataBase::listerDoublonsTaille()
{


    QSqlQuery query;
    if(query.exec("SELECT COUNT(size) AS nb_doublon, filepath  FROM " + QString(TFILES.c_str()) +" GROUP BY size HAVING COUNT(size) > 1" ) )
    {
        //cout<<query.record().count()<<endl;
        while(query.next())
        {
                boost::filesystem::path p(query.value(1).toString().toStdString());
                cout<<"= > Taille "<<file_size(p)<< " octets : "<<endl;
                QSqlQuery q;
                q.prepare("SELECT filepath FROM " + QString(TFILES.c_str()) + " WHERE size = ?");
                q.addBindValue(quint64(file_size(p))); /// taille
                if(q.exec())
                {
                    while(q.next())
                    {
                        cout<<"\t"+q.value(0).toString().toStdString()<<endl;
                    }
                }
                else
                {

                    cout<<q.lastQuery().toStdString()<<endl;
                    cout<<q.lastError().text().toStdString()<<endl;
                }

        }
    }
    else
    {
        cout<<query.lastError().text().toStdString()<<endl;
    }
}



void DataBase::updateMD5(std::list<boost::filesystem::path *> &liste)
{
    DataBase::instance().transaction();

    QSqlQuery requete;
    boost::filesystem::path* fic;

    /// Insert ou met à jour dans la base si la clé existe déjà
    requete.prepare("UPDATE " + QString(TFILES.c_str()) + " SET md5sum=? WHERE filepath=?");

    for(list<boost::filesystem::path*>::iterator it = liste.begin(); it != liste.end(); ++it)
    {
        fic = new boost::filesystem::path((*it)->c_str());
        requete.addBindValue( QString(md5sum(*fic).c_str()) ); /// ajout de la clé md5
        requete.addBindValue(fic->c_str()); /// ajout du filename
        requete.exec();
        //cout<<requete.executedQuery().toStdString()<<endl;
        delete fic;


    }

    if(!DataBase::instance().commit())
    {
        std::cerr << "Erreur mise à jour " << requete.lastError().text().toStdString()<< std::endl;
        DataBase::instance().rollback();

    }


}





list<boost::filesystem::path *>& DataBase::getListSizeDuplicate(void)
{
    QSqlQuery query;
    list<boost::filesystem::path*> *liste = new list<boost::filesystem::path*>();
    boost::filesystem::path* p;

    if(query.exec("select filepath from " + QString(TFILES.c_str()) + " where size in (select  size from " + QString(TFILES.c_str()) + " group by size having count(size) > 1)"))
    {
        //cout<<query.record().count()<<endl;
        while(query.next())
        {
            try{
                p = new boost::filesystem::path(query.value(0).toString().toStdString());
                liste->push_back(p);
                //cout<<(*liste.begin())->c_str()<<endl;
            }
            catch (const filesystem_error& ex)
            {
               cerr<<" Permission Denied !\n";
            }

        }
    }
    else
    {
        cout<<query.lastError().text().toStdString()<<endl;
    }

    //cout<<liste->size()<<endl;

    return *liste;
}






/**
  @brief liste les doublons de la base de données
*/
void DataBase::listerDoublons()
{


    QSqlQuery query;
    if(query.exec("SELECT COUNT(md5sum) AS nb_doublon, filepath  FROM " + QString(TFILES.c_str()) +" GROUP BY md5sum HAVING COUNT(md5sum) > 1" ) )
    {
        cout<<query.lastQuery().toStdString()<<endl;
        while(query.next())
        {
                boost::filesystem::path p(query.value(1).toString().toStdString());
                cout<<"= > clé md5 : "<<md5sum(p)<< " : "<<endl;
                QSqlQuery q;
                q.prepare("SELECT filepath FROM " + QString(TFILES.c_str()) + " WHERE md5sum = ?");
                q.addBindValue(QString(md5sum(p).c_str())); /// clé md5
                if(q.exec())
                {
                    while(q.next())
                    {
                        cout<<"\t"+q.value(0).toString().toStdString()<<endl;
                    }
                }
                else
                {

                    cout<<q.lastQuery().toStdString()<<endl;
                    cout<<q.lastError().text().toStdString()<<endl;
                }

        }
    }
    else
    {
        cout<<query.lastError().text().toStdString()<<endl;
    }
}


std::list<boost::filesystem::path *>& DataBase::getListParents(void)
{
    QSqlQuery query;
    list<boost::filesystem::path*> *liste = new list<boost::filesystem::path*>();
    boost::filesystem::path* p;

    if(query.exec("SELECT DISTINCT parentpath FROM " + QString(TFILES.c_str()) + " WHERE md5sum != \"\""))
    {
        //cout<<query.record().count()<<endl;
        while(query.next())
        {
            try{
                p = new boost::filesystem::path(query.value(0).toString().toStdString());
                QSqlQuery q;
                q.prepare("SELECT COUNT(md5sum) FROM " + QString(TFILES.c_str()) + " WHERE parentpath = ?");
                q.addBindValue(QString(p->c_str())); /// Nombre de clé ayant pour parent p
                if(q.exec())
                {
                    while(q.next()) /// Un seul résultat donc un seul passage dans la boucle
                    {
                        //cout<<"\t"+q.value(0).toString().toStdString()<<endl;
                        if(q.value(0).toInt() == getNbFiles(*p))
                            liste->push_back(p);
                            //cout<<*p<<endl;
                    }
                }
                else
                {

                    cout<<q.lastQuery().toStdString()<<endl;
                    cout<<q.lastError().text().toStdString()<<endl;
                }

            }
            catch (const filesystem_error& ex)
            {
               cerr<<" Permission Denied !\n";
            }


        }
    }
    else
    {
        cout<<query.lastError().text().toStdString()<<endl;
    }

    cout<<liste->size()<<endl;
    return *liste;
}








