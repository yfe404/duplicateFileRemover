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

    /**
      @brief Les constructeurs/opérateurs de copie.
      Privés car on ne veut pas que ces fonctions puissent être appelées (afin de respecter le design pattern)
    */
    DataBase();
    DataBase (const DataBase&); /// Constructeur de copie (non-implémenté car singleton)
    void operator =(const DataBase&); /// Pour la meme raison que le constructeur de copie, l'opérateur de coppie ne sera pas implémenté
public:
    // ATTRIBUTS PUBLICS A MODIFIER (fait dans un premier temps dans un but de simplification)
    static std::string tableName;
    static std::string name;
    static std::string updater;

    /**
      @brief Récupère l'instance de la base de données existante
      Au premier appel, crée la base en appelant le constructeur
    */
    static DataBase& instance(){
        static DataBase instance;
        return instance;
    }
};

/**
  N.B : comme il s'agit d'un Singleton, nous ne pouvons utiliser la macro Q_OBJECT, et donc les signaux et slots.
  Le constructeur d'un singleton ne prenant pas de paramètres, le constructeur explicit (ajouté par Qtcreator, par défaut) a également été supprimé
*/
#endif // DATABASE_H
