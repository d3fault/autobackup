#ifndef NOTHINGFANCYNOTEPADWITHAUTOFILENAMEBUTMANUALSAVEMAINWINDOW_H
#define NOTHINGFANCYNOTEPADWITHAUTOFILENAMEBUTMANUALSAVEMAINWINDOW_H

#include <QMainWindow>

class QPlainTextEdit;

class NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow(QWidget *parent = 0);
protected:
    virtual void closeEvent(QCloseEvent *closeEvent);
private:
    QPlainTextEdit *m_MainTextEdit;
    QAction *m_AutoFilenameSaveAction;

    void createActions();
    void createToolbars();

    QString autoFileNameFromContentsAndCurrentTime(const QByteArray &fileContents);
private slots:
    void handleAutoFilenameSaveActionTriggered();
};

#endif // NOTHINGFANCYNOTEPADWITHAUTOFILENAMEBUTMANUALSAVEMAINWINDOW_H
