#ifndef WTQTUTIL_H
#define WTQTUTIL_H

#include <Wt/WString>
using namespace Wt;
#include <QString>

class WtQtUtil
{
public:
    static WString fromQString(const QString &input);
    static QString fromWString(const WString &input);
private:
    WtQtUtil();
};

#endif // WTQTUTIL_H
