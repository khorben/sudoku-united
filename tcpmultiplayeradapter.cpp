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

#include "tcpmultiplayeradapter.h"
#include "message.h"
#include "player.h"
#include "game.h"
#include "board.h"
#include "sudoku.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>

TCPPlayerInfo::TCPPlayerInfo() :
    state(Connecting) {

}

TCPGameInfo::TCPGameInfo(QObject *parent) :
    GameInfo(parent) {

}

TCPMultiplayerAdapter::TCPMultiplayerAdapter(Sudoku *parent) :
    MultiplayerAdapter(parent), m_server(new QTcpServer(this))
{
    connect(m_server, SIGNAL(newConnection()), SLOT(onNewConnection()));

    QTcpSocket *socket = new QTcpSocket(this);

    m_local = new PlayerInfo();
    m_local->device = socket;

    connect(m_local->device, SIGNAL(connected()), SLOT(onConnectedToServer()));
    connect(m_local->device, SIGNAL(readyRead()), SLOT(onReadyRead()));
    connect(m_local->device, SIGNAL(readChannelFinished()), SLOT(onReadChannelFinished()));
    connect(socket, SIGNAL(disconnected()), SLOT(onLocalDisconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(onLocalError(QAbstractSocket::SocketError)));

    connect(m_sudoku, SIGNAL(gameChanged()), SLOT(onGameChanged()));
}

void TCPMultiplayerAdapter::onLocalDisconnected() {
    if (m_local->state == PlayerInfo::Connecting) {
        m_local->device->close();
        m_local->state = PlayerInfo::Disconnected;

        emit joinFailed("Remote host closed the connection.");
    }
}

void TCPMultiplayerAdapter::onLocalError(QAbstractSocket::SocketError error) {
    if (m_local->state == PlayerInfo::Connecting) {
        QString errorMessage = m_local->device->errorString();
        m_local->device->close();
        m_local->state = PlayerInfo::Disconnected;

        emit joinFailed(errorMessage);
    }
}

GameInfoModel *TCPMultiplayerAdapter::discoverGames() {
    return new TCPGameInfoModel(this);
}

bool TCPMultiplayerAdapter::canJoinGameInfo(GameInfo *game) const {
    return qobject_cast<TCPGameInfo *>(game) != NULL;
}

void TCPMultiplayerAdapter::join(GameInfo *game) {
    TCPGameInfo *gameInfo = qobject_cast<TCPGameInfo *>(game);

    if (gameInfo == NULL) {
        qDebug() << "Unsupported GameInfo parameter.";
        return;
    }

    if (m_local->device->isOpen())
        m_local->device->close();

    m_local->state = PlayerInfo::Connecting;
    ((QTcpSocket *) m_local->device)->connectToHost(gameInfo->address, gameInfo->port);

    qDebug() << "Connecting to " << gameInfo->address.toString() << ":" << gameInfo->port << "...";
}

void TCPMultiplayerAdapter::onGameChanged() {
    if (game() == NULL) {
        if (m_server->isListening())
            m_server->close();
    } else {
        if (!m_server->listen(QHostAddress::Any, 56585))
            qWarning() << "Failed to listen for incoming connections.";
    }
}

void TCPMultiplayerAdapter::onNewConnection() {
    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();

        handleNewConnection(socket);
    }
}

TCPGameInfoModel::TCPGameInfoModel(QObject *parent) :
    GameInfoModel(parent) {

    QTimer::singleShot(1500, this, SLOT(addFakeServer()));

}

void TCPGameInfoModel::addFakeServer() {
    TCPGameInfo *gameInfo = new TCPGameInfo(this);

    gameInfo->address = QHostAddress("127.0.0.1");
    gameInfo->port = 56585;

    appendGameInfo(gameInfo);

    m_state = Complete;

    emit stateChanged();
}
