#include <mbgl/renderer/sources/render_image_source.hpp>
#include <mbgl/sourcemanager/image_source_factory.hpp>
#include <mbgl/style/conversion/coordinate.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/sources/image_source.hpp>
#include <mbgl/style/sources/image_source_impl.hpp>

namespace mbgl {

const style::SourceTypeInfo* ImageSourceFactory::getTypeInfo() const noexcept {
    return style::ImageSource::Impl::staticTypeInfo();
}

std::unique_ptr<style::Source> ImageSourceFactory::createSource(const std::string& id,
                                                                const style::conversion::Convertible& value,
                                                                style::conversion::Error& error) noexcept {
    auto urlValue = objectMember(value, "url");
    if (!urlValue) {
        error.message = "Image source must have a url value";
        return nullptr;
    }

    auto urlString = toString(*urlValue);
    if (!urlString) {
        error.message = "Image url must be a URL string";
        return nullptr;
    }

    auto coordinatesValue = objectMember(value, "coordinates");
    if (!coordinatesValue) {
        error.message = "Image source must have a coordinates values";
        return nullptr;
    }

    if (!isArray(*coordinatesValue) || arrayLength(*coordinatesValue) != 4) {
        error.message = "Image coordinates must be an array of four longitude latitude pairs";
        return nullptr;
    }

    std::array<LatLng, 4> coordinates;
    for (std::size_t i = 0; i < 4; i++) {
        auto latLng = style::conversion::convert<LatLng>(arrayMember(*coordinatesValue, i), error);
        if (!latLng) {
            return nullptr;
        }
        coordinates[i] = *latLng;
    }
    auto result = std::make_unique<style::ImageSource>(id, coordinates);
    result->setURL(*urlString);

    return {std::move(result)};
}

std::unique_ptr<RenderSource> ImageSourceFactory::createRenderSource(Immutable<style::Source::Impl> impl) noexcept {
    assert(impl->getTypeInfo() == getTypeInfo());
    return std::make_unique<RenderImageSource>(staticImmutableCast<style::ImageSource::Impl>(impl));
}

} // namespace mbgl
