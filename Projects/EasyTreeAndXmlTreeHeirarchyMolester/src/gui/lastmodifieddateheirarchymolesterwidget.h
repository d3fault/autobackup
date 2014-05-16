#ifndef LASTMODIFIEDDATEHEIRARCHYMOLESTERWIDGET_H
#define LASTMODIFIEDDATEHEIRARCHYMOLESTERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QPlainTextEdit>
#include <QFileDialog>

class LastModifiedDateHeirarchyMolesterWidget : public QWidget
{
    Q_OBJECT    
public:
    LastModifiedDateHeirarchyMolesterWidget(QWidget *parent = 0);
private:
    QVBoxLayout *m_Layout;

    QLabel *m_DirectoryHeirarchyCorrespingToEasyTreeFileLabel;
    QLineEdit *m_DirectoryHeirarchyCorrespingToEasyTreeFileLineEdit;
    QPushButton *m_DirectoryHeirarchyCorrespingToEasyTreeFileBrowseButton;

    QLabel *m_EasyTreeFilePathLabel;
    QLineEdit *m_EasyTreeFilePathLineEdit;
    QPushButton *m_EasyTreeFilePathBrowseButton;

    QCheckBox *m_EasyTreeLinesHaveCreationDateCheckBox;

    QPushButton *m_MolestButton;

    QPlainTextEdit *m_Debug;
signals:
    void lastModifiedDateHeirarchyMolestationRequested(const QString &directoryHeirarchyCorrespingToEasyTreeFile, const QString &easyTreeFilePath, bool easyTreeLinesHaveCreationDate);
public slots:
    void handleD(const QString &msg);
private slots:
    void handleDirectoryHeirarchyCorrespingToEasyTreeFileBrowseButtonClicked();
    void handleEasyTreeFilePathBrowseButtonClicked();
    void handleMolestButtonClicked();
};

#endif // LASTMODIFIEDDATEHEIRARCHYMOLESTERWIDGET_H
