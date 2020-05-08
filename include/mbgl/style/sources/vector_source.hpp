#pragma once

#include <mbgl/style/sources/tileset_source.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {

class AsyncRequest;

namespace style {

class VectorSource final : public TilesetSource {
public:
    VectorSource(std::string id, variant<std::string, Tileset> urlOrTileset, optional<float> maxZoom = nullopt,
                 optional<float> minZoom = nullopt);
    ~VectorSource() final;

    class Impl;
    const Impl& impl() const;

    void loadDescription(FileSource&) final;

    bool supportsLayerType(const mbgl::style::LayerTypeInfo*) const override;

    mapbox::base::WeakPtr<Source> makeWeakPtr() override {
        return weakFactory.makeWeakPtr();
    }

protected:
    Mutable<Source::Impl> createMutable() const noexcept final;

private:
    std::unique_ptr<AsyncRequest> req;
    optional<float> maxZoom;
    optional<float> minZoom;
    mapbox::base::WeakPtrFactory<Source> weakFactory{this};
};

} // namespace style
} // namespace mbgl
