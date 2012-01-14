/**
    This file is part of Sudoku United.

    Sudoku United is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Sudoku United is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Sudoku United.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "telepathyhandler.h"
#include "telepathyclient.h"
#include "telepathyserver.h"

#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/ChannelClassSpecList>

TelepathyHandler *TelepathyHandler::m_instance = NULL;

TelepathyStreamHandler::TelepathyStreamHandler(TelepathyHandler *handler, const Tp::ChannelClassSpecList &channelFilter)
    : Tp::AbstractClientHandler(channelFilter), handler(handler)
{
}

bool TelepathyStreamHandler::bypassApproval() const
{
    return false;
}

void TelepathyStreamHandler::onChannelReady(Tp::PendingOperation *operation) {
    Tp::ChannelPtr channel = Tp::ChannelPtr::dynamicCast(operation->object());

    if (operation->isError()) {
        channel->requestClose();
        return;
    }

    if (channel->isRequested()) {
        Tp::OutgoingStreamTubeChannelPtr outgoingStream =
                Tp::OutgoingStreamTubeChannelPtr::qObjectCast(channel);
        if (handler->client)
            handler->client->handleOutgoingStream(outgoingStream);
    } else {
        Tp::IncomingStreamTubeChannelPtr incomingStream =
                Tp::IncomingStreamTubeChannelPtr::qObjectCast(channel);
        if (handler->server)
            handler->server->handleIncomingStream(incomingStream);
    }
}

void TelepathyStreamHandler::handleChannels(const Tp::MethodInvocationContextPtr<> &context,
                                            const Tp::AccountPtr &account,
                                            const Tp::ConnectionPtr &connection,
                                            const QList<Tp::ChannelPtr> &channels,
                                            const QList<Tp::ChannelRequestPtr> &requestsSatisfied,
                                            const QDateTime &userActionTime,
                                            const Tp::AbstractClientHandler::HandlerInfo &handlerInfo)
{
    Q_UNUSED(account)
    Q_UNUSED(connection)
    Q_UNUSED(requestsSatisfied)
    Q_UNUSED(userActionTime)
    Q_UNUSED(handlerInfo)

    foreach(const Tp::ChannelPtr &channel, channels) {
        QVariantMap properties = channel->immutableProperties();
        if (properties[TELEPATHY_INTERFACE_CHANNEL ".ChannelType"] ==
                TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAM_TUBE) {
            qDebug() << "Got stream tube...";
            connect(channel->becomeReady(Tp::StreamTubeChannel::FeatureCore | Tp::StreamTubeChannel::FeatureConnectionMonitoring), SIGNAL(finished(Tp::PendingOperation*)), SLOT(onChannelReady(Tp::PendingOperation*)));
        }
    }
    context->setFinished();
}

TelepathyHandler::TelepathyHandler()
    : client(NULL), server(NULL), inGame(false)
{
    Tp::registerTypes();

    registrar = Tp::ClientRegistrar::create();

    registerClient();
}

TelepathyHandler *TelepathyHandler::instance() {
    if (!m_instance)
        m_instance = new TelepathyHandler();

    return m_instance;
}

void TelepathyHandler::setClient(TelepathyClient *client) {
    this->client = client;
}

void TelepathyHandler::setServer(TelepathyServer *server) {
    this->server = server;
}

void TelepathyHandler::setPublishInGame(bool inGame) {
    if (inGame == this->inGame)
        return;

    this->inGame = inGame;

    registerClient();
}

void TelepathyHandler::registerClient() {
    Tp::ChannelClassSpecList channelClassSpecList;

    channelClassSpecList << Tp::ChannelClassSpec::incomingStreamTube("x-sudoku-united")
                         << Tp::ChannelClassSpec::outgoingStreamTube("x-sudoku-united");

    if (inGame)
        channelClassSpecList << Tp::ChannelClassSpec::incomingStreamTube("x-sudoku-united-game");

    Tp::SharedPtr<TelepathyStreamHandler> newHandler = Tp::SharedPtr<TelepathyStreamHandler>(
                new TelepathyStreamHandler(this,
                                           channelClassSpecList));

    if (!handler.isNull())
        registrar->unregisterClient(handler);

    handler = newHandler;

    QString target = "sudokuunited";
    while (!registrar->registerClient(handler, target)) {
        target = target + "1";
        qWarning() << "Failed to register Telepathy stream handler.";
    }
}
