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

#ifndef NOTEMODEL_H
#define NOTEMODEL_H

#include <QAbstractListModel>
#include <QBitArray>

class Note;
class Game;

class NoteModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum NoteModelType {
        ValueType = Qt::UserRole + 1,
        MarkedType
    };

public:
    explicit NoteModel(QObject *parent = 0);
    
    int rowCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    Q_INVOKABLE
    QObject *get(int index);
private:
    friend QDataStream &operator<<(QDataStream &stream, Game &game);
    friend QDataStream &readGameV2(QDataStream &stream, Game &game);

    QBitArray notes;
};

class Note : public QObject {
    Q_OBJECT
    Q_PROPERTY(int modelValue READ value CONSTANT)
    Q_PROPERTY(bool modelMarked READ marked WRITE setMarked NOTIFY markedChanged)
public:
    explicit Note(NoteModel *model, int value);

    int value() const;

    bool marked() const;
    void setMarked(bool marked);
signals:
    void markedChanged();
private slots:
    void onDataChanged(const QModelIndex &topLeft,
                       const QModelIndex &bottomRight);
private:
    NoteModel *m_model;
    int m_value;
};

#endif // NOTEMODEL_H
