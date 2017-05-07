#include "mainwidget.h"

#include "quickdirtyautobackuphalperbusiness.h"

mainWidget::mainWidget(QWidget *parent)
    : QWidget(parent), m_Business(0), m_DateTimeSpecialString(COMMIT_MESSAGE_DATE_SPECIAL_REPLACE_STRING), m_CancelShutdownText("Cancel Shutdown"), m_ShutdownText("Shutdown"), m_BrokenOverride(false), m_ShuttingDown(false)
{
    m_DeviceAndPasswordPathsToBePopulatedJustBeforeMountRequestedOrPersistRequestedOrCommit = new QList<QPair<QString,QString> >();
    m_FilenameIgnoreList = new QStringList();
    m_FilenameEndsWithIgnoreList = new QStringList();
    m_DirnameIgnoreList = new QStringList();
    m_DirnameEndsWithIgnoreList = new QStringList();
    setupGui();
    this->setEnabled(false);
}
mainWidget::~mainWidget()
{
    delete m_DirnameEndsWithIgnoreList;
    delete m_DirnameIgnoreList;
    delete m_FilenameEndsWithIgnoreList;
    delete m_FilenameIgnoreList;
    delete m_DeviceAndPasswordPathsToBePopulatedJustBeforeMountRequestedOrPersistRequestedOrCommit;
}
void mainWidget::setBusiness(QuickDirtyAutoBackupHalperBusiness *business)
{
    m_Business = business;

    connectToBusiness();
}
QHBoxLayout *mainWidget::createNewTcRow(const QString &containerPath, const QString &passwordPath)
{
    QHBoxLayout *newTcRow = new QHBoxLayout();

    QLabel *tcLocationLabel = new QLabel("Truecrypt Device/Container:");
    QLineEdit *tcLocationLe = new QLineEdit(containerPath);
    QPushButton *browseForTcLocationBtn = new QPushButton("Browse");
    m_BrowseButtonLineEditAssociations.insert(browseForTcLocationBtn, tcLocationLe);
    connect(browseForTcLocationBtn, SIGNAL(clicked()), this, SLOT(handleBrowseButtonClickedAndFigureOutWhichLineEditToUpdate()));

    QLabel *tcPasswordLocationLabel = new QLabel("Password File:");
    QLineEdit *tcPasswordLocationLe = new QLineEdit(passwordPath);
    QPushButton *browseForTcPasswordLocationBtn = new QPushButton("Browse");
    m_BrowseButtonLineEditAssociations.insert(browseForTcPasswordLocationBtn, tcPasswordLocationLe);
    connect(browseForTcPasswordLocationBtn, SIGNAL(clicked()), this, SLOT(handleBrowseButtonClickedAndFigureOutWhichLineEditToUpdate()));

    newTcRow->addWidget(tcLocationLabel);
    newTcRow->addWidget(tcLocationLe, 1);
    newTcRow->addWidget(browseForTcLocationBtn);
    newTcRow->addWidget(tcPasswordLocationLabel);
    newTcRow->addWidget(tcPasswordLocationLe, 1);
    newTcRow->addWidget(browseForTcPasswordLocationBtn);

    m_TcRowsList.append(qMakePair(tcLocationLe,tcPasswordLocationLe));

    return newTcRow;
}
void mainWidget::rePopulateListFromGuiRowsExcludingEmpty()
{
    m_DeviceAndPasswordPathsToBePopulatedJustBeforeMountRequestedOrPersistRequestedOrCommit->clear();

    int tcRowCount = m_TcRowsList.size();
    for(int i = 0; i < tcRowCount; ++i)
    {
        //thought about doing file exists/notDir sanitization here... but it belongs in back-end. still going to do !empty checks though
        if(m_TcRowsList.at(i).first->text().trimmed().isEmpty() || m_TcRowsList.at(i).second->text().trimmed().isEmpty())
        {
            //we don't error on empty fields, we just skip them
            continue;
        }
        else
        {
            m_DeviceAndPasswordPathsToBePopulatedJustBeforeMountRequestedOrPersistRequestedOrCommit->append(qMakePair(m_TcRowsList.at(i).first->text(),m_TcRowsList.at(i).second->text()));
        }
    }
}
void mainWidget::rePopulateNameFilterListFromGui()
{
    m_FilenameIgnoreList->clear();
    m_FilenameEndsWithIgnoreList->clear();
    m_DirnameIgnoreList->clear();
    m_DirnameEndsWithIgnoreList->clear();

    addCommaSeparatedValuesToListFromLineEdit(m_FilenameIgnoreLineEdit, m_FilenameIgnoreList);
    addCommaSeparatedValuesToListFromLineEdit(m_FilenamesEndsWithIgnoreLineEdit, m_FilenameEndsWithIgnoreList);
    addCommaSeparatedValuesToListFromLineEdit(m_DirnameIgnoreLineEdit, m_DirnameIgnoreList);
    addCommaSeparatedValuesToListFromLineEdit(m_DirnameEndsWithIgnoreLineEdit, m_DirnameEndsWithIgnoreList);
}
void mainWidget::addCommaSeparatedValuesToListFromLineEdit(QLineEdit *commaSeparatedValuesLineEdit, QStringList *listToPutNonEmptyValuesInto)
{
    if(commaSeparatedValuesLineEdit->text().trimmed().isEmpty())
    {
        return;
    }

    if(commaSeparatedValuesLineEdit->text().contains("/") || commaSeparatedValuesLineEdit->text().contains("\\"))
    {
        handleD("filters can't contain a backslash, only a forward slash for escaping the use of commas in filters, and the forward slash itself"); //TODOoptional: put this in mouse hover over field help popup for the filters line edits
        handleBrokenStateDetected();
        return;
    }

    //hack: we're hiding in the directory identifier that we just filtered out :)
    QString commaFiltered = commaSeparatedValuesLineEdit->text();
    commaFiltered.replace("\\,", "/");

    QStringList separatedByComma = commaFiltered.split(QString(","), QString::SkipEmptyParts);

    QListIterator<QString> it(separatedByComma);
    while(it.hasNext())
    {
        QString tehValue = it.next().trimmed();
        if(tehValue.isEmpty())
        {
            continue;
        }

        //the second step of the hack: putting the comma back in (or whatever we were trying to escape):
        tehValue.replace("/", ",");


        listToPutNonEmptyValuesInto->append(tehValue);
    }
}
void mainWidget::connectToBusiness()
{
    //connect our signals (buttons etc) to business slots
    connect(this, SIGNAL(pushToGitIgnoreRequested(QString,QStringList*,QStringList*,QStringList*,QStringList*)), m_Business, SLOT(pushToGitIgnore(QString,QStringList*,QStringList*,QStringList*,QStringList*)));
    connect(this, SIGNAL(persistRequested(QList<QPair<QString,QString> >*)), m_Business, SLOT(persist(QList<QPair<QString,QString> >*)));
    connect(this, SIGNAL(mountRequested(QList<QPair<QString,QString> >*,bool)), m_Business, SLOT(mount(QList<QPair<QString,QString> >*,bool)));
    connect(m_DismountTcRowsButton, SIGNAL(clicked()), m_Business, SLOT(dismount()));
    connect(this, SIGNAL(commitRequested(QString,QList<QPair<QString,QString> >*,QString,QString,QString,QStringList*,QStringList*,QStringList*,QStringList*,bool,bool)), m_Business, SLOT(commit(QString,QList<QPair<QString,QString> >*,QString,QString,QString,QStringList*,QStringList*,QStringList*,QStringList*,bool,bool)));
    connect(this, SIGNAL(shutdownRequested()), m_Business, SLOT(shutdown()));
    connect(this, SIGNAL(cancelShutdownRequested()), m_Business, SLOT(cancelShutdown()));

    //connect business's signals (results) to our slots (update GUI)
    connect(m_Business, SIGNAL(brokenStateDetected()), this, SLOT(handleBrokenStateDetected()));
    connect(m_Business, SIGNAL(readSettingsAndProbed(bool,bool,QList<QPair<QString,QString> >*)), this, SLOT(handleSettingsReadAndDevicesProbed(bool,bool,QList<QPair<QString,QString> >*)));
    connect(m_Business, SIGNAL(mounted()), this, SLOT(handleMounted()));
    connect(m_Business, SIGNAL(dismounted()), this, SLOT(handleDismounted()));
    connect(m_Business, SIGNAL(committed()), this, SLOT(handleCommitted()));

    connect(m_Business, SIGNAL(d(const QString &)), this, SLOT(handleD(const QString &)));
}
void mainWidget::setupGui()
{
    m_Layout = new QVBoxLayout();
    m_ReadOnlyCheckBox = new QCheckBox("Mount First As Read-Only:"); //TODOreq: impl the 'first' [only] aspect'
    m_FilenameIgnoreLineEdit = new QLineEdit();
    m_FilenamesEndsWithIgnoreLineEdit = new QLineEdit(".pro.user,~");
    m_DirnameIgnoreLineEdit = new QLineEdit(".git");
    m_DirnameEndsWithIgnoreLineEdit = new QLineEdit("_GCC_32bit-Release,_GCC_32bit-Debug,_GCC_64bit-Debug,_GCC_64bit-Release,-Qt_4_8_6-Debug,-Qt_4_8_6-Release,-Qt_5_3_2-Debug,-Qt_5_3_2-Release,-Qt_4_8_6_Clang-Debug,-Qt_5_3_2_Clang-Debug,-Qt_4_8_6_Clang-Release,-Qt_5_3_2_Clang-Release,-Qt_4_8_6_GCC-Debug,-Qt_5_3_2_GCC-Debug,-Qt_4_8_6_GCC-Release,-Qt_5_3_2_GCC-Release");
    m_PushToGitIgnoreButton = new QPushButton("Push To .gitignore"); //because i don't want it to be rewritten every time, and detecting changes is too hard
    m_WorkingDirectoryLineEdit = new QLineEdit("/home/user/text");
    m_MountPointSubDirToBareGitRepoLineEdit = new QLineEdit("autobackup"); //TODOoptional: customizable/persist'able. we append this sub-dir to each mount point when doing git push. we might additionally want to be able to specify the "working dir" (the FULL dir, not just sub-dir)... and hell this could even transform into "profiles" (one working dir has many tc containers and a configured subdir specific to that one working dir). for now hard-coded and single 'profile' is fine
    m_DirStructureFileNameLineEdit = new QLineEdit(".lastModifiedTimestamps");
    m_RowsPlaceholderWidget = new QWidget();
    m_RowsLayout = new QVBoxLayout();
    m_AddTcRowButton = new QPushButton("Add Row");
    m_PersistTcRowsButton = new QPushButton("Save Rows");
    m_MountTcRowsButton = new QPushButton("Mount");
    m_DismountTcRowsButton = new QPushButton("Dismount");
    m_CommitMessageLineEdit = new QLineEdit(QString("QuickDirtyAutoBackupHalper3 Commit @") + m_DateTimeSpecialString);

    m_PushEvenWhenNothingToCommitCheckbox = new QCheckBox("git push even when nothing to commit");
    m_PushEvenWhenNothingToCommitCheckbox->setChecked(true);
    m_SignCommitCheckbox = new QCheckBox("Sign commit");
    m_CommitIfNeededBeforeShuttingDownCheckbox = new QCheckBox("Commit if needed before shutting down"); //before dismounting is implied. we get to see the results of the git commit/push in our debug output with 3 seconds of reading time + cancelling the shutdown and fixing the commit/push'ing if needed
    m_CommitIfNeededBeforeShuttingDownCheckbox->setChecked(true);

    m_CommitAfterMountingIfNeededButton = new QPushButton("Commit");
    m_ShutdownAfterDismountingIfNeededButton = new QPushButton(m_ShutdownText);
    m_Debug = new QPlainTextEdit();

    m_Layout->setAlignment(Qt::AlignTop);

    QHBoxLayout *miscSettingsHBox = new QHBoxLayout();

    miscSettingsHBox->addWidget(m_ReadOnlyCheckBox);

    //filters
    miscSettingsHBox->addWidget(m_FilenameIgnoreLineEdit);
    miscSettingsHBox->addWidget(m_FilenamesEndsWithIgnoreLineEdit);
    miscSettingsHBox->addWidget(m_DirnameIgnoreLineEdit);
    miscSettingsHBox->addWidget(m_DirnameEndsWithIgnoreLineEdit);
    miscSettingsHBox->addWidget(m_PushToGitIgnoreButton);

    m_Layout->addLayout(miscSettingsHBox);

    QHBoxLayout *dirsToWorkWithHLayout = new QHBoxLayout();
    dirsToWorkWithHLayout->addWidget(new QLabel("Tree Stats File:"));
    dirsToWorkWithHLayout->addWidget(m_DirStructureFileNameLineEdit);
    dirsToWorkWithHLayout->addWidget(new QLabel("Working Dir:"));
    dirsToWorkWithHLayout->addWidget(m_WorkingDirectoryLineEdit);
    dirsToWorkWithHLayout->addWidget(new QLabel("Git Bare Repo Sub-Dir On Mount Point:"));
    dirsToWorkWithHLayout->addWidget(m_MountPointSubDirToBareGitRepoLineEdit, 1);
    m_Layout->addLayout(dirsToWorkWithHLayout);

    m_Layout->addWidget(m_RowsPlaceholderWidget);

    QHBoxLayout *rowControlButtons = new QHBoxLayout();
    rowControlButtons->addWidget(m_AddTcRowButton);
    rowControlButtons->addWidget(m_PersistTcRowsButton);
    m_Layout->addLayout(rowControlButtons);

    QHBoxLayout *mountControlButtons = new QHBoxLayout();
    mountControlButtons->addWidget(m_MountTcRowsButton);
    mountControlButtons->addWidget(m_DismountTcRowsButton);
    m_Layout->addLayout(mountControlButtons);

    QHBoxLayout *commitClearAndLineEditHBox = new QHBoxLayout();
    m_ClearCommitMessageButton = new QPushButton("Clear");
    commitClearAndLineEditHBox->addWidget(m_ClearCommitMessageButton);
    commitClearAndLineEditHBox->addWidget(m_CommitMessageLineEdit, 1);
    m_Layout->addLayout(commitClearAndLineEditHBox);

    QHBoxLayout *autoCommitAndPushControlsHBox = new QHBoxLayout();
    autoCommitAndPushControlsHBox->addWidget(m_PushEvenWhenNothingToCommitCheckbox);
    autoCommitAndPushControlsHBox->addWidget(m_SignCommitCheckbox);
    autoCommitAndPushControlsHBox->addWidget(m_CommitIfNeededBeforeShuttingDownCheckbox);
    m_Layout->addLayout(autoCommitAndPushControlsHBox);

    QHBoxLayout *commitAndShutdownButtons = new QHBoxLayout();
    commitAndShutdownButtons->addWidget(m_CommitAfterMountingIfNeededButton);
    commitAndShutdownButtons->addWidget(m_ShutdownAfterDismountingIfNeededButton); //TODOoptional: after clicking 'shutdown', start a 3 second timer (debug output at each second tick) and change shutdown to 'cancel' that can be clicked within those three seconds... just in case i click shutdown on accident or something. the dismount procedure should take place before the 3 seconds start... which also gives me the ability to read the debug message that dismount was successful during those 3 seconds
    //TODOoptional: a checkbox for 'commit any unsaved changes before shutting down', unchecked by default
    m_Layout->addLayout(commitAndShutdownButtons);

    m_Layout->addWidget(m_Debug, 1);

    //GUI-only connections go here
    connect(m_ReadOnlyCheckBox, SIGNAL(toggled(bool)), this, SLOT(handleReadOnlyCheckToggled(bool)));
    connect(m_PushToGitIgnoreButton, SIGNAL(clicked()), this, SLOT(handlePushToGitIgnoreButtonClicked()));
    connect(m_AddTcRowButton, SIGNAL(clicked()), this, SLOT(addTcRowToGui()));
    connect(m_PersistTcRowsButton, SIGNAL(clicked()), this, SLOT(handlePersistRowsButtonClicked()));
    connect(m_MountTcRowsButton, SIGNAL(clicked()), this, SLOT(handleMountButtonClicked()));
    connect(m_ClearCommitMessageButton, SIGNAL(clicked()), this, SLOT(clearCommitMessageAndFocusTheLineEdit()));
    connect(m_CommitAfterMountingIfNeededButton, SIGNAL(clicked()), this, SLOT(handleCommitButtonClicked()));
    connect(m_ShutdownAfterDismountingIfNeededButton, SIGNAL(clicked()), this, SLOT(handleShutdownButtonClicked()));

    this->setLayout(m_Layout);

    connect(m_CommitMessageLineEdit, SIGNAL(returnPressed()), m_CommitAfterMountingIfNeededButton, SLOT(click()));
}
void mainWidget::enableDisableMountDismountButtonsAndRowLineEditChangeability(bool mounted)
{
    m_ReadOnlyCheckBox->setEnabled(!mounted);
    m_MountTcRowsButton->setEnabled(!mounted);
    m_DismountTcRowsButton->setEnabled(mounted);
    int rowCount = m_TcRowsList.size();
    for(int i = 0; i < rowCount; ++i)
    {
        m_TcRowsList.at(i).first->setEnabled(!mounted);
        m_TcRowsList.at(i).second->setEnabled(!mounted);
    }
}
void mainWidget::handleD(const QString &msg)
{
    m_Debug->appendPlainText(msg);
}
void mainWidget::handleBrokenStateDetected()
{
    m_BrokenOverride = true;
    QSettings settings;
    handleD(QString("you broke something (check above errors). disabling the GUI so shit doesn't get fucked worse. if you need to start your configuration over, delete the file: ") + settings.fileName());
    this->setEnabled(false);
}
void mainWidget::handleReadOnlyCheckToggled(bool checked)
{
    m_MountPointSubDirToBareGitRepoLineEdit->setEnabled(!checked);
    m_ClearCommitMessageButton->setEnabled(!checked);
    m_CommitMessageLineEdit->setEnabled(!checked);
    m_CommitAfterMountingIfNeededButton->setEnabled(!checked);
    m_FilenameIgnoreLineEdit->setEnabled(!checked);
    m_FilenamesEndsWithIgnoreLineEdit->setEnabled(!checked);
    m_DirnameIgnoreLineEdit->setEnabled(!checked);
    m_DirnameEndsWithIgnoreLineEdit->setEnabled(!checked);
    m_WorkingDirectoryLineEdit->setEnabled(!checked);
    m_PushToGitIgnoreButton->setEnabled(!checked);
    m_SignCommitCheckbox->setEnabled(!checked);
    m_PushEvenWhenNothingToCommitCheckbox->setEnabled(!checked);
    m_CommitIfNeededBeforeShuttingDownCheckbox->setEnabled(!checked);
    //TOD ONEreq: disable the 'commit if necessary' m_CommitIfNeededBeforeShuttingDownCheckbox next to shutdown when read-only. it might still be checked though so we need to make sure we emit the m_CommitIfNeededBeforeShuttingDownCheckbox::isChecked && m_CommitIfNeededBeforeShuttingDownCheckbox::isEnabled value... (overriding the isChecked) before sending a signal to backend. or i could do m_CommitIfNeededBeforeShuttingDownCheckbox::isChecked && !ReadOnly::isChecked, but it's the same since we setEnabled to true whenever checked changes. still, best not to rely on gui enable/disabled state and better to rely on checked state of read only. just seems proper'er
    //^^basically, a read-only shutdown just does a truecrypt -d to dismount ALL volumes (even ones not on our list********) then just shuts down... we don't want the "commit if necessary" to factor in because we are read-only!
    m_DirStructureFileNameLineEdit->setEnabled(!checked);
}
void mainWidget::handlePushToGitIgnoreButtonClicked()
{
    if(m_WorkingDirectoryLineEdit->text().trimmed().isEmpty())
    {
        handleD("working dir cannot be empty");
        handleBrokenStateDetected();
        return;
    }
    rePopulateNameFilterListFromGui();
    emit pushToGitIgnoreRequested(m_WorkingDirectoryLineEdit->text().trimmed(), m_FilenameIgnoreList, m_FilenameEndsWithIgnoreList, m_DirnameIgnoreList, m_DirnameEndsWithIgnoreList);
}
//In retrospect, it was stupid of me to probe the settings in the backend business thread... and in all honesty the backend was too monolithic and should have been more modular, with the front-end doing most of the stuff (or a middle CONTROLLER (hmm something to think about in my cli/gui/lib "split" ultra perfect design attempts). BUT who gives a fuck this was quick/dirty like the title says. The git interaction should have been it's own class, same with the truecrypt interaction.
//^20 minutes after writing that: it WAS better to have the "business" probe the content, because IT is the "core logic" that should be utilizing hypothetical git/truecrypt helper-classes. It is the "controller" I was saying should be re-usable between cli/gui. In this specific app, reading a list of truecrypt volumes to mount is a core piece of the functionality. BUT THEN AGAIN if you were doing it in CLI mode then you'd probably have it "scripted" (the loading of truecrypt volumes would be in the command line calling the CLI (but actually not really.. if you were doing that, you wouldn't need this app at all! just call truecrypt/git directly xD) -- idk i'm lost and this is just a fucking hack anyways, but i think you COULD say that reading settings is a core part of functionality. you could make a cli to this app be "interactive" (add/remove/customize truecrypt shit), but also "auto"/non-interactive which just loads the shit from the settings... bah)
void mainWidget::handleSettingsReadAndDevicesProbed(bool allMountedTrueOrAllDismountedFalse, bool theyAreMountedAsReadOnlyIfTheyAreMountedAtAll, QList<QPair<QString, QString> > *deviceAndPasswordPathsFromSettingsRegardlessOfMountStatus)
{
    if(allMountedTrueOrAllDismountedFalse)
    {
        //we can't rely on the state of it if we are not mounted
        m_ReadOnlyCheckBox->setChecked(theyAreMountedAsReadOnlyIfTheyAreMountedAtAll);
    }
    if(!m_BrokenOverride)
    {
        this->setEnabled(true);
        m_CommitMessageLineEdit->setFocus();

        handleD("probed settings received from business");
        if(deviceAndPasswordPathsFromSettingsRegardlessOfMountStatus->size() == 0)
        {
            addTcRowToGui();
        }
        else
        {
            int pathCount = deviceAndPasswordPathsFromSettingsRegardlessOfMountStatus->size();
            for(int i = 0; i < pathCount; ++i)
            {
                addTcRowToGui(deviceAndPasswordPathsFromSettingsRegardlessOfMountStatus->at(i).first, deviceAndPasswordPathsFromSettingsRegardlessOfMountStatus->at(i).second);
            }
        }
        enableDisableMountDismountButtonsAndRowLineEditChangeability(allMountedTrueOrAllDismountedFalse);
    }
}
void mainWidget::addTcRowToGui(const QString &containerPath, const QString &passwordPath)
{
    m_RowsLayout->addLayout(createNewTcRow(containerPath, passwordPath));
    m_RowsPlaceholderWidget->setLayout(m_RowsLayout);
}
void mainWidget::handlePersistRowsButtonClicked()
{
    rePopulateListFromGuiRowsExcludingEmpty();

    if(m_DeviceAndPasswordPathsToBePopulatedJustBeforeMountRequestedOrPersistRequestedOrCommit->size() > 0)
    {
        emit persistRequested(m_DeviceAndPasswordPathsToBePopulatedJustBeforeMountRequestedOrPersistRequestedOrCommit);
    }
    else
    {
        handleD("nothing to save. select some truecrypt containers and password files first");
    }
}
void mainWidget::handleMountButtonClicked()
{
    rePopulateListFromGuiRowsExcludingEmpty();

    if(m_DeviceAndPasswordPathsToBePopulatedJustBeforeMountRequestedOrPersistRequestedOrCommit->size() > 0)
    {
        emit mountRequested(m_DeviceAndPasswordPathsToBePopulatedJustBeforeMountRequestedOrPersistRequestedOrCommit, m_ReadOnlyCheckBox->isChecked());
    }
    else
    {
        handleD("Select some truecrypt containers and password files before mounting");
    }
}
void mainWidget::handleMounted()
{
    enableDisableMountDismountButtonsAndRowLineEditChangeability(true);
}
void mainWidget::handleDismounted()
{
    enableDisableMountDismountButtonsAndRowLineEditChangeability(false);
}
void mainWidget::handleCommitted()
{
    //TODOoptional: disable 'commit' button and re-enable it when we get here... just a safety measure
}
void mainWidget::clearCommitMessageAndFocusTheLineEdit()
{
    m_CommitMessageLineEdit->clear();
    m_CommitMessageLineEdit->setFocus();
}
void mainWidget::handleCommitButtonClicked()
{
    rePopulateListFromGuiRowsExcludingEmpty();
    rePopulateNameFilterListFromGui();
    if(m_BrokenOverride) //currently would only be set when detecting an escaped comma in an input filter. still, a good idea to always check this before doing any *requested signal?
    {
        return;
    }
    QString commitMessage(m_CommitMessageLineEdit->text());
    if(commitMessage.trimmed().isEmpty())
    {
        handleD("Commit Message Cannot Be Empty");
        return;
    }
    QString workingDirString(m_WorkingDirectoryLineEdit->text());
    if(workingDirString.trimmed().isEmpty())
    {
        handleD("Specify a working dir");
        return;
    }
    QString dirStructureFileNameString(m_DirStructureFileNameLineEdit->text());
    if(dirStructureFileNameString.trimmed().isEmpty())
    {
        handleD("specify a dir structure file name");
        return;
    }
    QString mountedSubDirToBareRepoString(m_MountPointSubDirToBareGitRepoLineEdit->text());
    if(mountedSubDirToBareRepoString.trimmed().isEmpty())
    {
        mountedSubDirToBareRepoString = mountedSubDirToBareRepoString.trimmed();
    }

    if(commitMessage.contains(m_DateTimeSpecialString))
    {
        commitMessage.replace(m_DateTimeSpecialString, QDateTime::currentDateTime().toString(Qt::ISODate)); //TODOreq: would be better if I did this one line before calling commitProcess.start()... because I noticed (and lol'd) that there's a few seconds (and growing) difference between the special string in commit message and the git-specific one (but who gives a shit). On the other hand, it makes for a funnily interesting statistic (and when I upgrade computers, it should shrink/regrow etc :-P) (TODOreq: graph this before death)
    }

    emit commitRequested(commitMessage, m_DeviceAndPasswordPathsToBePopulatedJustBeforeMountRequestedOrPersistRequestedOrCommit, workingDirString, mountedSubDirToBareRepoString, dirStructureFileNameString, m_FilenameIgnoreList, m_FilenameEndsWithIgnoreList, m_DirnameIgnoreList, m_DirnameEndsWithIgnoreList, m_PushEvenWhenNothingToCommitCheckbox->isChecked(), m_SignCommitCheckbox->isChecked()); //an emit instead of invokeMethod because of the existene of "connectToBusiness". it's just proper'er (one less error message potentially)
}
void mainWidget::handleShutdownButtonClicked()
{
    //change text to cancel. dismount should happen instantly, backend should say "done dismounting. shutting down in 3... 2... 1.. [QProcess::start(shutdown)] ... unless we've clicked cancel by then.. in which case all we've done is dismounted (or perhaps commit'd)
    //a checkbox right next to the shutdown button for "commit if necessary", which also mounts if necessary too (implicit with the call to commit)!
    //TODOreq:^^^store in QSettings the state of that checkbox

    //disable all of the GUI except the "Cancel" button and debug output (because we haven't crashed). PITA IMO, iterate through child widgets of main widget with exception to these two widgets? hiding all it's children? idfk should re-enable all of them upon Cancel... with exception to others supposed to still be hidden (from read only checkbox, for example. another would be the tc rows depending on if mounted or not)

    //TOD ONEreq: clicking shutdown with auto-commit should bypass typical errors and still shutdown. one such error is no commit message. another could be mouting (though we should try to tell the user before the 3 seconds is up). the shutdown button should be safe to click during a power outage where UPS is draining fast and we need to go down now and as safely as possible. basically what i'm saying is the emitting of brokenStateDetected should not stop the shutdown from being emitted. since we're doing shutdown -h, i _THINK_ (but am unsure, hence the creation of this app) it gives truecrypt the opportunity to dismount it's volumes anyways. would be very shittily coded if it didn't. still, i like watching them dismount... makes me feel comfortable. 3 seconds extra time is so worth it... and i can cancel to read/deal-with any relevant error messages
    //^^done enough that the shutdown continues... but not done enough to allow cancel button to stay enabled on error :(

    if(!m_ShuttingDown)
    {
        //begin shutdown
        m_ShuttingDown = true;
        m_ShutdownAfterDismountingIfNeededButton->setText(m_CancelShutdownText);

        //emit shutdownRequested((!m_ReadOnlyCheckBox->isChecked()) && m_CommitIfNeededBeforeShuttingDownCheckbox->isChecked());
        //^^the proper way is to let the business call commit() based on the bool i used to pass into shutdownRequested... but i don't feel like passing a bunch of params around SO FUCK THAT

        //hack:
        if(!m_ReadOnlyCheckBox->isChecked() && m_CommitIfNeededBeforeShuttingDownCheckbox->isChecked())
        {
            handleCommitButtonClicked();
            //we'll just listen to commited(), then check m_ShuttingDown and then bam that's when/where i emit shutdownRequested() after the commit
        }

        //right now i'd be queueing 2 messages on teh business thread (the first one being in the handleCommitButtonClicked call), HOPING/DEPENDING on the fact that they are processed in order. can't commit after shutdown. i think qt's signals/slots impl guarantees that, does it not?
        emit shutdownRequested();

        /*
        else
        {
            //this is a hack because we're dealing with the business logic of whether or not to commit... in the gui. but it's just easier and less typing SO FUCK IT
            emit shutdownRequested();
        }*/

    }
    else
    {
        //cancel
        m_ShuttingDown = false;
        m_ShutdownAfterDismountingIfNeededButton->setText(m_ShutdownText);
        emit cancelShutdownRequested();
    }
}
void mainWidget::handleBrowseButtonClickedAndFigureOutWhichLineEditToUpdate()
{
    QPushButton *btnClicked = qobject_cast<QPushButton*>(sender());
    if(btnClicked)
    {
        QLineEdit *lineEditToAssignResults = m_BrowseButtonLineEditAssociations.value(btnClicked);

        QFileDialog aFileDialog;
        aFileDialog.setFileMode(QFileDialog::ExistingFile);
        aFileDialog.setViewMode(QFileDialog::List);

        if(aFileDialog.exec() && aFileDialog.selectedFiles().size() > 0)
        {
            lineEditToAssignResults->setText(aFileDialog.selectedFiles().at(0));
        }
    }
    else
    {
        //should never get here...
        handleD("Failed to figure out which line edit to update from browse button click");
    }
}
