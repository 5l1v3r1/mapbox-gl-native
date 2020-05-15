#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/sources/raster_dem_source.hpp>
#include <mbgl/style/sources/raster_dem_source_impl.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/util/enum.hpp>
#include <utility>

namespace mbgl {

using TilesetDEMEncoding = Tileset::DEMEncoding;
MBGL_DEFINE_ENUM(TilesetDEMEncoding,
                 {{TilesetDEMEncoding::Mapbox, "mapbox"}, {TilesetDEMEncoding::Terrarium, "terrarium"}});

namespace style {

RasterDEMSource::RasterDEMSource(std::string id, variant<std::string, Tileset> urlOrTileset_, uint16_t tileSize)
    : RasterSource(makeMutable<Impl>(std::move(id), tileSize), std::move(urlOrTileset_)) {}

bool RasterDEMSource::supportsLayerType(const mbgl::style::LayerTypeInfo* info) const {
    return mbgl::underlying_type(TileKind::RasterDEM) == mbgl::underlying_type(info->tileKind);
}

Value RasterDEMSource::serialize() const {
    auto value = RasterSource::serialize();
    if (auto* tileset = getTileset()) {
        value.getObject()->insert({"encoding", conversion::makeValue(tileset->encoding)});
    }
    return value;
}

const RasterDEMSource::Impl& RasterDEMSource::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Value RasterDEMSource::getPropertyInternal(const std::string& name) const {
    if (name == "encoding") {
        if (auto* tileset = getTileset()) {
            return conversion::makeValue(tileset->encoding);
        }
    }
    return NullValue();
}

Value RasterDEMSource::getPropertyDefaultValueInternal(const std::string& name) const {
    if (name == "encoding") {
        return conversion::makeValue(TilesetDEMEncoding::Mapbox);
    }
    return NullValue();
}

Mutable<Source::Impl> RasterDEMSource::createMutable() const noexcept {
    return staticMutableCast<Source::Impl>(makeMutable<Impl>(impl()));
}

Mutable<Source::Impl> RasterDEMSource::createMutable(Tileset tileset) const noexcept {
    return staticMutableCast<Source::Impl>(makeMutable<Impl>(impl(), std::move(tileset)));
}

} // namespace style
} // namespace mbgl
