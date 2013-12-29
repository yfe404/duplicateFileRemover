#include "extended_filesystem.h"

/// Inclusion pour la fonction hash()
#include <QCryptographicHash>
#include <QString>

/// Inclusions pour le open()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/// Inclusion pour mmap/unmap
#include <sys/mman.h>



using namespace boost::filesystem;



/**
  @brief Calcule la clé md5 d'un fichier passé en paramètre.

  @param p Le fichier dont il faut calculer la clé md5.
  @return Une chaine de caractère contenant la somme md5 du fichier, une chaine vide en cas d'erreur.
*/
std::string md5sum(boost::filesystem::path p)
{

    try{

        QCryptographicHash result(QCryptographicHash::Md5);

        int file_descript;                              /// Descripteur de fichier correpondant au Path
        char* file_buffer;                              /// Pointeur sur la zone mémoire du fichier correpondant au Path


        file_descript = open(p.c_str(), O_RDONLY);      /// ouverture du fichier en lecture seule
        if(file_descript < 0)                           /// gestion de l'erreur d'ouverture
        {
            std::cerr<<p<<" : "<<strerror(errno)<<std::endl;
            return "";
        }

        file_buffer = (char*)mmap(                      /// "Projette" le fichier en mémoire
                                0,                      /// laisse le noyau choisir la zone de mapping
                                file_size(p),           /// taille du mapping
                                PROT_READ,              /// les pages peuvent être lues
                                MAP_PRIVATE,            /// le mapping n'est pas visible pour les autres processus
                                file_descript,          /// descripteur de fichier du fichier à mapper
                                0);                     /// pas d'offset
        if(file_buffer == MAP_FAILED)                   /// gestion d'erreur du mmap
        {
            close(file_descript);                       /// fermeture du descripteur de fichier
            throw std::runtime_error("");               /// On quitte par le gestionnaire d'exceptions
        }

        result.addData(file_buffer, file_size(p));      /// ajout des données à "hasher"

        QString hash(result.result().toHex());          /// récupération du hash sous forme de QString


        munmap(file_buffer, file_size(p));              /// libération du mapping
        close(file_descript);                           /// fermeture du fichier
        return hash.toStdString();                      /// le hash est retourné sous forme de std::string

    }

    catch(...)                                          /// Attrape toutes les exceptions lancées
    {

        std::cerr<<"Can't compute md5sum : "<<p<<std::endl;
        return "";
    }

}



/**
  @brief Ajoute les fichiers contenus dans un dossie dans une liste.


  @param p Le dossier à parcourir.
  @param liste Un pointeur sur la liste à remplir.
  @param m Définit le type de parcours : recursif ou non.
  @param h Définit la politique à adopter par rapport aux fichiers cachés : les ignorer ou non.
*/
void addContentRecursively(path p, std::list<path *> *liste, mode m, hiddenFiles h)
{
    try{
        if(isForbidden(p))
            return;
        directory_iterator end;
        directory_iterator it(p);

        while(it != end)
        {
           try
           {
                if(m == recursive)
                {
                    if(is_directory(*it) && !is_symlink(*it)) /// Si c'est un dossier et non un lien symbolique
                    {
                        path dir(*it);
                        addContentRecursively(dir, liste);      /// Pas besoin de rajouter le mode car recursif par défaut
                    }
                }

                if (is_regular_file(*it) && file_size(*it) > 0) /// Si c'est un fichier régulier
                {
                    path* fic = new path(*it);                  /// Création d'un pointeur sur Path temporaire
                    if((!isHidden(*it)) || (isHidden(*it) && h==keep))
                        liste->push_back(fic);                      /// Ajout à la liste
                }
           }
            catch (const filesystem_error& ex)
            {
                std::cerr << *it << " Permission Denied !\n";
            }

            ++it;
        }
    }
    catch (const filesystem_error& ex)
    {
        return;
    }
}



/**
  @brief vérifie si un fichier est caché ou non
  Vérifie si le premier caractère du nom du fichier est un '.'
*/
bool isHidden(path p) {
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

  @param p le fichier à tester

  @return true si le fichier est dans un dossier "interdit"
  @return false sinon
*/
bool isForbidden(path p){
    path::iterator it = p.begin(); /// it ='/'

    ++it; /// le dossier après la racine

    if (*it == "proc" || *it == "sys" || *it == "usr")
        return true;

    return false;
}


/**
  @brief Retourne le nombre de fichiers reguliers contenus dans une réperoire.

  @param p le reperoire à parcourir
  @param h Définit la politique à adopter par rapport aux fichiers cachés : les ignorer ou non.
  @return le nombre de fichiers réguliers du réperoire, -1 en cas d'erreur

*/
int getNbFiles(boost::filesystem::path p, hiddenFiles h)
{
    int cpt = 0;

    try{
        directory_iterator end;
        directory_iterator it(p);

        while(it != end)
        {
           try
           {
               if (((!isHidden(*it)) || (isHidden(*it) && h==keep)) && is_regular_file(*it) && file_size(*it) > 0) /// Si c'est un fichier régulier
                {
                    ++cpt;
                }
           }
            catch (const filesystem_error& ex)
            {
               return -1;
            }

            ++it;
        }
    }
    catch (const filesystem_error& ex)
    {
        return -1;
    }

    return cpt;

}

