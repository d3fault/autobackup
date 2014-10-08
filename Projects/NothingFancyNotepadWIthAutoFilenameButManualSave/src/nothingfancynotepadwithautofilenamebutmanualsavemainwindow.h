#ifndef NOTHINGFANCYNOTEPADWITHAUTOFILENAMEBUTMANUALSAVEMAINWINDOW_H
#define NOTHINGFANCYNOTEPADWITHAUTOFILENAMEBUTMANUALSAVEMAINWINDOW_H

#include <QMainWindow>

class QTabWidget;

class NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    NothingFancyNotepadWIthAutoFilenameButManualSaveMainWindow(QWidget *parent = 0);
protected:
    virtual void closeEvent(QCloseEvent *closeEvent);
private:
    QTabWidget *m_MainTabWidget;
    QAction *m_NewFileTabAction;
    QAction *m_AutoFilenameSaveAction;
    quint32 m_CurrentVisibleTabIndex;

    QString m_CurrentProfile;
    QString m_CurrentProfileBaseDir;
    bool m_FolderizeBaseDir;

    void createActions();
    void createToolbars();

    QString autoFileNameFromContentsAndCurrentTime(const QByteArray &fileContents);

    QByteArray myHexHash(const QByteArray &inputByteArray);
    void addNewFileTabAndMakeItCurrent();
private slots:
    void handleMainTabWidgetCurrentTabChanged(int newIndex);
    void handleMainTextEditChanged();
    void handleNewFileTabActionTriggered();
    void handleAutoFilenameSaveActionTriggered();
};

#endif // NOTHINGFANCYNOTEPADWITHAUTOFILENAMEBUTMANUALSAVEMAINWINDOW_H
