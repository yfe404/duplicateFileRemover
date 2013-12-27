#ifndef PATH_H
#define PATH_H

#include <boost/filesystem.hpp>
class Visitor;

// HERITAGE À MODIFIER !! (destructeur de path non-virtuel)
class Path : public boost::filesystem::path
{
public:
    Path() : path(){}
    Path(const std::string& v) : path(v) {}
    Path(const char* p):path(p){}
    Path(const path& p):path(p){}

    void Accept(Visitor* v);

    bool isHidden(path p) const;
    bool isForbidden(path p) const; /// vérifie si un fichier donnné fait partie de la blacklist
    bool isExistingRegular() const;
};

#endif // PATH_H
