#ifndef NEWPROFILEDIALOG_H
#define NEWPROFILEDIALOG_H

#include <QDialog>

class QLineEdit;
class QCheckBox;

class LabelLineEditBrowseButton;

class NewProfileDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NewProfileDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);

    //these are only valid after the dialog has been accepted
    QString newProfileName();
    QString newProfileBaseDir();
    bool folderizeBaseDir();

    static inline QString appendSlashIfNeeded(const QString &stringInput)
    {
        if(stringInput.endsWith("/"))
        {
            return stringInput;
        }
        return stringInput + "/";
    }
private:
    QLineEdit *m_ProfileNameLineEdit;
    LabelLineEditBrowseButton *m_AutoSaveBaseDirRow;
    QCheckBox *m_FolderizeCheckbox;
private slots:
    void sanitizeAndMaybeAcceptUserInput();
};

#endif // NEWPROFILEDIALOG_H
