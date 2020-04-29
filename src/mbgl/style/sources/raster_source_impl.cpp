#include <mbgl/style/sources/raster_source_impl.hpp>

namespace mbgl {
namespace style {

RasterSource::Impl::Impl(std::string id_, uint16_t tileSize_) : Source::Impl(std::move(id_)), tileSize(tileSize_) {}

RasterSource::Impl::Impl(const Impl& other, Tileset tileset_)
    : Source::Impl(other), tileset(std::move(tileset_)), tileSize(other.tileSize) {}

uint16_t RasterSource::Impl::getTileSize() const {
    return tileSize;
}

optional<std::string> RasterSource::Impl::getAttribution() const {
    if (!tileset) {
        return {};
    }
    return tileset->attribution;
}

const SourceTypeInfo* RasterSource::Impl::staticTypeInfo() noexcept {
    const static SourceTypeInfo typeInfo{
        "raster", SourceTypeInfo::TilePrefetch::Yes, SourceTypeInfo::TileSet::Yes, TileKind::Raster};
    return &typeInfo;
}

} // namespace style
} // namespace mbgl
