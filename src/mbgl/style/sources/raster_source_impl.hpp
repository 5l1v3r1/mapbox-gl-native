#pragma once

#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/sources/raster_source.hpp>

namespace mbgl {
namespace style {

class RasterSource::Impl : public Source::Impl {
public:
    Impl(std::string id, uint16_t tileSize);
    Impl(const Impl&, Tileset);

    uint16_t getTileSize() const override;

    optional<std::string> getAttribution() const final;

    const optional<Tileset> tileset;

    DECLARE_SOURCE_TYPE_INFO;

private:
    uint16_t tileSize;
};

} // namespace style
} // namespace mbgl
