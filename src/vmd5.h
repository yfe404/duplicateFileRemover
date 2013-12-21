#ifndef VMD5_H
#define VMD5_H

#include "visitor.h"


/**
  @brief Calcul et affichage de la clé md5sum du fichier passé en paramètre
  utilisation de QByteArray et QCryptographicHasg pour le calcul
*/
class VMd5 : public Visitor
{
public:
    VMd5();
    void visit(Path *p);
};

#endif // VMD5_H
