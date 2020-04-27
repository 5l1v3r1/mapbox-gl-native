#pragma once

#include <mbgl/style/sources/raster_dem_source.hpp>
#include <mbgl/style/sources/raster_source_impl.hpp>

namespace mbgl {
namespace style {

class RasterDEMSource::Impl : public RasterSource::Impl {
public:
    Impl(std::string id, uint16_t tileSize);
    Impl(const Impl&, Tileset);

    DECLARE_SOURCE_TYPE_INFO;
};

} // namespace style
} // namespace mbgl
