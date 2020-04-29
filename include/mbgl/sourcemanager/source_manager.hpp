#pragma once

#include <mbgl/style/source.hpp>

namespace mbgl {
namespace style {
namespace conversion {
class Convertible;
struct Error;
} // namespace conversion
} // namespace style

class SourceFactory;
class RenderSource;

/**
 * @brief A singleton class responsible for creating source instances.
 *
 * The SourceManager has implementation per platform. The SourceManager implementation
 * defines what source types are available.
 *
 * Linker excludes the unreachable code for the disabled sources from the binaries,
 * significantly reducing their size.
 */
class SourceManager {
public:
    /**
     * @brief A singleton getter.
     *
     * @return SourceManager*
     */
    static SourceManager* get() noexcept;

    /// Returns a new Source instance on success call; returns `nullptr` otherwise.
    std::unique_ptr<style::Source> createSource(const std::string& type,
                                                const std::string& id,
                                                const style::conversion::Convertible& value,
                                                style::conversion::Error& error) noexcept;

    /// Returns a new RenderSource instance on success call; returns `nullptr` otherwise.
    std::unique_ptr<RenderSource> createRenderSource(Immutable<style::Source::Impl>) noexcept;

protected:
    virtual ~SourceManager() = default;
    virtual SourceFactory* getFactory(const std::string& type) noexcept = 0;
    virtual SourceFactory* getFactory(const style::SourceTypeInfo*) noexcept = 0;
};

} // namespace mbgl
