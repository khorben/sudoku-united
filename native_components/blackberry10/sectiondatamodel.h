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

#ifndef SECTIONDATAMODEL_H
#define SECTIONDATAMODEL_H

#include <QAbstractItemModel>

class SectionDataModel : public QAbstractItemModel
{
    Q_OBJECT

    Q_PROPERTY(QAbstractItemModel * model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QString sectionKey READ sectionKey WRITE setSectionKey NOTIFY sectionKeyChanged)

public:
    explicit SectionDataModel(QObject *parent = 0);

    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;

    QModelIndex parent(const QModelIndex &child) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;

    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *model);

    QString sectionKey() const;
    void setSectionKey(const QString &sectionKey);

private slots:
    void buildSectionMap();

    void handleRowsInserted(const QModelIndex &parent, int start, int end);
    void handleRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &newParent, int newStart);
    void handleRowsRemoved(const QModelIndex &parent, int start, int end);
    void handleDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

signals:
    void modelChanged();

    void sectionKeyChanged();

private:
    struct SectionEntries {
        QVariant section;
        QList<QModelIndex> entries;
    };
private:
    QAbstractItemModel *m_model;
    QString m_sectionKey;
    int m_sectionRole;
    QList<SectionEntries> m_sectionMap;

};

#endif // SECTIONDATAMODEL_H
