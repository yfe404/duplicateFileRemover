#include <string>
#include <iostream>
#include <fstream>


#include "database.h"
#include "extended_filesystem.h"



#define TFILES string("TFILES")
#define DATABASE_NAME string(string(getenv("USER")) + string(".db"))
#define CONFIGFILE ".config"        // path du fichier de configuration; sera bien entendu placé ailleur sur la version finale

#define CREATE_FILE_DB string("echo  \"CREATE TABLE IF NOT EXISTS " + TFILES  + " (filepath varchar(255) primary key, filename varchar(255), lastmodified varchar(255), size integer, md5sum varchar(255));\" | /usr/bin/sqlite3 " + DATABASE_NAME)

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
    m_lastError = "";   /// Initialisation du champ d'erreur à chaine vide

    m_databaseobject = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));

    /** Appel du script pour créer la base de données sqlite */

    if(system(CREATE_FILE_DB.c_str()) == -1)
    {
        setLastError(QObject::tr("Impossible de créer la base de données, vérifiez que sqlite3 est bien installé sur votre ordinateur."));
        return;
    }

    /** Paramétrage de la base de données */
    m_databaseobject->setHostName("localhost");
    m_databaseobject->setUserName( QString( getenv("USER")) );
    m_databaseobject->setPassword("");
    m_databaseobject->setDatabaseName( QString(DATABASE_NAME.c_str()) );
}




/**
  @brief ouverture de la base de données

  Ouvre la base de données et affiche un message en fonction du bon fonctionnement (ou non) de la méthode.

  @return true la base est ouverte
  @return false il y a eu un problème durant l'ouverture
*/
bool DataBase::ouvrirDB(){
    if( m_databaseobject->open() ) /// Appelle la méthode open de QSqlDatabase et retourne la valeur retournée par cette dernière.
    {
        return true;
    }
    else
    {
        QString err = QObject::tr("Echec de connexion à la base de données : ") + m_databaseobject->lastError().text();
        setLastError(err);
        return false;
    }
}


/**
  @brief Fermeture de la base de données
    Ferme la connexion à la base de données, libérant toutes les ressources acquises, et rendant invalide tout objet QQuery utilisant la base.
*/
void DataBase::fermerDB(){
    m_databaseobject->close();
}

const QString& DataBase::lastError()
{
    return m_lastError;
}

void DataBase::setLastError(const QString& msg) /// @todo utiliser C++ 2011 pour éviter la recopie de la chaine passée en paramètre
{
    m_lastError = msg;
}



bool DataBase::commit()
{
    return m_databaseobject->commit();
}

bool DataBase::rollback()
{
    return m_databaseobject->rollback();
}

bool DataBase::transaction()
{
    return m_databaseobject->transaction();
}



/**
  @brief Met à jour la base de données des fichiers.
*/

/// @todo doit accepter un nom de dossier, si aucun dossier n'est spécifié, mettre à jour à l'aide du fichier de configuration
void DataBase::update(){
    /** Sélection des dossiers à scanner à l'aide du fichier de configuration */
    ifstream config(CONFIGFILE); /// création d'un flux pour la manipulation du fichier de configuration

    if(!config)
    {
        setLastError(QObject::tr("Erreur ouverture fichier de configuration, veuillez choisir 'Configurer' dans le menu principal pour remédier à ce problème."));
    }

    string parcours_configfile;

    getline(config, parcours_configfile); /// récupère une ligne du fichier de configuration

    /// @todo les lignes commentées suivantes seront incluses dans une fonction de vérification du fichier de
    ///configuration, voir les lignes suivantes pour plus d'informations sur cette fonction.
    // if(parcours_configfile.size() == 0)
        //cerr<<"Fichier de configuration vide !"<<endl;



    /** Récupération des dossiers sélectionnés, parcours et ajout à notre liste */

    list<boost::filesystem::path*> *lFiles = new list<boost::filesystem::path*>;

    while( parcours_configfile.size() != 0 )
    {

        boost::filesystem::path rep(parcours_configfile);

        /// @todo les vérifications suivantes doivent êtres insérées dans une fonction qui fera la vérification
        ///du fichier de configuration à chaque update et à chaque fin d'édition du fichier de configuration.
        /// et eventuellemnt à chaque démarrage du programme dans le cas ou le fichier de configuration aurait
        /// été édité à la main en dehors du programme.
        /*
        try
        {
            if ( exists(rep) )    /// Si le chemin existe
               addContentRecursively(rep, lFiles, recursive); /// ajout des fichiers de manière récursive (ou pas) à la liste

            else
              cout << rep << " does not exist\n";
        }
        catch(const filesystem_error& ex)
        {
            cerr << rep << " Permission Denied !\n";
        }

        */

        getline(config, parcours_configfile); /// continue le parcours du fichier de configuration
    }


    /** Ajout à la base de données */

    DataBase::instance().transaction();       /// Début de transaction

    QSqlQuery query;

    query.prepare("INSERT OR REPLACE INTO " + QString(TFILES.c_str()) + " values (?, ?, ?, ?, ?)");    /// Insert ou met à jour dans la base si la clé existe déjà


    for(list<boost::filesystem::path*>::iterator file = lFiles->begin(); file != lFiles->end(); ++file)
    {
            query.addBindValue( (*file)->c_str() ); /// ajout du pathname
            query.addBindValue((*file)->filename().c_str()); /// ajout du filename
            query.addBindValue( quint64(last_write_time(*(*file)))  ); /// date de dernière modification
            query.addBindValue(quint64(file_size(*(*file)))); /// taille
            query.addBindValue("");                   /// Vide pour le moment (future place de la clé md5)

            query.exec();
    }

    if(!DataBase::instance().commit())                          /// Commit (seul accès à la base)
    {
        DataBase::instance().rollback();
        setLastError(QObject::tr(qPrintable("Erreur de mise à jour de la base de données : " + query.lastError().text())));
    }

    /// @todo Ces opération doivent être faites si l'on sort en cas d'erreur, il faudra donc gérer ce cas
    delete lFiles;       /// @todo deleter tous les pointeurs de la liste
    config.close();     /// fermeture du fichier de configuration
}





/**
  @brief liste les éléments de la base de données
  utilise une requête select puis affiche les pathname de tous les fichiers
*/
/// @todo Supprimer cette fonction qui n'aura plus lieu d'être car la console et la GUI on un affichage distinct qui ne
/// être factorisé par une fonction telle que celle-ci

void DataBase::listerFichiers()
{

    QSqlQuery select;
    if( select.exec("SELECT filepath FROM " + QString(TFILES.c_str())) )
    {
        while(select.next())
        {
            for(int res=0; res < select.record().count(); ++res) /// pour chaque ligne de résultat de la requête...
                cout << boost::filesystem::path( select.value(res).toString().toStdString() ) << endl;    /// affiche le path
        }
    }

}


/**
  @brief liste les éléments de la base de données qui ont des doublons de taille ( 1 fichier par taille )
*/
/// @todo cette fonction prendra en paramètre une multimap de la stl, elle remplira la map se détachant ainsi de la tache
/// d'affichage des résultats, à la charge de l'afficheur (qu'il soit console ou GUI).
/// Dans un premier temps le résultat pourra être affiché dans cette fonction dans la version debug uniquement.
void DataBase::listerDoublonsTaille()
{
    QSqlQuery selectSize;

    if(selectSize.exec("SELECT COUNT(size) AS nb_doublon, filepath  FROM " + QString(TFILES.c_str()) +" GROUP BY size HAVING COUNT(size) > 1" ) )
    {
        //cout<<query.record().count()<<endl;
        while(selectSize.next())
        {
            /// Récupération des résultats de la requête sous forme de path
            boost::filesystem::path file(selectSize.value(1).toString().toStdString());

            //cout << "= > Taille " << file_size(file) << " octets : " << endl;

            QSqlQuery selectEqualSize;
            selectEqualSize.prepare("SELECT filepath FROM " + QString(TFILES.c_str()) + " WHERE size = ?");
            selectEqualSize.addBindValue(quint64(file_size(file))); /// taille
            if(selectEqualSize.exec())
            {
                while(selectEqualSize.next())
                    cout << "\t" + selectEqualSize.value(0).toString().toStdString() << endl;
            }
            else
            {

                // Debug => cout << selectEqualSize.lastQuery().toStdString() << endl;
                setLastError(QObject::tr(qPrintable("Erreur d'accès à la base de donnée " + selectEqualSize.lastError().text()), "Recherche des doublons dans la base"));
            }

        }
    }

    else
        setLastError(QObject::tr(qPrintable("Erreur d'accès à la base de donnée " + selectSize.lastError().text()), "Recherche des doublons dans la base"));
}









/// @todo fusionner les fonctions updateMD5, getListSizeDuplicate, et listerDoublons dans une fonction
/// rechercherFichiersDoublons(std::map<string, path>& map) avec string la clé md5.


/**
  @brief mise à jour des clés md5
  @param filesToUpdate liste des fichiers de la base dont il faut mettre à jour la valeur de la clé md5
*/
void DataBase::updateMD5(std::list<boost::filesystem::path *> &filesToUpdate)
{
    DataBase::instance().transaction(); /// début de la transaction

    QSqlQuery update;
    boost::filesystem::path* fic;

    /// mise à jour des clés md5 pour les fichiers de la listes
    update.prepare("UPDATE " + QString(TFILES.c_str()) + " SET md5sum=? WHERE filepath=?");

    for(list<boost::filesystem::path*>::iterator it = filesToUpdate.begin(); it != filesToUpdate.end(); ++it)
    {
        fic = new boost::filesystem::path( (*it)->c_str() );
        update.addBindValue( QString( md5sum(*fic).c_str() ) ); /// ajout de la clé md5
        update.addBindValue( fic->c_str() ); /// ajout du pathname
        update.exec();
        //cout<<requete.executedQuery().toStdString()<<endl;
        delete fic;
    }

    if(!DataBase::instance().commit())
    {
        DataBase::instance().rollback();
        setLastError(QObject::tr(qPrintable("Erreur d'accès à la base de donnée " + update.lastError().text()), "Recherche des doublons dans la base"));
    }
}




/**
  @brief crée et retourne la liste des fichiers de même taille

  @return equalsFile contenant les fichiers de même taille dans la base
*/
list<boost::filesystem::path *>& DataBase::getListSizeDuplicate()
{
    QSqlQuery query;
    list<boost::filesystem::path*> *equalsFile = new list<boost::filesystem::path*>(); // ATTENTION FUITE MÉMOIRE
    boost::filesystem::path *file;

    if( query.exec("select filepath from " + QString(TFILES.c_str()) + " where size in (select  size from " + QString(TFILES.c_str()) + " group by size having count(size) > 1)") )
    {
        //cout<<query.record().count()<<endl;
        while(query.next())
        {
            try{
                file = new boost::filesystem::path(query.value(0).toString().toStdString());
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
        setLastError(QObject::tr(qPrintable("Erreur d'accès à la base de donnée " + query.lastError().text()), "Recherche des doublons dans la base"));

    // Debug => cout<<liste->size()<<endl;


    return *equalsFile;
}






/**
  @brief liste les doublons de la base de données (en fonction de la clé md5)
*/
void DataBase::listerDoublons()
{
    QSqlQuery selectGroupMd5;
    if( selectGroupMd5.exec("SELECT COUNT(md5sum) AS nb_doublon, filepath  FROM " + QString(TFILES.c_str()) +" GROUP BY md5sum HAVING COUNT(md5sum) > 1" ) )
    {
        cout<<selectGroupMd5.lastQuery().toStdString()<<endl;
        while(selectGroupMd5.next())
        {
                boost::filesystem::path p(selectGroupMd5.value(1).toString().toStdString());
                cout << "= > clé md5 : " << md5sum(p) << " : " << endl; /// affiche la clé md5 pour les doublons trouvés

                QSqlQuery selectDoublons;
                selectDoublons.prepare("SELECT filepath FROM " + QString(TFILES.c_str()) + " WHERE md5sum = ?");
                selectDoublons.addBindValue(QString(md5sum(p).c_str())); /// clé md5
                if(selectDoublons.exec())
                {
                    while(selectDoublons.next())
                        cout << "\t"+selectDoublons.value(0).toString().toStdString() << endl;

                }
                else
                {
                    cout << selectDoublons.lastQuery().toStdString( )<< endl;
                    cout << selectDoublons.lastError().text().toStdString() << endl;
                }

        }
    }
    else
        cout << selectGroupMd5.lastError().text().toStdString() << endl;

}



/**
  @brief Destructeur
*/
DataBase::~DataBase()
{
    delete m_databaseobject;
}
