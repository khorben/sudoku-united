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

#include "highscore.h"
#include "player.h"


HighscoreModel::HighscoreModel(QObject *parent) :
    QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;

    roles[PlayTimeRole] = "playTime";
    roles[DifficultyRole] = "difficulty";
    roles[NumberOfPlayersRole] = "numberOfPlayers";
    roles[DifficultyStringRole] = "difficultyString";
    roles[SectionIndexRole] = "sectionIndex";
    roles[PlayerNamesRole] = "playerNames";
    roles[FinishedDateRole] = "finishedDate";

    setRoleNames(roles);
}

QVariant HighscoreModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_highscoreList.size())
        return QVariant();

    switch (role) {
    case PlayTimeRole:
        return QVariant::fromValue(m_highscoreList[index.row()]->playTime());
    case DifficultyRole:
        return QVariant::fromValue((quint32) m_highscoreList[index.row()]->difficulty());
    case NumberOfPlayersRole:
        return QVariant::fromValue(m_highscoreList[index.row()]->numberOfPlayers());
    case DifficultyStringRole:
        switch ((quint32) m_highscoreList[index.row()]->difficulty()) {
        case 0: return QString("Simple");
        case 1: return QString("Easy");
        case 2: return QString("Intermediate");
        case 3: return QString("Hard");
        case 4: return QString("Expert");
        default: return QString("Unknown");
        }
    case FinishedDateRole:
        return QVariant::fromValue(m_highscoreList[index.row()]->finishedDate());
    case PlayerNamesRole:
        QString names = "";
        foreach (Player *player, m_highscoreList[index.row()]->players()) {
            if (names.length() + player->name().length() <= 20) {
                names += player->name() + (player ==  m_highscoreList[index.row()]->players().last() ? "" : ", ");
            } else {
                names += "...";
            }
        }
        return names;
    }

    return QVariant();
}

int HighscoreModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)

    return m_highscoreList.size();
}

void HighscoreModel::addHighscore(QList<Player *> players, quint64 playTime, Sudoku::Difficulty difficulty, QDateTime finsihedDate) {
    // Prune entries if more than MAX_ENTRIES per difficulty are present
    quint32 entriesByDifficulty = 0;
    quint32 index = 0;
    quint64 maxPlayTime = playTime;
    bool wasEmpty = isEmpty();

    for (int i = 0; i < m_highscoreList.size(); i++) {
        HighscoreEntry *he = m_highscoreList[i];
        if (he->difficulty() == difficulty){
            entriesByDifficulty++;
            if (he->playTime() > maxPlayTime) {
                maxPlayTime = he->playTime();
                index = i;
            }
        }
    }
    // nothing to do if list is full and play times are better than new one
    if (maxPlayTime <= playTime && entriesByDifficulty == MAX_ENTRIES) return;

    // Delete entry if playTime < maxPlayTime
    if (maxPlayTime > playTime && entriesByDifficulty >= MAX_ENTRIES) {
        beginRemoveRows(QModelIndex(), index, index);
        m_highscoreList.removeAt(index);
        endRemoveRows();
    }

    beginInsertRows(QModelIndex(), m_highscoreList.size(), m_highscoreList.size());
    m_highscoreList.append(new HighscoreEntry(players, playTime, difficulty, finsihedDate, this));
    endInsertRows();

    if (wasEmpty)
        emit emptyChanged();
}

bool HighscoreModel::isEmpty() const
{
    return m_highscoreList.isEmpty();
}

void HighscoreModel::clear()
{
    bool wasEmpty = isEmpty();

    beginResetModel();
    m_highscoreList.clear();
    endResetModel();

    if (wasEmpty)
        emit emptyChanged();
}

QDataStream &operator<<(QDataStream &stream, HighscoreModel &highscore) {
    // Write encoding version
    stream << quint16(1);

    // Write highscore list
    stream << quint8(highscore.highscores().size());

    foreach (HighscoreEntry *highscoreEntry, highscore.highscores()) {

        stream << highscoreEntry->numberOfPlayers();
        foreach (Player *player, highscoreEntry->players()) {
            stream << player->name();
            stream << player->uuid();
        }

        stream << highscoreEntry->playTime();
        stream << (quint8) highscoreEntry->difficulty();
        stream << highscoreEntry->finishedDate();
    }

    return stream;
}

QDataStream &operator>>(QDataStream &stream, HighscoreModel &highscore) {
    // Read version
    quint16 version;
    stream >> version;

    if (version != 1) {
        stream.setStatus(QDataStream::ReadCorruptData);
        return stream;
    }

    // Read player list
    quint8 highscoreCount;
    stream >> highscoreCount;

    for (quint8 highscoreIndex = 0; highscoreIndex < highscoreCount; highscoreIndex++) {
        QList<Player *> players;
        qint8 numberOfPlayers;
        quint64 playTime;
        quint8 difficulty;
        QDateTime finishedDate;

        stream >> numberOfPlayers;
        for (quint8 playerIndex = 0; playerIndex < numberOfPlayers; playerIndex++) {
            QString name;
            QUuid uuid;

            stream >> name;
            stream >> uuid;

            Player *player = new Player(uuid, name);
            player->setState(Player::Disconnected);
            players.append(player);
        }
        stream >> playTime;
        stream >> difficulty;
        stream >> finishedDate;

        highscore.addHighscore(players, playTime, (Sudoku::Difficulty) difficulty, finishedDate);
    }

    return stream;
}

HighscoreEntry::HighscoreEntry(QObject *parent):
    QObject(parent), m_players(QList<Player *>()), m_playTime(0), m_diffculty(Sudoku::EASY), m_finishedDate(QDateTime()) {
}

HighscoreEntry::HighscoreEntry(const QList<Player *> &players, quint64 playTime, Sudoku::Difficulty difficulty, QDateTime finishedDate, QObject *parent): QObject(parent) {
    foreach (const Player *player, players) {
        m_players.append(new Player(*player, this));
    }

    m_playTime = playTime;
    m_diffculty = difficulty;
    m_finishedDate = finishedDate;
}

HighscoreFilterModel::HighscoreFilterModel(QObject *parent) : QSortFilterProxyModel(parent) {
    connect(this, SIGNAL(modelReset()), SIGNAL(sourceModelChanged()));
}

bool HighscoreFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    quint32 leftValue = (left.model() ? left.model()->data(left, HighscoreModel::DifficultyRole) : QVariant()).toUInt();
    quint32 rightValue = (right.model() ? right.model()->data(right, HighscoreModel::DifficultyRole) : QVariant()).toUInt();

    return (QSortFilterProxyModel::lessThan(left, right) && leftValue <= rightValue) || leftValue < rightValue;
}

QVariant HighscoreFilterModel::data(const QModelIndex &index, int role) const {

    switch (role){
    case HighscoreModel::SectionIndexRole:
    {
        quint8 currentDifficulty = (quint8) QSortFilterProxyModel::data(index, HighscoreModel::DifficultyRole).toUInt();
        int i;
        for (i = index.row() - 1; i >= 0; i--) {
            quint8 difficulty = (quint8) QSortFilterProxyModel::data(index.sibling(i, index.column()), HighscoreModel::DifficultyRole).toUInt();
            if (difficulty < currentDifficulty){
                return index.row() - i;
            }
        }
        return 1 + index.row();
        break;
    }
    default:
        return QSortFilterProxyModel::data(index, role);
    }

}


