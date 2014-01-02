#ifndef CONSOLE_H
#define CONSOLE_H

#include "displayer.h"

class Console : public Displayer
{

public:
    Console(QWidget *parent = 0);
    void menuPrincipal();
    void menuAide();

};

#endif // CONSOLE_H
