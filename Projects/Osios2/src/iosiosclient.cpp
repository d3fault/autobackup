#include "iosiosclient.h"

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QBuffer>

#include "osios.h"

QByteArray IOsiosClient::calculateCryptographicHashOfWidgetRenderedToImage(QWidget *widgetToRenderAndCalculateSha1SumOfRenderingFor)
{
    //#ifdef OSIOS_DHT_CONFIG_NEIGHBOR_SENDS_BACK_RENDERING_WITH_CRYPTOGRAPHIC_VERIFICATION_OF_TIMELINE_NODE
    QImage widgetRendereredAsImage(widgetToRenderAndCalculateSha1SumOfRenderingFor->sizeHint(), QImage::Format_ARGB32);
    QPainter widgetPainter(&widgetRendereredAsImage);
    widgetToRenderAndCalculateSha1SumOfRenderingFor->render(&widgetPainter); //TODOreq: don't render notification panel (see note above above putting another main window as central widget of outer most main window). TODOmb: add timestamp to the png before calculating has, or maybe mix in the timeline node data itself... the reason being that lots of rendered widgets will generate identical cryptographic hashes even if they are for different timeline nodes (this is mostly a safeguard against programming error, probably not necessary)

    QByteArray renderedWidgetPngBytes;

    {
        QBuffer renderedWidgetPngBuffer(&renderedWidgetPngBytes);
        renderedWidgetPngBuffer.open(QIODevice::WriteOnly);
        widgetRendereredAsImage.save(&renderedWidgetPngBuffer, "PNG", 0); //TODOoptimization: maybe don't use the highest compression (0). try/benchmark others, including the default of -1. This should probably be compile time specifiable. And additionally, when the deployment target has plenty of RAM (had:room) but a slow CPU, BMP would probably be better
    }

    return Osios::calculateCrytographicHashOfByteArray(renderedWidgetPngBytes); //if i wanted to use the rendered image itself, all I'd have to do is return renderedWidgetPngBytes instead xD. Just a bandwidth optimization to not send entire image but only send hash
    //QByteArray renderingProof_Sha1OfPngRofl = Osios::calculateCrytographicHashOfByteArray(renderedWidgetPngBytes);
    //emit copycatTimelineNodeRendered(osiosDhtPeer, timelineNode, renderingProof_Sha1OfPngRofl);
    //#endif
}
