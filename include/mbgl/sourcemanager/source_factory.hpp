#pragma once

#include <mbgl/style/source.hpp>

namespace mbgl {
namespace style {
namespace conversion {
class Convertible;
struct Error;
} // namespace conversion
} // namespace style

class RenderSource;

/**
 * @brief The SourceFactory abstract class
 *
 * This class is responsible for creation of the source objects that belong to a concrete source type.
 */
class SourceFactory {
public:
    virtual ~SourceFactory() = default;

    /// Returns the source type data.
    virtual const style::SourceTypeInfo* getTypeInfo() const noexcept = 0;

    /// Returns a new Source instance on success call; returns `nullptr` otherwise with Error updated
    /// accordingly.
    virtual std::unique_ptr<style::Source> createSource(const std::string& id,
                                                        const style::conversion::Convertible& value,
                                                        style::conversion::Error&) noexcept = 0;

    /// Returns a new RenderSource instance.
    virtual std::unique_ptr<RenderSource> createRenderSource(Immutable<style::Source::Impl>) noexcept = 0;
};

} // namespace mbgl
