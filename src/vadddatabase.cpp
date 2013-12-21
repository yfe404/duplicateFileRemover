#include "vadddatabase.h"
#include "path.h"
#include "database.h"

VAddDatabase::VAddDatabase()
{
}


void VAddDatabase::visit(Path* p)
{

    if (!exists(*p) || !is_regular_file(*p)) /// Sortie si le Path ne correspond pas à un fichier ou si le fichier n'existe pas
    {
        return;
    }


    QSqlQuery requete;

    /// Insert ou met à jour dans la base si la clé existe déjà
    requete.prepare("INSERT OR REPLACE INTO " + QString(DataBase::instance().tableName()) + " values (?, ?, ?, ?, ?)");

    requete.addBindValue( p->c_str() ); /// ajout du pathname
    requete.addBindValue( p->filename().c_str()); /// ajout du filename
    requete.addBindValue( quint64(last_write_time(*p))  ); /// date de dernière modification
    requete.addBindValue(quint64(file_size(*p))); /// taille
    // requete.addBindValue( QString( md5Key.c_str() ) ); /// clé md5
    requete.addBindValue("");                   /// Vide pour le moment

    if (requete.exec())
    {
        std::cout<<"Row added\n";
    }
    else
    {
        std::cerr << "Erreur mise à jour du fichier "<<p->c_str() << std::endl;
    }


}



