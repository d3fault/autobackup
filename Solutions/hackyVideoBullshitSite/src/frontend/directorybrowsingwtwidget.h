#ifndef DIRECTORYBROWSINGWTWIDGET_H
#define DIRECTORYBROWSINGWTWIDGET_H

#include <boost/unordered_map.hpp>

#include <Wt/WContainerWidget>

#include <QString>

using namespace std;
using namespace Wt;

class QDirIterator;

namespace Wt {
class WStackedWidget;
class WSpinBox;
}

typedef boost::unordered_map<int /* 1-index'd page */, WContainerWidget* /* pointer to page already in stacked widget */> DirectoryPagesHashType;

//for Wt, but depends on Qt :)
class DirectoryBrowsingWtWidget : public WContainerWidget
{
public:
    DirectoryBrowsingWtWidget(const QString &absolutePathToIterate, const std::string &theInternalPathCleanedStdString, int pageOfDirectoryToShow, WContainerWidget *parent = 0);
    ~DirectoryBrowsingWtWidget();
private:
    WStackedWidget *m_DirectoryPagesStack;
    WText *m_SorryNoPage;

    QString m_AbsolutePathToIterate;
    std::string m_TheInternalPathCleanedStdString;
    std::string m_DirInternalPath;
    QDirIterator *m_DirIterator;
    int m_DirIteratorPosition_ZeroIndexBased;
    int m_DirCurrentPage_OneIndexBased;

    WPushButton *m_PreviousPageButton;
    WSpinBox *m_PageSpinbox;
    WPushButton *m_NextPageButton;

    DirectoryPagesHashType m_DirectoryPagesOneBasedIndexHash;

    void createDirIterator();
    void resetDirIterator();

    void displayCurrentPage();
    void buildOnePageOf100dirEntriesAndPresentAndCacheIt_BasedOnCurrentPage();

    void previousPageButtonClicked();
    void goToPageButtonClicked();
    void nextPageButtonClicked();
};

#endif // DIRECTORYBROWSINGWTWIDGET_H
