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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QUuid>

class QDataStream;
class Board;
class Player;
class Game;

class Message
{
public:
    enum MessageTypeId {
        JoinMessage,
        SetValueMessage,
        GameMessage,
        HelloMessage
    };
    static Message *parse(QDataStream &dataStream);

    virtual MessageTypeId type() const = 0;

    virtual bool writeStream(QDataStream &dataStream) = 0;
protected:
    Message();

    virtual bool parseStream(QDataStream &dataStream) = 0;

    void writeString(QDataStream &stream, const QString &string);
    QString readString(QDataStream &stream);

    void writeUuid(QDataStream &strea, const QUuid &uuid);
    QUuid readUuid(QDataStream &stream);
};

class JoinMessage : public Message {
public:
    JoinMessage();
    JoinMessage(QUuid uuid, QString name);

    const QString &name() const { return m_name; }
    const QUuid &uuid() const { return m_uuid; }

    virtual bool writeStream(QDataStream &dataStream);

    Message::MessageTypeId type() const { return Message::JoinMessage; }
protected:
    virtual bool parseStream(QDataStream &dataStream);
private:
    QString m_name;
    QUuid m_uuid;
};

class SetValueMessage : public Message {
public:
    SetValueMessage();
    SetValueMessage(quint8 x, quint8 y, quint8 value, QUuid valueOwner);

    quint8 x() const { return m_x; }
    quint8 y() const { return m_y; }
    quint8 value() const { return m_value; }
    const QUuid & valueOwner() const { return m_valueOwner; }

    bool writeStream(QDataStream &dataStream);

    Message::MessageTypeId type() const { return Message::SetValueMessage; }
protected:
    bool parseStream(QDataStream &dataStream);
private:
    quint8 m_x;
    quint8 m_y;
    quint8 m_value;
    QUuid m_valueOwner;
};

class GameMessage : public Message {
public:
    GameMessage();
    GameMessage(Game *game);
    ~GameMessage();

    Game *game() const { return m_game; }

    bool writeStream(QDataStream &dataStream);

    Message::MessageTypeId type() const { return Message::GameMessage; }
protected:
    bool parseStream(QDataStream &dataStream);

private:
    Game *m_game;
};

class HelloMessage : public Message {
public:
    HelloMessage();

    bool writeStream(QDataStream &dataStream);

    Message::MessageTypeId type() const { return Message::HelloMessage; }

    quint16 protocolVersion() const { return m_protocolVersion; }
protected:
    bool parseStream(QDataStream &dataStream);

private:
    quint16 m_protocolVersion;
};

#endif // MESSAGE_H
