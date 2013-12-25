#include "vdisplayer.h"
#include "path.h"

#include <iostream>

VDisplayer::VDisplayer()
{
}

void VDisplayer::visit(Path *file)
{
    if (!file->isExistingRegular()) /// si c'est un fichier régulier et qu'il existe...
        return;

    std::cout << last_write_time(*file) << " : " << file_size(*file) << " : "<< file->filename() << " : "<< *file << '\n';
    std::cout << "\n";

}
