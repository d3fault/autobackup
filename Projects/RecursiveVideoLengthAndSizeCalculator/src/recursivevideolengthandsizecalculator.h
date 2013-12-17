#ifndef RECURSIVEVIDEOLENGTHANDSIZECALCULATOR_H
#define RECURSIVEVIDEOLENGTHANDSIZECALCULATOR_H

#include <QObject>
#include <QDir>
#include <QProcess>
#include <QProcessEnvironment>
#include <QFileInfoList>
#include <QFileInfoList>
#include <QFileInfo>
#include <QListIterator>
#include <QTextStream>
#include <QBuffer>

class RecursiveVideoLengthAndSizeCalculator : public QObject
{
    Q_OBJECT
public:
    explicit RecursiveVideoLengthAndSizeCalculator(QObject *parent = 0);
private:
    static const QString m_FfProbeProcessFilePath;
    QProcess m_FfProbeProcess;
    QList<QString> m_VideoExtensions;
    QDir::Filters m_DirFilter;
    bool privateRecursivelyCalculateVideoLengthsAndSizes(const QFileInfoList &rootFileInfoList);
    bool fileNameEndsWithOneOfTheseExentions(const QString &fileName, const QList<QString> &fileNameExtensions);
    double getValueFromOutputLine(const QString &outputLine);

    double m_RunningTotalDuration;
    double m_RunningTotalSize;
signals:
    void d(const QString &);
public slots:
    void recursivelyCalculateVideoLengthsAndSizes(const QString &dirString);
    void recursivelyCalculateVideoLengthsAndSizes(const QDir &theDir);
};

#endif // RECURSIVEVIDEOLENGTHANDSIZECALCULATOR_H
