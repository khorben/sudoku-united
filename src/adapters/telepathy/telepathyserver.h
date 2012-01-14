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

#ifndef TELEPATHYSERVER_H
#define TELEPATHYSERVER_H

#include "../abstractserver.h"
#include <TelepathyQt4/IncomingStreamTubeChannel>

class QTcpSocket;

class TelepathyServer : public AbstractServer
{
    Q_OBJECT
public:
    explicit TelepathyServer(QObject *parent = 0);

    GameInfoModel *discoverGames();

    void enable();
    void disable();
signals:

public slots:

private slots:
    void onConnectionEstablished(Tp::PendingOperation *operation);
    void onSocketConnected();
    void onSocketError(QAbstractSocket::SocketError error);
    void onClientRemoved();
private:
    friend class TelepathyStreamHandler;

    void handleIncomingStream(Tp::IncomingStreamTubeChannelPtr incomingStream);

    void removeClient(QTcpSocket *device);
private:
    QMap<QTcpSocket *, Tp::IncomingStreamTubeChannelPtr> streamMap;
};

#endif // TELEPATHYSERVER_H
