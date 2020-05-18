#include <mbgl/renderer/sources/render_custom_geometry_source.hpp>
#include <mbgl/sourcemanager/custom_geometry_source_factory.hpp>
#include <mbgl/style/sources/custom_geometry_source.hpp>
#include <mbgl/style/sources/custom_geometry_source_impl.hpp>

namespace mbgl {

const style::SourceTypeInfo* CustomGeometrySourceFactory::getTypeInfo() const noexcept {
    return style::CustomGeometrySource::Impl::staticTypeInfo();
}

std::unique_ptr<style::Source> CustomGeometrySourceFactory::createSource(const std::string& id,
                                                                         const style::conversion::Convertible&,
                                                                         style::conversion::Error&) noexcept {
    return std::unique_ptr<style::Source>(new style::CustomGeometrySource(id, style::CustomGeometrySource::Options{}));
}

std::unique_ptr<RenderSource> CustomGeometrySourceFactory::createRenderSource(
    Immutable<style::Source::Impl> impl) noexcept {
    assert(impl->getTypeInfo() == getTypeInfo());
    return std::make_unique<RenderCustomGeometrySource>(staticImmutableCast<style::CustomGeometrySource::Impl>(impl));
}

Value CustomGeometrySourceFactory::getPropertyDefaultValue(const std::string& property) noexcept {
    return style::CustomGeometrySource::getPropertyDefaultValue(property);
}

} // namespace mbgl
