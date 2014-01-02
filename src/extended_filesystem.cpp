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
  @brief calcul de la clé md5sum pour un fichier donné

  @param file le fichier dont il va falloir calculer la clé md5

  @returns la clé md5 du fichier
  @return "" chaine vide si erreur
*/
std::string md5sum(path file)
{

    try{

        QCryptographicHash result(QCryptographicHash::Md5);

        int file_descript;     /// Descripteur de fichier correpondant au Path


        if( (file_descript = open(file.c_str(), O_RDONLY)) < 0)  /// ouverture du fichier en lecture seule
        {
            std::cerr << "open() : " << file.c_str() << std::endl; // NE MARCHE PAS AVEC LES ACCENTS (pour l'affichage)
            return "";
        }

        char* file_buffer;     /// Pointeur sur la zone mémoire du fichier correpondant au Path
        file_buffer = (char*)mmap(                  /// "Projette" le fichier en mémoire
                                0,                  /// laisse le noyau choisir la zone de mapping
                                file_size(file),   /// taille du mapping
                                PROT_READ,          /// les pages peuvent être lues
                                MAP_PRIVATE,        /// le mapping n'est pas visible pour les autres processus
                                file_descript,      /// descripteur de fichier du fichier à mapper
                                0);                 /// pas d'offset


        if(file_buffer == MAP_FAILED)                   /// gestion d'erreur du mmap
        {
            close(file_descript);                       /// fermeture du descripteur de fichier
            throw std::runtime_error("");               /// On quitte par le gestionnaire d'exceptions
        }

        result.addData(file_buffer, file_size(file));  /// ajout des données à "hasher"


        QString hash( result.result().toHex() );          /// récupération du hash sous forme de QString

        munmap(file_buffer, file_size(file));          /// libération du mapping

        close(file_descript);                           /// fermeture du fichier
        return hash.toStdString();                      /// le hash est retourné sous forme de std::string

    }

    catch(...)                                          /// Attrape toutes les exceptions lancées
    {
        std::cerr << "Can't compute md5sum : " << file << std::endl;
        return "";
    }

}



/**
  @brief Ajoute les fichiers d'un dossier récursivement à une liste

  @param p le fichier/dossier à parcourir
  @param filesToAdd la liste qui va contenir le parcours de nos fichiers
  @param m le mode sélectionné pour le parcours (récursif ou normal). Par défaut : récursif

  @param h le mode sélectionné pour le traitement des fichiers cachés (ajouté ou passés). Par défaut : skip

  @return liste remplie (via le pointeur)
*/
void addContentRecursively(path &p, std::list<path *> *filesToAdd, Mode m, HiddenFiles h)
{
    try{
        if(isForbidden(p)) /// si le dossier fait partie de notre blacklist...
            return; /// on passe a suivant
        directory_iterator end;
        directory_iterator file(p);

        while(file != end)
        {
           try
           {
                if(m == recursive)
                {
                    if(is_directory(*file) && !is_symlink(*file) && ((!isHidden(*file)) ||(h == keep && isHidden(*file))) ) /// Si c'est un dossier et non un lien symbolique
                    {
                        path dir(*file);
                        addContentRecursively(dir, filesToAdd);      /// Pas besoin de rajouter le mode car recursif par défaut
                    }

                }

                if (is_regular_file(*file) && file_size(*file) > 0) /// Si c'est un fichier régulier
                {
                    path *fic = new path(*file);                  /// Création d'un pointeur sur Path temporaire
                    if( (h == keep && isHidden(*file)) || !isHidden(*file)) /// selon le traitement qu'on a choisi pour les fichiers cachés
                        filesToAdd->push_back(fic);  /// Ajout à la liste
                }
           }
            catch (const filesystem_error& ex)
            {
                std::cerr << *file << " Permission Denied !\n";
            }

            ++file;
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
    path::iterator pathMember = p.begin(); /// pathMember ='/'

    ++pathMember; /// le dossier après la racine

    if (*pathMember == "proc" || *pathMember == "sys" || *pathMember == "usr" || *pathMember == "var") /// si le dossier est dans notre 'blacklist'
        return true;

    return false;
}




