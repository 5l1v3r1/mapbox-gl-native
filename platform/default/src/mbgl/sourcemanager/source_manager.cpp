#include <map>
#include <mbgl/layermanager/layer_manager.hpp>
#include <mbgl/sourcemanager/annotation_source_factory.hpp>
#include <mbgl/sourcemanager/custom_geometry_source_factory.hpp>
#include <mbgl/sourcemanager/geojson_source_factory.hpp>
#include <mbgl/sourcemanager/image_source_factory.hpp>
#include <mbgl/sourcemanager/raster_dem_source_factory.hpp>
#include <mbgl/sourcemanager/raster_source_factory.hpp>
#include <mbgl/sourcemanager/source_manager.hpp>
#include <mbgl/sourcemanager/vector_source_factory.hpp>
#include <mbgl/util/logging.hpp>
#include <memory>
#include <vector>

namespace mbgl {

class SourceManagerDefault final : public SourceManager {
public:
    SourceManagerDefault();

private:
    void addSourceType(std::unique_ptr<SourceFactory>);
    // SourceManager overrides.
    SourceFactory* getFactory(const std::string& type) noexcept final;
    SourceFactory* getFactory(const style::SourceTypeInfo*) noexcept final;

    std::vector<std::unique_ptr<SourceFactory>> factories;
    std::map<std::string, SourceFactory*> typeToFactory;
};

SourceManagerDefault::SourceManagerDefault() {
    if (LayerManager::annotationsEnabled) {
        addSourceType(std::make_unique<AnnotationSourceFactory>());
    }
#if !defined(MBGL_SOURCE_CUSTOM_GEOMETRY_DISABLE_ALL)
    addSourceType(std::make_unique<CustomGeometrySourceFactory>());
#endif
#if !defined(MBGL_SOURCE_GEOJSON_DISABLE_ALL)
    addSourceType(std::make_unique<GeoJSONSourceFactory>());
#endif
#if !defined(MBGL_SOURCE_IMAGE_DISABLE_ALL)
    addSourceType(std::make_unique<ImageSourceFactory>());
#endif
#if !defined(MBGL_SOURCE_RASTER_DEM_DISABLE_ALL)
    addSourceType(std::make_unique<RasterDEMSourceFactory>());
#endif
#if !defined(MBGL_SOURCE_RASTER_DISABLE_ALL)
    addSourceType(std::make_unique<RasterSourceFactory>());
#endif
#if !defined(MBGL_SOURCE_VECTOR_DISABLE_ALL)
    addSourceType(std::make_unique<VectorSourceFactory>());
#endif
}

void SourceManagerDefault::addSourceType(std::unique_ptr<SourceFactory> factory) {
    std::string type{factory->getTypeInfo()->type};
    if (!type.empty()) {
        typeToFactory.emplace(std::make_pair(std::move(type), factory.get()));
    } else {
        Log::Warning(Event::Setup, "Failure adding source factory. getTypeInfo() returned an empty type string.");
    }
    factories.emplace_back(std::move(factory));
}

SourceFactory* SourceManagerDefault::getFactory(const mbgl::style::SourceTypeInfo* typeInfo) noexcept {
    assert(typeInfo);
    for (const auto& factory : factories) {
        if (factory->getTypeInfo() == typeInfo) {
            return factory.get();
        }
    }
    assert(false);
    return nullptr;
}

SourceFactory* SourceManagerDefault::getFactory(const std::string& type) noexcept {
    auto search = typeToFactory.find(type);
    return (search != typeToFactory.end()) ? search->second : nullptr;
}

// static
SourceManager* SourceManager::get() noexcept {
    static SourceManagerDefault instance;
    return &instance;
}

} // namespace mbgl
