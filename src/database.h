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
class path;

class DataBase
{
private:
    static std::string dataBaseCreator;
    static std::string m_name;

    QSqlDatabase* m_databaseobject;
    QString m_lastError;          /// Contient la dernière erreur d'exécution décrite sous forme d'une chaine de caractères.


    /**
      @brief Les constructeurs/opérateurs de copie.
      Privés (et non-implémentés pour certains) car on ne veut pas que ces fonctions puissent être appelées (afin de respecter le design pattern)
    */
    DataBase();
    DataBase (const DataBase&); /// Constructeur de copie (non-implémenté car singleton)
    void operator =(const DataBase&); /// Pour la meme raison que le constructeur de copie, l'opérateur de coppie ne sera pas implémenté
    void setLastError(const QString& msg);   /// Remplit le champ de dernière erreur d'exécution

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

    void update(); /// met à jour la base de données en fonction du fichier de configuration

    /* void listerFichiers(); /// liste le contenu de la base */
    void listerDoublonsTaille(); /// liste les fichiers ayant la même taille
    void listerDoublons(); /// liste les doublons (selon différents critères)

    void updateMD5(std::list<boost::filesystem::path*> &filesToUpdate);
    std::list<boost::filesystem::path *> &getListSizeDuplicate();

    void ouvrirDB(); /// ouverture de la base de données. // (appelle juste la méthode de father donc ok je pense)
    void fermerDB(); /// fermeture de la base de données. // (appelle juste la méthode de father donc ok je pense)

    const QString& lastError();

    bool commit();
    bool rollback();
    bool transaction();
    void rechercherDoublons(std::multimap<std::string, boost::filesystem::path *> map);
    bool verifierConfiguration();
    void editerConfiguration();


    ~DataBase();
};






























#endif // DATABASE_H
