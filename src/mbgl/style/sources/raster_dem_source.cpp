#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/sources/raster_dem_source.hpp>
#include <mbgl/style/sources/raster_dem_source_impl.hpp>
#include <mbgl/tile/tile.hpp>
#include <utility>

namespace mbgl {
namespace style {

RasterDEMSource::RasterDEMSource(std::string id, variant<std::string, Tileset> urlOrTileset_, uint16_t tileSize)
    : RasterSource(makeMutable<Impl>(std::move(id), tileSize), std::move(urlOrTileset_)) {}

bool RasterDEMSource::supportsLayerType(const mbgl::style::LayerTypeInfo* info) const {
    return mbgl::underlying_type(TileKind::RasterDEM) == mbgl::underlying_type(info->tileKind);
}

static std::string tilesetDEMEncodingToString(mbgl::Tileset::DEMEncoding encoding) {
    switch (encoding) {
        case mbgl::Tileset::DEMEncoding::Mapbox:
            return "mapbox";
        case mbgl::Tileset::DEMEncoding::Terrarium:
            return "terrarium";
    }
    return "terrarium";
}

Value RasterDEMSource::serialize() const {
    auto value = RasterSource::serialize();
    getURLOrTileset()->match([](const std::string&) {},
                             [&](const mbgl::Tileset& tileset) {
                                 value.getObject()->insert({"encoding", tilesetDEMEncodingToString(tileset.encoding)});
                             });
    return value;
}

const RasterDEMSource::Impl& RasterDEMSource::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<Source::Impl> RasterDEMSource::createMutable() const noexcept {
    return staticMutableCast<Source::Impl>(makeMutable<Impl>(impl()));
}

Mutable<Source::Impl> RasterDEMSource::createMutable(Tileset tileset) const noexcept {
    return staticMutableCast<Source::Impl>(makeMutable<Impl>(impl(), std::move(tileset)));
}

} // namespace style
} // namespace mbgl
