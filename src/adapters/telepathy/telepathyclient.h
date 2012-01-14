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

#ifndef TELEPATHYCLIENT_H
#define TELEPATHYCLIENT_H

#include "../abstractclient.h"
#include <TelepathyQt4/AbstractClientHandler>
#include <TelepathyQt4/ClientRegistrar>
#include <TelepathyQt4/OutgoingStreamTubeChannel>

namespace Tp {
class PendingOperation;
}

class QTcpServer;

class TelepathyClient : public AbstractClient
{
    Q_OBJECT
public:
    explicit TelepathyClient(QObject *parent = 0);

    void join(GameInfo *gameInfo);
    void leave();
signals:

protected:
    void setState(State state);
private slots:
    void onCreateTubeFinished(Tp::PendingOperation *op);
    void onNewConnection();
    void onConnectionClosed(uint connectionId,
                            const QString &errorName,
                            const QString &errorMessage);
    void onContactPresenceChanged(const Tp::Presence &presence);
    void onOfferServerFinished(Tp::PendingOperation *operation);
private:
    friend class TelepathyStreamHandler;

    void handleOutgoingStream(Tp::OutgoingStreamTubeChannelPtr outgoingStream);
private:
    QTcpServer *server;

    Tp::OutgoingStreamTubeChannelPtr outgoingStream;
    Tp::ContactPtr remoteContact;
};

#endif // TELEPATHYCLIENT_H
