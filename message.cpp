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
#include "board.h"
#include "player.h"
#include "multiplayeradapter.h"
#include "game.h"

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

JoinMessage::JoinMessage() {

}

JoinMessage::JoinMessage(QUuid uuid, QString name) :
    m_name(name), m_uuid(uuid) {

}


bool JoinMessage::parseStream(QDataStream &dataStream) {
    m_name = readString(dataStream);
    m_uuid = readUuid(dataStream);

    return dataStream.status() == QDataStream::Ok;
}

bool JoinMessage::writeStream(QDataStream &dataStream) {
    QByteArray buffer;
    QDataStream tempStream(&buffer, QIODevice::WriteOnly);

    quint16 messageType = Message::JoinMessage;

    tempStream << messageType;
    writeString(tempStream, m_name);
    writeUuid(tempStream, m_uuid);

    quint16 length = buffer.size() + 2;

    dataStream << length;
    dataStream.writeRawData(buffer.constData(), buffer.size());

    return dataStream.status() == QDataStream::Ok;
}

GameMessage::GameMessage(Game *game) :
    m_game(game) {

}

GameMessage::GameMessage() :
    m_game(NULL) {

}

GameMessage::~GameMessage() {
    if (m_game != NULL && !m_game->parent())
        delete m_game;
}

bool GameMessage::parseStream(QDataStream &dataStream) {
    if (m_game && !m_game->parent())
        delete m_game;

    m_game = new Game();


    QMap<quint8, Player *> playerIndexMap;
    quint8 playerCount;
    dataStream >> playerCount;

    for (quint8 playerIndex = 0; playerIndex < playerCount; playerIndex++) {
        QUuid uuid = readUuid(dataStream);
        QString name = readString(dataStream);
        quint8 state;

        dataStream >> state;

        Player *player = m_game->addPlayer(uuid, name);
        player->setState((Player::State) state);

        playerIndexMap[playerIndex] = player;
    }

    bool hasBoard;

    dataStream >> hasBoard;

    if (hasBoard) {
        Board *board = new Board();

        for (quint8 y = 0; y < 9; y++) {
            for (quint8 x = 0; x < 9; x++) {
                Cell *cell = board->cellAt(x, y);

                dataStream >> board->m_cellValues[x][y];
                dataStream >> cell->m_fixedCell;

                if (!cell->m_fixedCell && board->m_cellValues[x][y] != 0) {
                    quint8 playerIndex;

                    dataStream >> playerIndex;
                    cell->setValueOwner(playerIndexMap[playerIndex]);
                }
            }
        }

        m_game->setBoard(board);
    }

    return dataStream.status() == QDataStream::Ok;
}

bool GameMessage::writeStream(QDataStream &dataStream) {
    QByteArray buffer;
    QDataStream tempStream(&buffer, QIODevice::WriteOnly);
    QMap<Player *, quint8> playerIndexMap;

    quint16 messageType = Message::GameMessage;

    tempStream << messageType;

    // Write the players which are currently part of the game
    tempStream << quint8(m_game->players().size());

    quint8 playerIndex = 0;
    foreach (Player *player, m_game->players()) {
        writeUuid(tempStream, player->uuid());
        writeString(tempStream, player->name());
        tempStream << quint8(player->state());

        playerIndexMap[player] = playerIndex++;
    }

    if (m_game->board() != NULL) {
        tempStream << true;
        for (quint8 y = 0; y < 9; y++) {
            for (quint8 x = 0; x < 9; x++) {
                Cell *cell = m_game->board()->cellAt(x, y);

                tempStream << cell->value();
                tempStream << cell->isFixedCell();
                if (cell->value() != 0 && !cell->isFixedCell()) {
                    tempStream << playerIndexMap[cell->valueOwner()];
                }
            }
        }
    } else {
        tempStream << false;
    }

    quint16 length = buffer.size() + 2;

    dataStream << length;
    dataStream.writeRawData(buffer.constData(), buffer.size());

    return dataStream.status() == QDataStream::Ok;
}

SetValueMessage::SetValueMessage() {

}

SetValueMessage::SetValueMessage(quint8 x, quint8 y, quint8 value, QUuid valueOwner) :
    m_x(x), m_y(y), m_value(value), m_valueOwner(valueOwner) {

}

bool SetValueMessage::parseStream(QDataStream &dataStream) {
    dataStream >> m_x;
    dataStream >> m_y;
    dataStream >> m_valueOwner;
    dataStream >> m_value;

    return dataStream.status() == QDataStream::Ok;
}

bool SetValueMessage::writeStream(QDataStream &dataStream) {
    QByteArray buffer;
    QDataStream tempStream(&buffer, QIODevice::WriteOnly);

    quint16 messageType = Message::SetValueMessage;

    tempStream << messageType;
    tempStream << m_x;
    tempStream << m_y;
    tempStream << m_valueOwner;
    tempStream << m_value;

    quint16 length = buffer.size() + 2;

    dataStream << length;
    dataStream.writeRawData(buffer.constData(), buffer.size());

    return dataStream.status() == QDataStream::Ok;
}

HelloMessage::HelloMessage() : m_protocolVersion(0) {

}

bool HelloMessage::writeStream(QDataStream &dataStream) {
    QByteArray buffer;
    QDataStream tempStream(&buffer, QIODevice::WriteOnly);

    quint16 messageType = Message::HelloMessage;

    tempStream << messageType;
    tempStream << MultiplayerAdapter::ProtocolVersion;

    quint16 length = buffer.size() + 2;

    dataStream << length;
    dataStream.writeRawData(buffer.constData(), buffer.size());

    return dataStream.status() == QDataStream::Ok;
}

bool HelloMessage::parseStream(QDataStream &dataStream) {
    dataStream >> m_protocolVersion;

    return dataStream.status() == QDataStream::Ok;
}
