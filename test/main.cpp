#include <QCoreApplication>
#include <QDir>
#include <iostream>
#include <QDateTime>


using namespace std;

void display(QFileInfoList file);


int main(int argc, char *argv[])
{

    //QCoreApplication a(argc, argv);
    

    QDir directory(QDir::current());        // affiche le contenu du repertoire courant

    QFileInfoList files = directory.entryInfoList();     // recupère une liste de tous les fichiers et repertoires contenus


    display(files);


    //return a.exec();
    return 0;
}


void display(QFileInfoList files)
{

    QDir* d;

    for(int i = 2; i < files.size(); ++i)        // on commence à 2 car [0] => le pathname du dossier contenant, [1] => pathname du parent
    {
        if(files[i].isDir())         // si c'est un repertoire
        {
            cout<<"[DIR]"<<files[i].absoluteFilePath().toStdString()<<"[DIR]"<<endl; // on affiche [DIR] pathname [DIR]
            d = new QDir(files[i].absoluteFilePath());           // On crée un nouveau QDir
            display(d->entryInfoList());                        // on appelle la méthode display sur le contenu du QDir créé

        }

        else{
            cout<<files[i].absoluteFilePath().toStdString()<<endl;               // affiche le pathname
            cout<<files[i].baseName().toStdString()<<endl;                       // affiche le nom du fichier
            cout<<files[i].lastModified().toString("dddd dd MMMM yyyy à hh:mm:ss").toStdString()<<endl;  // affiche la date de modification
            cout<<endl;
        }
    }
}
