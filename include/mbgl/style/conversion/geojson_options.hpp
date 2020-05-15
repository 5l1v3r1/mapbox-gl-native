#pragma once

#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<GeoJSONOptions> {
    optional<GeoJSONOptions> operator()(const Convertible& value, Error& error) const;
};

template <>
struct ValueFactory<GeoJSONOptions> {
    static Value make(const GeoJSONOptions&);
};

} // namespace conversion
} // namespace style
} // namespace mbgl
