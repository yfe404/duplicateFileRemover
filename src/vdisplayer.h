#ifndef VDISPLAYER_H
#define VDISPLAYER_H

#include "visitor.h"


/**
  @brief liste les éléments de la base de données
  utilise une requête select puis affiche les pathname de tous les fichiers
*/
class VDisplayer : public Visitor
{
public:
    VDisplayer();
    void visit(Path *p);
};

#endif // VDISPLAYER_H
