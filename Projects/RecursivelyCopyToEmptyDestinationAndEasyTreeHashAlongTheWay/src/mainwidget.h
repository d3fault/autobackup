#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtGui/QWidget>
#include <QCryptographicHash>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QFileDialog>

Q_DECLARE_METATYPE(QCryptographicHash::Algorithm)

class MainWidget : public QWidget
{
    Q_OBJECT    
public:
    MainWidget(QWidget *parent = 0);
private:
    bool m_CopyInProgress;
    QString m_BeginCopyString;
    void updateGuiToReflectCopyInProgress();

    //Layout
    QVBoxLayout *m_Layout;

    QLabel *m_SourceDirectoryLabel;
    QLineEdit *m_SourceDirectoryLineEdit;
    QPushButton *m_SourceDirectoryBrowseButton;

    QLabel *m_EmptyDestinationDirectoryLabel;
    QLineEdit *m_EmptyDestinationDirectoryLineEdit;
    QPushButton *m_EmptyDestinationDirectoryBrowseButton;

    QLabel *m_EasyTreeHashOutputFilePathLabel;
    QLineEdit *m_EasyTreeHashOutputFilePathLineEdit;
    QPushButton *m_EasyTreeHashOutputFilePathBrowseButton;

    QComboBox *m_CryptographicHashAlgorithmComboBox;
    QPushButton *m_BeginOrCancelCopyButton;

    QPlainTextEdit *m_Debug;
    QPlainTextEdit *m_Error;
signals:
    void copyOperationRequested(const QString &sourceDirectory, const QString &emptyDestinationDirectory, const QString &easyTreeHashOutputFilePath, QCryptographicHash::Algorithm algorithm);
public slots:
    void handleD(const QString &msg);
    void handleE(const QString &msg);
    void handleSourceDirectoryBrowseButtonClicked();
    void handleEmptyDestinationDirectoryBrowseButtonClicked();
    void handleEasyTreeHashOutputFilePathBrowseButtonClicked();
    void handleBeginOrCancelCopyButtonClicked();
    void handleCopyOperationComplete(); //TODOreq: re-enable user input or something? or just flip the enabled of "Cancel" and "Begin" etc...
};

#endif // MAINWIDGET_H
