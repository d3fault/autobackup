#ifndef NOTHINGFANCYNOTEPADWITHAUTOFILENAMEBUTMANUALSAVEMAINWINDOW_H
#define NOTHINGFANCYNOTEPADWITHAUTOFILENAMEBUTMANUALSAVEMAINWINDOW_H

#include <QMainWindow>

class QTabWidget;

class NothingFancyNotepadWithAutoFilenameButManualSaveMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    NothingFancyNotepadWithAutoFilenameButManualSaveMainWindow(QWidget *parent = 0);
protected:
    virtual void closeEvent(QCloseEvent *closeEvent);
private:
    QTabWidget *m_MainTabWidget;
    QAction *m_NewFileTabAction;
    QAction *m_AutoFilenameSaveAction;
    quint32 m_CurrentVisibleTabIndex;

    QString m_CurrentProfile;
    QString m_CurrentProfileBaseDir_WithSlashAppended;
    bool m_FolderizeBaseDir;

    void createActions();
    void createToolbars();

    bool filePathTemplateFromCurrentDateTime(QString *out_FilePathTemplate);

    QByteArray myHexHash(const QByteArray &inputByteArray);
    void addNewFileTabAndMakeItCurrent();
private slots:
    void handleMainTabWidgetCurrentTabChanged(int newIndex);
    void handleMainTextEditChanged();
    void handleNewFileTabActionTriggered();
    void handleAutoFilenameSaveActionTriggered();
};

#endif // NOTHINGFANCYNOTEPADWITHAUTOFILENAMEBUTMANUALSAVEMAINWINDOW_H
