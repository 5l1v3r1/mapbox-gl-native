#include <mbgl/annotation/annotation_source.hpp>
#include <mbgl/annotation/render_annotation_source.hpp>
#include <mbgl/sourcemanager/annotation_source_factory.hpp>

namespace mbgl {

const style::SourceTypeInfo* AnnotationSourceFactory::getTypeInfo() const noexcept {
    return AnnotationSource::Impl::staticTypeInfo();
}

std::unique_ptr<style::Source> AnnotationSourceFactory::createSource(const std::string&,
                                                                     const style::conversion::Convertible&,
                                                                     style::conversion::Error&) noexcept {
    return std::unique_ptr<style::Source>(new AnnotationSource());
}

std::unique_ptr<RenderSource> AnnotationSourceFactory::createRenderSource(
    Immutable<style::Source::Impl> impl) noexcept {
    assert(impl->getTypeInfo() == getTypeInfo());
    return std::make_unique<RenderAnnotationSource>(staticImmutableCast<AnnotationSource::Impl>(impl));
}

} // namespace mbgl
