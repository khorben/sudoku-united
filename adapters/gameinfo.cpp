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

#include "gameinfo.h"

GameInfo::GameInfo(QObject *parent) :
    QObject(parent)
{
}

GameInfoModel::GameInfoModel(QObject *parent) :
    QAbstractListModel(parent), m_state(Discovering), m_autoRefresh(true) {

    QHash<int, QByteArray> roles;

    roles[NameRole] = "name";
    roles[PlayerCountRole] = "playerCount";
    roles[GameInfoRole] = "gameInfo";

    setRoleNames(roles);
}

int GameInfoModel::insertGameInfo(GameInfo *gameInfo) {
    Q_ASSERT(gameInfo);
    quint16 rowCount = m_gameInfoList.size();

    for (int i = 0; i < rowCount; i++) {
        GameInfo *other = m_gameInfoList.at(i);

        if (*other == *gameInfo) {
            m_gameInfoList.replace(i, gameInfo);

            QModelIndex index = createIndex(i, 0);
            emit dataChanged(index, index);

            other->deleteLater();
            return i;
        }
    }

    beginInsertRows(QModelIndex(), rowCount, rowCount);

    m_gameInfoList.append(gameInfo);

    endInsertRows();

    return rowCount;
}

void GameInfoModel::setState(State state) {
    if (state == m_state)
        return;

    m_state = state;

    emit stateChanged();
}

QVariant GameInfoModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_gameInfoList.size())
        return QVariant();

    switch (role) {
    case NameRole:
        return QVariant::fromValue(m_gameInfoList[index.row()]->name());
    case GameInfoRole:
        return QVariant::fromValue(m_gameInfoList[index.row()]);
    }

    return QVariant();
}

int GameInfoModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    return m_gameInfoList.size();
}

GameInfo *GameInfoModel::row(int index) const {
    return m_gameInfoList[index];
}

/**
  * Enables or disables auto refresh mode. In auto refresh mode the underlying
  * model will update itself automatically if new games are found during its
  * lifetime.
  */
void GameInfoModel::setAutoRefresh(bool enabled) {
    if (enabled == m_autoRefresh)
        return;

    m_autoRefresh = enabled;
    emit autoRefreshChanged();
}

bool GameInfoModel::autoRefresh() const {
    return m_autoRefresh;
}
