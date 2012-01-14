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

#define PROTOCOL_VERSION 0x1

class QDataStream;

class Message
{
public:
    enum MessageTypeId {
        JoinMessage,
        SetValueMessage,
        GameMessage,
        HelloMessage,
        PlayerMessage
    };
    static Message *parse(QDataStream &dataStream);

    virtual MessageTypeId type() const = 0;

    virtual bool writeStream(QDataStream &dataStream) = 0;
protected:
    Message();

    virtual bool parseStream(QDataStream &dataStream) = 0;

    void writeString(QDataStream &stream, const QString &string);
    QString readString(QDataStream &stream);

    void writeUuid(QDataStream &stream, const QUuid &uuid);
    QUuid readUuid(QDataStream &stream);
};

#endif // MESSAGE_H
