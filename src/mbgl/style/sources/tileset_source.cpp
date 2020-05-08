#include <mbgl/style/sources/tileset_source.hpp>

#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/enum.hpp>

namespace mbgl {

using TilesetScheme = Tileset::Scheme;
MBGL_DEFINE_ENUM(TilesetScheme, {{TilesetScheme::XYZ, "xyz"}, {TilesetScheme::TMS, "tms"}});

namespace style {

TilesetSource::TilesetSource(Immutable<Impl> impl_, variant<std::string, Tileset> urlOrTileset_)
    : Source(std::move(impl_)), urlOrTileset(std::move(urlOrTileset_)) {}

TilesetSource::~TilesetSource() = default;

const Tileset* TilesetSource::getTileset() const {
    if (urlOrTileset.is<Tileset>()) {
        return &urlOrTileset.get<Tileset>();
    }
    return nullptr;
}

optional<std::string> TilesetSource::getURL() const {
    if (urlOrTileset.is<Tileset>()) return nullopt;
    return urlOrTileset.get<std::string>();
}

optional<Resource> TilesetSource::getResource() const {
    if (urlOrTileset.is<std::string>()) {
        return Resource::source(urlOrTileset.get<std::string>());
    }
    return nullopt;
}

namespace {

void serializeTileSet(Value& value, const mbgl::Tileset& tileset) {
    assert(value.getObject());
    using namespace conversion;
    PropertyMap* properties = value.getObject();
    properties->insert({"tiles", makeValue(tileset.tiles)});
    properties->insert({"minzoom", tileset.zoomRange.min});
    properties->insert({"maxzoom", tileset.zoomRange.max});
    properties->insert({"scheme", makeValue(tileset.scheme)});
    properties->insert({"bounds", makeValue(tileset.bounds)});
}

} // namespace

Value TilesetSource::serialize() const {
    auto result = Source::serialize();
    assert(result.getObject());
    urlOrTileset.match(
        [&](const std::string& url) {
            result.getObject()->insert({"url", url});
        },
        [&](const mbgl::Tileset& tileset) { serializeTileSet(result, tileset); });
    return result;
}

Value TilesetSource::getPropertyInternal(const std::string& name) const {
    using namespace conversion;
    if (auto* tileset = getTileset()) {
        if (name == "tiles") return makeValue(tileset->tiles);
        if (name == "minzoom") return tileset->zoomRange.min;
        if (name == "maxzoom") return tileset->zoomRange.max;
        if (name == "scheme") return makeValue(tileset->scheme);
        if (name == "bounds") return makeValue(tileset->bounds);
    } else if (auto url = getURL()) {
        if (name == "url") return makeValue(*url);
    }
    return Value();
}

} // namespace style
} // namespace mbgl
