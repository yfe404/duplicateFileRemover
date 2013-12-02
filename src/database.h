#ifndef DATABASE_H
#define DATABASE_H

#include <string>

#include <QSqlDatabase>

/**
 @class DataBase
 @brief classe singleton, héritant de QSqlDatabase, nous permettant de manipuler la base de données sqlite3
*/

class DataBase : public QSqlDatabase
{
private:
    static std::string dataBaseCreator;
    static std::string updater;

    DataBase();
    DataBase (const DataBase&); /// Constructeur de copie (non-implémenté car singleton)
    void operator =(const DataBase&); /// Pour la meme raison que le constructeur de copie, l'opérateur de coppie ne sera pas implémenté
public:
    static std::string tableName;
    static std::string name;

    static DataBase& instance(){
        static DataBase instance; /// instance statique car crée une seule fois
        return instance;
    }
};

/**
  N.B : comme il s'agit d'un Singleton, nous ne pouvons utiliser la macro Q_OBJECT, et donc les signaux et slots.
  Le constructeur d'un singleton ne prenant pas de paramètres, le constructeur explicite ajouté par Qt a également été supprimé
*/
#endif // DATABASE_H
