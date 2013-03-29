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

#include "imagedatacache.h"

#include <QHash>

class ImageDataCachePrivate {
public:
    ImageDataCachePrivate(ImageDataCache *q);

    ImageDataCache::Ptr lookup(ImageDataMetadata::Ptr metadata);

    void cleanup();

private:
    typedef QWeakPointer<ImageDataCache::CacheEntry> WeakPtr;

private:
    QHash<ImageDataMetadata::Ptr, ImageDataCachePrivate::WeakPtr> cache;

private:
    Q_DECLARE_PUBLIC(ImageDataCache);
    ImageDataCache * const q_ptr;
};

ImageDataCache::~ImageDataCache()
{
    delete d_ptr;
}

ImageDataCache::Ptr ImageDataCache::lookup(ImageDataMetadata::Ptr metadata)
{
    Q_D(ImageDataCache);

    return d->lookup(metadata);
}

void ImageDataCache::cleanup()
{
    Q_D(ImageDataCache);

    return d->cleanup();
}

ImageDataCache::ImageDataCache() :
    d_ptr(new ImageDataCachePrivate(this))
{
}

ImageDataCachePrivate::ImageDataCachePrivate(ImageDataCache *q) :
    q_ptr(q)
{
}

ImageDataCache::Ptr ImageDataCachePrivate::lookup(ImageDataMetadata::Ptr metadata)
{
    if (cache.contains(metadata)) {
        WeakPtr weakPtr = cache[metadata];
        ImageDataCache::Ptr ptr = weakPtr.toStrongRef();

        if (ptr.isNull()) {
            cache.remove(metadata);
        } else {
            return ptr;
        }
    }

    ImageDataCache::Ptr ptr(new ImageDataCache::CacheEntry());

    cache.insert(metadata, ptr.toWeakRef());

    return ptr;
}

void ImageDataCachePrivate::cleanup()
{
    QMutableHashIterator<ImageDataMetadata::Ptr, ImageDataCachePrivate::WeakPtr> it(cache);

    while (it.hasNext()) {
        it.next();

        if (it.value().toStrongRef().isNull())
            it.remove();
    }
}

uint qHash(const ImageDataMetadata::Ptr &meta)
{
    return meta->hash();
}

bool operator ==(const ImageDataMetadata::Ptr &p1, const ImageDataMetadata::Ptr &p2) {
    return (p1.isNull() && p2.isNull()) || (p1 && p2 && *p1 == *p2);
}
