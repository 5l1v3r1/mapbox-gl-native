#include <mbgl/storage/file_source.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/coordinate.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/sources/image_source.hpp>
#include <mbgl/style/sources/image_source_impl.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/util/async_request.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/premultiply.hpp>

namespace mbgl {
namespace style {

ImageSource::ImageSource(std::string id, const std::array<LatLng, 4> coords_)
    : Source(makeMutable<Impl>(std::move(id), coords_)) {
}

ImageSource::~ImageSource() = default;

const ImageSource::Impl& ImageSource::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

void ImageSource::setCoordinates(const std::array<LatLng, 4>& coords_) {
    baseImpl = makeMutable<Impl>(impl(), coords_);
    observer->onSourceChanged(*this);
}

std::array<LatLng, 4> ImageSource::getCoordinates() const {
    return impl().getCoordinates();
}

void ImageSource::setURL(const std::string& url_) {
    url = url_;
    // Signal that the source description needs a reload
    if (loaded || req) {
        loaded = false;
        req.reset();
        observer->onSourceDescriptionChanged(*this);
    }
}

void ImageSource::setImage(PremultipliedImage&& image_) {
    url = {};
    if (req) {
        req.reset();
    }
    loaded = true;
    baseImpl = makeMutable<Impl>(impl(), std::move(image_));
    observer->onSourceChanged(*this);
}

void ImageSource::setSourceData(SourceData data) {
    if (data.image) {
        setImage(std::move(*data.image));
    }
}

optional<Resource> ImageSource::getResource() const {
    if (!url) return nullopt;
    return Resource::image(*url);
}

optional<std::string> ImageSource::getURL() const {
    return url;
}

void ImageSource::loadDescription(FileSource& fileSource) {
    if (!url) {
        loaded = true;
    }

    if (req ||  loaded) {
        return;
    }
    const Resource imageResource { Resource::Image, *url, {} };

    req = fileSource.request(imageResource, [this](const Response& res) {
        if (res.error) {
            observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            observer->onSourceError(*this, std::make_exception_ptr(std::runtime_error("unexpectedly empty image url")));
        } else {
            try {
                baseImpl = makeMutable<Impl>(impl(), decodeImage(*res.data));
            } catch (...) {
                observer->onSourceError(*this, std::current_exception());
            }
            loaded = true;
            observer->onSourceLoaded(*this);
        }
    });
}

bool ImageSource::supportsLayerType(const mbgl::style::LayerTypeInfo* info) const {
    return mbgl::underlying_type(TileKind::Raster) == mbgl::underlying_type(info->tileKind);
}

Mutable<Source::Impl> ImageSource::createMutable() const noexcept {
    return staticMutableCast<Source::Impl>(makeMutable<Impl>(impl()));
}

Value ImageSource::serialize() const {
    using namespace conversion;
    auto value = Source::serialize();
    assert(value.getObject());
    auto object = value.getObject();
    if (url) object->insert({"url", url.value()});
    object->insert({"coordinates", makeValue(getCoordinates())});

    return value;
}

optional<conversion::Error> ImageSource::setPropertyInternal(const std::string& name,
                                                             const conversion::Convertible& value) {
    using namespace conversion;
    optional<Error> error = Source::setPropertyInternal(name, value);
    assert(error);
    if (name == "url") {
        if (auto url_ = convert<std::string>(value, *error)) {
            setURL(*url_);
            return nullopt;
        }
    } else if (name == "coordinates") {
        if (auto coordinates = convert<std::array<LatLng, 4>>(value, *error)) {
            setCoordinates(*coordinates);
            return nullopt;
        }
    }
    return error;
}

Value ImageSource::getPropertyInternal(const std::string& name) const {
    using namespace conversion;
    if (name == "url" && url) return *url;
    if (name == "coordinates") return makeValue(getCoordinates());
    return Value();
}

} // namespace style
} // namespace mbgl
