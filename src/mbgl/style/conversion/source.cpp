#include <mbgl/sourcemanager/source_manager.hpp>
#include <mbgl/style/conversion/coordinate.hpp>
#include <mbgl/style/conversion/source.hpp>
#include <mbgl/style/conversion/tileset.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/util/geo.hpp>

namespace mbgl {
namespace style {
namespace conversion {

namespace {
bool setObjectMember(std::unique_ptr<Source>& source, const Convertible& value, const char* member, Error& error) {
    if (auto memberValue = objectMember(value, member)) {
        if (auto error_ = source->setProperty(member, *memberValue)) {
            error = *error_;
            return false;
        }
    }
    return true;
}
} // namespace

optional<std::unique_ptr<Source>> Converter<std::unique_ptr<Source>>::operator()(const Convertible& value,
                                                                                 Error& error,
                                                                                 const std::string& id) const {
    if (!isObject(value)) {
        error.message = "source must be an object";
        return nullopt;
    }

    auto typeValue = objectMember(value, "type");
    if (!typeValue) {
        error.message = "source must have a type";
        return nullopt;
    }

    optional<std::string> type = toString(*typeValue);
    if (!type) {
        error.message = "source type must be a string";
        return nullopt;
    }
    const std::string& tname = type.value();

    auto source = SourceManager::get()->createSource(tname, id, value, error);
    if (!source) return nullopt;
    if (!setObjectMember(source, value, "volatile", error)) return nullopt;
    return source;
}

} // namespace conversion
} // namespace style
} // namespace mbgl
