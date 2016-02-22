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
    QString tabTitle() const;
    bool isSaved() const;
    bool isEmpty() const;
    bool saveAndFudgeLastModifiedTimestamp();
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
public slots:
    void setFocusOnDocument();
private slots:
    void handleTextChanged();
};

#endif // MINDDUMPDOCUMENT_H
