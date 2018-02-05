#ifndef UIGENERATORFORMAT_H
#define UIGENERATORFORMAT_H

#include <QList>

#include "uivariable.h"

struct UIGeneratorFormat
{
    QString Name;
    QList<UIVariable> UIVariables;
};

#endif // UIGENERATORFORMAT_H
