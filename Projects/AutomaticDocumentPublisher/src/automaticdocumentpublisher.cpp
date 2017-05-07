#include "automaticdocumentpublisher.h"

AutomaticDocumentPublisher::AutomaticDocumentPublisher(QObject *parent)
    : QObject(parent)
{ }
void AutomaticDocumentPublisher::publishDocument(const QString &documentFilePath, const QMimeType &documentType)
{
    //if documentType is Picture, use <img> tags, etc. but try to push that shit out into modules for cleaner code!
    IDocumentRenderer *renderer = getDocumentRendererByDocumentType(documentType);
    renderer->setTargetFilePath(documentFilePath + ".html"); //or we could use referer detection to decide whether to serve up the html rendering of the file or the raw file itself (wait what? does this strategy work with browser caching? why would the browser try to embed it's own current url into an image and NOT use the cached copy of itself (ofc I can turn off caching for the rendered file to get around that, but subsequent attempts to visit the 'rendered' file would then show the 'raw' file because that one would/should be cached!)) how do those sites do it...? (who cares how 'they' do it, you can always do it the way you did on Hvbs using Wt (but without using Wt this time, just stealing Wt's techniques))). this "append dot html extension" has collisions so is shit, but is good enough for now TODOreq: fix collisions
    renderer->render(documentFilePath); //render means (in the context of Picture): copy picture.jpg to public_html folder and correct subfolder (this part is same for each type, so it should be performed in a base class), then create picture.jpg.html and write header/body/footer -- body obviously has an <img> tag pointing to picture.jpg
    updateRootIndexDotHtmlFileToPublishTheDocumentOnFrontPage(documentFilePath);

    //I don't think I want to have it as part of this app, but incidentally at this point I want to git add the new html rendering of the document, the raw document itself, and the root index.html. then git commit and git push to github-pages. but see it shouldn't depend on git[hub] or that jekyll/jeryll shit whatever it's called so it should be one final script hacked onto the end of the chain of commands. either that or I have a MODULAR 'finalizePublish' method, of which pushing to github-pages is just the first/initial implementation. I think that breaks the "do one thing and do it well" idea though, perhaps github-pages pushing should go in different process (but there's a flaw in that stupid quote: classes ARE 'processes' imo. at least if you do it RIGHT in Qt they are/can-be[made-into-processes-or-threads-or-whatever-at-any-time]. it could be a standalone app but it could be a module of this app, or both, it makes no difference really (assuming I have also a/the generic module with 'finalizePublish' by calling some process X you provide the path to)

    emit documentPublished(true);
}
