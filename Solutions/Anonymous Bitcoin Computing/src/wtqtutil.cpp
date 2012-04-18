#include "wtqtutil.h"

WtQtUtil::WtQtUtil()
{
}
Wt::WString WtQtUtil::fromQString(const QString &input)
{
    return WString::fromUTF8((const char *)input.toUtf8());
}
QString WtQtUtil::fromWString(const Wt::WString &input)
{
    return QString::fromUtf8(input.toUTF8().c_str());
}
