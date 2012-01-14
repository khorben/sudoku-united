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

#ifndef TELEPATHYHANDLER_H
#define TELEPATHYHANDLER_H

#include <QObject>
#include <TelepathyQt4/AbstractClientHandler>
#include <TelepathyQt4/ClientRegistrar>

namespace Tp {
class PendingOperation;
}

class TelepathyClient;
class TelepathyServer;
class TelepathyHandler;

class TelepathyStreamHandler : public QObject, public Tp::AbstractClientHandler
{
    Q_OBJECT
public:
    TelepathyStreamHandler(TelepathyHandler *handler, const Tp::ChannelClassSpecList &channelFilter);
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
    TelepathyHandler *handler;
};


class TelepathyHandler {
public:
    static TelepathyHandler *instance();

    void setPublishInGame(bool ingame);
    void setClient(TelepathyClient *client);
    void setServer(TelepathyServer *server);
private:
    TelepathyHandler();

    void registerClient();
private:
    static TelepathyHandler *m_instance;
private:
    friend class TelepathyStreamHandler;

    TelepathyClient *client;
    TelepathyServer *server;

    Tp::ClientRegistrarPtr registrar;
    Tp::SharedPtr<TelepathyStreamHandler> handler;

    bool inGame;
};

#endif // TELEPATHYHANDLER_H
