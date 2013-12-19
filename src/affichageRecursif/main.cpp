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

    path p (argv[1]);   // p reads clearer than argv[1] in the following code

    try
    {
        if (exists(p))    // does p actually exist?
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


// displays name, path, and last modification date of the file given
void stats(path p)
{
    if (exists(p) && is_regular_file(p))
    {
        cout << last_write_time(p) << " : " << file_size(p) << " : "<< p.filename() << " : "<< p << '\n';
        cleMd5(QString(p.c_str()));
        cout << "\n";
    }

}


void cleMd5(QString pathname)
{
    QFile fFichier(pathname);
    fFichier.open(QIODevice::ReadOnly);
    QByteArray contenuFichier = fFichier.readAll();
    QByteArray hashData = QCryptographicHash::hash(contenuFichier,QCryptographicHash::Md5);
    cout << QString(hashData.toHex()).toStdString();
}

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
                if(is_directory(*it) && !is_symlink(*it))
                {
                    path dir(*it);
                    printContentRecursively(dir, recursive);
                }
            }

            if (is_regular_file(*it))
            {
                stats(*it);
            }

       }

        catch(const filesystem_error& ex)
        {
            cerr << *it << " Permission Denied !\n";
        }

        ++it;
    }

}
