#include <mbgl/style/sources/raster_dem_source_impl.hpp>

namespace mbgl {
namespace style {

RasterDEMSource::Impl::Impl(std::string id_, uint16_t tileSize_) : RasterSource::Impl(std::move(id_), tileSize_) {}

RasterDEMSource::Impl::Impl(const Impl& other, Tileset tileset_) : RasterSource::Impl(other, std::move(tileset_)) {}

const SourceTypeInfo* RasterDEMSource::Impl::staticTypeInfo() noexcept {
    const static SourceTypeInfo typeInfo{"raster-dem", true, TileKind::RasterDEM};
    return &typeInfo;
}

} // namespace style
} // namespace mbgl
