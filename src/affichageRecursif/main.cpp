#include <iostream>
#include <boost/filesystem.hpp>
#include <QFile>
#include <QCryptographicHash>
#include <QDebug>
using namespace std;
using namespace boost::filesystem;

typedef enum {normal, recursive} mode;


void stats(path p);
void printContentRecursively(path p, mode m);
void cleMd5(QString pathname);



int main(int argc, char* argv[])
{

    path p (argv[1]);   /// p reads clearer than argv[1] in the following code

    try
    {
        if (exists(p))    /// does p actually exist?
            printContentRecursively(p, recursive);

        else
          cout << p << " does not exist\n";
    }
    catch(const filesystem_error& ex)
    {
        cerr << p << " Permission Denied !\n";
    }


  return 0;
}


/**
@brief displays name, path, and last modification date of the file given
*/
void stats(path p)
{
    if (exists(p) && is_regular_file(p))
    {
        cout << last_write_time(p) << " : " << file_size(p) << " : "<< p.filename() << " : "<< p << '\n';
        cleMd5(QString(p.c_str()));
        cout << "\n";
    }

}


/**
  @brief Calcul et affichage de la clé md5sum du fichier passé en paramètre
  utilisation de QByteArray et QCryptographicHasg pour le calcul
*/
void cleMd5(QString pathname)
{
    QFile fFichier(pathname);
    fFichier.open(QIODevice::ReadOnly);
    QByteArray contenuFichier = fFichier.readAll();
    QByteArray hashData = QCryptographicHash::hash(contenuFichier,QCryptographicHash::Md5);
    cout << QString(hashData.toHex()).toStdString();
}


/**
  @brief Affiche un dossier et son contenu récursivement
  */
void printContentRecursively(path p, mode m)
{
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
                    printContentRecursively(dir, recursive);
                }
            }

            if (is_regular_file(*it)) /// Si c'est un fichier régulier
                stats(*it); /// on affiche ses informations.
       }

        catch(const filesystem_error& ex)
        {
            cerr << *it << " Permission Denied !\n";
        }

        ++it;
    }

}
