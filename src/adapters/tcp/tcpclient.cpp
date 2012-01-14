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

#include "tcpclient.h"
#include "tcpgameinfo.h"

TCPClient::TCPClient(QObject *parent) :
    AbstractClient(parent), socket(NULL)
{
    socket = new QTcpSocket();

    connect(socket, SIGNAL(connected()), SLOT(onConnected()));
    connect(socket,
            SIGNAL(error(QAbstractSocket::SocketError)),
            SLOT(onError(QAbstractSocket::SocketError)));

    setDevice(socket);
}

void TCPClient::join(GameInfo *info) {
    TCPGameInfo *gameInfo = qobject_cast<TCPGameInfo *>(info);

    if (gameInfo == NULL) {
        qDebug() << "Unsupported GameInfo parameter.";
        return;
    }

    setState(Connecting);

    socket->connectToHost(gameInfo->address, gameInfo->port);

    qDebug() << "Connecting to " << gameInfo->address.toString() << ":" << gameInfo->port << "...";
}


void TCPClient::leave() {
    socket->close();
    setState(Disconnected);
}

void TCPClient::onConnected() {
    startTimeoutTimer();
}

void TCPClient::onError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error);

    setError(socket->errorString());

    leave();
}
