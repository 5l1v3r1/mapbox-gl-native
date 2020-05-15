#pragma once

#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {

class AsyncRequest;

namespace style {

class RasterDEMSource : public RasterSource {
public:
    RasterDEMSource(std::string id, variant<std::string, Tileset> urlOrTileset, uint16_t tileSize);
    bool supportsLayerType(const mbgl::style::LayerTypeInfo*) const override;

    class Impl;
    const Impl& impl() const;

    Value serialize() const override;

protected:
    Value getPropertyInternal(const std::string&) const override;
    Value getPropertyDefaultValueInternal(const std::string&) const override;
    Mutable<Source::Impl> createMutable() const noexcept final;
    Mutable<Source::Impl> createMutable(Tileset tileset) const noexcept final;
};

} // namespace style
} // namespace mbgl
