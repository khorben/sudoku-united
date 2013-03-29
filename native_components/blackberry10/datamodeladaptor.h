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

#ifndef DATAMODELADAPTOR_H
#define DATAMODELADAPTOR_H

#include <bb/cascades/DataModel>

#include <QAbstractItemModel>

class DataModelAdaptor : public bb::cascades::DataModel
{
    Q_OBJECT

    Q_PROPERTY(QAbstractItemModel * model READ model WRITE setModel NOTIFY modelChanged)

public:
    DataModelAdaptor(QObject *parent = 0);

    int childCount(const QVariantList &indexPath);

    bool hasChildren(const QVariantList &indexPath);

    QVariant data(const QVariantList &indexPath);

    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *model);

signals:
    void modelChanged();

private slots:
    void relayDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void relayRowsInserted(const QModelIndex & parent, int start, int end);
    void relayRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);
    void relayRowsRemoved(const QModelIndex &parent, int start, int end);
    void relayModelReset();

private:
    QModelIndex toModelIndex(const QVariantList &indexPath) const;
    QVariantList toIndexPath(const QModelIndex &index) const;

private:
    QAbstractItemModel *m_model;

};

Q_DECLARE_METATYPE(QAbstractItemModel *);
Q_DECLARE_METATYPE(DataModelAdaptor *);

#endif // DATAMODELADAPTOR_H
