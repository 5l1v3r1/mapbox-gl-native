#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>
#include <mbgl/tile/tile.hpp>

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/util/logging.hpp>

#include <mapbox/eternal.hpp>

namespace mbgl {
namespace style {

static_assert(mbgl::underlying_type(TileKind::Geometry) == mbgl::underlying_type(LayerTypeInfo::TileKind::Geometry),
              "tile kind error");
static_assert(mbgl::underlying_type(TileKind::Raster) == mbgl::underlying_type(LayerTypeInfo::TileKind::Raster),
              "tile kind error");
static_assert(mbgl::underlying_type(TileKind::RasterDEM) == mbgl::underlying_type(LayerTypeInfo::TileKind::RasterDEM),
              "tile kind error");

static LayerObserver nullObserver;

namespace {

enum class Property : uint8_t { Visibility = 0u, Minzoom, Maxzoom, Filter, SourceLayer, Source, Type };

constexpr const auto layerProperties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>(
    {{"visibility", underlying_type(Property::Visibility)},
     {"minzoom", underlying_type(Property::Minzoom)},
     {"maxzoom", underlying_type(Property::Maxzoom)},
     {"filter", underlying_type(Property::Filter)},
     {"source-layer", underlying_type(Property::SourceLayer)},
     {"source", underlying_type(Property::Source)},
     {"type", underlying_type(Property::Type)}});

const StyleProperty kUndefinedProperty{Value(), StyleProperty::Kind::Undefined};

StyleProperty getLayerPropertyDefaultValue(Property property) {
    using namespace conversion;
    switch (property) {
        case Property::Visibility:
            return makeConstantStyleProperty(makeValue(VisibilityType::Visible));
        case Property::Minzoom:
            return makeConstantStyleProperty(-std::numeric_limits<float>::infinity());
        case Property::Maxzoom:
            return makeConstantStyleProperty(std::numeric_limits<float>::infinity());
        case Property::Filter:
            return makeExpressionStyleProperty(makeValue(Filter()));
        case Property::Source:
        case Property::SourceLayer:
            return makeConstantStyleProperty(std::string());
        case Property::Type:
            return {};
    }
    assert(false);
    return {};
}

StyleProperty getLayerProperty(const Layer* layer, Property property) {
    using namespace conversion;
    switch (property) {
        case Property::Visibility:
            return makeConstantStyleProperty(layer->getVisibility());
        case Property::Minzoom:
            return makeConstantStyleProperty(layer->getMinZoom());
        case Property::Maxzoom:
            return makeConstantStyleProperty(layer->getMaxZoom());
        case Property::Filter:
            return makeExpressionStyleProperty(layer->getFilter());
        case Property::Source:
            return makeConstantStyleProperty(layer->getSourceID());
        case Property::SourceLayer:
            return makeConstantStyleProperty(layer->getSourceLayer());
        case Property::Type:
            return makeConstantStyleProperty(layer->getTypeInfo()->type);
    }
    assert(false);
    return kUndefinedProperty;
}

} // namespace

Layer::Layer(Immutable<Impl> impl)
    : baseImpl(std::move(impl)),
      observer(&nullObserver) {
}

Layer::~Layer() = default;

std::string Layer::getID() const {
    return baseImpl->id;
}

std::string Layer::getSourceID() const {
    return baseImpl->source;
}

std::string Layer::getSourceLayer() const {
    return baseImpl->sourceLayer;
}

void Layer::setSourceLayer(const std::string& sourceLayer) {
    if (getSourceLayer() == sourceLayer) return;
    auto impl_ = mutableBaseImpl();
    impl_->sourceLayer = sourceLayer;
    baseImpl = std::move(impl_);
}

void Layer::setSourceID(const std::string& sourceID) {
    if (getSourceID() == sourceID) return;
    auto impl_ = mutableBaseImpl();
    impl_->source = sourceID;
    baseImpl = std::move(impl_);
};

const Filter& Layer::getFilter() const {
    return baseImpl->filter;
}

void Layer::setFilter(const Filter& filter) {
    if (getFilter() == filter) return;
    auto impl_ = mutableBaseImpl();
    impl_->filter = filter;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

VisibilityType Layer::getVisibility() const {
    return baseImpl->visibility;
}

void Layer::setVisibility(VisibilityType value) {
    if (value == getVisibility())
        return;
    auto impl_ = mutableBaseImpl();
    impl_->visibility = value;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

float Layer::getMinZoom() const {
    return baseImpl->minZoom;
}

float Layer::getMaxZoom() const {
    return baseImpl->maxZoom;
}

void Layer::setMinZoom(float minZoom) {
    if (getMinZoom() == minZoom) return;
    auto impl_ = mutableBaseImpl();
    impl_->minZoom = minZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

void Layer::setMaxZoom(float maxZoom) {
    if (getMaxZoom() == maxZoom) return;
    auto impl_ = mutableBaseImpl();
    impl_->maxZoom = maxZoom;
    baseImpl = std::move(impl_);
    observer->onLayerChanged(*this);
}

Value Layer::serialize() const {
    mapbox::base::ValueObject result;
    result.emplace(std::make_pair("id", getID()));
    for (const auto& pair : layerProperties) {
        Property property = static_cast<Property>(pair.second);
        std::string key = pair.first.c_str();
        auto styleProperty = getLayerProperty(this, property);
        if (styleProperty != getLayerPropertyDefaultValue(property)) {
            auto keyValue = std::make_pair(std::move(key), std::move(styleProperty.getValue()));
            if (property == Property::Visibility) { // :-(
                result["layout"] = mapbox::base::ValueObject{std::move(keyValue)};
            } else {
                result.emplace(std::move(keyValue));
            }
        }
    }
    return result;
}

void Layer::serializeProperty(Value& out, const StyleProperty& property, const char* propertyName, bool isPaint) const {
    assert(out.getObject());
    auto& object = *(out.getObject());
    std::string propertyType = isPaint ? "paint" : "layout";
    auto it = object.find(propertyType);
    auto pair = std::make_pair(std::string(propertyName), Value{property.getValue()});
    if (it != object.end()) {
        assert(it->second.getObject());
        it->second.getObject()->emplace(std::move(pair));
    } else {
        object[propertyType] = mapbox::base::ValueObject{{std::move(pair)}};
    }
}

void Layer::setObserver(LayerObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver;
}

optional<conversion::Error> Layer::setProperty(const std::string& name, const conversion::Convertible& value) {
    using namespace conversion;
    optional<Error> error = setPropertyInternal(name, value);
    if (!error) return error; // Successfully set by the derived class implementation.
    if (name == "visibility") return setVisibility(value);
    if (name == "minzoom") {
        if (auto zoom = convert<float>(value, *error)) {
            setMinZoom(*zoom);
            return nullopt;
        }
    } else if (name == "maxzoom") {
        if (auto zoom = convert<float>(value, *error)) {
            setMaxZoom(*zoom);
            return nullopt;
        }
    } else if (name == "filter") {
        if (auto filter = convert<Filter>(value, *error)) {
            setFilter(*filter);
            return nullopt;
        }
    } else if (name == "source-layer") {
        if (auto sourceLayer = convert<std::string>(value, *error)) {
            if (getTypeInfo()->source != LayerTypeInfo::Source::Required) {
                Log::Warning(mbgl::Event::General,
                             "'source-layer' property cannot be set to"
                             "the layer %s",
                             baseImpl->id.c_str());
                return nullopt;
            }
            setSourceLayer(*sourceLayer);
            return nullopt;
        }
    } else if (name == "source") {
        if (auto sourceID = convert<std::string>(value, *error)) {
            if (getTypeInfo()->source != LayerTypeInfo::Source::Required) {
                Log::Warning(mbgl::Event::General,
                             "'source' property cannot be set to"
                             "the layer %s",
                             baseImpl->id.c_str());
                return nullopt;
            }
            setSourceID(*sourceID);
            return nullopt;
        }
    }
    return error;
}

StyleProperty Layer::getProperty(const std::string& name) const {
    const auto it = layerProperties.find(name.c_str());
    if (it != layerProperties.end()) {
        return getLayerProperty(this, static_cast<Property>(it->second));
    }
    return getPropertyInternal(name);
}

// static
StyleProperty Layer::getPropertyDefaultValue(const std::string& name) {
    const auto it = layerProperties.find(name.c_str());
    if (it != layerProperties.end()) {
        return getLayerPropertyDefaultValue(static_cast<Property>(it->second));
    }
    return {};
}

optional<conversion::Error> Layer::setVisibility(const conversion::Convertible& value) {
    using namespace conversion;

    if (isUndefined(value)) {
        setVisibility(VisibilityType::Visible);
        return nullopt;
    }

    Error error;
    optional<VisibilityType> visibility = convert<VisibilityType>(value, error);
    if (!visibility) {
        return error;
    }

    setVisibility(*visibility);
    return nullopt;
}

const LayerTypeInfo* Layer::getTypeInfo() const noexcept {
    return baseImpl->getTypeInfo();
}

} // namespace style
} // namespace mbgl
