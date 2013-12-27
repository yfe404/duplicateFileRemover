#include "vadddatabase.h"
#include "path.h"
#include "database.h"

VAddDatabase::VAddDatabase()
{
}


void VAddDatabase::visit(Path* file)
{
    if (!file->isExistingRegular()) /// Sortie si le Path ne correspond pas à un fichier ou si le fichier n'existe pas
        return;


    QSqlQuery requete;

    /// Insert ou met à jour dans la base si la clé existe déjà
    requete.prepare("INSERT OR REPLACE INTO " + QString(DataBase::instance().tableName()) + " values (?, ?, ?, ?, ?)");

    requete.addBindValue( file->c_str() ); /// ajout du pathname
    requete.addBindValue( file->filename().c_str()); /// ajout du filename
    requete.addBindValue( quint64( last_write_time(*file) )  ); /// date de dernière modification
    requete.addBindValue( quint64( file_size(*file) ) ); /// taille
    requete.addBindValue("");                   /// Vide pour le moment
    // requete.addBindValue( QString( md5Key.c_str() ) ); /// clé md5

    if (requete.exec())
    {
//        std::cout<<"Row added\n";
    }
    else
    {
        std::cerr << "Erreur mise à jour du fichier "<< file->c_str() << std::endl;
    }
}
