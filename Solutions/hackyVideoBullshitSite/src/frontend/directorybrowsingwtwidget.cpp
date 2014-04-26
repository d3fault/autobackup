#include "directorybrowsingwtwidget.h"

#include <Wt/WApplication>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WStackedWidget> //should I build previous pages when jumping directly to a page, or should i allow the unoptimal delete/new'ing of the dir iterator? both options suck, which sucks less? i'm guessing that deleting/new'ing is more optimal memory-wise, but building pages might be more optimal spe- (yea fucking right, delete/new strategy it is! it's uncommon anyways)
#include <Wt/WSpinBox>

#include <QDirIterator>

#define HVBS_BROWSE_MY_BRAIN_THOUGHTS_PER_PAGE 100

//TODOoptional: user selectable filters, dirs, files, hidden, etc :). changing the filters goes back to page 1 i'd imagine, basically resetting everything including the stack widget...

#define DBWTW_MAKE_PREVIOUS_PAGE_BUTTAN(variableName) \
variableName = new WPushButton("Previous Page", this); \
variableName->clicked().connect(this, &DirectoryBrowsingWtWidget::previousPageButtonClicked); \
variableName->disable();

#define DBWTW_MAKE_SPINBOX(variableName) \
m_TopPageSpinbox = new WSpinBox(this); \
m_TopPageSpinbox->setRange(1, 99999999); /* TODOoptional: make range 1 if no other pages... or even disable it altogether...*/ \
m_TopPageSpinbox->enterPressed().connect(this, &DirectoryBrowsingWtWidget::goToPageButtonClicked);

#define DBWTW_MAKE_PAGE_SELECTION_ROW(variablePart) \
DBWTW_MAKE_PREVIOUS_PAGE_BUTTAN(m_##variablePart##PreviousPageButton) \
new WText(" Page: ", Wt::XHTMLUnsafeText, this); \
DBWTW_MAKE_SPINBOX(m_##variablePart##PageSpinbox) \
new WText(" ", Wt::XHTMLUnsafeText, this); \
WPushButton *variablePart##goToPageButton = new WPushButton("Go", this); \
variablePart##goToPageButton->clicked().connect(this, &DirectoryBrowsingWtWidget::goToPageButtonClicked); \
WText *variablePart##splaination = new WText(" of ??? pages. ", this); \
variablePart##splaination->setToolTip("It's an optimization for the software to not know how many pages there are in total. This may be fixed in the future, but is low as fuck priority", Wt::XHTMLUnsafeText); \
m_##variablePart##NextPageButton = new WPushButton("Next Page", this); \
m_##variablePart##NextPageButton->clicked().connect(this, &DirectoryBrowsingWtWidget::nextPageButtonClicked); \
m_##variablePart##NextPageButton->disable();
//new WText(" ", this);

DirectoryBrowsingWtWidget::DirectoryBrowsingWtWidget(const QString &absolutePathToIterate, /*int pageOfDirectoryToShow,*/ const string &theInternalPathCleanedStdString, WContainerWidget *parent)
    : WContainerWidget(parent)
    , m_SorryNoPage(0)
    , m_AbsolutePathToIterate(absolutePathToIterate)
    , m_TheInternalPathCleanedStdString(theInternalPathCleanedStdString)
    , m_DirInternalPath(WApplication::instance()->internalPath())
    , m_DirIteratorPosition_ZeroIndexBased(0)
    , m_DirCurrentPage_OneIndexBased(/*(pageOfDirectoryToShow > 0) ? pageOfDirectoryToShow : */ 1)
{
    DBWTW_MAKE_PAGE_SELECTION_ROW(Top)

    new WBreak(this);
    m_DirectoryPagesStack = new WStackedWidget(this);

    new WBreak(this);
    DBWTW_MAKE_PAGE_SELECTION_ROW(Bottom)

    createDirIterator();

    displayCurrentPage();
}
DirectoryBrowsingWtWidget::~DirectoryBrowsingWtWidget()
{
    delete m_DirIterator;
}
void DirectoryBrowsingWtWidget::createDirIterator()
{
    //TODOreq: when ?page=>1, call a bunch of .nexts to get it in the right place -- TODOreq: if next returns false before then, 404
    m_DirIterator = new QDirIterator(m_AbsolutePathToIterate, (QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs | QDir::Hidden));
}
void DirectoryBrowsingWtWidget::resetDirIterator()
{
    delete m_DirIterator;
    createDirIterator();
    m_DirIteratorPosition_ZeroIndexBased = 0;
}
void DirectoryBrowsingWtWidget::displayCurrentPage()
{
    if(m_SorryNoPage)
    {
        delete m_SorryNoPage;
        m_SorryNoPage = 0;
    }

    //Enable/Disable Previous/Next buttons depending...
    setPreviousPageButtonsDisabled(m_DirCurrentPage_OneIndexBased == 1);
    setPageSpinBoxValues(m_DirCurrentPage_OneIndexBased); //TODOreq: set to ?page=X, and maybe only if it exists...

    setNextPageButtonsDisabled(false);

    //first see if the page is already made. if it is, we just bring it to the top of the stacked widget
    try
    {
        WContainerWidget *pageAlreadyInStackedWidgetMaybe = m_DirectoryPagesOneBasedIndexHash.at(m_DirCurrentPage_OneIndexBased);

        //no exception = page already created
        m_DirectoryPagesStack->setCurrentWidget(pageAlreadyInStackedWidgetMaybe);
    }
    catch(std::out_of_range &pageNotVisitedYetException)
    {
        //page not in hash/stack yet, so make it and add it
        buildOnePageOf100dirEntriesAndPresentAndCacheIt_BasedOnCurrentPage();
    }

}
void DirectoryBrowsingWtWidget::buildOnePageOf100dirEntriesAndPresentAndCacheIt_BasedOnCurrentPage()
{
    int startOffset_ZeroBased = (m_DirCurrentPage_OneIndexBased-1)*HVBS_BROWSE_MY_BRAIN_THOUGHTS_PER_PAGE;
    //int endOffset_ZeroBased = (startOffset_ZeroBased + HVBS_BROWSE_MY_BRAIN_THOUGHTS_PER_PAGE);

    if(m_DirIteratorPosition_ZeroIndexBased > startOffset_ZeroBased)
    {
        //can't go backwards, have to start all over :(
        resetDirIterator();
    }

    //catch up
    while(m_DirIteratorPosition_ZeroIndexBased < startOffset_ZeroBased)
    {
        ++m_DirIteratorPosition_ZeroIndexBased;
        if(!m_DirIterator->hasNext())
        {
            resetDirIterator(); //because what else should be done with it? needs to be in valid state and obviously can't be used anymore...
            m_SorryNoPage = new WText("Sorry, that page in this directory does not exist. This may seem strange, but we had no way of knowing this page wasn't there until just now.", m_DirectoryPagesStack);
            m_DirectoryPagesStack->setCurrentWidget(m_SorryNoPage);
            setNextPageButtonsDisabled(true);
            //TODOreq: we don't necessarily want to enable the previous page button... because they might have done it via spinbox and been way off. HOWEVER we do know that the previous page before this one was there... so we can err calculate it and yea enable previous page button. for now and to KISS, i'm just going to disable it:
            setPreviousPageButtonsDisabled(true);
            return;
        }
        m_DirIterator->next(); //dgaf about what it returns
    }

    WContainerWidget *currentPage = new WContainerWidget(m_DirectoryPagesStack);
    int shownSoFar = 0;
    while(m_DirIterator->hasNext() && (shownSoFar < HVBS_BROWSE_MY_BRAIN_THOUGHTS_PER_PAGE))
    {
        //TODOoptional: make dirs visually different. again optionally, could have a "dir only" mode (would still probably warrant pagination theoretically)

        ++m_DirIteratorPosition_ZeroIndexBased;
        m_DirIterator->next(); //next() returns absolute, but we don't want it
        const std::string &currentEntryString = m_DirIterator->fileName().toStdString();

        new WAnchor(WLink(WLink::InternalPath, m_TheInternalPathCleanedStdString + "/" + currentEntryString), currentEntryString, currentPage);
        new WBreak(currentPage);
        ++shownSoFar;
    }
#if 0 //nope, because if sitting at last page and i add some shit, err blah. i think the checks at the beginning of this method should handle that case anyways...
    if(!m_DirIterator->hasNext())
    {
        resetDirIterator();
    }
#endif

    //rare-ish case wehre we had an exactly divisible by 100 amount of entries and they tried to view 'next page' and it's empty..
    if(shownSoFar == 0 && m_DirCurrentPage_OneIndexBased > 1) //if current page == 1, then the dir was just empty (but my git shit wouldn't have gotten it then, so that's extremely unlikely to ever happen!)...
    {
        //not an entry on new page, set it to page=nope
        //WApplication::instance()->setInternalPath(m_DirInternalPath + "?page=nope", false); //TODOreq: detect page=nope pasted into url bar as soon as possible and yea don't show or try to show anything
        //TODOreq: oops sorry no page
        //TODOreq: enable previous page button (back button will also work pretty sure)
        m_SorryNoPage = new WText("You've reached the end of this directory. This may seem strange, but we had no way of knowing there wasn't another page until just now. Go ahead and hit that previous button.", m_DirectoryPagesStack);
        m_DirectoryPagesStack->setCurrentWidget(m_SorryNoPage);
        setNextPageButtonsDisabled(true);
        delete currentPage;
        return;
    }

    //view a page -> previous page -> next page (pulled from cache) = don't know how many entries in current page (so no idea if maybe a next page). BUT the first time it's created we know so can at least disable it the first time. known semi-buggy fuck it
    if(shownSoFar < HVBS_BROWSE_MY_BRAIN_THOUGHTS_PER_PAGE)
    {
        //OLD: we don't know for sure, but there's very likely another page...

        //since we didn't see how many we present per page, we know that there isn't another page (TODOoptional: maybe just leave it enabled because if they're watching my vidya shit and i add a file or a group of files, they could then press next and shit would just show up (i think, depends on qdiriterator but yea it makes sense since qdiriterator just keeps going until no more)... whereas now they'd need to hard refresh and yea~)
        setNextPageButtonsDisabled(true);
    }

    //at least one item on new page

    //present
    m_DirectoryPagesStack->setCurrentWidget(currentPage);
    //cache
    m_DirectoryPagesOneBasedIndexHash[m_DirCurrentPage_OneIndexBased] = currentPage;

#if 0
    if(m_DirCurrentPage_OneIndexBased == 1)
    {
       WApplication::instance()->setInternalPath(m_DirInternalPath, false);
    }
    else
    {
        WApplication::instance()->setInternalPath(m_DirInternalPath + "?page=" + QString::number(m_DirCurrentPage_OneIndexBased).toStdString(), false);
    }
#endif
}
void DirectoryBrowsingWtWidget::previousPageButtonClicked()
{
    if(m_DirCurrentPage_OneIndexBased == 1)
    {
        setPreviousPageButtonsDisabled(true);
        return; //wtf
    }
    --m_DirCurrentPage_OneIndexBased;
    displayCurrentPage();
}
void DirectoryBrowsingWtWidget::goToPageButtonClicked()
{
    //want to ddos me? go to the last page and keep clicking previous page :-P
    if(m_TopPageSpinbox->validate() != WValidator::Valid)
    {
        //uhh.....
        return;
    }
    const int newPage = m_TopPageSpinbox->value();
    if(newPage == m_DirCurrentPage_OneIndexBased)
    {
        //change to same page? bah. TODOoptional: if js is enabled, listen to 'changed' and then enable/disable it if not current page (still do this sanity check though of course). would start off disabled in that case...
        return;
    }
    m_DirCurrentPage_OneIndexBased = newPage;
    displayCurrentPage();
}
void DirectoryBrowsingWtWidget::nextPageButtonClicked()
{
    ++m_DirCurrentPage_OneIndexBased;
    displayCurrentPage();
}
void DirectoryBrowsingWtWidget::setPreviousPageButtonsDisabled(bool disabled)
{
    m_TopPreviousPageButton->setDisabled(disabled);
    m_BottomPreviousPageButton->setDisabled(disabled);
}
void DirectoryBrowsingWtWidget::setPageSpinBoxValues(int newValue)
{
    m_TopPageSpinbox->setValue(newValue);
    m_BottomPageSpinbox->setValue(newValue);
}
void DirectoryBrowsingWtWidget::setNextPageButtonsDisabled(bool disabled)
{
    m_TopNextPageButton->setDisabled(disabled);
    m_BottomNextPageButton->setDisabled(disabled);
}
