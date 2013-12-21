#include "vmd5.h"
#include "path.h"
#include <iostream>
#include <QFile>
#include <QCryptographicHash>

VMd5::VMd5()
{
}

void VMd5::visit(Path *p)
{
    if (exists(*p) && is_regular_file(*p))
    {
        QFile file(p->c_str());
        file.open(QIODevice::ReadOnly);
        QByteArray fileContent = file.readAll();
        QByteArray hashData = QCryptographicHash::hash(fileContent,QCryptographicHash::Md5);
        std::cout << QString(hashData.toHex()).toStdString() + "\n";
    }

}
