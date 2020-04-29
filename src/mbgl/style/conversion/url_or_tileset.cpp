#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/style/conversion/url_or_tileset.hpp>
#include <mbgl/style/conversion_impl.hpp>

namespace mbgl {
namespace style {
namespace conversion {

optional<variant<std::string, Tileset>> Converter<variant<std::string, Tileset>>::operator()(const Convertible& value,
                                                                                             Error& error) const {
    auto urlVal = objectMember(value, "url");
    if (!urlVal) {
        optional<Tileset> tileset = convert<Tileset>(value, error);
        if (!tileset) {
            return nullopt;
        }
        return {*tileset};
    }

    optional<std::string> url = toString(*urlVal);
    if (!url) {
        error.message = "source url must be a string";
        return nullopt;
    }

    return {*url};
}

} // namespace conversion
} // namespace style
} // namespace mbgl
