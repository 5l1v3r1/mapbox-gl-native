#include <mbgl/renderer/render_source.hpp>
#include <mbgl/sourcemanager/source_factory.hpp>
#include <mbgl/sourcemanager/source_manager.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/source.hpp>
#include <mbgl/style/source_impl.hpp>

namespace mbgl {

std::unique_ptr<style::Source> SourceManager::createSource(const std::string& type,
                                                           const std::string& id,
                                                           const style::conversion::Convertible& value,
                                                           style::conversion::Error& error) noexcept {
    SourceFactory* factory = getFactory(type);
    if (factory) {
        return factory->createSource(id, value, error);
    } else {
        error.message = "Null factory for type: " + type;
    }
    error.message = "Unsupported source type! " + error.message;
    return nullptr;
}

std::unique_ptr<RenderSource> SourceManager::createRenderSource(Immutable<style::Source::Impl> impl) noexcept {
    SourceFactory* factory = getFactory(impl->getTypeInfo());
    assert(factory);
    return factory->createRenderSource(std::move(impl));
}

} // namespace mbgl
