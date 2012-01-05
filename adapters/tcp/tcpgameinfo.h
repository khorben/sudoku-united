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

#ifndef TCPGAMEINFO_H
#define TCPGAMEINFO_H

#include "../gameinfo.h"

class TCPGameInfo : public GameInfo
{
    Q_OBJECT
public:
    explicit TCPGameInfo(QObject *parent = 0);

    QHostAddress address;
    quint16 port;

    QString name() const;
    AbstractClient *client() const;

    bool operator ==(GameInfo &other) const;
};

class TCPGameInfoModel : public GameInfoModel {
    Q_OBJECT
public:
    TCPGameInfoModel(QObject *parent);

protected slots:
    void addFakeServer();
};

#endif // TCPGAMEINFO_H
