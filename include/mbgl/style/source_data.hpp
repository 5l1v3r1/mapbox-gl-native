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
    explicit SourceData(PremultipliedImage image_) : image(std::move(image_)) {}
    optional<PremultipliedImage> image;
};

} // namespace style
} // namespace mbgl
