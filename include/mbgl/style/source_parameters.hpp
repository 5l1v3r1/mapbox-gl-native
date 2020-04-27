#pragma once

#include <mbgl/util/geo.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {

struct SourceParameters {
    explicit SourceParameters(std::array<LatLng, 4> bounds_) : bounds(bounds_) {}
    optional<std::array<LatLng, 4>> bounds;
};

} // namespace style
} // namespace mbgl
