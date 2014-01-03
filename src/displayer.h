#ifndef DISPLAYER_H
#define DISPLAYER_H

/**
    @class Displayer héritant de QWidget
    @brief Classe abstraite d'affichage
*/
#include <QWidget>

class Displayer : public QWidget
{
    Q_OBJECT
public:
    explicit Displayer(QWidget *parent = 0){this->setParent(parent);}


signals:

public slots:

};

#endif // DISPLAYER_H
