#pragma once

#include <mbgl/util/geojson.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/optional.hpp>
#include <memory>
#include <string>

namespace mbgl {
namespace style {

class GeoJSONData;

struct SourceData {
    SourceData() = default;
    explicit SourceData(std::string url_) : url(std::move(url_)) {}
    explicit SourceData(PremultipliedImage image_) : image(std::move(image_)) {}
    explicit SourceData(GeoJSON geoJSON_) : geoJSON(std::move(geoJSON_)) {}
    explicit SourceData(std::shared_ptr<GeoJSONData> data) : geoJSONData(std::move(data)) {}

    optional<std::string> url;
    optional<PremultipliedImage> image;
    optional<GeoJSON> geoJSON;
    std::shared_ptr<GeoJSONData> geoJSONData;
};

} // namespace style
} // namespace mbgl
