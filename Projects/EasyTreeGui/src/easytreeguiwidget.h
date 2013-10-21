#ifndef EASYTREEGUIWIDGET_H
#define EASYTREEGUIWIDGET_H

#include <QtGui/QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QCheckBox>

class EasyTreeGuiWidget : public QWidget
{
    Q_OBJECT    
public:
    EasyTreeGuiWidget(QWidget *parent = 0);
private:
    QVBoxLayout *m_Layout;

    QLabel *m_DirToGenerateEasyTreeForLabel;
    QLineEdit *m_DirToGenerateEasyTreeForLineEdit;
    QPushButton *m_DirToGenerateEasyTreeForBrowseButton;

    QLabel *m_EasyTreeOutputFilePathLabel;
    QLineEdit *m_EasyTreeOutputFilePathLineEdit;
    QPushButton *m_EasyTreeOutputFilePathBrowseButton;

    QCheckBox *m_CalculateHashesToo;

    QPushButton *m_GenerateEasyTreeFileButton;

    QPlainTextEdit *m_DebugOutput;
signals:
    void generateEasyTreeFileRequested(const QString &dirToTree, const QString &treeOutputFilePath, bool calculateMd5Sums);
public slots:
    void handleD(const QString &msg);
    void chooseDirToTree();
    void chooseTreeOutputFilePath();
    void doGenerateEasyTreeFile();
    void handleEasyTreeGuiBusinessFinishedGeneratingEasyTreeFile();
};

#endif // EASYTREEGUIWIDGET_H
