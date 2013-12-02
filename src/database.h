#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>

/**
 @class DataBase
 @brief classe singleton, héritant de QSqlDatabase, nous permettant de manipuler la base de données sqlite3
*/

class DataBase //: public QSqlDatabase
{
    //Q_OBJECT
private:
    DataBase(){}
    DataBase (const DataBase&); /// Constructeur de copie (non-implémenté car singleton)
    void operator =(const DataBase&); /// Pour la meme raison que le constructeur de copie, l'opérateur de coppie ne sera pas implémenté
public:
    //explicit DataBase(QObject *parent = 0);

    static DataBase& getInstance(){
        static DataBase instance;
        return instance;
    }
    
//signals:
    
//public slots:
};

#endif // DATABASE_H
