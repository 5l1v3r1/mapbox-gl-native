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

const SourceTypeInfo* Source::getTypeInfo() const noexcept {
    return baseImpl->getTypeInfo();
}

Value Source::serialize() const {
    mapbox::base::ValueObject result;
    result.insert(std::make_pair("id", getID()));
    result.insert(std::make_pair("type", getTypeInfo()->type));
    return result;
}

std::string tilesetSchemeToString(mbgl::Tileset::Scheme scheme) {
    switch (scheme) {
        case mbgl::Tileset::Scheme::XYZ:
            return "xyz";
        case mbgl::Tileset::Scheme::TMS:
            return "tms";
    }

    return "xyz";
}

void Source::serializeTileSet(Value& value, const mbgl::Tileset& tileset) const {
    assert(value.getObject());
    const auto json = value.getObject();
    std::vector<mapbox::base::Value> tiles;
    tiles.reserve(tileset.tiles.size());
    for (const auto& tile : tileset.tiles) tiles.emplace_back(tile);

    json->insert({"tiles", tiles});
    json->insert({"minzoom", tileset.zoomRange.min});
    json->insert({"maxzoom", tileset.zoomRange.max});
    json->insert({"scheme", tilesetSchemeToString(tileset.scheme)});

    if (tileset.bounds.has_value()) {
        json->insert({"bounds",
                      std::vector<mapbox::base::Value>{tileset.bounds->southwest().longitude(),
                                                       tileset.bounds->southwest().latitude(),
                                                       tileset.bounds->northeast().longitude(),
                                                       tileset.bounds->northeast().latitude()}});
    }
}

void Source::serializeUrlOrTileSet(Value& value, const mbgl::variant<std::string, mbgl::Tileset>& urlOrTileSet) const {
    assert(value.getObject());

    urlOrTileSet.match(
        [&](const std::string& url) {
            value.getObject()->insert({"url", url});
        },
        [&](const mbgl::Tileset& tileset) { serializeTileSet(value, tileset); });
}

uint16_t Source::getTileSize() const {
    return baseImpl->getTileSize();
}

} // namespace style
} // namespace mbgl
