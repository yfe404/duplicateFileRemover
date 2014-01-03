#ifndef COMMAND_H
#define COMMAND_H

#include "database.h"

#include <boost/filesystem.hpp>
#include <map>

using std::multimap;
using std::string;


/// Patron de conception commande

class Command
{
protected:
    Command();

public:
    virtual void Execute() = 0;
    virtual ~Command();

};



class RechercherDoublons : Command
{
private :
    DataBase* m_db;
    multimap<string, boost::filesystem::path*>* m_doublonsFiles;

public:
    RechercherDoublons(DataBase* db, multimap<string, boost::filesystem::path*>* map)
    {
        m_db = db;
        m_doublonsFiles = map;
    }

    virtual void Execute()
    {
        m_db->rechercherDoublons(*m_doublonsFiles);
    }
};

#endif // COMMAND_H
