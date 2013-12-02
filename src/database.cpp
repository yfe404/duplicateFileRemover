#include <string>
#include <iostream>

#include "database.h"

using std::string;
using std::cout;
using std::endl;
using std::cerr;

/** Définition des variables de classes */
string DataBase::dataBaseCreator = "./create_database.sh ";
string DataBase::updater = "./update.sh ";
string DataBase::name = (string(getenv("USER")) + ".db");
string DataBase::tableName = "files";


DataBase::DataBase()
{
    /** Changement des droits sur les scripts afin que l'on puisse les exécuter */
    string cmd1 = string("chmod +x ") + dataBaseCreator + updater;
    cout<<cmd1<<endl;
    system(cmd1.c_str()); // CODERET À TESTER

    /* Passera dans le constructeur de DataBase */
    string cmd2 = dataBaseCreator + name + ' ' + tableName + string(" 2> /dev/null");
    cout<<cmd2<<endl;
    if(system(cmd2.c_str()) == -1)
    {
        cerr<<"Can't create database !"<<endl;
    }

}

