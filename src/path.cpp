#include "path.h"
#include "visitor.h"

#include <string>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

void Path::Accept(Visitor* v){
    v->visit(this);
}


/**
  @brief vérifie si un fichier donné est caché ou non

  @idea : convertir en qfileinfo (si fonction présente)
  -vérifier si le premier caractère du fichier est un '.' (problème de portabilité ! => ce format que sous linux ! Comment faire pour windows ?)
*/
bool Path::isHidden(path p) const{
    std::string fileName = (p.filename()).string();

    if (fileName[0] == '.')
        return true;
    return false;
}

/**
  @brief vérifie si le fichier existe et est régulier

  @returns la valeur renvoyée par l'expression booléenne
*/
bool Path::isExistingRegular() const{
    return exists(*this) && is_regular_file(*this);
}
