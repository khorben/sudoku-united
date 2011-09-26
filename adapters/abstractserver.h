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

#ifndef ABSTRACTSERVERADAPTER_H
#define ABSTRACTSERVERADAPTER_H

#include <QObject>
#include <QAbstractListModel>

class ServerAdapter;
class AbstractClient;
class GameInfoModel;

class AbstractServer : public QObject
{
    Q_OBJECT
public:
    explicit AbstractServer(QObject *parent = 0);

    /**
      * This method is called to return a list of available games for
      * this server adapter.
      */
    virtual GameInfoModel *discoverGames() = 0;

    /**
      * This method is invoked if the server should accept new connections.
      */
    virtual void enable() {}

    /**
      * This method is invoked if the server should stop accepting new
      * connections.
      */
    virtual void disable() {}
protected:
    ServerAdapter *serverAdapter() const;
private:
    friend class ServerAdapter;

    void setServerAdapter(ServerAdapter *serverAdapter);
private:
    ServerAdapter *m_serverAdapter;

};

#endif // ABSTRACTSERVERADAPTER_H
