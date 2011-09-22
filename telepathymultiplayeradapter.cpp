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

#include "telepathymultiplayeradapter.h"
#include "sudoku.h"

#include <TelepathyQt4/PendingStreamTubeConnection>
#include <TelepathyQt4/PendingChannelRequest>

#include <QLocalServer>

#include <stdio.h>

TelepathyStreamHandler::TelepathyStreamHandler(TelepathyMultiplayerAdapter *multiplayerAdapter, const Tp::ChannelClassSpecList &channelFilter)
    : Tp::AbstractClientHandler(channelFilter), multiplayerAdapter(multiplayerAdapter)
{
}

bool TelepathyStreamHandler::bypassApproval() const
{
    return false;
}

void TelepathyStreamHandler::onChannelReady(Tp::PendingOperation *operation) {
    Tp::ChannelPtr channel = Tp::ChannelPtr::dynamicCast(operation->object());

    if (channel->isRequested()) {
        Tp::OutgoingStreamTubeChannelPtr outgoingChannel =
                Tp::OutgoingStreamTubeChannelPtr::qObjectCast(channel);
        multiplayerAdapter->handleOutgoingChannel(outgoingChannel);
    } else {
        Tp::IncomingStreamTubeChannelPtr incomingChannel =
                Tp::IncomingStreamTubeChannelPtr::qObjectCast(channel);
        multiplayerAdapter->handleIncomingChannel(incomingChannel);
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
            connect(channel->becomeReady(), SIGNAL(finished(Tp::PendingOperation*)), SLOT(onChannelReady(Tp::PendingOperation*)));
        }
    }
    context->setFinished();
}

TelepathyMultiplayerAdapter::TelepathyMultiplayerAdapter(Sudoku *parent) :
    MultiplayerAdapter(parent)
{
    Tp::registerTypes();

    m_server = new QLocalServer(this);
    m_server->listen(QString(tempnam(QDir::tempPath().toUtf8().constData(), "sudokuunited")));

    connect(m_server, SIGNAL(newConnection()), SLOT(onNewConnection()));

    registrar = Tp::ClientRegistrar::create();

    registerChannels();

    accountManager = Tp::AccountManager::create(Tp::AccountFactory::create(QDBusConnection::sessionBus(),
                                                                           Tp::Account::FeatureCore),
                                                Tp::ConnectionFactory::create(QDBusConnection::sessionBus(),
                                                                              Tp::Connection::FeatureRoster),
                                                Tp::ChannelFactory::create(QDBusConnection::sessionBus()),
                                                Tp::ContactFactory::create(Tp::Contact::FeatureCapabilities | Tp::Contact::FeatureAlias));

    connect(m_sudoku, SIGNAL(gameChanged()), SLOT(onGamesChanged()));
}

void TelepathyMultiplayerAdapter::onGamesChanged() {
    registerChannels();

    if (!game())

        if (!outgoingStream.isNull()) {
            outgoingStream->requestClose();
            outgoingStream = Tp::OutgoingStreamTubeChannelPtr();
        }
}

void TelepathyMultiplayerAdapter::registerChannels() {
    Tp::ChannelClassSpecList channelClassSpecList;

    channelClassSpecList << Tp::ChannelClassSpec::incomingStreamTube("x-sudoku-united")
                         << Tp::ChannelClassSpec::outgoingStreamTube("x-sudoku-united")
                         << Tp::ChannelClassSpec::outgoingStreamTube("x-sudoku-united-game");

    if (game() != NULL)
        channelClassSpecList << Tp::ChannelClassSpec::incomingStreamTube("x-sudoku-united-game");

    Tp::AbstractClientPtr newHandler = Tp::AbstractClientPtr::dynamicCast(
                Tp::SharedPtr<TelepathyStreamHandler>(new TelepathyStreamHandler(this,
                                                                                 channelClassSpecList)));

    if (!handler.isNull())
        registrar->unregisterClient(handler);

    handler = newHandler;

    QString target = "sudokuunited";
    while (!registrar->registerClient(handler, target)) {
        target = target + "1";
        qWarning() << "Failed to register Telepathy stream handler.";
    }
}

GameInfoModel *TelepathyMultiplayerAdapter::discoverGames() {
    return new TelepathyGameInfoModel(accountManager, this);
}

void TelepathyMultiplayerAdapter::join(GameInfo *game) {
    TelepathyGameInfo *gameInfo = qobject_cast<TelepathyGameInfo *>(game);

    if (gameInfo == NULL) {
        qDebug() << "Unsupported game parameter.";
        return;
    }

    MultiplayerAdapter::join(game);

    Tp::PendingChannelRequest *request = gameInfo->account->createStreamTube(gameInfo->contact, "x-sudoku-united-game");
    connect(request, SIGNAL(finished(Tp::PendingOperation*)), SLOT(onChannelRequestFinished(Tp::PendingOperation*)));

    qDebug() << "Connecting...";
}

bool TelepathyMultiplayerAdapter::canJoinGameInfo(GameInfo *game) const {
    return qobject_cast<TelepathyGameInfo *>(game) != NULL;
}

void TelepathyMultiplayerAdapter::handleIncomingChannel(Tp::IncomingStreamTubeChannelPtr incomingStream) {
    qDebug() << "Incoming connection...";

    if (game() == NULL) {
        incomingStream->requestClose();
        return;
    }

    Tp::PendingStreamTubeConnection *connection = incomingStream->acceptTubeAsUnixSocket();
    connect(connection, SIGNAL(finished(Tp::PendingOperation*)), SLOT(onClientConnectionEstablished(Tp::PendingOperation*)));
}

void TelepathyMultiplayerAdapter::onChannelRequestFinished(Tp::PendingOperation *operation) {
    if (operation->isError()) {
        qWarning() << "Failed to join:" << operation->errorMessage();
        emit joinFailed(operation->errorMessage());
    }
}

void TelepathyMultiplayerAdapter::onClientConnectionEstablished(Tp::PendingOperation *operation) {
    Tp::IncomingStreamTubeChannelPtr incomingStream = Tp::IncomingStreamTubeChannelPtr::dynamicCast(operation->object());

    if (operation->isError()) {
        qWarning() << "Failed to establish stream tube: " << operation->errorMessage();

        incomingStream->requestClose();
        return;
    }

    // Cast to Tp::StreamTubeChannelPtr to work around a bug in Telepathy Qt < 0.7.2
    QString address = Tp::StreamTubeChannelPtr::dynamicCast(incomingStream)->localAddress();
    Q_ASSERT(!address.isEmpty());

    QLocalSocket *socket = new QLocalSocket(this);

    incomingStreamMap[socket] = incomingStream;

    // Attach to all signals manually as using the stateChanged signal to detect socket state changes causes problems
    // when a new connection is established (for some reason the data written by the server process never arrives at
    // the client.
    connect(socket, SIGNAL(connected()), SLOT(onClientConnected()));
    connect(socket, SIGNAL(error(QLocalSocket::LocalSocketError)), SLOT(onClientSocketError(QLocalSocket::LocalSocketError)));
    connect(socket, SIGNAL(disconnected()), SLOT(onClientDisconnected()));

    qDebug() << "Connecting to local socket at: " << address;
    socket->connectToServer(address);
}

void TelepathyMultiplayerAdapter::onClientConnected() {
    QLocalSocket *socket = qobject_cast<QLocalSocket *>(sender());
    Q_ASSERT(socket);

    handleNewRemoteConnection(socket);
}

void TelepathyMultiplayerAdapter::onClientDisconnected() {
    QLocalSocket *socket = qobject_cast<QLocalSocket *>(sender());
    Q_ASSERT(socket);

    Tp::IncomingStreamTubeChannelPtr incomingStream = incomingStreamMap.take(socket);
    if (!incomingStream.isNull())
        incomingStream->requestClose();
}

void TelepathyMultiplayerAdapter::onClientSocketError(QLocalSocket::LocalSocketError error) {
    QLocalSocket *socket = qobject_cast<QLocalSocket *>(sender());
    Q_ASSERT(socket);

    qWarning() << "Socket error on " << socket << socket->errorString();

    Tp::IncomingStreamTubeChannelPtr incomingStream = incomingStreamMap.take(socket);
    if (!incomingStream.isNull())
        incomingStream->requestClose();
}

void TelepathyMultiplayerAdapter::handleOutgoingChannel(Tp::OutgoingStreamTubeChannelPtr outgoingStream) {
    Tp::PendingOperation *op = outgoingStream->offerUnixSocket(m_server, QVariantMap());

    connect(op, SIGNAL(finished(Tp::PendingOperation*)), SLOT(onOfferUnixSocketFinished(Tp::PendingOperation*)));
}

void TelepathyMultiplayerAdapter::onOfferUnixSocketFinished(Tp::PendingOperation *op) {
    Tp::OutgoingStreamTubeChannelPtr outgoingStream = Tp::OutgoingStreamTubeChannelPtr::dynamicCast(op->object());

    if (op->isError()) {
        outgoingStream->requestClose();

        qWarning() << "Failed to establish connection:" << op->errorMessage();

        emit joinFailed(op->errorMessage());
        return;
    }
}

void TelepathyMultiplayerAdapter::onNewConnection() {
    QLocalSocket *socket = (QLocalSocket *) localDevice();

    if (socket)
        socket->deleteLater();

    socket = m_server->nextPendingConnection();
    socket->setParent(this);

    setLocalDevice(socket);
}


QString TelepathyGameInfo::name() const {
    return contact->id();
}

TelepathyGameInfoModel::TelepathyGameInfoModel(Tp::AccountManagerPtr accountManager, TelepathyMultiplayerAdapter *parent) :
    GameInfoModel(parent), accountManager(accountManager) {

    if (!accountManager->isReady())
        connect(accountManager->becomeReady(),
                SIGNAL(finished(Tp::PendingOperation *)),
                SLOT(onAccountManagerReady(Tp::PendingOperation *)));
    else
        buildGameInfoList();
}

void TelepathyGameInfoModel::onAccountManagerReady(Tp::PendingOperation *operation) {
    if (operation->isError()) {
        qWarning() << "Failed to load account manager: " << operation->errorMessage();
        setState(Complete);
        return;
    }

    buildGameInfoList();
}

void TelepathyGameInfoModel::buildGameInfoList() {
    foreach (Tp::AccountPtr account, accountManager->allAccounts()) {
        if (!account->isValid() || account->connection().isNull())
            continue;
        if (!account->connection()->capabilities().streamTubes())
            continue;

        qDebug() << "Got account" << account->nickname();

        Tp::Contacts contacts = account->connection()->contactManager()->allKnownContacts();

        foreach (Tp::ContactPtr contact, contacts.toList()) {
            qDebug() << contact->id() << contact->capabilities().streamTubeServices() << contact->alias();
            if (contact->capabilities().streamTubes("x-sudoku-united-game")) {
                TelepathyGameInfo *gameInfo = new TelepathyGameInfo(this);

                gameInfo->contact = contact;
                gameInfo->account = account;

                appendGameInfo(gameInfo);
            }
        }
    }

    setState(Complete);
}
