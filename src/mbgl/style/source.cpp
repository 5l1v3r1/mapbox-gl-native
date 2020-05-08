#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/tileset.hpp>

namespace mbgl {
namespace style {

static SourceObserver nullObserver;

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
    using namespace conversion;
    if (name == "type") return makeValue(getTypeInfo()->type);
    if (name == "volatile") return makeValue(isVolatile());
    if (name == "attribution") return makeValue(getAttribution());
    if (name == "tile-size") return makeValue(getTileSize());
    if (name == "prefetch-zoom-delta") return makeValue(getPrefetchZoomDelta());
    if (name == "max-overscale-factor-for-parent-tiles") {
        return makeValue(getMaxOverscaleFactorForParentTiles());
    }
    if (name == "minimum-tile-update-interval") {
        auto seconds = std::chrono::duration_cast<std::chrono::duration<float>>(getMinimumTileUpdateInterval());
        return makeValue(seconds.count());
    }
    return getPropertyInternal(name);
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
