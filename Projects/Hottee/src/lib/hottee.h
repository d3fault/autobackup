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
    ~Hottee();
private:
    QProcess *m_InputProcess;
    QProcess *m_OutputProcess;
    bool m_WriteToOutputProcess;
    QFile *m_CurrentOutputFile;
    QString m_CurrentDestinationEndingWithSlash;
    qint64 m_Current100mbChunkWriteOffset;
    QString m_Destination1endingWithSlash;
    QString m_Destination2endingWithSlash;
    bool m_Dest2;
    bool m_CurrentlyWritingToEitherDestination;
    bool m_StopWritingAtEndOfThisChunk;
    bool m_StartWritingAtBeginningOfNextChunk;
    bool m_QuitAfterThisChunkFinishes;
    qint64 m_100mbChunkOffsetForFilename;
    double m_DestinationStoragePercentUsedLastTime;

    bool eitherDestinationIsLessThan1gbCapacity();
    bool filesystemIsLessThan1gbCapacity(const QString &pathOfFilesystem);
    void toggleDestinations();
    bool createAndOpen100mbFileAtCurrentDestination();
    inline QString appendSlashIfNeeded(QString inputString)
    {
        if(!inputString.endsWith("/"))
            inputString.append("/");
        return inputString;
    }
    bool readInputProcessesStdOutAndWriteAccordingly();
signals:
    void d(const QString &);
    void o(const QString &);
    void quitRequested();
public slots:
    void startHotteeing(const QString &inputProcessPathAndArgs, const QString &destinationDir1, const QString &destinationDir2, const QString &outputProcessPathAndArgs);
    void queryChunkWriteOffsetAndStorageCapacityStuff();
    void startWritingAtNextChunkStart();
    void stopWritingAtEndOfThisChunk();
    void quitAfterThisChunkFinishes();
    void cleanupHotteeing();
private slots:
    void handleInputProcessReadyReadStandardOutput();
    void handleInputStdErr();
};

#endif // HOTTEE_H
