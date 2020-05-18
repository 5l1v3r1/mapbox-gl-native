#pragma once

#include <mbgl/layermanager/layer_factory.hpp>

namespace mbgl {

class CircleLayerFactory : public LayerFactory {
protected:
    const style::LayerTypeInfo* getTypeInfo() const noexcept final;
    std::unique_ptr<style::Layer> createLayer(const std::string& id, const style::conversion::Convertible& value) noexcept final;
    std::unique_ptr<Layout> createLayout(const LayoutParameters& parameters,
                                         std::unique_ptr<GeometryTileLayer> tileLayer,
                                         const std::vector<Immutable<style::LayerProperties>>& group) noexcept final;
    std::unique_ptr<RenderLayer> createRenderLayer(Immutable<style::Layer::Impl>) noexcept final;
    style::StyleProperty getPropertyDefaultValue(const std::string& property) const noexcept final;
};

} // namespace mbgl
