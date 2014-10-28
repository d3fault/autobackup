#ifndef OSIOSPROFILEMANAGERDIALOG_H
#define OSIOSPROFILEMANAGERDIALOG_H

#include <QDialog>

class QComboBox;
class QPushButton;

class OsiosProfileManagerDialog : public QDialog
{
    Q_OBJECT
public:
    static const int ComboBoxIndexThatIndicatesUnchosen;

    explicit OsiosProfileManagerDialog(QWidget *parent = 0);
    QString profileNameChosen() const;
private:
    QComboBox *m_ExistingProfilesComboBox;
    QPushButton *m_OkButton;
private slots:
    void enableOrDisableOkButtonBasedOnIndexValidity(int newExistingUsersComboBoxIndex);
    void showCreateProfileDialog();
};

#endif // OSIOSPROFILEMANAGERDIALOG_H
