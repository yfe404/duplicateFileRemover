#ifndef COMMAND_H
#define COMMAND_H

#include <boost/filesystem.hpp>
#include <map>
#include "database.h"

using std::multimap;
using std::string;


/// Patron de conception commande

class Command
{
public:
    virtual ~Command();

    virtual void Execute() = 0;

protected:
    Command();
};



class RechercherDoublons : Command
{
public:
    RechercherDoublons(DataBase* db, multimap<string, boost::filesystem::path*>* map)
    {
        m_db = db;
        m_map = map;
    }

    virtual void Execute()
    {
        m_db->rechercherDoublons(*m_map);
    }
private :
    DataBase* m_db;
    multimap<string, boost::filesystem::path*>* m_map;
};

#endif // COMMAND_H
