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

#ifndef PAINTCOMPONENT_H
#define PAINTCOMPONENT_H

#include "imagedatacache.h"

#include <bb/cascades/CustomControl>
#include <bb/cascades/Layout>

#include <QImage>

class PaintComponent : public bb::cascades::CustomControl
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeListProperty<bb::cascades::Control> controls READ controls FINAL)
    Q_PROPERTY(bb::cascades::Layout * layout READ layout WRITE setLayout RESET resetLayout NOTIFY layoutChanged FINAL)

    Q_CLASSINFO("DefaultProperty", "controls")

public:
    ~PaintComponent();

    QDeclarativeListProperty<bb::cascades::Control> controls() const;

    bb::cascades::Layout *layout() const;
    void setLayout(bb::cascades::Layout *layout);
    void resetLayout();

signals:
    void layoutChanged(bb::cascades::Layout *layout);

protected:
    PaintComponent(ImageDataCache *cache, bb::cascades::Container *parent = 0);

    void requestUpdate(bool instant = false);

    const QRectF &layoutFrame() const;

    virtual QImage paint() = 0;

    virtual ImageDataMetadata::Ptr cacheMetadata() const = 0;

private slots:
    void update();

    void handleLayoutFrameUpdated(const QRectF &layoutFrame);

private:
    bool m_updateRequested;
    bb::cascades::Container *m_content;

    ImageDataCache *m_cache;
    ImageDataCache::Ptr m_cacheEntry;

    mutable QRectF m_layoutFrame;
};

#endif // PAINTCOMPONENT_H
