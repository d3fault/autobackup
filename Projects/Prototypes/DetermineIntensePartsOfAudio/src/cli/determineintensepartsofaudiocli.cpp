#include "determineintensepartsofaudiocli.h"

#include <QCoreApplication>
#include <QDebug>

#include "determineintensepartsofaudio.h"

DetermineIntensePartsOfAudioCli::DetermineIntensePartsOfAudioCli(QObject *parent)
    : QObject(parent)
{
    connect(this, &DetermineIntensePartsOfAudioCli::exitRequested, qApp, &QCoreApplication::exit, Qt::QueuedConnection);

    DetermineIntensePartsOfAudio *determineIntensePartsOfAudio = new DetermineIntensePartsOfAudio(this);
    connect(determineIntensePartsOfAudio, SIGNAL(doneDeterminingIntensePartsOfAudio(bool,QList<quint64>)), this, SLOT(handleDeterminingIntensePartsOfAudioFinished(bool,QList<quint64>)));
    determineIntensePartsOfAudio->startDeterminingIntensePartsOfAudio("/run/shm/ark.wav");
}
void DetermineIntensePartsOfAudioCli::handleE(const QString &msg)
{
    qDebug() << msg;
}
void DetermineIntensePartsOfAudioCli::handleDeterminingIntensePartsOfAudioFinished(bool success, QList<quint64> msTimestampsOfIntensePartsOfAudio)
{
    qDebug() << "ms timestamps of intense parts of audio:" << msTimestampsOfIntensePartsOfAudio;
    emit exitRequested(success ? 0 : 1); //the logical to POSIX translation (yes, I -AM- saying that POSIX is not logical muahahahaha (but it's still an order of magnitude more logical than that other shitty rogue OS that 99% of people use)
}
