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

#ifndef ABSTRACTCLIENT_H
#define ABSTRACTCLIENT_H

#include <QObject>
#include <QTimer>

class Game;
class Board;
class Cell;
class GameInfo;

class QIODevice;

class Message;
class SetValueMessage;
class HelloMessage;
class GameMessage;

class AbstractClient : public QObject
{
    Q_OBJECT
public:
    enum State {
        Connecting,
        Connected,
        Disconnected
    };

public:
    explicit AbstractClient(QObject *parent = 0);

    virtual void join(GameInfo *gameInfo) = 0;
    virtual void leave() = 0;

    QString error() const;

    Game *game() const;

    State state() const;
signals:
    void stateChanged(AbstractClient::State state);

protected:
    void startTimeoutTimer(quint32 timeoutInMilliseconds = 60000);
    void stopTimeoutTimer();

    virtual void setState(State state);
    void setError(const QString &error);

    QIODevice *device() const;
    void setDevice(QIODevice *device);
private slots:
    void onReadyRead();
    void onReadChannelFinished();

    void onBoardChanged();
    void onCellValueChanged(Cell *cell);

    void onTimeout();
private:
    void parseMessages();
    void sendMessage(Message &message);

    void handleSetValueMessage(SetValueMessage *message);
    void handleHelloMessage(HelloMessage *message);
    void handleGameMessage(GameMessage *message);
private:
    State m_state;
    QString m_error;
    QIODevice *m_device;
    Game *m_game;
    Board *m_board;
    QByteArray buffer;
    QTimer timeoutTimer;

};

#endif // ABSTRACTCLIENT_H
