#include <mbgl/renderer/sources/render_geojson_source.hpp>
#include <mbgl/sourcemanager/geojson_source_factory.hpp>
#include <mbgl/style/conversion/geojson.hpp>
#include <mbgl/style/conversion/geojson_options.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/sources/geojson_source_impl.hpp>

namespace mbgl {

const style::SourceTypeInfo* GeoJSONSourceFactory::getTypeInfo() const noexcept {
    return style::GeoJSONSource::Impl::staticTypeInfo();
}

std::unique_ptr<style::Source> GeoJSONSourceFactory::createSource(const std::string& id,
                                                                  const style::conversion::Convertible& value,
                                                                  style::conversion::Error& error) noexcept {
    auto dataValue = objectMember(value, "data");
    if (!dataValue) {
        error.message = "GeoJSON source must have a data value";
        return nullptr;
    }

    Immutable<style::GeoJSONOptions> options = style::GeoJSONOptions::defaultOptions();
    if (optional<style::GeoJSONOptions> converted = style::conversion::convert<style::GeoJSONOptions>(value, error)) {
        options = makeMutable<style::GeoJSONOptions>(std::move(*converted));
    }

    auto result = std::make_unique<style::GeoJSONSource>(id, std::move(options));

    if (isObject(*dataValue)) {
        optional<GeoJSON> geoJSON = style::conversion::convert<GeoJSON>(*dataValue, error);
        if (!geoJSON) {
            return nullptr;
        }
        result->setGeoJSON(*geoJSON);
    } else if (toString(*dataValue)) {
        result->setURL(*toString(*dataValue));
    } else {
        error.message = "GeoJSON data must be a URL or an object";
        return nullptr;
    }

    return {std::move(result)};
}

std::unique_ptr<RenderSource> GeoJSONSourceFactory::createRenderSource(Immutable<style::Source::Impl> impl) noexcept {
    assert(impl->getTypeInfo() == getTypeInfo());
    return std::make_unique<RenderGeoJSONSource>(staticImmutableCast<style::GeoJSONSource::Impl>(impl));
}

} // namespace mbgl
