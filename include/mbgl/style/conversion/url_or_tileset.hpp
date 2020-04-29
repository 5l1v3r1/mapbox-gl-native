#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<variant<std::string, Tileset>> {
public:
    optional<variant<std::string, Tileset>> operator()(const Convertible& value, Error& error) const;
};

} // namespace conversion
} // namespace style
} // namespace mbgl
