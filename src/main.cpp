#include <iostream>
#include <QFileInfo>
#include <QStringList>
#include <QDateTime>
#include<fstream>

#define DB_SEP ':'

using namespace std;


void menu();
void init(void);
void update(void);
void afficherAide(void);
void lister(void);



int main()
{
    init();
    menu();
    return 0;
}


void init(void)
{
    system("chmod +x update.sh");
}

void update(void)
{

    if(system("./update.sh /tmp")==-1)
    {
        cerr<<"Erreur execution fichier : update.sh"<<endl;
    }

    string filepath;
    string key;
    ifstream pathnames("pathnames.db");
    if(!pathnames)
    {
        cerr<<"Erreur ouverture fichier : pathnames.db"<<endl;
    }


    ifstream md5sum("md5.db");
    if(!md5sum)
    {
        cerr<<"Erreur ouverture fichier : md5.db"<<endl;
    }

    getline(pathnames, filepath);
    getline(md5sum, key);

    ofstream output("database.db");
    if(!output)
    {
        cerr<<"Erreur ouverture fichier : database.db"<<endl;
    }


    while(filepath.size() != 0)
    {

        string line;
        QFileInfo fichier(filepath.c_str());

        line += filepath + DB_SEP +
                fichier.baseName().toStdString() + DB_SEP +
                fichier.lastModified().toString("dd MMMM yyyy hh-mm-ss").toStdString() + DB_SEP +
                key;

        output << line << std::endl;

        getline(pathnames, filepath);
        getline(md5sum, key);
    }

    system("rm pathnames.db 2> /dev/null");
    system("rm md5.db 2> /dev/null");


} // ferme automatiquement tous les flux



void lister(void)
{
    ifstream database("database.db");

    if(!database)
    {
        cerr<<"Erreur ouverture fichier : database.db"<<endl;
    }

    string line;
    string notDisplayed;
    getline(database, line, DB_SEP);            // récupère une chaine de caractère jusqu'au ':' càd le nom du fichier (cat database.db)
    getline(database,notDisplayed );            // récupère le reste de la ligne que nous n'afficherons pas pour positionner le curseur dans le fichier sur l'entrée suivante

    while(line.size() != 0)
    {
        cout<<line<<endl;
        getline(database, line, DB_SEP);
        getline(database,notDisplayed );
    }

}




void menu()
{
    char choix = 'm';


    while(choix!='q')
    {
        cout<<"Commande (m pour l'aide): ";
        scanf("%c%*c", &choix);                  // Problème : si l'utilisateur entre une chaine de caractères, pas géré !
        switch(choix)
        {
        case 'l' :
            lister();
        break;
        case 'u' :
            update();
        case 'm' :
            afficherAide();
        break;
        case 'q' :
            return;
        break;
        default:
            cout<<choix<<" : commande inconnue"<<endl;

        }
    }

}


void afficherAide(void)
{
    cout<<"Commande d'action"<<endl<<
    "   l   lister les fichiers (sera remplacé par doublons) du dossier passé en paramètre"<<endl<<
    "   m   afficher ce menu"<<endl<<
    "   u   mettre à jour la base de données"<<endl<<
    "   q   quitter sans enrengistrer les changements"<<endl;
}







