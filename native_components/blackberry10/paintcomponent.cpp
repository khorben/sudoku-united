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

#include "paintcomponent.h"

#include <bb/cascades/Container>
#include <bb/cascades/Image>
#include <bb/cascades/ImagePaint>
#include <bb/cascades/LayoutUpdateHandler>
#include <bb/PixelFormat>

#include <QTimer>

void PaintComponent::requestUpdate(bool instant)
{
    m_updateRequested = true;
    instant = true;
    if (instant)
        update();
    else
        QTimer::singleShot(0, this, SLOT(update()));
}

const QRectF &PaintComponent::layoutFrame() const
{
    if (m_layoutFrame.isEmpty() && isPreferredWidthSet() && isPreferredHeightSet())
        m_layoutFrame = QRectF(0, 0, preferredWidth(), preferredHeight());

    return m_layoutFrame;
}

PaintComponent::PaintComponent(ImageDataCache *cache, bb::cascades::Container *parent) :
    bb::cascades::CustomControl(parent),
    m_updateRequested(false),
    m_cache(cache)
{
    bb::cascades::LayoutUpdateHandler *handler
            = bb::cascades::LayoutUpdateHandler::create(this).onLayoutFrameChanged(this, SLOT(handleLayoutFrameUpdated(QRectF)));
    m_layoutFrame = handler->layoutFrame();

    m_content = bb::cascades::Container::create();
    connect(m_content, SIGNAL(layoutChanged(bb::cascades::Layout*)), SIGNAL(layoutChanged(bb::cascades::Layout*)));
    setRoot(m_content);
}

void PaintComponent::update()
{
    if (!m_updateRequested)
        return;

    bool hadCacheEntry = !m_cacheEntry.isNull();

    ImageDataMetadata::Ptr metadata = cacheMetadata();
    m_cacheEntry = m_cache->lookup(metadata);

    if (m_cacheEntry->image.isNull()) {
        QImage image = paint();

        if (image.isNull())
            return;

        image = image.rgbSwapped();

        m_cacheEntry->image = bb::cascades::ImagePaint(bb::cascades::Image(bb::ImageData::fromPixels(image.constBits(), bb::PixelFormat::RGBA_Premultiplied, image.width(), image.height(), image.bytesPerLine())));
    }

    m_content->resetBackground();
    m_content->setBackground(m_cacheEntry->image);

    if (hadCacheEntry)
        m_cache->cleanup();
}

void PaintComponent::handleLayoutFrameUpdated(const QRectF &layoutFrame)
{
    m_layoutFrame = layoutFrame;
    requestUpdate(true);
}

PaintComponent::~PaintComponent()
{
    delete m_content;
    m_cache->cleanup();
}

QDeclarativeListProperty<bb::cascades::Control> PaintComponent::controls() const
{
    return m_content->property("controls").value<QDeclarativeListProperty<bb::cascades::Control> >();
}

bb::cascades::Layout *PaintComponent::layout() const
{
    return m_content->layout();
}

void PaintComponent::setLayout(bb::cascades::Layout *layout)
{
    m_content->setLayout(layout);
}

void PaintComponent::resetLayout()
{
    m_content->resetLayout();
}
