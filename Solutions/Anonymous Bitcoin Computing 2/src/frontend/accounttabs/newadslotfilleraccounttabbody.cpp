#include "newadslotfilleraccounttabbody.h"

#include "../anonymousbitcoincomputingwtgui.h"
#include "../validatorsandinputfilters/safetextvalidatorandinputfilter.h"
#include "stupidmimefromextensionutil.h"
#include "nonanimatedimagheaderchecker.h"
#include "abc2couchbaseandjsonkeydefines.h"

//accepts jpg/bmp only via magic header whitelist. OLD (fucking png/webp suck too): primitive gif detection/removal (safely reading first few bytes/header)? maybe i should change to jpg/png/webp only, but don't png/webp support animations anyways? my fellow engineers are fucking noobs. there's a word for animated image: it's called video

#define ABC_AD_SLOT_FILLERS_ACCEPT_IMAGE_FORMATS_MESSAGE "Accepted image formats: jpeg/bmp"
#define NewAdSlotFillerAccountTabBody_INPUT_FORMS_VISUAL_WIDTH_IN_CHARS 32

NewAdSlotFillerAccountTabBody::NewAdSlotFillerAccountTabBody(AnonymousBitcoinComputingWtGUI *abcApp)
    : IAccountTabWidgetTabBody(abcApp),
      m_MostRecentlyUploadedImageWasSetAsPreview(false),
      m_MostRecentlyUploadedImageAsFileResource(0)
{ }
void NewAdSlotFillerAccountTabBody::populateAndInitialize()
{
    WScrollArea *newAdSlotFillerScrollArea = new WScrollArea(this);
    WContainerWidget *newAdSlotFillerContainerWidget = new WContainerWidget();
    newAdSlotFillerScrollArea->setWidget(newAdSlotFillerContainerWidget);
    m_AdSlotFillersVLayout = new WVBoxLayout(newAdSlotFillerContainerWidget);

    m_AdSlotFillersVLayout->addWidget(new WText("<b>Upload New Advertisement (For use in buying ad space)</b>"), 0, Wt::AlignTop | Wt::AlignLeft); //TODOoptimization: perhaps a captcha must be solved before every upload, thwarting it's potential as a DDOS point (but it would take a specific app to take advantage of it (but Wt's element id randomization protects me zilch against screen based automation clickers (i could hack me)))
    m_AdSlotFillersVLayout->addWidget(new WBreak(), 0, Wt::AlignTop | Wt::AlignLeft);
    m_AdSlotFillersVLayout->addWidget(new WBreak(), 0, Wt::AlignTop | Wt::AlignLeft);
    m_AdSlotFillersVLayout->addWidget(new WText("Note: For the time being, the maximum number of advertisements you can upload is ten thousand. If you need more (wtf?), create a new account (or bitch at me to fix/raise this limit)"), 0, Wt::AlignTop | Wt::AlignLeft);
    m_AdSlotFillersVLayout->addWidget(new WBreak(), 0, Wt::AlignTop | Wt::AlignLeft);
    m_AdSlotFillersVLayout->addWidget(new WBreak(), 0, Wt::AlignTop | Wt::AlignLeft);
    WGridLayout *uploadNewSlotFillerGridLayout = new WGridLayout();

    int rowIndex = -1;

    uploadNewSlotFillerGridLayout->addWidget(new WText("--- 1) Nickname (unseen by others):"), ++rowIndex, 0, Wt::AlignTop | Wt::AlignLeft);
    m_UploadNewSlotFiller_NICKNAME = new WLineEdit();
    uploadNewSlotFillerGridLayout->addWidget(m_UploadNewSlotFiller_NICKNAME, rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);
    SafeTextValidatorAndInputFilter *nicknameSafeTextValidator = new SafeTextValidatorAndInputFilter("64", m_UploadNewSlotFiller_NICKNAME);
    m_UploadNewSlotFiller_NICKNAME->setMaxLength(64);
    m_UploadNewSlotFiller_NICKNAME->setValidator(nicknameSafeTextValidator);
    m_UploadNewSlotFiller_NICKNAME->setTextSize(NewAdSlotFillerAccountTabBody_INPUT_FORMS_VISUAL_WIDTH_IN_CHARS); //visual only

    //these more lenient validators (allowing characters and spaces) go through b64encode before being stored in db, otherwise they're vulnerable like an sql injection (except json injection)

    uploadNewSlotFillerGridLayout->addWidget(new WText("--- 2) Mouse Hover Text:"), ++rowIndex, 0, Wt::AlignTop | Wt::AlignLeft);
    m_UploadNewSlotFiller_HOVERTEXT = new WLineEdit();
    uploadNewSlotFillerGridLayout->addWidget(m_UploadNewSlotFiller_HOVERTEXT, rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);
    SafeTextValidatorAndInputFilter *hovertextSafeTextValidator = new SafeTextValidatorAndInputFilter("512", m_UploadNewSlotFiller_HOVERTEXT);
    m_UploadNewSlotFiller_HOVERTEXT->setValidator(hovertextSafeTextValidator);
    m_UploadNewSlotFiller_HOVERTEXT->setMaxLength(512); //for randall
    m_UploadNewSlotFiller_HOVERTEXT->setTextSize(NewAdSlotFillerAccountTabBody_INPUT_FORMS_VISUAL_WIDTH_IN_CHARS); //visual only

    uploadNewSlotFillerGridLayout->addWidget(new WText("--- 3) URL:"), ++rowIndex, 0, Wt::AlignTop | Wt::AlignLeft);
    m_UploadNewSlotFiller_URL = new WLineEdit("http://");
    uploadNewSlotFillerGridLayout->addWidget(m_UploadNewSlotFiller_URL, rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);
    WRegExpValidator *urlValidator = new WRegExpValidator("(((https?|ftps?|gnunet)://)(%[0-9A-Fa-f]{2}|[-()_.!~*';/?:@&=+$,A-Za-z0-9])+)([).!';/?:,][[:blank:]])?", m_UploadNewSlotFiller_URL); //TODOoptional: i was thinking about making this an input filter just like the other ones, but i'm not sure that url regex will work as one (not sure that it won't, but quite franky i have no idea wtf it's even doing)
    urlValidator->setMandatory(true);
    urlValidator->setInvalidNoMatchText("Invalid URL");
    urlValidator->setNoMatchText("Invalid URL");
    m_UploadNewSlotFiller_URL->setValidator(urlValidator);
    m_UploadNewSlotFiller_URL->setMaxLength(2048);
    m_UploadNewSlotFiller_URL->setTextSize(NewAdSlotFillerAccountTabBody_INPUT_FORMS_VISUAL_WIDTH_IN_CHARS); //visual only

    uploadNewSlotFillerGridLayout->addWidget(new WText("--- 4) Ad Image:"), ++rowIndex, 0, Wt::AlignTop | Wt::AlignLeft);

    m_AdImageUploaderPlaceholder = new WContainerWidget();
    uploadNewSlotFillerGridLayout->addWidget(m_AdImageUploaderPlaceholder, rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);

    uploadNewSlotFillerGridLayout->addWidget(new WText("==Image Maximums==  FileSize: 164 kb --- Width: " + boost::lexical_cast<std::string>(ABC_MAX_AD_SLOT_FILLER_IMAGE_WIDTH_PIXELS) + " px --- Height: " + boost::lexical_cast<std::string>(ABC_MAX_AD_SLOT_FILLER_IMAGE_HEIGHT_PIXELS) + " px"), ++rowIndex, 0, 1, 2, Wt::AlignTop | Wt::AlignLeft);
    uploadNewSlotFillerGridLayout->addWidget(new WText(ABC_AD_SLOT_FILLERS_ACCEPT_IMAGE_FORMATS_MESSAGE), ++rowIndex, 0, 1, 2, Wt::AlignTop | Wt::AlignLeft);

    m_AdImageUploadButton = new WPushButton("Submit");
    setUpAdImageUploaderAndPutItInPlaceholder();
    m_AdImageUploadButton->clicked().connect(this, &NewAdSlotFillerAccountTabBody::handleAdImageUploadButtonClicked);
    uploadNewSlotFillerGridLayout->addWidget(m_AdImageUploadButton, ++rowIndex, 1, Wt::AlignTop | Wt::AlignLeft);
    //TODOoptional: progress bar would be nice (and not too hard), but eh these images aren't going to take long to upload so fuck it

    uploadNewSlotFillerGridLayout->addWidget(new WText("WARNING: Ads can't be deleted or modified after uploading"), ++rowIndex, 0, 1, 2, Wt::AlignTop | Wt::AlignLeft);

    m_AdSlotFillersVLayout->addLayout(uploadNewSlotFillerGridLayout, 0, Wt::AlignTop | Wt::AlignLeft);
}
void NewAdSlotFillerAccountTabBody::handleAdImageUploadButtonClicked()
{
    //OLD (post upload validation was intermittently failing. so now we capture them before the upload):would be dumb to upload the image THEN sanitize, but we sanitize twice because they can change the fields while it's uploading
    if(!userSuppliedAdSlotFillerFieldsAreValid())
    {
        return;
    }
    m_Validated_NewAdSlotFiller_HOVERTEXT = m_UploadNewSlotFiller_HOVERTEXT->text().toUTF8();
    m_Validated_NewAdSlotFiller_NICKNAME = m_UploadNewSlotFiller_NICKNAME->text().toUTF8();
    m_Validated_NewAdSlotFiller_URL = m_UploadNewSlotFiller_URL->text().toUTF8();

    m_AdImageUploadButton->disable();
    m_AdImageUploader->upload();
}
void NewAdSlotFillerAccountTabBody::setUpAdImageUploaderAndPutItInPlaceholder()
{
    m_AdImageUploader = new WFileUpload(m_AdImageUploaderPlaceholder);
    //TODOreq: i can probably specify the temporary location of the uploaded file, and obviously i would want to use a tmpfs. would be even better if i could just upload into memory...
    m_AdImageUploader->setFileTextSize(64); //TODOreq: wtf is this, filename size or file size? more importantly, what happens when the user exceeds it?
    m_AdImageUploader->uploaded().connect(this, &NewAdSlotFillerAccountTabBody::handleAdSlotFillerImageUploadFinished);
    m_AdImageUploader->fileTooLarge().connect(this, &NewAdSlotFillerAccountTabBody::handleAdImageUploadFailedFileTooLarge);
}
void NewAdSlotFillerAccountTabBody::handleAdSlotFillerImageUploadFinished()
{
    //TO DOnereq(relying on the fact that wfileupload gets deleted on logout, but using a simple m_LoggedIn check below to protect against said hypothetical race condition describe herein (any username is better than no username ;-P)): since we can't deferRendering() before/during the upload (WHY!??!?), the user may have done something (namely logged out, but the possibilities extend to every possible op theoretically). If the user for example logged out while the upload was in progress, we obviously don't want to continue forward with adding the ad slot filler, especially if we were to rely on a now blanked username (or worse, a logout/login NEW username WTF TODOreq). However it's also worth noting that logout deletes the account widget, which deletes the file upload object itself. Wtf happens when you delete a file upload object when an upload is in progress? Even still, there's probably a race condition where the upload object sends the upload finished signal, gets deleted just after that, and we still process the upload object's finished signal here. Maybe I need to protect the log out functionality with a bool m_AnUploadIsInProgress and just refuse until it's false again (note do it for tooBigOfFile as well ofc).... I think checking m_LoggedIn here/now won't cut it because they could have logged out and back in again as a different user, so that bool would still be true (that's a ridiculously unlikely race condition and would probably require automation and luck to accomplish.... and ultimately there would be very little gain because it's not funds or anything, just an advertisement image/etc). Protecting the log out with 'uploadIsInProgress' plugs that race condition, BUT now am I supposed to check that uploadIsInProgress before doing _ANY_ app op? Blah I wish I could just deferRendering and this wouldn't even be a problem.
    //^^pretty sure the slot connected to upload finished (this one) WON'T get invoked if log out is pressed during upload (since log out deletes it). the signal emitter will not be exposed because it will be deleted, so the slot invocation will be discarded (i think/hope ;-P)
#if 0 //this was intermittently failing, no idea why
    if(!userSuppliedAdSlotFillerFieldsAreValid())
    {
        resetAdSlotFillerImageUploadFieldsForAnotherUpload(); //TODOoptional: don't clear nickname/hover/url on this specific call (other calls to the method do want to)
        return;
    }
#endif
    m_AbcApp->deferRendering();

    //TO DOnereqNOPE-ish (outdated, see below): maybe optional, idfk, but basically i want this store to just eh 'keep incrementing slot filler index until the LCB_ADD succeeds'. I'm just unsure whether or not the front-end or backend should be driving that. I think I have a choice here... and idk which is betterererer. Having the backend do it would probably be more efficient, but eh I think the front-end has to here/now/before-the-STORE-one-line-below determine the slot filler index to start trying to LCB_ADD at (else we'd be incredibly inefficient if we started at 0 and crawled up every damn time)... SO SINCE the front-end is already getting involved with it, maybe he should be the one directing the incrementing? It does seem "user specific" (user being frontend -> using backend), _BUT_ it also could be very easily genericized and used in tons of other apps... so long as we either use some "%N%" (no) in the key or make the number the very last part of the key (yes). I might even use this same thing with the bitcoin keys, but eh I haven't figured those out yet and don't want to yet (one problem at a time).

    //^^^^decided to just KISS and do it here on the front-end, knowing full well it'd be stupidly re-sending the image over and over on retries, FUCKIT. I'm going to be doing 'inline recovery' and the design for that is able to get complex, but by doing it here instead of on backend I am able to not eat my shotgun.
    //^^^^^tl;dr: get allAdSlotFillers doc, choose the n+1 index, LCB_ADD to it, if lcbOpFail: cas-swap-accepting-fail updating allAdSlotFillers (inline recovery), do n+2 (really n+1 relative to the newest update) LCB_ADD, if fail: <repeat-this-segment-X-times> -> when the LCB_ADD doesn't fail, cas-swap-accepting-fail updating allAdSlotFillers, DONE. the very last cas-swap-accepting-fail can fail (as in, not happen!) and on the NEXT ad upload the state will be recovered (might be strange from end user's perspective, but fuck it)

    //get adSpaceAllSlotFillers<username>
    m_AbcApp->getCouchbaseDocumentByKeySavingCasBegin(adSpaceAllSlotFillersKey(m_AbcApp->m_CurrentlyLoggedInUsername));
    m_AbcApp->m_WhatTheGetSavingCasWasFor = AnonymousBitcoinComputingWtGUI::GETALLADSLOTFILLERSTODETERMINENEXTINDEXANDTOUPDATEITAFTERADDINGAFILLERGETSAVINGCAS;

    //fml: memory(httpd-wt) -> file(chillen) -> memory(my-wt) ->b64encode-> memory(my-wt) ->messageQueue-> memory(sendQueue) -> memory(receiveQueue) ->->messageQueue-> memory(my-couchbase) -> memory(couchbase-send-buffer)
    //8 copies woo how efficient. maybe i should have just passed the backend the filename :-/...
    //comparison(hypothetical): memory(httpd-wt) -> file(chillen), <send-filename-to-backend>, -> memory(my-couchbase) ->b64encode-> memory(my-couchbase) -> memory(couchbase-send-buffer)
    //^^forgot the json, so 9? 10? fuckit just want it to work...
    //4, hmmmm......... BUT I PUT SO MUCH EFFORT INTO THAT MACRO REFACTOR ;-p (fail argument is fail, but very common among humans). still, doing it here has the advantage of being on a threadpool thread (backend is just one thread), so it keeps the backend free from doing a disk read and base64encode. there's also the weak argument that my base64encode is a Wt function (that boost b64 shit (in commit history) failed like fuck (b64? seriously boost? 2014?)).. but bleh i could always hack a wrapper to Wt's into the frontend2backend message class. I genuinely don't know which is more efficient, but doing it here on the front-end seems to make more DESIGN/sanity sense (despite probably being less efficient (definitely less efficient with copies, but idk about whether or not it's less efficient overall (and such true/false evaluation might be dependent on filesize!))
}
void NewAdSlotFillerAccountTabBody::handleAdImageUploadFailedFileTooLarge()
{
    m_AdSlotFillersVLayout->addWidget(new WText("Your ad's image exceeded the maximum file size of 164 kilobytes"));
    resetAdSlotFillerImageUploadFieldsForAnotherUpload();
}
void NewAdSlotFillerAccountTabBody::resetAdSlotFillerImageUploadFieldsForAnotherUpload()
{
    m_UploadNewSlotFiller_NICKNAME->setText("");
    m_UploadNewSlotFiller_NICKNAME_B64 = "";
    m_UploadNewSlotFiller_HOVERTEXT->setText("");
    m_UploadNewSlotFiller_URL->setText("http://");
    m_AdImageUploadButton->setEnabled(true);
    delete m_AdImageUploader;
    setUpAdImageUploaderAndPutItInPlaceholder();

    if(!m_MostRecentlyUploadedImageWasSetAsPreview)
    {
        if(m_MostRecentlyUploadedImageAsFileResource)
        {
            delete m_MostRecentlyUploadedImageAsFileResource;
        }
    }
    m_MostRecentlyUploadedImageAsFileResource = 0; //delete only under certain circumstances, but always set to zero for next image upload
    m_MostRecentlyUploadedImageWasSetAsPreview = false;

    m_AbcApp->resumeRendering();
}
void NewAdSlotFillerAccountTabBody::determineNextSlotFillerIndexAndThenAddSlotFillerToIt(const string &allAdSlotFillersJsonDoc, u_int64_t casOfAllSlotFillersDocForUpdatingSafely, bool lcbOpSuccess, bool dbError)
{
    //TODOreq: doesn't belong here, but there's a race condition that we wouldn't recover from using current recover design for ad slot filler creation. user has 3 tabs open and hits submit for all 3 at the same time. guh i can't think of it but it has something to do with the last cas-swap-accepting-fail update of allAdSlotFillers doc at the end... AND i might be wrong anyways and it isn't a problem. BUT IF IT IS, it's a potential segfault.

    if(dbError)
    {
        m_AdSlotFillersVLayout->addWidget(new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE));
        //TO DOnereq: handle. "500 Internal Server Error: Kill Yourself, Re-Evolve, Try Again (doing so puts you in an infinite loop of re-evolving/suiciding-at-the-same-spot that keeps looping and looping... until you eventually choose to not commit suicide. And since you can't live in a universe where you are dead (heh), you are already here in the universe where you chose not kill yourself. So yea just Try Again)"

        //temp:
        cerr << "determineNextSlotFillerIndexAndThenAddSlotFillerToIt db error" << endl;

        resetAdSlotFillerImageUploadFieldsForAnotherUpload();
        return;
    }

    m_RunningAllSlotFillersJsonDoc.clear();
    m_RunningAllSlotFillersJsonDocCAS = casOfAllSlotFillersDocForUpdatingSafely;

    string slotFillerIndexToAttemptToAdd;

    if(!lcbOpSuccess)
    {
        //TODOreq: this is first slot filler add (or 2nd and first partially failed xD), allSlotFillersDoc doesn't exist yet, make it [after adding slot filler]. perhaps just set a flag indicating that after the slot filler is added, allSlotFillers doc needs to be ADD-accepting-fail INSTEAD OF cas-swap-accepting-fail (no previous = no cas)

        slotFillerIndexToAttemptToAdd = "0";
        m_RunningAllSlotFillersJsonDoc.put(JSON_ALL_SLOT_FILLERS_ADS_COUNT, slotFillerIndexToAttemptToAdd); //initialize this 'string' index because later we +1 it, and it simplifies the code paths to already have it existing [on first ad slot filler create]
    }
    else
    {
        //allSlotFillersDoc exists
        std::istringstream is(allAdSlotFillersJsonDoc);
        read_json(is, m_RunningAllSlotFillersJsonDoc);

        slotFillerIndexToAttemptToAdd = m_RunningAllSlotFillersJsonDoc.get<std::string>(JSON_ALL_SLOT_FILLERS_ADS_COUNT); //seems like off-by-one to not add "+1" here, but isn't :-P

        if(boost::lexical_cast<int>(slotFillerIndexToAttemptToAdd) == 9999)
        {
            m_AdSlotFillersVLayout->addWidget(new WText("You hit the maximum amount of advertisement uploads (ten thousand). Please make a new account and/or bitch at me to raise this limit"));
            resetAdSlotFillerImageUploadFieldsForAnotherUpload();
            return;
        }
    }

    tryToAddAdSlotFillerToCouchbase(slotFillerIndexToAttemptToAdd);
    //^will most likely succeed, but our inline recovery code uses it too
}
void NewAdSlotFillerAccountTabBody::tryToAddAdSlotFillerToCouchbase(const string &slotFillerIndexToAttemptToAdd)
{
    ptree pt;
    pt.put(JSON_SLOT_FILLER_USERNAME, m_AbcApp->m_CurrentlyLoggedInUsername);

    //already sanitized, but we base64 these since we allow all kinds of crazy characters our json might not like
    m_UploadNewSlotFiller_NICKNAME_B64 = base64Encode(m_Validated_NewAdSlotFiller_NICKNAME, false);
    pt.put(JSON_SLOT_FILLER_NICKNAME, m_UploadNewSlotFiller_NICKNAME_B64);
    pt.put(JSON_SLOT_FILLER_HOVERTEXT, base64Encode(m_Validated_NewAdSlotFiller_HOVERTEXT, false));
    pt.put(JSON_SLOT_FILLER_URL, base64Encode(htmlEncode(m_Validated_NewAdSlotFiller_URL), false));

    std::string adImageUploadFileLocation = m_AdImageUploader->spoolFileName();
    pair<string,string> guessedExtensionAndMimeType = StupidMimeFromExtensionUtil::guessExtensionAndMimeType(m_AdImageUploader->clientFileName().toUTF8());
    m_MostRecentlyUploadedImageAsFileResource = new FileDeletingFileResource(adImageUploadFileLocation, "image" + guessedExtensionAndMimeType.first, "image/" + guessedExtensionAndMimeType.second, WResource::Inline, this); //semi-old (now using m_AccountWidget instead of 'this'): 'this' is set as parent for last resort cleanup of the WResource, but that doesn't account for it's underlying buffer. I also might want to delete this resource if they upload a 2nd/3rd/etc image, BUT TODOreq there are threading issues because the WImage/WResource is served concurrently I believe (or is that only true for static resources?). Perhaps changing internal paths would be a good time to delete all the images (or in WResource::handleRequest itself, since they'll only be used once anyways..), idfk. There is also the issue of me trying to re-use the same byte buffer for a 2nd upload (which is backing a resource for a first upload that hasn't yet finished streaming back / previewing back to them)
    m_AdImageUploader->stealSpooledFile();

    //unsure if i should do the expensive read/b64encode here or in the db backend (passing only filename). i want an async api obviously, but laziness/KISS might dictate otherwise

    //TODOoptimization: had this as 'req' but memory management/sanity is much simpler when doing it less optimially xD (it's only ever paid during recovery code anyways): when doing inline recovery, don't re-read from disk. also if we then 'store it' as member or some such, make sure to delete it when it finishes successfully. The solution should use some form of WObject parent/child deletion...
    streampos fileSizeHack;
    char *adImageBuffer;
    ifstream adImageFileStream(adImageUploadFileLocation.c_str(), ios::in | ios::binary | ios::ate);
    if(adImageFileStream.is_open())
    {
        fileSizeHack = adImageFileStream.tellg();
        adImageFileStream.seekg(0,ios::beg);
        bool imageHeaderMagicAccepted = false; //prove to me otherwise!
        if(fileSizeHack > 1) //yea i mean i've heard of ImageMagick.. but I've also seen it in numerous CVE reports xD...
        {
            char headerMagic[2] = {0};
            adImageFileStream.read(headerMagic, 2);
            imageHeaderMagicAccepted = NonAnimatedImageHeaderChecker::headerIndicatesNonAnimatedImage(headerMagic);
        }
        if(!imageHeaderMagicAccepted)
        {
            new WBreak(this);
            new WText("Invalid image file format. " ABC_AD_SLOT_FILLERS_ACCEPT_IMAGE_FORMATS_MESSAGE, this);
            adImageFileStream.close();
            resetAdSlotFillerImageUploadFieldsForAnotherUpload();
            return;
        }
        adImageFileStream.seekg(0,ios::beg);
        adImageBuffer = new char[fileSizeHack]; //TODOoptimization: running out of memory (server critical load) throws an exception, we could pre-allocate this, but how to then share it among WApplications? guh fuckit. i think 'new' is mutex protected underneath anyways, so bleh maybe using a mutex and a pre-allocated buffer is even faster than this (would of course be best to use rand() + mutex shits xD)... but then we can't have multiple doing it on thread pool at same time (unless rand() + mutex shits)
        adImageFileStream.read(adImageBuffer, fileSizeHack);
        adImageFileStream.close();
    }
    else
    {
        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);
        cerr << "failed to open new ad slot filler just uploaded for reading" << endl;
        //NOPE: delete [] adImageBuffer;
        resetAdSlotFillerImageUploadFieldsForAnotherUpload();
        return;
    }

    std::string adImageString = std::string(adImageBuffer, fileSizeHack);
    std::string adImageB64string = base64Encode(adImageString, false); //TODOreq(done i THINK. made 2.5mb queue max, but 164 kb wfileupload max): doesn't encoding in base64 make it larger, so don't i need to make my "StoreLarge" message size bigger? I know in theory it doesn't change the size, but it all depends on representation or some such idfk (i get con-

    pt.put(JSON_SLOT_FILLER_IMAGEB64, adImageB64string);
    pt.put(JSON_SLOT_FILLER_IMAGE_GUESSED_EXTENSION, guessedExtensionAndMimeType.first);

    std::ostringstream adSlotFillerJsonDocBuffer;
    write_json(adSlotFillerJsonDocBuffer, pt, false);


    m_AbcApp->storeLarge_ADDbyDefault_WithoutInputCasCouchbaseDocumentByKeyBegin(adSpaceSlotFillerKey(m_AbcApp->m_CurrentlyLoggedInUsername, slotFillerIndexToAttemptToAdd), adSlotFillerJsonDocBuffer.str());
    //storeLarge has it's own 'begin', but it shares the 'finish' with the typical store
    m_AbcApp->m_WhatTheStoreWithoutInputCasWasFor = AnonymousBitcoinComputingWtGUI::LARGE_ADIMAGEUPLOADBUYERSETTINGUPADSLOTFILLERFORUSEINPURCHASINGLATERONSTOREWITHOUTINPUTCAS; //TODOoptional: make these enums more readable with underscores, and put numbering toward the beginning of them that matches up with the callback/function (perhaps renaming those too)

    delete [] adImageBuffer;
}
void NewAdSlotFillerAccountTabBody::storeLargeAdImageInCouchbaseDbAttemptComplete(const string &keyToSlotFillerAttemptedToAddTo, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        m_AdSlotFillersVLayout->addWidget(new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE));

        //temp:
        cerr << "storeLargeAdImageInCouchbaseDbAttemptComplete reported db error" << endl;

        resetAdSlotFillerImageUploadFieldsForAnotherUpload();
        return;
    }
    if(!lcbOpSuccess)
    {
        //this indicates that the allAdSlotFillers we GET'd was in a bad state (either it just failed to be updated OR they have two tabs open doing two uploads at once etc). we need to do a GET for the one we just LCB_ADD fail'd in order to get the nickname for that slot filler, then we need to put it in our "running all slot fillers doc" and try to LCB_ADD the n+1 slot...... it all needs to loop sexily and after we finally get an LCB_ADD to succeed, we then take our "running all slot fillers doc" (which has been updated (modified, not submitted) 0+ times), add our final LCB_ADD that _just_ succeeded to it at the very end, then cas-swap-accepting-fail the allAdSlotFillers. This design does have the drawback that multiple tabs uploading simultaneously may frequently get the allAdSlotFillers into a bad state, BUT it will always be recovered from on subsequent updates. AND really it is a pretty rare race condition, because it'd have to go in between a sub-millisecond "GET" and an "LCB_ADD" (excluding the durability portion)..... trying to do that intentionally WHILE uploading binary images 'before' the GET/ADD sequence would be difficult, doing it accidentally is possible but not likely.
        //TODOoptional: give user a button "hey where is my most recently uploaded ad" that does the recovery process manually (fuck this idea for now)

        m_AbcApp->getCouchbaseDocumentByKeyBegin(keyToSlotFillerAttemptedToAddTo); //couchbase needs a "add or get" lcb op :), maybe i should file it as an enhancement
        m_AbcApp->m_WhatTheGetWasFor = AnonymousBitcoinComputingWtGUI::GETNICKNAMEOFADSLOTFILLERNOTINALLADSLOTFILLERSDOCFORADDINGITTOIT_THEN_TRYADDINGTONEXTSLOTFILLERINDEXPLZ; //kind of grossly inefficient to go in and out of the WApplication context so often like this to do 'business' work, but KISS idgaf suck mah dick. "AbcAppDb" was a 2+ year tangent (still incomplete)

        //temp:
        //cerr << "storeLargeAdImageInCouchbaseDbAttemptComplete reported lcb op failed" << endl;

        //m_AbcApp->resumeRendering();
        return;
    }

    //getting here means the add was successful, so record that fact in allAdSlotFillers
    std::string oldAdsCountButAlsoOurIndexOfTheOneJustAdded = m_RunningAllSlotFillersJsonDoc.get<std::string>(JSON_ALL_SLOT_FILLERS_ADS_COUNT);
    m_RunningAllSlotFillersJsonDoc.put(oldAdsCountButAlsoOurIndexOfTheOneJustAdded, m_UploadNewSlotFiller_NICKNAME_B64);

    //now increment adsCount and put it back in xD
    oldAdsCountButAlsoOurIndexOfTheOneJustAdded = boost::lexical_cast<std::string>(boost::lexical_cast<int>(oldAdsCountButAlsoOurIndexOfTheOneJustAdded)+1); //old becomes new :)
    m_RunningAllSlotFillersJsonDoc.put(JSON_ALL_SLOT_FILLERS_ADS_COUNT, oldAdsCountButAlsoOurIndexOfTheOneJustAdded);


    std::ostringstream updatedAllAdSlotFillersJsonDocBuffer;
    write_json(updatedAllAdSlotFillersJsonDocBuffer, m_RunningAllSlotFillersJsonDoc, false);
    m_AbcApp->storeLarge_SETonly_CouchbaseDocumentByKeyWithInputCasBegin(adSpaceAllSlotFillersKey(m_AbcApp->m_CurrentlyLoggedInUsername), updatedAllAdSlotFillersJsonDocBuffer.str(), m_RunningAllSlotFillersJsonDocCAS, StoreCouchbaseDocumentByKeyRequest::DiscardOuputCasMode);
    m_AbcApp->m_WhatTheStoreWithInputCasWasFor = AnonymousBitcoinComputingWtGUI::UPDATEALLADSLOTFILLERSDOCSINCEWEJUSTCREATEDNEWADSLOTFILLER;
}
void NewAdSlotFillerAccountTabBody::doneAttemptingToUpdateAllAdSlotFillersDocSinceWeJustCreatedANewAdSlotFiller(bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        m_AdSlotFillersVLayout->addWidget(new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE));

        //temp:
        cerr << "doneAttemptingToUpdateAllAdSlotFillersDocSinceWeJustCreatedANewAdSlotFiller db error" << endl;

        resetAdSlotFillerImageUploadFieldsForAnotherUpload();
        return;
    }
    if(!lcbOpSuccess)
    {
        //cas swap fail
        //TODOreq: handle? this is what i was attempting to write might be a non-recoverable fault (not necessarily, but this is where we MIGHT need to do more processing). If they are adding two images simultaneously on two different tabs it would be semi-likely to get here (still quite rare). BUT that common case is handled fine when a 3rd image is uploaded. What does get me confused is if 3 images are uploaded simultaneously... it MIGHT put me in a state of non-recoverable fault (which would cause the app itself to segfault when iterating all ad slot fillers doc for populating the combo box in the buy step 2 population stage)

        //TODOreq: should i still display their image to them? i mean the LCB_ADD did succeed for us to get here... so idfk...
        //^^makes sense that as soon as the image is received from the file upload, i put it in a WFileResource with 'this' as parent, regardless of the db stuff. i also need to make sure i delete the file only ever by deleting that WFileResource, like when the image is too big or something

        //TODOreq: the obvious/expensive (and more work ;-P) answer is to say that if we get _HERE_ we just do an allAdSlotFillers doc verification/recovery process (even though we supposedly/probably just did it!), idfk need to smoke a bowl and meditate on this shit

        new WBreak(this);
        new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE, this);

        cerr << "doneAttemptingToUpdateAllAdSlotFillersDocSinceWeJustCreatedANewAdSlotFiller lcb op fail, may or may not be an error read the comments near this error string xD idfk. MAYBE/probably in recoverable state" << endl;

        resetAdSlotFillerImageUploadFieldsForAnotherUpload();
        return;
    }

    //cas-swap-update succeeded

    //so now show them the image

    WImage *adImagePreview = new WImage(m_MostRecentlyUploadedImageAsFileResource, m_Validated_NewAdSlotFiller_HOVERTEXT);
    adImagePreview->resize(ABC_MAX_AD_SLOT_FILLER_IMAGE_WIDTH_PIXELS, ABC_MAX_AD_SLOT_FILLER_IMAGE_HEIGHT_PIXELS);
    WAnchor *adImageAnchor = new WAnchor(WLink(WLink::Url, m_Validated_NewAdSlotFiller_URL), adImagePreview);
    adImageAnchor->setTarget(TargetNewWindow);

    //difference between these two?
    adImagePreview->setToolTip(m_Validated_NewAdSlotFiller_HOVERTEXT);
    adImageAnchor->setToolTip(m_Validated_NewAdSlotFiller_HOVERTEXT);

    m_AdSlotFillersVLayout->addWidget(new WText("Preview of '" + m_Validated_NewAdSlotFiller_NICKNAME + "':"));
    m_AdSlotFillersVLayout->addWidget(adImageAnchor);
    m_AdSlotFillersVLayout->addWidget(new WBreak());
    m_AdSlotFillersVLayout->addWidget(new WBreak());
    m_AdSlotFillersVLayout->addWidget(new WBreak());

    //prevents image file from being deleted in the 'reset' coming up
    m_MostRecentlyUploadedImageWasSetAsPreview = true;

    //done, so...
    resetAdSlotFillerImageUploadFieldsForAnotherUpload();
}
void NewAdSlotFillerAccountTabBody::getAdSlotFillerThatIsntInAllAdSlotFillersAttemptFinished_soAddItToAllAddSlotFillersAndInitiateSlotFillerAddAtNextIndex(const string &adSlotFillerToExtractNicknameFrom, bool lcbOpSuccess, bool dbError)
{
    if(dbError)
    {
        m_AdSlotFillersVLayout->addWidget(new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE));

        cerr << "getAdSlotFillerThatIsntInAllAdSlotFillersAttemptFinished_soAddItToAllAddSlotFillersAndInitiateSlotFillerAddAtNextIndex db error" << endl;
        resetAdSlotFillerImageUploadFieldsForAnotherUpload();
        return;
    }
    if(!lcbOpSuccess)
    {
        m_AdSlotFillersVLayout->addWidget(new WText(ABC_500_INTERNAL_SERVER_ERROR_MESSAGE));
        cerr << "TOTAL SYSTEM FAILURE: getAdSlotFillerThatIsntInAllAdSlotFillersAttemptFinished_soAddItToAllAddSlotFillersAndInitiateSlotFillerAddAtNextIndex -- an LCB_ADD to slot filler failed, so we tried to get it to extract nickname from it for updating allAdSlotFillers... but now it doesn't exist?? wtf?" << endl;
        resetAdSlotFillerImageUploadFieldsForAnotherUpload();
        return;
    }

    //get succeeded
    ptree pt;
    std::istringstream is(adSlotFillerToExtractNicknameFrom);
    read_json(is, pt);

    std::string nicknameOfSlotFillerNotInAllAdSlotFillersDoc = pt.get<std::string>(JSON_SLOT_FILLER_NICKNAME); //mfw the entire base64 image is get'd just to figure out this nickname :)... but this is recovery code and won't run often

    //json-add nickname to the 'running' allAdSlotFillers doc
    std::string indexWeTriedToAddToBecauseAllSlotFillersDocSaidItWasEmptyButItWasWrongSoHereWeAreRecoveringFromThat = m_RunningAllSlotFillersJsonDoc.get<std::string>(JSON_ALL_SLOT_FILLERS_ADS_COUNT);
    m_RunningAllSlotFillersJsonDoc.put(indexWeTriedToAddToBecauseAllSlotFillersDocSaidItWasEmptyButItWasWrongSoHereWeAreRecoveringFromThat, nicknameOfSlotFillerNotInAllAdSlotFillersDoc); //TO DOnereq(StoreLarge): nicknames 64*10000 might go over message queue max size guh, this applies to normal code as well (we are recovery). Maybe allAdSlotFillers needs to use StoreLarge (even then, i have no maximum amount of ads!)?

    //also increment 'adsCount' because we've just found/recovered one slot filler, and additionally will be using adsCount as our index to add the slot filler AGAIN (the one that initiated this recovery) in the 'next' slot
    indexWeTriedToAddToBecauseAllSlotFillersDocSaidItWasEmptyButItWasWrongSoHereWeAreRecoveringFromThat = boost::lexical_cast<std::string>(boost::lexical_cast<int>(indexWeTriedToAddToBecauseAllSlotFillersDocSaidItWasEmptyButItWasWrongSoHereWeAreRecoveringFromThat) + 1); //the +1 here is important (had:key)!!!!
    m_RunningAllSlotFillersJsonDoc.put(JSON_ALL_SLOT_FILLERS_ADS_COUNT, indexWeTriedToAddToBecauseAllSlotFillersDocSaidItWasEmptyButItWasWrongSoHereWeAreRecoveringFromThat);

    //now LCB_ADD index+1 again
    tryToAddAdSlotFillerToCouchbase(indexWeTriedToAddToBecauseAllSlotFillersDocSaidItWasEmptyButItWasWrongSoHereWeAreRecoveringFromThat);
}
bool NewAdSlotFillerAccountTabBody::userSuppliedAdSlotFillerFieldsAreValid()
{
    if(m_UploadNewSlotFiller_HOVERTEXT->validate() != WValidator::Valid)
    {
        m_AdSlotFillersVLayout->addWidget(new WText("Invalid Hover text"));
        return false;
    }
    if(m_UploadNewSlotFiller_NICKNAME->validate() != WValidator::Valid)
    {
        m_AdSlotFillersVLayout->addWidget(new WText("Invalid Nickname"));
        return false;
    }
    if(m_UploadNewSlotFiller_URL->validate() != WValidator::Valid)
    {
        m_AdSlotFillersVLayout->addWidget(new WText("Invalid URL"));
        return false;
    }
    return true;
}
