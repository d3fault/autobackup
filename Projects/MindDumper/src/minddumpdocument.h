#ifndef MINDDUMPDOCUMENT_H
#define MINDDUMPDOCUMENT_H

#include <QWidget>

class QLineEdit;
class QCheckBox;
class QPlainTextEdit;

class MindDumpDocument : public QWidget
{
    Q_OBJECT
public:
    explicit MindDumpDocument(const QString &tabTitle, const QString &mindDumpDirectoryWithSlashAppended, QWidget *parent = 0);
    void setFocusOnDocument();
    QString tabTitle() const;
    bool isSaved() const;
private:
    QString m_TabTitle;
    QString m_MindDumpDirectoryWithSlashAppended;
    bool m_IsSaved;
    QLineEdit *m_FilenameLineEdit;
    QCheckBox *m_DreamCheckbox;
    QPlainTextEdit *m_Document;
    qint64 m_TimestampOfFirstKeystroke; //TODoreq: backspace restarts this
    qint64 m_TimestampOfLastKeystroke;

    static bool charOfFilenameisSane(const QString &charOfFilename);
    static QString filterFilenameChar(const QString &charOfFilenameToFilter);
    static QString fileNameSanitized(const QString &inputFileName);
signals:
    void savedAndFudgedLastModifiedTimestamp(bool success);
public slots:
    void saveAndFudgeLastModifiedTimestamp();
private slots:
    void handleTextChanged();
};
class ResultEmitter : public QObject
{
    Q_OBJECT
public:
    explicit ResultEmitter(QObject *parent = 0)
        : QObject(parent)
        , m_Success(false)
    { }
    void setSuccess(bool success) { m_Success = success; }
    ~ResultEmitter() { emit haveResult(m_Success); }
signals:
    void haveResult(bool success);
private:
    bool m_Success;
};

#endif // MINDDUMPDOCUMENT_H
