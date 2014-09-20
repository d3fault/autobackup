#ifndef RENAMEPROJECTDIALOG_H
#define RENAMEPROJECTDIALOG_H

#include <QDialog>

class QLineEdit;

class DesignEqualsImplementationProject;

class RenameProjectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RenameProjectDialog(DesignEqualsImplementationProject *project, QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~RenameProjectDialog() { }
private:
    DesignEqualsImplementationProject *m_Project;
    QLineEdit *m_ProjectNameLineEdit;
private slots:
    void handleAccepted();
};

#endif // RENAMEPROJECTDIALOG_H
