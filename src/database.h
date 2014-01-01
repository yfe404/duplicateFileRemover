#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <boost/filesystem.hpp>
#include <QSqlDatabase>
#include <QtSql>

/**
 @class DataBase
 @brief classe singleton, héritant de QSqlDatabase, nous permettant de manipuler la base de données sqlite3
*/

class QSqlDatabase;
class path;

class DataBase
{
private:
    static std::string dataBaseCreator;
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
    static DataBase& instance(){
        static DataBase instance;
        return instance;
    }

    void update();
    void listerFichiers();
    void listerDoublonsTaille();
    void listerDoublons();
    void updateMD5(std::list<boost::filesystem::path*> &liste);
    //void updateDIRMD(std::list<boost::filesystem::path*> &liste);
    std::list<boost::filesystem::path *> &getListSizeDuplicate(void);
    std::list<boost::filesystem::path *> &getListParents(void); /// Liste des dossiers qui ne contiennent que des doublons potentiels

    bool ouvrirDB(); /// ouverture de la base de données. // (appelle juste la méthode de father donc ok je pense)
    void fermerDB(); /// fermeture de la base de données. // (appelle juste la méthode de father donc ok je pense)

    ~DataBase();


    bool commit();
    bool rollback();
    bool transaction();


};






























#endif // DATABASE_H
