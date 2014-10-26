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
    QString chosenDataDir() const;
private:
    QLineEdit *m_ProfileNameLineEdit;
    LabelLineEditBrowseButton *m_DataDirRow;
};

#endif // OSIOSCREATEPROFILEDIALOG_H
