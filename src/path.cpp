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

  vérifier si le premier caractère du fichier est un '.'
   (problème de portabilité ! => ce format que sous linux ! Comment faire pour windows ?)
  Idées : convertir en qfileinfo (si méthode plus portable)
*/
bool Path::isHidden(path p) const{
    std::string fileName = (p.filename()).string();

    if (fileName[0] == '.') /// si le fichier commence par '.', il est caché
        return true;
    return false;
}


/**
  @brief vérifie si un fichier est dans un dossier faisant partie de notre 'blacklist'

  un path peut se décomposer en éléments sur lesquels on peut itérer.
  Exemple : /foo/bar/bar.txt se décompose de la manière suivante :
  /     foo     bar     bar.txt

  @param p le fichier a testé

  @return true si le fichier est dans un dossier "interdit"
  @return false sinon
*/
bool Path::isForbidden(path p) const{
    path::iterator it = p.begin(); /// it ='/'

    ++it; /// le dossier après la racine

    if (*it == "proc" || *it == "sys" || *it == "usr")
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
