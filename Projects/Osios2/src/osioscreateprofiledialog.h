#ifndef OSIOSCREATEPROFILEDIALOG_H
#define OSIOSCREATEPROFILEDIALOG_H

#include <QDialog>

class QLineEdit;

class LabelLineEditBrowseButton;

class OsiosCreateProfileDialog : public QDialog
{
    Q_OBJECT
public:
    explicit OsiosCreateProfileDialog(QWidget *parent = 0);
    QString newProfileName() const;
    //QString chosenDataDir() const;
private:
    QLineEdit *m_ProfileNameLineEdit;
    LabelLineEditBrowseButton *m_DataDirRow;
    QPushButton *m_OkButton;

    const QString &defaultDataDirectory() const;
    QString dataDirectoryResolved();
    bool profileContentsValid();
private slots:
    void enableOrDisableOkButtonDependingOnContentsValidate();
    void createProfileAndAcceptDialog();
};

#endif // OSIOSCREATEPROFILEDIALOG_H
