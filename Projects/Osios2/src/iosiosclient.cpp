#include "iosiosclient.h"

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QBuffer>

//debug:
//#define OSIOS_SAVE_EACH_RENDERED_TIMELINE_NODE_PROOF_IN_SETTINGS_DIR 1
#ifdef OSIOS_SAVE_EACH_RENDERED_TIMELINE_NODE_PROOF_IN_SETTINGS_DIR
#include <QDir>
#include <QSettings>
#include <QFileInfo>
#include <QFile>
#include <QScopedPointer>
#include <QDateTime>
#include "osiossettings.h"
#endif // OSIOS_SAVE_EACH_RENDERED_TIMELINE_NODE_PROOF_IN_SETTINGS_DIR

#include "osios.h"

QByteArray IOsiosClient::calculateCryptographicHashOfWidgetRenderedToImage(QWidget *widgetToRenderAndCalculateSha1SumOfRenderingFor)
{
    //#ifdef OSIOS_DHT_CONFIG_NEIGHBOR_SENDS_BACK_RENDERING_WITH_CRYPTOGRAPHIC_VERIFICATION_OF_TIMELINE_NODE
    QImage widgetRendereredAsImage(widgetToRenderAndCalculateSha1SumOfRenderingFor->size() /*TODOreq: i was using sizeHint, but that only ended up giving me "reasonable default/minimum size". Calling size() like this does work right now, but I worry/wonder if it will still work when the widget being drawn isn't being shown. FOR EXAMPLE, showMaximized() might not have been called (since widget is never shown in the rendering-proof node) and so the size would be ??? (not maximized). One way of solving this is to send the size in our network protocol, preferably during init/resize-events only, but to KISS up front maybe with every action. I am confidenet that calling resize on the unshown widget will solve it, but getting the coords to the replica/renderer still needs to happen */, QImage::Format_ARGB32);
    QPainter widgetPainter(&widgetRendereredAsImage);

    bool widgetHasFocus = widgetToRenderAndCalculateSha1SumOfRenderingFor->hasFocus(); //this seems hacky as fuck but in my testing does appear to work. Investigated qstyle qproxystyle and qstyleoption with no luck (although I felt I was close)
    if(widgetHasFocus)
        widgetToRenderAndCalculateSha1SumOfRenderingFor->clearFocus();
    widgetToRenderAndCalculateSha1SumOfRenderingFor->render(&widgetPainter); //TODOreq: don't render notification panel (see note above above putting another main window as central widget of outer most main window). TODOmb: add timestamp to the png before calculating has, or maybe mix in the timeline node data itself... the reason being that lots of rendered widgets will generate identical cryptographic hashes even if they are for different timeline nodes (this is mostly a safeguard against programming error, probably not necessary)
    if(widgetHasFocus)
        widgetToRenderAndCalculateSha1SumOfRenderingFor->setFocus();

    QByteArray renderedWidgetPngBytes;

    {
        QBuffer renderedWidgetPngBuffer(&renderedWidgetPngBytes);
        renderedWidgetPngBuffer.open(QIODevice::WriteOnly);
        widgetRendereredAsImage.save(&renderedWidgetPngBuffer, "PNG", 0); //TODOoptimization: maybe don't use the highest compression (0). try/benchmark others, including the default of -1. This should probably be compile time specifiable. And additionally, when the deployment target has plenty of RAM (had:room) but a slow CPU, BMP would probably be better
    }

    //debug:
#ifdef OSIOS_SAVE_EACH_RENDERED_TIMELINE_NODE_PROOF_IN_SETTINGS_DIR
    QScopedPointer<QSettings> settings(OsiosSettings::newSettings());
    QFileInfo settingsFileInfo(settings->fileName());
    QString settingsDirString = settingsFileInfo.absolutePath() + "/";
    QDir settingsDir(settingsDirString);
    if(!settingsDir.exists())
    {
        settingsDir.mkpath(settingsDirString);
    }
    QFile screenshotFile(settingsDirString + "timelineNodeRenderedAt-" + QString::number(QDateTime::currentMSecsSinceEpoch()) + ".png");
    screenshotFile.open(QIODevice::WriteOnly);
    screenshotFile.write(renderedWidgetPngBytes);
#endif

    return Osios::calculateCrytographicHashOfByteArray(renderedWidgetPngBytes); //if i wanted to use the rendered image itself, all I'd have to do is return renderedWidgetPngBytes instead xD. Just a bandwidth optimization to not send entire image but only send hash
    //QByteArray renderingProof_Sha1OfPngRofl = Osios::calculateCrytographicHashOfByteArray(renderedWidgetPngBytes);
    //emit copycatTimelineNodeRendered(osiosDhtPeer, timelineNode, renderingProof_Sha1OfPngRofl);
    //#endif
}
