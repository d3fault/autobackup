#ifndef EASYTREEHASHDIFFANALYZER_H
#define EASYTREEHASHDIFFANALYZER_H

#include <QObject>
#include <QIODevice>
#include <QTextStream>
#include <QList>

#include "easytreeparser.h"
#include "easytreehashitem.h"

class EasyTreeHashDiffAnalyzer : public QObject
{
    Q_OBJECT
public:
    explicit EasyTreeHashDiffAnalyzer(QObject *parent = 0);
    void analyzeEasyTreeHashesAndReportDifferences(QIODevice *easyTreeHash1, QIODevice *easyTreeHash2); //assumes that both of them have a hash or both don't have a hash. can't intermix or else we'll get fucked results
private:
    static const QString SpaceString;
    static const QString ColonString;
    static const QString MismatchString;
    static const QString ExistenceString;
    static const QString OnlyString;
    static const QString FileSizeString;
    static const QString HashString;
    static const QString VersusString;
    static const QString EqualsString;
    static const QString Input1String;
    static const QString Input2String;
signals:
    void d(const QString &);
public slots:
    
};

#endif // EASYTREEHASHDIFFANALYZER_H
