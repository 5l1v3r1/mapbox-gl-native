#include <mbgl/style/conversion/coordinate.hpp>
#include <mbgl/style/conversion_impl.hpp>

namespace mbgl {
namespace style {
namespace conversion {

optional<LatLng> Converter<LatLng>::operator() (const Convertible& value, Error& error) const {
    if (!isArray(value) || arrayLength(value) < 2 ) {
        error.message = "coordinate array must contain numeric longitude and latitude values";
        return nullopt;
    }
    //Style spec uses GeoJSON convention for specifying coordinates
    optional<double> latitude = toDouble(arrayMember(value, 1));
    optional<double> longitude = toDouble(arrayMember(value, 0));

    if (!latitude || !longitude) {
        error.message = "coordinate array must contain numeric longitude and latitude values";
        return nullopt;
    }
    if (*latitude < -90 || *latitude > 90 ){
        error.message = "coordinate latitude must be between -90 and 90";
        return nullopt;
    }
    return LatLng(*latitude, *longitude);
}

optional<std::array<LatLng, 4>> Converter<std::array<LatLng, 4>>::operator()(const Convertible& value,
                                                                             Error& error) const {
    if (!isArray(value) || arrayLength(value) != 4u) {
        error.message = "Image coordinates must be an array of four longitude latitude pairs";
        return nullopt;
    }
    std::array<LatLng, 4> coordinates;
    for (std::size_t i = 0; i < 4u; ++i) {
        auto latLng = convert<LatLng>(arrayMember(value, i), error);
        if (!latLng) {
            return nullopt;
        }
        coordinates[i] = *latLng;
    }
    return coordinates;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
