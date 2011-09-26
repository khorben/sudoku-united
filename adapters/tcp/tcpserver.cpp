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

#include "tcpserver.h"
#include "tcpgameinfo.h"
#include "../serveradapter.h"

#include <QTcpServer>
#include <QTcpSocket>

TCPServer::TCPServer(QObject *parent) :
    AbstractServer(parent)
{
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), SLOT(onClientConnected()));
}

void TCPServer::enable() {
    if (server->isListening())
        return;

    if (!server->listen(QHostAddress::Any, 56585))
        qWarning() << "Failed to listen for incoming connections";
}

void TCPServer::disable() {
    server->close();
}

void TCPServer::onClientConnected() {
    while (server->hasPendingConnections()) {
        QTcpSocket *socket = server->nextPendingConnection();

        serverAdapter()->addClient(socket);
    }
}

GameInfoModel *TCPServer::discoverGames() {
    return new TCPGameInfoModel(this);
}
