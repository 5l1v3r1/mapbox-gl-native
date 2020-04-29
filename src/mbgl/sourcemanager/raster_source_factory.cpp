#include <mbgl/renderer/sources/render_raster_source.hpp>
#include <mbgl/sourcemanager/raster_source_factory.hpp>
#include <mbgl/style/conversion/url_or_tileset.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/sources/raster_source_impl.hpp>

namespace mbgl {

const style::SourceTypeInfo* RasterSourceFactory::getTypeInfo() const noexcept {
    return style::RasterSource::Impl::staticTypeInfo();
}

std::unique_ptr<style::Source> RasterSourceFactory::createSource(const std::string& id,
                                                                 const style::conversion::Convertible& value,
                                                                 style::conversion::Error& error) noexcept {
    optional<variant<std::string, Tileset>> urlOrTileset =
        style::conversion::convert<variant<std::string, Tileset>>(value, error);

    if (!urlOrTileset) {
        return nullptr;
    }

    uint16_t tileSize = util::tileSize;
    auto tileSizeValue = objectMember(value, "tileSize");
    if (tileSizeValue) {
        optional<float> size = toNumber(*tileSizeValue);
        if (!size || *size < 0 || *size > std::numeric_limits<uint16_t>::max()) {
            error.message = "invalid tileSize";
            return nullptr;
        }
        tileSize = *size;
    }

    return {std::make_unique<style::RasterSource>(id, std::move(*urlOrTileset), tileSize)};
}

std::unique_ptr<RenderSource> RasterSourceFactory::createRenderSource(Immutable<style::Source::Impl> impl) noexcept {
    assert(impl->getTypeInfo() == getTypeInfo());
    return std::make_unique<RenderRasterSource>(staticImmutableCast<style::RasterSource::Impl>(impl));
}

} // namespace mbgl
