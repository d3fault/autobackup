#ifndef QUICKDIRTYAUTOBACKUPHALPERWIDGET_H
#define QUICKDIRTYAUTOBACKUPHALPERWIDGET_H

#include <QtGui/QWidget>
#include <QCloseEvent>

class QuickDirtyAutoBackupHalper;

class QuickDirtyAutoBackupHalperWidget : public QWidget
{
    Q_OBJECT
public:
    QuickDirtyAutoBackupHalperWidget(QWidget *parent = 0);
    void setHalper(QuickDirtyAutoBackupHalper *halper);
    ~QuickDirtyAutoBackupHalperWidget();
private:
    QuickDirtyAutoBackupHalper *m_Halper;
protected:
    virtual void closeEvent(QCloseEvent *closeEvent);
};

#endif // QUICKDIRTYAUTOBACKUPHALPERWIDGET_H
