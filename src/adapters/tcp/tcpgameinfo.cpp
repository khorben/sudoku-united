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

#include "tcpgameinfo.h"
#include "tcpclient.h"

TCPGameInfo::TCPGameInfo(QObject *parent) :
    GameInfo(parent)
{
}

QString TCPGameInfo::name() const {
    return "TCP Server Stub";
}

AbstractClient *TCPGameInfo::client() const {
    return new TCPClient();
}

bool TCPGameInfo::operator ==(GameInfo &other) const {
    const TCPGameInfo *otherGameInfo =
            qobject_cast<const TCPGameInfo *>(&other);

    if (!otherGameInfo)
        return false;

    return address == otherGameInfo->address && port == otherGameInfo->port;
}

TCPGameInfoModel::TCPGameInfoModel(QObject *parent) :
    GameInfoModel(parent) {

    QTimer::singleShot(1500, this, SLOT(addFakeServer()));

}

void TCPGameInfoModel::addFakeServer() {
    TCPGameInfo *gameInfo = new TCPGameInfo(this);

    gameInfo->address = QHostAddress("127.0.0.1");
    gameInfo->port = 56585;

    insertGameInfo(gameInfo);

    setState(Complete);
}
