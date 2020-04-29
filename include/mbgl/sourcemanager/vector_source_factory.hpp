#pragma once

#include <mbgl/sourcemanager/source_factory.hpp>

namespace mbgl {

class VectorSourceFactory : public SourceFactory {
protected:
    const style::SourceTypeInfo* getTypeInfo() const noexcept final;
    std::unique_ptr<style::Source> createSource(const std::string& id,
                                                const style::conversion::Convertible& value,
                                                style::conversion::Error&) noexcept final;
    std::unique_ptr<RenderSource> createRenderSource(Immutable<style::Source::Impl>) noexcept final;
};

} // namespace mbgl
