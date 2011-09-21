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

#ifndef TELEPATHYMULTIPLAYERADAPTER_H
#define TELEPATHYMULTIPLAYERADAPTER_H

#include "multiplayeradapter.h"

#include <TelepathyQt4/ClientRegistrar>
#include <TelepathyQt4/AccountManager>
#include <TelepathyQt4/AbstractClient>

#include <TelepathyQt4/AbstractClientHandler>
#include <TelepathyQt4/Channel>
#include <TelepathyQt4/ClientRegistrar>
#include <TelepathyQt4/ChannelClassSpec>
#include <TelepathyQt4/IncomingStreamTubeChannel>
#include <TelepathyQt4/OutgoingStreamTubeChannel>
#include <TelepathyQt4/ContactManager>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/ContactCapabilities>

class TelepathyGameInfo : public GameInfo {
    Q_OBJECT
public:
    TelepathyGameInfo(QObject *parent = 0) : GameInfo(parent) {}
    QString name() const;
    quint16 playerCount() const { return 0; }

    Tp::AccountPtr account;
    Tp::ContactPtr contact;
};

class TelepathyMultiplayerAdapter : public MultiplayerAdapter
{
    Q_OBJECT
public:
    explicit TelepathyMultiplayerAdapter(Sudoku *parent);

    GameInfoModel *discoverGames();

    void join(GameInfo *game);

    bool canJoinGameInfo(GameInfo *game) const;
signals:

public slots:
private:
    friend class TelepathyStreamHandler;

    void handleIncomingChannel(Tp::IncomingStreamTubeChannelPtr incomingStream);
    void handleOutgoingChannel(Tp::OutgoingStreamTubeChannelPtr outgoingStream);
    void registerChannels();
private slots:
    void onGamesChanged();
    void onClientConnectionEstablished(Tp::PendingOperation *operation);
    void onChannelRequestFinished(Tp::PendingOperation *operation);
    void onNewConnection();
    void onOfferUnixSocketFinished(Tp::PendingOperation *op);
    void onClientConnected();
    void onClientDisconnected();
    void onClientSocketError(QLocalSocket::LocalSocketError error);
private:
    friend class TelepathyGameInfoModel;

    Tp::ClientRegistrarPtr registrar;
    Tp::AbstractClientPtr handler;
    Tp::AccountManagerPtr accountManager;
    Tp::OutgoingStreamTubeChannelPtr outgoingStream;
    QMap<QLocalSocket *, Tp::IncomingStreamTubeChannelPtr> incomingStreamMap;
    QLocalServer *m_server;
};

class TelepathyGameInfoModel : public GameInfoModel {
    Q_OBJECT
public:
    TelepathyGameInfoModel(Tp::AccountManagerPtr accountManager, TelepathyMultiplayerAdapter *parent);

private:
    void buildGameInfoList();
private slots:
    void onAccountManagerReady(Tp::PendingOperation *operation);
private:
    Tp::AccountManagerPtr accountManager;
};

class TelepathyStreamHandler : public QObject, public Tp::AbstractClientHandler
{
    Q_OBJECT
public:
    TelepathyStreamHandler(TelepathyMultiplayerAdapter *multiplayerAdapter, const Tp::ChannelClassSpecList &channelFilter);
    ~TelepathyStreamHandler() { }

    bool bypassApproval() const;
    void handleChannels(const Tp::MethodInvocationContextPtr<> &context,
                        const Tp::AccountPtr &account,
                        const Tp::ConnectionPtr &connection,
                        const QList<Tp::ChannelPtr> &channels,
                        const QList<Tp::ChannelRequestPtr> &requestsSatisfied,
                        const QDateTime &userActionTime,
                        const Tp::AbstractClientHandler::HandlerInfo &handlerInfo);

private slots:
    void onChannelReady(Tp::PendingOperation *operation);

private:
    TelepathyMultiplayerAdapter *multiplayerAdapter;
};

#endif // TELEPATHYMULTIPLAYERADAPTER_H
