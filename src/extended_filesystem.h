#ifndef CRYPTOGRAPHIC_H
#define CRYPTOGRAPHIC_H
#include <boost/filesystem.hpp>
#include <string>
#include <list>


enum mode {normal, recursive};
enum hiddenFiles {skip, keep};

std::string md5sum(boost::filesystem::path p);
void addContentRecursively(boost::filesystem::path p, std::list<boost::filesystem::path *> *liste, mode m=recursive, hiddenFiles h=skip);
bool isForbidden(boost::filesystem::path p); /// vérifie si un fichier donnné fait partie de la blacklist
bool isHidden(boost::filesystem::path p);
int getNbFiles(boost::filesystem::path p, hiddenFiles h = skip);

#endif // CRYPTOGRAPHIC_H
