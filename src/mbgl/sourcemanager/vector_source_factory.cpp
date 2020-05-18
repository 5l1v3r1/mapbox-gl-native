#include <mbgl/renderer/sources/render_vector_source.hpp>
#include <mbgl/sourcemanager/vector_source_factory.hpp>
#include <mbgl/style/conversion/url_or_tileset.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/sources/vector_source.hpp>
#include <mbgl/style/sources/vector_source_impl.hpp>

namespace mbgl {

const style::SourceTypeInfo* VectorSourceFactory::getTypeInfo() const noexcept {
    return style::VectorSource::Impl::staticTypeInfo();
}

std::unique_ptr<style::Source> VectorSourceFactory::createSource(const std::string& id,
                                                                 const style::conversion::Convertible& value,
                                                                 style::conversion::Error& error) noexcept {
    optional<variant<std::string, Tileset>> urlOrTileset =
        style::conversion::convert<variant<std::string, Tileset>>(value, error);
    if (!urlOrTileset) {
        return nullptr;
    }
    auto maxzoomValue = objectMember(value, "maxzoom");
    optional<float> maxzoom;
    if (maxzoomValue) {
        maxzoom = toNumber(*maxzoomValue);
        if (!maxzoom || *maxzoom < 0 || *maxzoom > std::numeric_limits<uint8_t>::max()) {
            error.message = "invalid maxzoom";
            return nullptr;
        }
    }
    auto minzoomValue = objectMember(value, "minzoom");
    optional<float> minzoom;
    if (minzoomValue) {
        minzoom = toNumber(*minzoomValue);
        if (!minzoom || *minzoom < 0 || *minzoom > std::numeric_limits<uint8_t>::max()) {
            error.message = "invalid minzoom";
            return nullptr;
        }
    }
    return {
        std::make_unique<style::VectorSource>(id, std::move(*urlOrTileset), std::move(maxzoom), std::move(minzoom))};
}

std::unique_ptr<RenderSource> VectorSourceFactory::createRenderSource(Immutable<style::Source::Impl> impl) noexcept {
    assert(impl->getTypeInfo() == getTypeInfo());
    return std::make_unique<RenderVectorSource>(staticImmutableCast<style::VectorSource::Impl>(impl));
}

Value VectorSourceFactory::getPropertyDefaultValue(const std::string& property) noexcept {
    return style::VectorSource::getPropertyDefaultValue(property);
}

} // namespace mbgl
