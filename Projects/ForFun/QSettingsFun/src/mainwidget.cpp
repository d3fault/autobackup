#include "mainwidget.h"

mainWidget::mainWidget(QWidget *parent)
    : QWidget(parent)//, m_TcRowsCount(0)
{
    m_Layout = new QVBoxLayout();
    m_AddNewRowBtn = new QPushButton("Add New Tc Location/Password");
    m_PersistTcRows = new QPushButton("Persist Truecrypt Device/Container Locations and Passwords");

    QSettings settings;
    settings.beginGroup("tcLocations");
    //int tcLocationsCount = settings.value("tcLocationsCount", 0).toInt();

    QHBoxLayout *userControlsHLayout = new QHBoxLayout();
    userControlsHLayout->addWidget(m_AddNewRowBtn);
    userControlsHLayout->addWidget(m_PersistTcRows);

    m_Layout->addLayout(userControlsHLayout);
    m_Layout->setAlignment(Qt::AlignTop);

    //bool gotEmpty = false;
    bool gotOne = false;

    //for(int i = 0; i < tcLocationsCount; ++i)
    //while(!gotEmpty)
    int i = 0;
    while(true)
    {
        QString tcLocationAtStr(settings.value((QString("tcLocationAt") + QString::number(i))).toString());
        QString tcLocationPasswordFileAtStr(settings.value(QString("tcLocationPasswordFileAt") + QString::number(i)).toString());
        if((!tcLocationAtStr.isEmpty()) && (!tcLocationPasswordFileAtStr.isEmpty()))
        {
            QHBoxLayout *newRow = createNewTcFileAndPwFileRow(tcLocationAtStr,tcLocationPasswordFileAtStr);
            m_Layout->addLayout(newRow);
            gotOne = true;
        }
        else
        {
            //gotEmpty = true;
            break;
        }
        ++i;
    }
    if(!gotOne)
    {
        //add our first one because this is first launch (or they deleted the config file or whatever)
        m_Layout->addLayout(createNewTcFileAndPwFileRow());
    }
    settings.endGroup();

    connect(m_AddNewRowBtn, SIGNAL(clicked()), this, SLOT(handleAddNewRowBtnClicked()));
    connect(m_PersistTcRows, SIGNAL(clicked()), this, SLOT(handlePersistTcRowsBtnClicked()));

    this->setLayout(m_Layout);
}
mainWidget::~mainWidget()
{
    
}
QHBoxLayout *mainWidget::createNewTcFileAndPwFileRow(QString containerLocation, QString containerPasswordLocation)
{
    QHBoxLayout *newHLayout = new QHBoxLayout();

    QLabel *tcLocationLabel = new QLabel("Truecrypt Device/Container:");
    QLineEdit *tcLocationLe = new QLineEdit(containerLocation);
    QPushButton *browseForTcLocationBtn = new QPushButton("Browse");
    m_BrowseButtonLineEditAssociations.insert(browseForTcLocationBtn, tcLocationLe);
    connect(browseForTcLocationBtn, SIGNAL(clicked()), this, SLOT(handleBrowseButtonClickedAndFigureOutWhich()));

    QLabel *tcPasswordLocationLabel = new QLabel("Password File:");
    QLineEdit *tcPasswordLocationLe = new QLineEdit(containerPasswordLocation);
    QPushButton *browseForTcPasswordLocationBtn = new QPushButton("Browse");
    m_BrowseButtonLineEditAssociations.insert(browseForTcPasswordLocationBtn, tcPasswordLocationLe);
    connect(browseForTcPasswordLocationBtn, SIGNAL(clicked()), this, SLOT(handleBrowseButtonClickedAndFigureOutWhich()));

    newHLayout->addWidget(tcLocationLabel);
    newHLayout->addWidget(tcLocationLe);
    newHLayout->addWidget(browseForTcLocationBtn);
    newHLayout->addWidget(tcPasswordLocationLabel);
    newHLayout->addWidget(tcPasswordLocationLe);
    newHLayout->addWidget(browseForTcPasswordLocationBtn);

    //m_TcRowsMap.insert(m_TcRowsCount,qMakePair(tcLocationLe,tcPasswordLocationLe));
    m_TcRowsList.append(qMakePair(tcLocationLe,tcPasswordLocationLe));
    //++m_TcRowsCount;

    return newHLayout;
}
void mainWidget::handleAddNewRowBtnClicked()
{
    QHBoxLayout *newRow = createNewTcFileAndPwFileRow();
    m_Layout->addLayout(newRow);
    this->setLayout(m_Layout);
}
void mainWidget::handlePersistTcRowsBtnClicked()
{
    QSettings settings;
    settings.beginGroup("tcLocations");
    settings.remove(""); //clear old ones. this is to get rid of paths that were deleted by the user by highlighting the text (either field) and simply deleting it
    int saveIndex = 0; //save index = because if there's a gap in the rows, the ones further down need to be saved at the next index, else we won't load them properly
    int tcRowsCount = m_TcRowsList.size();
    //for(int i = 0; i < m_TcRowsCount; ++i)
    for(int i = 0; i < tcRowsCount; ++i) //iterate over all rows, whether empty or not. we check them.
    {
        QPair<QLineEdit*,QLineEdit*> apair = m_TcRowsList.value(i);
        QString firstLoL = apair.first->text();
        QString secondLoL = apair.second->text();
        if((!firstLoL.isEmpty()) && (!secondLoL.isEmpty()))
        {
            settings.setValue((QString("tcLocationAt") + QString::number(saveIndex)), firstLoL);
            settings.setValue((QString("tcLocationPasswordFileAt") + QString::number(saveIndex)), secondLoL);
            ++ saveIndex;
        }
    }
    //settings.setValue("tcLocationsCount", neitherEmptyCount);
    settings.endGroup();
}
void mainWidget::handleBrowseButtonClickedAndFigureOutWhich()
{
    QPushButton *btnClicked = qobject_cast<QPushButton*>(sender()); //hack, breaks OO principles. fuck it >_>
    QLineEdit *lineEditToAssignResults = m_BrowseButtonLineEditAssociations.value(btnClicked);

    QFileDialog aFileDialog;
    aFileDialog.setFileMode(QFileDialog::ExistingFile);
    aFileDialog.setViewMode(QFileDialog::List);


    //aFileDialog.setDirectory();


    if(aFileDialog.exec())
    {
        lineEditToAssignResults->setText(aFileDialog.selectedFiles().at(0));
    }
}
