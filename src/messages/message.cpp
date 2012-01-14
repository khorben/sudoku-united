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

#include "message.h"
#include "gamemessage.h"
#include "hellomessage.h"
#include "joinmessage.h"
#include "playermessage.h"
#include "setvaluemessage.h"

#include <QDataStream>
#include <QDebug>

Message::Message()
{
}

Message *Message::parse(QDataStream &dataStream) {
    QIODevice *device = dataStream.device();
    qint64 pos = device->pos();
    quint16 messageLength;
    quint16 messageType;

    dataStream >> messageLength >> messageType;

    if (dataStream.status() != QDataStream::Ok || pos + messageLength > device->size()) {
        qWarning() << "Not enough data in buffer to read whole message (length is" << device->size() << messageLength << pos << " bytes).";

        // Reset to start position and retry when there is more data
        device->seek(pos);
        return NULL;
    }

    Message *message = NULL;

    switch (messageType) {
    case Message::JoinMessage:
        message = new ::JoinMessage();
        break;
    case Message::GameMessage:
        message = new ::GameMessage();
        break;
    case Message::SetValueMessage:
        message = new ::SetValueMessage();
        break;
    case Message::HelloMessage:
        message = new ::HelloMessage();
        break;
    case Message::PlayerMessage:
        message = new ::PlayerMessage();
        break;
    }

    if (message == NULL || !message->parseStream(dataStream)) {
        qWarning() << "Unsupported message type or corrupt message.";
        device->seek(pos + messageLength);

        if (message != NULL)
            delete message;

        return parse(dataStream);
    } else {
        return message;
    }
}

void Message::writeString(QDataStream &stream, const QString &string) {
    QByteArray utf8String = string.toUtf8();

    // Add the length to make it easier for other platforms to read it
    stream << quint16(utf8String.size());
    stream.writeRawData(utf8String.constData(), string.size());
}

QString Message::readString(QDataStream &stream) {
    quint16 length;

    stream >> length;

    QByteArray stringData(length, 0);
    stream.readRawData(stringData.data(), length);

    return QString::fromUtf8(stringData.constData(), length);
}

void Message::writeUuid(QDataStream &stream, const QUuid &uuid) {
    stream << uuid;
}

QUuid Message::readUuid(QDataStream &stream) {
    QUuid uuid;

    stream >> uuid;

    return uuid;
}
