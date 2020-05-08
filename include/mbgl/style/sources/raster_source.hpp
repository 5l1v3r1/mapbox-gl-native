#pragma once

#include <mbgl/style/sources/tileset_source.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {

class AsyncRequest;

namespace style {

class RasterSource : public TilesetSource {
public:
    RasterSource(std::string id, variant<std::string, Tileset> urlOrTileset, uint16_t tileSize);
    ~RasterSource() override;

    class Impl;
    const Impl& impl() const;

    void loadDescription(FileSource&) final;

    bool supportsLayerType(const mbgl::style::LayerTypeInfo*) const override;

    mapbox::base::WeakPtr<Source> makeWeakPtr() final {
        return weakFactory.makeWeakPtr();
    }

protected:
    RasterSource(Immutable<Impl>, variant<std::string, Tileset> urlOrTileset_);

    Mutable<Source::Impl> createMutable() const noexcept override;
    virtual Mutable<Source::Impl> createMutable(Tileset tileset) const noexcept;

private:
    std::unique_ptr<AsyncRequest> req;
    mapbox::base::WeakPtrFactory<Source> weakFactory {this};
};

} // namespace style
} // namespace mbgl
