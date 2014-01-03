#ifndef CONSOLE_H
#define CONSOLE_H

#include "displayer.h"

/**
  @class Console héritant de displayer
  @brief permet l'affichage en mode console
*/

class Console : public Displayer
{

public:
    Console(QWidget *parent = 0);
    void menuPrincipal();
    void menuAide();

};

#endif // CONSOLE_H
