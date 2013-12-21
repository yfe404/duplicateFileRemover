#ifndef VISITOR_H
#define VISITOR_H

class Path;

class Visitor
{
public:
    Visitor(){}
    virtual void visit(Path* p) = 0;
};

#endif // VISITOR_H
