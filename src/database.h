#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <boost/filesystem.hpp>
#include <QSqlDatabase>
#include <QtSql>

/**
 @class DataBase
 @brief classe singleton, basée sur QSqlDatabase, nous permettant de manipuler la base de données sqlite3
*/

class QSqlDatabase;

class DataBase
{
private:
    static std::string dataBaseCreator;
    static std::string m_tableName;
    static std::string m_name;

    QSqlDatabase* father; // passé en argument car, si on hérite, la connexion ne marche plus (A VOIR !)


    /**
      @brief Les constructeurs/opérateurs de copie.
      Privés (et non-implémentés pour certains) car on ne veut pas que ces fonctions puissent être appelées (afin de respecter le design pattern)
    */
    DataBase();
    DataBase (const DataBase&); /// Constructeur de copie (non-implémenté car singleton)
    void operator =(const DataBase&); /// Pour la meme raison que le constructeur de copie, l'opérateur de coppie ne sera pas implémenté

public:
    const char* tableName() const; /// getter du nom de la table (testé, OK)

    /**
      @brief Récupère l'instance de la base de données existante
      Au premier appel, crée la base en appelant le constructeur
    */
    static DataBase& instance(){ // ok
        static DataBase instance;
        return instance;
    }

    bool ouvrirDB(); /// ouverture de la base de données. // (appelle juste la méthode de father donc ok je pense)
    void fermerDB(); /// fermeture de la base de données. // (appelle juste la méthode de father donc ok je pense)

    ~DataBase();
};

/**
  N.B : comme il s'agit d'un Singleton, nous ne pouvons utiliser la macro Q_OBJECT, et donc les signaux et slots.
  Le constructeur d'un singleton ne prenant pas de paramètres, le constructeur explicit (ajouté par Qtcreator, par défaut) a également été supprimé
*/
#endif // DATABASE_H
