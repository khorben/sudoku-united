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

#include "gamemessage.h"

#include "../board.h"
#include "../game.h"
#include "../player.h"

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
        if (!player)
            break;

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

                    if (playerIndex != 0xff)
                        cell->setValueOwner(playerIndexMap[playerIndex]);
                }
            }
        }

        for (quint8 y = 0; y < 9; y++) {
            for (quint8 x = 0; x < 9; x++) {
                dataStream >> board->m_solution[x][y];
            }
        }

        // Read start time
        quint64 elapsedTime = 0;
        dataStream >> elapsedTime;
        board->m_elapsedTime = elapsedTime;

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
                    if (cell->valueOwner())
                        tempStream << playerIndexMap[cell->valueOwner()];
                    else
                        tempStream << quint8(0xff);
                }
            }
        }
        for (quint8 y = 0; y < 9; y++) {
            for (quint8 x = 0; x < 9; x++) {
                tempStream << m_game->board()->solutionValue(x, y);
            }
        }

        // Elapsed time in seconds
        tempStream << m_game->board()->elapsedTime();
    } else {
        tempStream << false;
    }

    quint16 length = buffer.size() + 2;

    dataStream << length;
    dataStream.writeRawData(buffer.constData(), buffer.size());

    return dataStream.status() == QDataStream::Ok;
}
