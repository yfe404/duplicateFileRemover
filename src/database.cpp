#include "database.h"
#include "extended_filesystem.h"

#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include <QDebug>
#include <boost/filesystem.hpp>

//#define QT_NO_DEBUG

#ifndef QT_NO_DEBUG                         /// indique que l'on est en mode debug
#define DEBUG(msg) qDebug(qPrintable(msg)) // pt également qDebug() << mess...;
#define FATAL_ERROR(msg) qFatal(qPrintable(msg))
#define CRITICAL(msg) qCritical(qPrintable(msg)) // pt également qCritical() << mess...;

#else /// QT_NO_DEBUG
// DANS CE CAS, 'd' (doublons) n'affiche plus les fichiers doublons ! (juste les 'open() : _____')
#define DEBUG(msg)
#define FATAL_ERROR(msg)
#define CRITICAL(msg)

#endif




#define TFILES string("TFILES")
#define DATABASE_NAME string(string(getenv("USER")) + string(".db"))
#define CONFIGFILE ".config"        // path du fichier de configuration; sera bien entendu placé ailleur sur la version finale

#define CREATE_FILE_DB string("echo  \"CREATE TABLE IF NOT EXISTS " + TFILES  + " (filepath varchar(255) primary key, filename varchar(255), lastmodified varchar(255), size integer, md5sum varchar(255));\" | /usr/bin/sqlite3 " + DATABASE_NAME)


/// un seul using namespace pour éviter les conflits
using namespace boost::filesystem;

using std::string;
using std::list;
using std::ifstream;



/**
  @brief Constructeur de la classe Singleton DataBase
  cree la base de données
  Grâce au pattern Singleton, il ne sera qu'une seule et unique fois (au premier appel d'une instance de DataBase)
  Ce pattern est utilisé car chaque utilisateur utilisant ce programme aura une seule et unique base de données contenant les informations sur ses fichiers
*/
DataBase::DataBase()
{
    DEBUG(QObject::trUtf8("Appel du constructeur de DataBase", "Constructeur de la base de données"));
    m_lastError = "";   /// Initialisation du champ d'erreur à chaine vide

    m_databaseobject = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    Q_ASSERT_X(m_databaseobject!=NULL, "DataBase::DataBase()", "m_lastError == NULL");    /// Quitte le programme si le pointeur vaut NULL


    /** Appel du script pour créer la base de données sqlite */

    DEBUG(QObject::trUtf8("Création de la base de données", "Constructeur de la base de données"));
    if(system(CREATE_FILE_DB.c_str()) == -1)
    {
        /// @todo Le programme devra quitter si la base de donnée n'a pas pu être créée.
        //! Idée : lancer une exception
        FATAL_ERROR(QObject::trUtf8("Echec de création de la base de données", "Constructeur de la base de données"));
        setLastError(QObject::trUtf8("Impossible de créer la base de données, vérifiez que sqlite3 est bien installé sur votre ordinateur."));
        return;
    }


    /** Paramétrage de la base de données */

    DEBUG(QObject::trUtf8("Configuration de la base de données"));
    m_databaseobject->setHostName("localhost");

    DEBUG(QObject::trUtf8(qPrintable("Nom d'hôte : " + m_databaseobject->hostName())));
    m_databaseobject->setUserName( QString( getenv("USER")));

    DEBUG(QObject::trUtf8(qPrintable("Nom d'utilisateur : " + m_databaseobject->userName())));
    m_databaseobject->setPassword("");

    DEBUG(QObject::trUtf8(qPrintable("Mot de passe : " + m_databaseobject->password())));
    m_databaseobject->setDatabaseName( QString(DATABASE_NAME.c_str()) );

    DEBUG(QObject::trUtf8(qPrintable("Nom de la base de données : " + m_databaseobject->databaseName())));
}




/**
  @brief ouverture de la base de données

  Ouvre la base de données (en appelant la méthode de QSqlDatabase) et affiche un message en fonction du bon fonctionnement (ou non) de la méthode.
*/
void DataBase::ouvrirDB()
{
    DEBUG(QObject::trUtf8("Ouverture de la base de données"));

    if( !m_databaseobject->open() )
    {
        FATAL_ERROR(QObject::trUtf8("Echec ouverture de la base de données", "ouvrirDB()")); /// quitte le programme (via qFatal) en affichant un message d'erreur
        QString err = QObject::trUtf8("Echec ouverture de la base de données : ") + m_databaseobject->lastError().text();
        setLastError(err);
        return ;
    }

}


/**
  @brief Fermeture de la base de données
    Ferme la connexion à la base de données, libérant toutes les ressources acquises, et rendant invalide tout objet QQuery utilisant la base.
*/
void DataBase::fermerDB(){
    DEBUG(QObject::trUtf8("Fermeture de la base de données"));
    m_databaseobject->close();
}


const QString& DataBase::lastError()
{
    return m_lastError;
}

void DataBase::setLastError(const QString& msg) /// @todo utiliser C++ 2011 (rvalue&&) pour éviter la recopie de la chaine passée en paramètre
{
    DEBUG(QObject::trUtf8(qPrintable("DataBase::setLastError : " + msg)));
    m_lastError = msg;
}



bool DataBase::commit()
{
    DEBUG(QObject::trUtf8("DataBase::commit()"));
    return m_databaseobject->commit();
}

bool DataBase::rollback()
{
    DEBUG(QObject::trUtf8("DataBase::rollback()"));
    return m_databaseobject->rollback();
}

bool DataBase::transaction()
{
    DEBUG(QObject::trUtf8("DataBase::transaction()"));
    return m_databaseobject->transaction();
}



/**
  @brief Met à jour la base de données des fichiers.
*/

/// @todo doit accepter un nom de dossier, si aucun dossier n'est spécifié, mettre à jour à l'aide du fichier de configuration
void DataBase::update()
{

    DEBUG(QObject::trUtf8("Tentative de mise à jour de la base de données", "DataBase::update()"));

    DEBUG(QObject::trUtf8("Sélection des dossiers à scanner à l'aide du fichier de configuration"));

    /** Parcours du fichier de configuration */
    ifstream config(CONFIGFILE); /// création d'un flux pour la manipulation du fichier de configuration

    if(!config)
        setLastError(QObject::trUtf8("Erreur ouverture fichier de configuration, veuillez choisir 'Configurer' dans le menu principal pour remédier à ce problème."));

    string parcours_configfile;

    DEBUG(QObject::trUtf8("Lecture d'une ligne du fichier de configuration"));
    getline(config, parcours_configfile); /// récupère une ligne du fichier de configuration

    /// @todo les lignes commentées suivantes seront incluses dans une fonction de vérification du fichier de
    /// configuration, voir les lignes suivantes pour plus d'informations sur cette fonction.

    // if(parcours_configfile.size() == 0)
        //cerr<<"Fichier de configuration vide !"<<endl;



    /** Récupération des dossiers sélectionnés, parcours et ajout à notre liste */

    list<path*> *lFiles = new list<path*>;
    Q_ASSERT_X(lFiles!=NULL, "DataBase::update()", "lFiles == NULL");

    while( parcours_configfile.size() != 0 )
    {
        path rep(parcours_configfile);
        DEBUG(QObject::trUtf8(qPrintable("Parcours du dossier " + QString(rep.c_str()))));

        /// @todo les vérifications suivantes (lignes commentées doivent êtres insérées dans une fonction qui fera la vérification
        ///du fichier de configuration à chaque update et à chaque fin d'édition du fichier de configuration.
        /// et eventuellemnt à chaque démarrage du programme dans le cas ou le fichier de configuration aurait
        /// été édité à la main en dehors du programme.
        /*
        try
        {
            if ( exists(rep) )    /// Si le chemin existe*/
        DEBUG(QObject::trUtf8(qPrintable("Ajout des fichiers de " + QString(rep.c_str())+ " à la liste des fichiers")));
        addContentRecursively(rep, lFiles, recursive);
        /*
            else
              cout << rep << " does not exist\n";
        }
        catch(const filesystem_error& ex)
        {
            cerr << rep << " Permission Denied !\n";
        }

        */
        DEBUG(QObject::trUtf8("Lecture d'une ligne du fichier de configuration"));
        getline(config, parcours_configfile);
    }

    DEBUG(QObject::trUtf8("Fin de lecture du fichier de configuration"));
    DEBUG(QObject::trUtf8("Nombre de fichiers trouvés : %n", "", lFiles->size()));



    DEBUG(QObject::trUtf8("Tentative d'ajout des fichiers à la base de données"));

    DataBase::instance().transaction();
    QSqlQuery query;

    query.prepare("INSERT OR REPLACE INTO " + QString(TFILES.c_str()) + " values (?, ?, ?, ?, ?)");    /// Insert ou met à jour dans la base si la clé existe déjà
    DEBUG(QObject::trUtf8(qPrintable("Requête lancée : " + query.lastQuery() )));

    for(list<path*>::iterator file = lFiles->begin(); file != lFiles->end(); ++file)
    {
            query.addBindValue( (*file)->c_str() ); /// ajout du pathname
            query.addBindValue((*file)->filename().c_str()); /// ajout du filename
            query.addBindValue( quint64(last_write_time(*(*file)))  ); /// date de dernière modification
            query.addBindValue(quint64(file_size(*(*file)))); /// taille
            query.addBindValue("");                   /// Vide pour le moment (future place de la clé md5)

            query.exec();
    }

    if(!DataBase::instance().commit())
    {
        DataBase::instance().rollback();
        CRITICAL(QObject::trUtf8(qPrintable("Erreur d'ajout à la base de données : " + query.lastError().text() )));
        setLastError(QObject::trUtf8(qPrintable("Erreur de mise à jour de la base de données : " + query.lastError().text())));
    }

    /// @todo Ces opération doivent être faites si l'on sort en cas d'erreur, il faudra donc gérer ce cas
    delete lFiles;       /// @todo deleter tous les pointeurs de la liste

    DEBUG(QObject::trUtf8("Fermeture du fichier de configuration"));
    config.close();
}

/**
  @brief Vérifier la validité du fichier de configuration

  @return true si le fichier de configuration est valide
  @return false sinon
*/
/// @todo utiliser la méthode dans update (plutôt que de parcourir 2 fois le fichier)
bool DataBase::verifierConfiguration()
{
    DEBUG(QObject::trUtf8("Tentative d'ouverture du fichier de configuration"));
    ifstream config(CONFIGFILE);

    if(!config)
    {
        setLastError( QObject::trUtf8("Erreur ouverture fichier de configuration, veuillez choisir 'Configurer' dans le menu principal pour remédier à ce problème.") );
        return false;
    }

    string parcours_configfile;

    DEBUG(QObject::trUtf8("Lecture d'une ligne du fichier de configuration"));
    getline(config, parcours_configfile); /// récupère une ligne du fichier de configuration

    if(parcours_configfile.size() == 0)
    {
        CRITICAL(QObject::trUtf8("Fichier de configuration vide !"));
        return false;
    }

    return true;
}





/**
  @brief mise à jour des clés md5

  @param filesToUpdate liste des fichiers de la base dont il faut mettre à jour la valeur de la clé md5
*/
void DataBase::updateMD5(list<path *> &filesToUpdate)
{
    DataBase::instance().transaction(); /// début de la transaction

    QSqlQuery updateMd5;
    path* fic;

    /// mise à jour des clés md5 pour les fichiers de la listes
    updateMd5.prepare("UPDATE " + QString(TFILES.c_str()) + " SET md5sum=? WHERE filepath=?");

    for(list<path*>::iterator it = filesToUpdate.begin(); it != filesToUpdate.end(); ++it)
    {
        fic = new path( (*it)->c_str() );

        updateMd5.addBindValue( QString( md5sum(*fic).c_str() ) ); /// ajout de la clé md5
        updateMd5.addBindValue( fic->c_str() ); /// ajout du pathname
        updateMd5.exec();
        //cout<<requete.executedQuery().toStdString()<<endl;

        delete fic;
    }

    if(!DataBase::instance().commit())
    {
        DataBase::instance().rollback();
        setLastError(QObject::trUtf8(qPrintable("Erreur d'accès à la base de donnée " + updateMd5.lastError().text()), "Recherche des doublons dans la base"));
    }
}




/**
  @brief crée et retourne la liste des fichiers de même taille

  @return equalsFile contenant les fichiers de même taille dans la base
*/
list<path *>& DataBase::getListSizeDuplicate()
{
    QSqlQuery query;
    list<path*> *equalsFile = new list<path*>(); // ATTENTION FUITE MÉMOIRE
    path *file;

    if( query.exec("select filepath from " + QString(TFILES.c_str()) + " where size in (select  size from " + QString(TFILES.c_str()) + " group by size having count(size) > 1)") )
    {
        //cout<<query.record().count()<<endl;
        while(query.next())
        {
            try{
                file = new path(query.value(0).toString().toStdString());
                equalsFile->push_back(file);
                //cout<<(*liste.begin())->c_str()<<endl;
            }
            catch (const filesystem_error& ex)
            {
               // debug => cerr<<" Permission Denied !\n";
            }

        }
    }
    else
        setLastError(QObject::trUtf8(qPrintable("Erreur d'accès à la base de donnée " + query.lastError().text()), "Recherche des doublons dans la base"));

    // Debug => cout<<liste->size()<<endl;


    return *equalsFile;
}


/**
  @brief recherche les doublons dans la base de données, en fonction de la clé md5 (+ taille)
  */
void DataBase::rechercherDoublons(std::multimap<string, path*>& map)
{
    DEBUG(QObject::trUtf8("Mise à jour des clés md5 des fichiers de tailles équivalentes"));

    list<path *> dupSize; /// variable utilisée seulement pour afficher le nombre de clés md5 à calculer (rvalue (comme tempo)??)
    //! @todo une fois que nous ne voudrons plus afficher le nombre de clé md5 à calculer, directement appeler 'updateMDR( getListSizeDuplicate() );'
    dupSize = getListSizeDuplicate();
    DEBUG(QObject::trUtf8("Nombre de clés md5 à calculer : %n", "", dupSize.size()));

    updateMD5(dupSize); /// met à jour la clé md5 pour les fichiers de même taille

    DEBUG(QObject::trUtf8("Recherche de doublons en cours..."));

    QSqlQuery selectGroupMd5;
    if( selectGroupMd5.exec("SELECT COUNT(md5sum) AS nb_doublon, filepath  FROM " + QString(TFILES.c_str()) +" GROUP BY md5sum HAVING COUNT(md5sum) > 1" ) )
    {
        DEBUG(QObject::trUtf8("Utilisation de la requête : ") + selectGroupMd5.lastQuery());

        while(selectGroupMd5.next())
        { //! @todo une fonction dans le while 'foo(path p)'
                path p(selectGroupMd5.value(1).toString().toStdString());
                string key = md5sum(p);
                DEBUG(QObject::trUtf8("Nouvelle clé calculée : ") + qPrintable(key.c_str()));

                QSqlQuery selectDoublons;
                DEBUG(QObject::trUtf8("Récupération des fichiers correspondants..."));
                selectDoublons.prepare("SELECT filepath FROM " + QString(TFILES.c_str()) + " WHERE md5sum = ?");
                selectDoublons.addBindValue(QString(key.c_str())); /// clé md5

                if(selectDoublons.exec())
                {

                    while(selectDoublons.next())
                    {
                        path *fic = new path(selectGroupMd5.value(1).toString().toStdString());
                        Q_ASSERT_X(fic!=NULL, "DataBase::rechercherDoublons()", "fic == NULL");    /// Quitte le programme si le pointeur vaut NULL
                        map.insert(std::pair<string,path*> (key, fic));
                    }

                }
                else
                {
                    setLastError(QObject::trUtf8(qPrintable("Erreur d'accès à la base de donnée " + selectDoublons.lastError().text()), "Recherche des doublons dans la base"));
                }

        }
    }
    else
        setLastError(QObject::trUtf8(qPrintable("Erreur d'accès à la base de donnée " + selectGroupMd5.lastError().text()), "Recherche des doublons dans la base"));

}






/**
  @brief Destructeur
*/
DataBase::~DataBase()
{
    DEBUG(QObject::trUtf8("Appel du destructeur du singleton DataBase"));
    delete m_databaseobject;
}
