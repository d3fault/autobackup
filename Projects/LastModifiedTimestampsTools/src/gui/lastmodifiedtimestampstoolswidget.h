#ifndef LASTMODIFIEDTIMESTAMPSTOOLSWIDGET_H
#define LASTMODIFIEDTIMESTAMPSTOOLSWIDGET_H

#include <QWidget>

class QRadioButton;
class QPushButton;
class QPlainTextEdit;

class LabelLineEditBrowseButton;

class LastModifiedTimestampsToolsWidget : public QWidget
{
    Q_OBJECT
public:
    LastModifiedTimestampsToolsWidget(QWidget *parent = 0);
private:
    LabelLineEditBrowseButton *m_Directory;
    LabelLineEditBrowseButton *m_LastModifiedTimestampsFileRow;
    QRadioButton *m_GenerateLastModifiedTimestampsFileModeRadioButton;
    QPushButton *m_StartButton;
    QPlainTextEdit *m_Debug;
signals:
    void lastModifiedTimestampsFileGenerationRequested(const QString &dirToGenerateLastModifiedTimestampsFileFor, const QString &lastModifiedTimestampsFilePath);
    void heirarchyMolestationRequested(const QString &dirToMolest, const QString &lastModifiedTimestampsFilePath);
public slots:
    void handleD(const QString &msg);
    void reEnableStartButton();
private slots:
    void handleStartButtonClicked();
};

#endif // LASTMODIFIEDTIMESTAMPSTOOLSWIDGET_H
