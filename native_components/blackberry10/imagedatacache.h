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

#ifndef IMAGEDATACACHE_H
#define IMAGEDATACACHE_H

#include <bb/ImageData>
#include <bb/cascades/ImagePaint>

#include <QSharedPointer>

class ImageDataCachePrivate;

class ImageDataMetadata
{
public:
    virtual uint hash() const = 0;

    virtual bool operator ==(const ImageDataMetadata &other) const = 0;

public:
    typedef QSharedPointer<ImageDataMetadata> Ptr;
};

class ImageDataCache
{
public:
    struct CacheEntry {
        bb::cascades::ImagePaint image;
    };

    typedef QSharedPointer<CacheEntry> Ptr;

public:
    virtual ~ImageDataCache();

    ImageDataCache::Ptr lookup(ImageDataMetadata::Ptr metadata);

    void cleanup();

public:
    template <class C>
    static ImageDataCache *instance();

private:
    ImageDataCache();

private:
    Q_DECLARE_PRIVATE(ImageDataCache);
    ImageDataCachePrivate * const d_ptr;
};

template <class C>
ImageDataCache *ImageDataCache::instance()
{
    static ImageDataCache *instance = new ImageDataCache();

    return instance;
}

#endif // IMAGEDATACACHE_H
