#include "mainwidget.h"

mainWidget::mainWidget(DataStreamDoesNotHandleShortReadsBackend *debuggableStartableStoppableBackend, QWidget *parent)
    : IDebuggableStartableStoppableFrontend(debuggableStartableStoppableBackend, parent)
{ }
void mainWidget::addButtonsToLayoutAndConnectToBackend()
{
    m_SendNextChunkOfMessageButton = new QPushButton("Send Next Chunk Of Message");
    m_Layout->addWidget(m_SendNextChunkOfMessageButton);

    connect(m_SendNextChunkOfMessageButton, SIGNAL(clicked()), static_cast<DataStreamDoesNotHandleShortReadsBackend*>(m_DebuggableStartableStoppableBackend), SLOT(sendNextChunkOfMessage()));
}
