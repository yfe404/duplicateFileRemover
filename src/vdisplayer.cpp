#include "vdisplayer.h"
#include "path.h"
#include <iostream>

VDisplayer::VDisplayer()
{
}

void VDisplayer::visit(Path *p)
{
    if (exists(*p) && is_regular_file(*p))
    {
        std::cout << last_write_time(*p) << " : " << file_size(*p) << " : "<< p->filename() << " : "<< *p << '\n';
        std::cout << "\n";
    }
}
