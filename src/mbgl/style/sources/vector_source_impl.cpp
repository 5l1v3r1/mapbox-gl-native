#include <mbgl/style/sources/vector_source_impl.hpp>

namespace mbgl {
namespace style {

VectorSource::Impl::Impl(std::string id_) : Source::Impl(std::move(id_)) {}

VectorSource::Impl::Impl(const Impl& other, Tileset tileset_) : Source::Impl(other), tileset(std::move(tileset_)) {}

optional<std::string> VectorSource::Impl::getAttribution() const {
    if (!tileset) {
        return {};
    }
    return tileset->attribution;
}

const SourceTypeInfo* VectorSource::Impl::staticTypeInfo() noexcept {
    const static SourceTypeInfo typeInfo{
        "vector", SourceTypeInfo::TilePrefetch::Yes, SourceTypeInfo::TileSet::Yes, TileKind::Geometry};
    return &typeInfo;
}

} // namespace style
} // namespace mbgl
