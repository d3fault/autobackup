#ifndef FORMAT2UI_H
#define FORMAT2UI_H

#include <QObject>

#include "uigeneratorfactory.h"
#include "uivariable.h"

class format2ui : public QObject
{
    Q_OBJECT
public:
    explicit format2ui(QObject *parent = 0);
public slots:
    void beginFormat2ui(const QString &filePathOfJsonUiFormatInput, const QString &cliArgUiTypeString);
private:
    void populateUiVariables(const QJsonObject &inputJsonObject, QList<UIVariable> *out_uiVariables);

    UIGeneratorFactory m_UIGeneratorFactory;
#if 0
    const QMetaObject cliArgUiTypeString2metaObject(const QString &cliArgUiTypeString);
#endif
signals:
    void finishedFormat2ui(bool success);
};

#endif // FORMAT2UI_H
