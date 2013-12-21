#ifndef PATH_H
#define PATH_H

#include <boost/filesystem.hpp>
#include <visitor.h>


class Path : public boost::filesystem::path
{
public:
    Path() : path(){}
    Path(const std::string& v) : path(v) {}
    Path(const char* p):path(p){}
    Path(const path& p):path(p){}
    void Accept(Visitor* v) { v->visit(this); }
};

#endif // PATH_H
