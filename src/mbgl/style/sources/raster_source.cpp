#include <mbgl/storage/file_source.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/sources/raster_source_impl.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/util/async_request.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/mapbox.hpp>

namespace mbgl {
namespace style {

RasterSource::RasterSource(std::string id, variant<std::string, Tileset> urlOrTileset_, uint16_t tileSize)
    : Source(makeMutable<Impl>(std::move(id), tileSize)), urlOrTileset(std::move(urlOrTileset_)) {}

RasterSource::RasterSource(Immutable<Impl> impl, variant<std::string, Tileset> urlOrTileset_)
    : Source(std::move(impl)), urlOrTileset(std::move(urlOrTileset_)) {}

RasterSource::~RasterSource() = default;

const RasterSource::Impl& RasterSource::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

const variant<std::string, Tileset>* RasterSource::getURLOrTileset() const {
    return &urlOrTileset;
}

optional<std::string> RasterSource::getURL() const {
    if (urlOrTileset.is<Tileset>()) {
        return nullopt;
    }

    return urlOrTileset.get<std::string>();
}

void RasterSource::loadDescription(FileSource& fileSource) {
    if (urlOrTileset.is<Tileset>()) {
        baseImpl = createMutable(urlOrTileset.get<Tileset>());
        loaded = true;
        observer->onSourceLoaded(*this);
        return;
    }

    if (req) {
        return;
    }

    const auto& url = urlOrTileset.get<std::string>();
    req = fileSource.request(Resource::source(url), [this, url](const Response& res) {
        if (res.error) {
            observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error("unexpectedly empty TileJSON")));
        } else {
            conversion::Error error;
            optional<Tileset> tileset = conversion::convertJSON<Tileset>(*res.data, error);
            if (!tileset) {
                observer->onSourceError(*this, std::make_exception_ptr(util::StyleParseException(error.message)));
                return;
            }

            util::mapbox::canonicalizeTileset(*tileset, url, *this);
            bool changed = impl().tileset != *tileset;

            baseImpl = createMutable(*tileset);
            loaded = true;

            observer->onSourceLoaded(*this);

            if (changed) {
                observer->onSourceChanged(*this);
            }
        }
    });
}

bool RasterSource::supportsLayerType(const mbgl::style::LayerTypeInfo* info) const {
    return mbgl::underlying_type(TileKind::Raster) == mbgl::underlying_type(info->tileKind);
}

Mutable<Source::Impl> RasterSource::createMutable() const noexcept {
    return staticMutableCast<Source::Impl>(makeMutable<Impl>(impl()));
}

Mutable<Source::Impl> RasterSource::createMutable(Tileset tileset) const noexcept {
    return staticMutableCast<Source::Impl>(makeMutable<Impl>(impl(), std::move(tileset)));
}

Value RasterSource::serialize() const {
    auto value = Source::serialize();
    serializeUrlOrTileSet(value, getURLOrTileset());
    return value;
}

} // namespace style
} // namespace mbgl
