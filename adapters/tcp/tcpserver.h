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

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "../abstractserver.h"

class QTcpServer;

class TCPServer : public AbstractServer
{
    Q_OBJECT
public:
    explicit TCPServer(QObject *parent = 0);

    GameInfoModel *discoverGames();

    void enable();

    void disable();

private slots:
    void onClientConnected();
private:
    QTcpServer *server;

};

#endif // TCPSERVER_H
