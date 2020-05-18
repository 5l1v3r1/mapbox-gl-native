#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/tileset.hpp>

#include <mapbox/eternal.hpp>

namespace mbgl {
namespace style {

static SourceObserver nullObserver;

namespace {

enum class Property : uint8_t {
    Volatile,
    Attribution,
    TileSize,
    PrefetchZoomDelta,
    MaxOverscaleFactorForParentTiles,
    MinTileUpdateInterval,
    Type
};

constexpr const auto sourceProperties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>(
    {{"volatile", underlying_type(Property::Volatile)},
     {"attribution", underlying_type(Property::Attribution)},
     {"tile-size", underlying_type(Property::TileSize)},
     {"prefetch-zoom-delta", underlying_type(Property::PrefetchZoomDelta)},
     {"max-overscale-factor-for-parent-tiles", underlying_type(Property::MaxOverscaleFactorForParentTiles)},
     {"minimum-tile-update-interval", underlying_type(Property::MinTileUpdateInterval)},
     {"type", underlying_type(Property::Type)}});

Value getSourcePropertyDefaultValue(Property property) {
    switch (property) {
        case Property::Volatile:
            return false;
        case Property::TileSize:
            return util::tileSize;
        case Property::PrefetchZoomDelta:
            return util::DEFAULT_PREFETCH_ZOOM_DELTA;
        case Property::Attribution:
        case Property::MaxOverscaleFactorForParentTiles:
        case Property::Type:
            return {};
        case Property::MinTileUpdateInterval:
            return 0.0;
    }
    assert(false);
    return NullValue();
}

Value getSourceProperty(const Source* source, Property property) {
    using namespace conversion;
    switch (property) {
        case Property::Volatile:
            return source->isVolatile();
        case Property::Attribution:
            return makeValue(source->getAttribution());
        case Property::TileSize:
            return makeValue(source->getTileSize());
        case Property::PrefetchZoomDelta:
            return makeValue(source->getPrefetchZoomDelta());
        case Property::MaxOverscaleFactorForParentTiles:
            return makeValue(source->getMaxOverscaleFactorForParentTiles());
        case Property::MinTileUpdateInterval: {
            auto seconds =
                std::chrono::duration_cast<std::chrono::duration<float>>(source->getMinimumTileUpdateInterval());
            return makeValue(seconds.count());
        }
        case Property::Type:
            return makeValue(source->getTypeInfo()->type);
    }
    return NullValue();
}

} // namespace

Source::Source(Immutable<Impl> impl) : baseImpl(std::move(impl)), observer(&nullObserver) {}

Source::~Source() = default;

std::string Source::getID() const {
    return baseImpl->id;
}

optional<std::string> Source::getAttribution() const {
    return baseImpl->getAttribution();
}

bool Source::isVolatile() const noexcept {
    return baseImpl->isVolatile();
}

void Source::setVolatile(bool set) noexcept {
    if (isVolatile() == set) return;
    auto newImpl = createMutable();
    newImpl->setVolatile(set);
    baseImpl = std::move(newImpl);
    observer->onSourceChanged(*this);
}

void Source::setObserver(SourceObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver;
}

void Source::setPrefetchZoomDelta(optional<uint8_t> delta) noexcept {
    if (getPrefetchZoomDelta() == delta) return;
    auto newImpl = createMutable();
    newImpl->setPrefetchZoomDelta(std::move(delta));
    baseImpl = std::move(newImpl);
    observer->onSourceChanged(*this);
}

optional<uint8_t> Source::getPrefetchZoomDelta() const noexcept {
    return baseImpl->getPrefetchZoomDelta();
}

void Source::setMinimumTileUpdateInterval(Duration interval) noexcept {
    if (getMinimumTileUpdateInterval() == interval) return;
    auto newImpl = createMutable();
    newImpl->setMinimumTileUpdateInterval(interval);
    baseImpl = std::move(newImpl);
    observer->onSourceChanged(*this);
}

Duration Source::getMinimumTileUpdateInterval() const noexcept {
    return baseImpl->getMinimumTileUpdateInterval();
}

void Source::setMaxOverscaleFactorForParentTiles(optional<uint8_t> overscaleFactor) noexcept {
    if (getMaxOverscaleFactorForParentTiles() == overscaleFactor) return;
    auto newImpl = createMutable();
    newImpl->setMaxOverscaleFactorForParentTiles(std::move(overscaleFactor));
    baseImpl = std::move(newImpl);
    observer->onSourceChanged(*this);
}

optional<uint8_t> Source::getMaxOverscaleFactorForParentTiles() const noexcept {
    return baseImpl->getMaxOverscaleFactorForParentTiles();
}

int32_t Source::getCoveringZoomLevel(double z) const noexcept {
    return baseImpl->getCoveringZoomLevel(z);
}

void Source::dumpDebugLogs() const {
    Log::Info(Event::General, "Source::id: %s", getID().c_str());
    Log::Info(Event::General, "Source::loaded: %d", loaded);
}

optional<conversion::Error> Source::setProperty(const std::string& name, const conversion::Convertible& value) {
    using namespace conversion;
    optional<Error> error = setPropertyInternal(name, value);
    if (!error) return error; // Successfully set by the derived class implementation.
    if (name == "volatile") {
        if (auto isVolatile = convert<bool>(value, *error)) {
            setVolatile(*isVolatile);
            return nullopt;
        }
    } else if (name == "prefetch-zoom-delta") {
        if (auto zoomDelta = convert<float>(value, *error)) {
            setPrefetchZoomDelta(static_cast<uint8_t>(*zoomDelta));
            return nullopt;
        }
    } else if (name == "max-overscale-factor-for-parent-tiles") {
        if (auto zoomDelta = convert<float>(value, *error)) {
            setMaxOverscaleFactorForParentTiles(static_cast<uint8_t>(*zoomDelta));
            return nullopt;
        }
    } else if (name == "minimum-tile-update-interval") {
        if (auto updateInterval = convert<float>(value, *error)) {
            std::chrono::duration<float> seconds(*updateInterval);
            setMinimumTileUpdateInterval(std::chrono::duration_cast<Duration>(seconds));
            return nullopt;
        }
    }
    return error;
}

Value Source::getProperty(const std::string& name) const {
    const auto it = sourceProperties.find(name.c_str());
    if (it != sourceProperties.end()) {
        return getSourceProperty(this, static_cast<Property>(it->second));
    }
    return getPropertyInternal(name);
}
// static
Value Source::getPropertyDefaultValue(const std::string& name) {
    const auto it = sourceProperties.find(name.c_str());
    if (it != sourceProperties.end()) {
        return getSourcePropertyDefaultValue(static_cast<Property>(it->second));
    }
    return Value();
}

const SourceTypeInfo* Source::getTypeInfo() const noexcept {
    return baseImpl->getTypeInfo();
}

Value Source::serialize() const {
    mapbox::base::ValueObject result;
    result.insert(std::make_pair("id", getID()));
    result.insert(std::make_pair("type", getTypeInfo()->type));
    return result;
}

uint16_t Source::getTileSize() const {
    return baseImpl->getTileSize();
}

optional<conversion::Error> Source::setPropertyInternal(const std::string& name, const conversion::Convertible&) {
    return conversion::Error{"Cannot set property " + name + " for the source " + baseImpl->id};
}

Value Source::getPropertyInternal(const std::string&) const {
    return NullValue();
}

} // namespace style
} // namespace mbgl
