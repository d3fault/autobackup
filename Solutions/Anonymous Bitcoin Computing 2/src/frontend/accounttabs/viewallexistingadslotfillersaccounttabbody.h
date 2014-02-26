#ifndef VIEWALLEXISTINGADSLOTFILLERSACCOUNTTABBODY_H
#define VIEWALLEXISTINGADSLOTFILLERSACCOUNTTABBODY_H

#include "iaccounttabwidgettabbody.h"

#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WStackedWidget>
#include <Wt/WPushButton>
#include <Wt/WSpinBox>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/unordered_map.hpp>

#include "singleuseselfdeletingmemoryresource.h"

using namespace Wt;
using namespace std;
using namespace boost::property_tree;

typedef boost::unordered_map<int /* 1-index'd page */, WContainerWidget* /* pointer to page already in stacked widget */> AdSlotFillersPagesHashType;
typedef boost::unordered_map<std::string /* key to ad slot filler */, WContainerWidget* /* pointer to wcontainer (constructed and placed in current page, but empty) */> AdSlotFillersOnePageOrderingHashType; //since we use async hacky multi-get, the ad slot fillers can come in any order and this keeps track of the ordering while still allowing us to be async

class ViewAllExistingAdSlotFillersAccountTabBody : public IAccountTabWidgetTabBody
{
public:
    ViewAllExistingAdSlotFillersAccountTabBody(AnonymousBitcoinComputingWtGUI *abcApp);
private:
    friend class AnonymousBitcoinComputingWtGUI;

    WStackedWidget *m_AllAdSlotFillersPagesStack;
    int m_AdsCount;
    int m_CurrentPageOneIndexBased;
    int m_NumberOfAdsHackilyMultiGetting;
    WContainerWidget *m_PointerToCurrentPageInConstruction;
    WPushButton *m_FirstPageButton;
    WPushButton *m_PreviousPageButton;
    WSpinBox *m_PageSpinbox;
    WPushButton *m_NextPageButton;
    WPushButton *m_LastPageButton;
    AdSlotFillersPagesHashType m_AdSlotFillersPagesOneBasedIndexHash;
    AdSlotFillersOnePageOrderingHashType m_AdSlotFillersOnePageOrderingHash;

    void populateAndInitialize();
    void attemptToGetAllAdSlotFillersFinished(const std::string &allAdSlotFillersJsonDocMaybe, bool lcbOpSuccess, bool dbError);
    void buildCurrentPageAndAddToStackAndPageHash();
    void oneAdSlotFillerFromHackyMultiGetAttemptFinished(const string &keyToAdSlotFillerArriving, const std::string &oneAdSlotFillerJsonDoc, bool lcbOpSuccess, bool dbError);
    void firstPageButtonClicked();
    void previousPageButtonClicked();
    void goToPageButtonClicked();
    void nextPageButtonClicked();
    void lastPageButtonClicked();
    void doAllAdSlotFillersViewPageChange();
    int m_TotalPagesCount;
};

#endif // VIEWALLEXISTINGADSLOTFILLERSACCOUNTTABBODY_H
