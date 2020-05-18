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
    if (auto setDataError = result->setProperty("data", *dataValue)) {
        error = *setDataError;
        return nullptr;
    }

    return result;
}

std::unique_ptr<RenderSource> GeoJSONSourceFactory::createRenderSource(Immutable<style::Source::Impl> impl) noexcept {
    assert(impl->getTypeInfo() == getTypeInfo());
    return std::make_unique<RenderGeoJSONSource>(staticImmutableCast<style::GeoJSONSource::Impl>(impl));
}

Value GeoJSONSourceFactory::getPropertyDefaultValue(const std::string& property) noexcept {
    return style::GeoJSONSource::getPropertyDefaultValue(property);
}

} // namespace mbgl
