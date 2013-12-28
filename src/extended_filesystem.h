#ifndef CRYPTOGRAPHIC_H
#define CRYPTOGRAPHIC_H
#include <boost/filesystem.hpp>
#include <string>
#include <list>


enum Mode {normal, recursive};
enum HiddenFiles {skip, keep};

std::string md5sum(boost::filesystem::path file);
void addContentRecursively(boost::filesystem::path& p, std::list<boost::filesystem::path *> *filesToAdd, Mode m=recursive, HiddenFiles h=skip);
bool isForbidden(boost::filesystem::path p); /// vérifie si un fichier donnné fait partie de la blacklist
bool isHidden(boost::filesystem::path p);

#endif // CRYPTOGRAPHIC_H
