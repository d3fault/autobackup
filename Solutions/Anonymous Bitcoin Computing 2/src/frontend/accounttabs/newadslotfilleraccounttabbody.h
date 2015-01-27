#ifndef NEWADSLOTFILLERACCOUNTTABBODY_H
#define NEWADSLOTFILLERACCOUNTTABBODY_H

#include "iaccounttabwidgettabbody.h"

#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WScrollArea>
#include <Wt/WVBoxLayout>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "filedeletingfileresource.h"

using namespace Wt;
using namespace boost::property_tree;

class NewAdSlotFillerAccountTabBody : public IAccountTabWidgetTabBody
{
public:
    NewAdSlotFillerAccountTabBody(AnonymousBitcoinComputingWtGUI *abcApp);
private:
    friend class AnonymousBitcoinComputingWtGUI;

    WLineEdit *m_UploadNewSlotFiller_NICKNAME;
    std::string m_Validated_NewAdSlotFiller_NICKNAME;
    std::string m_UploadNewSlotFiller_NICKNAME_B64;
    WLineEdit *m_UploadNewSlotFiller_HOVERTEXT;
    std::string m_Validated_NewAdSlotFiller_HOVERTEXT;
    WLineEdit *m_UploadNewSlotFiller_URL;
    std::string m_Validated_NewAdSlotFiller_URL;
    WLineEdit *m_UploadNewSlotFiller_WIDTH;
    std::string m_Validated_UploadNewSlotFiller_WIDTH;
    WLineEdit *m_UploadNewSlotFiller_HEIGHT;
    std::string m_Validated_UploadNewSlotFiller_HEIGHT;
    WContainerWidget *m_AdImageUploaderPlaceholder;
    WFileUpload *m_AdImageUploader;
    WPushButton *m_AdImageUploadButton;
    //WVBoxLayout *m_AdImageUploadResultsVLayout;
    bool m_MostRecentlyUploadedImageWasSetAsPreview;
    FileDeletingFileResource *m_MostRecentlyUploadedImageAsFileResource;
    WVBoxLayout *m_AdSlotFillersVLayout;
    ptree m_RunningAllSlotFillersJsonDoc;
    u_int64_t m_RunningAllSlotFillersJsonDocCAS;

    void populateAndInitialize();
    void setUpAdImageUploaderAndPutItInPlaceholder();
    bool userSuppliedAdSlotFillerFieldsAreValid();
    void handleAdImageUploadButtonClicked();
    void handleAdSlotFillerImageUploadFinished();
    void handleAdImageUploadFailedFileTooLarge();
    void resetAdSlotFillerImageUploadFieldsForAnotherUpload();
    void determineNextSlotFillerIndexAndThenAddSlotFillerToIt(const string &allAdSlotFillersJsonDoc, u_int64_t casOfAllSlotFillersDocForUpdatingSafely, bool lcbOpSuccess, bool dbError);
    void tryToAddAdSlotFillerToCouchbase(const std::string &slotFillerIndexToAttemptToAdd);
    void storeLargeAdImageInCouchbaseDbAttemptComplete(const string &keyToSlotFillerAttemptedToAddTo, bool lcbOpSuccess, bool dbError); //read a huge article yesterday by the soup and was almost convinced to start using exceptions for error handling.... but how the fuck do you throw an exception across a thread O_o? (RpcGenerator's error mechanism was (is?) teh pro-est). oh apparently boost has a way of doing this. but does it cover exceptions across networks?
    void doneAttemptingToUpdateAllAdSlotFillersDocSinceWeJustCreatedANewAdSlotFiller(bool lcbOpSuccess, bool dbError);
    void getAdSlotFillerThatIsntInAllAdSlotFillersAttemptFinished_soAddItToAllAddSlotFillersAndInitiateSlotFillerAddAtNextIndex(const string &adSlotFillerToExtractNicknameFrom, bool lcbOpSuccess, bool dbError);
};

#endif // NEWADSLOTFILLERACCOUNTTABBODY_H
