#ifndef VADDDATABASE_H
#define VADDDATABASE_H

#include "visitor.h"

class VAddDatabase : public Visitor
{
public:
    VAddDatabase();
    void visit(Path *file);
};

#endif // VADDDATABASE_H