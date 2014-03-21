#ifndef HOTTEE_H
#define HOTTEE_H

#include <QObject>

class QProcess;
class QFile;

class Hottee : public QObject
{
    Q_OBJECT
public:
    explicit Hottee(QObject *parent = 0);
private:
    QProcess *m_InputProcess;
    QProcess *m_OutputProcess;

    QFile *m_CurrentOutputFile;
    QString m_CurrentDestinationEndingWithSlash;
    qint64 m_Current100mbChunkWriteOffset;
    QString m_Destination1endingWithSlash;
    QString m_Destination2endingWithSlash;
    bool m_Dest2;

    qint64 m_100mbChunkOffsetForFilename;

    void toggleDestinations();
    void createAndOpen100mbFileAtCurrentDestination();
    inline QString appendSlashIfNeeded(QString inputString)
    {
        if(!inputString.endsWith("/"))
            inputString.append("/");
        return inputString;
    }
    bool readInputProcessesStdOutAndWriteAccordingly();
signals:
    void d(const QString &);
    void changeAlternateSoon(const QString &destinationAboutSwitchToSoon);
    void destinationToggled(const QString &destinationJustSwitchedFrom);
public slots:
    void startHotteeing(const QString &inputProcessPathAndArgs, const QString &outputProcessPathAndArgs, const QString &destinationDir1, const QString &destinationDir2);
    void stopHotteeing();
private slots:
    void handleInputProcessReadyReadStandardOutput();
    void handleInputStdErr();
};

#endif // HOTTEE_H
