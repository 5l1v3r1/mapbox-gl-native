#pragma once

#include <mbgl/style/types.hpp>
#include <string>

namespace mbgl {

class Tileset;

namespace style {
class Source;
}

namespace util {
namespace mapbox {

bool isMapboxURL(const std::string& url);

std::string normalizeSourceURL(const std::string& baseURL, const std::string& str, const std::string& accessToken);
std::string normalizeStyleURL(const std::string& baseURL, const std::string& str, const std::string& accessToken);
std::string normalizeSpriteURL(const std::string& baseURL, const std::string& str, const std::string& accessToken);
std::string normalizeGlyphsURL(const std::string& baseURL, const std::string& str, const std::string& accessToken);
std::string normalizeTileURL(const std::string& baseURL, const std::string& str, const std::string& accessToken);

// Return a "mapbox://tiles/..." URL (suitable for normalizeTileURL) for the given Mapbox tile URL.
std::string canonicalizeTileURL(const std::string& str, const style::Source&);

// Replace URL templates with "mapbox://tiles/..." URLs (suitable for normalizeTileURL).
void canonicalizeTileset(Tileset&, const std::string& url, const style::Source&);

extern const uint64_t DEFAULT_OFFLINE_TILE_COUNT_LIMIT;

} // namespace mapbox
} // namespace util
} // namespace mbgl
