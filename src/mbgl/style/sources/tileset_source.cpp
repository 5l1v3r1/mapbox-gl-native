#include <mbgl/style/sources/tileset_source.hpp>

#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/tile/tile.hpp>
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
    PropertyMap* properties = value.getObject();
    std::vector<mapbox::base::Value> tiles;
    tiles.reserve(tileset.tiles.size());
    for (const auto& tile : tileset.tiles) tiles.emplace_back(tile);

    properties->insert({"tiles", std::move(tiles)});
    properties->insert({"minzoom", tileset.zoomRange.min});
    properties->insert({"maxzoom", tileset.zoomRange.max});
    properties->insert({"scheme", conversion::makeValue(tileset.scheme)});

    if (tileset.bounds.has_value()) {
        properties->insert({"bounds",
                            std::vector<mapbox::base::Value>{tileset.bounds->southwest().longitude(),
                                                             tileset.bounds->southwest().latitude(),
                                                             tileset.bounds->northeast().longitude(),
                                                             tileset.bounds->northeast().latitude()}});
    }
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

} // namespace style
} // namespace mbgl
