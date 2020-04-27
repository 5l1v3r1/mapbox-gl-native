#pragma once

#include <mbgl/style/source.hpp>
#include <string>

namespace mbgl {

class RenderSource;

namespace style {

class SourceObserver;

class Source::Impl {
public:
    virtual ~Impl() = default;

    Impl& operator=(const Impl&) = delete;

    virtual uint16_t getTileSize() const { return util::tileSize; };

    virtual optional<std::string> getAttribution() const = 0;

    void setPrefetchZoomDelta(optional<uint8_t> delta) noexcept;
    optional<uint8_t> getPrefetchZoomDelta() const noexcept;

    void setMinimumTileUpdateInterval(Duration interval) { minimumTileUpdateInterval = interval; }
    Duration getMinimumTileUpdateInterval() const { return minimumTileUpdateInterval; }
    void setMaxOverscaleFactorForParentTiles(optional<uint8_t> overscaleFactor) noexcept;
    optional<uint8_t> getMaxOverscaleFactorForParentTiles() const noexcept;

    bool isVolatile() const { return volatileFlag; }
    void setVolatile(bool set) { volatileFlag = set; }

    // Returns pointer to the statically allocated source type info structure.
    virtual const SourceTypeInfo* getTypeInfo() const noexcept = 0;

    int32_t getCoveringZoomLevel(double zoom) const noexcept;

    const std::string id;

protected:
    optional<uint8_t> prefetchZoomDelta;
    optional<uint8_t> maxOverscaleFactor;
    Duration minimumTileUpdateInterval{Duration::zero()};
    bool volatileFlag = false;

    explicit Impl(std::string);
    Impl(const Impl&) = default;
};

// To be used in the inherited classes.
#define DECLARE_SOURCE_TYPE_INFO                                                                          \
    const mbgl::style::SourceTypeInfo* getTypeInfo() const noexcept override { return staticTypeInfo(); } \
    static const mbgl::style::SourceTypeInfo* staticTypeInfo() noexcept

} // namespace style
} // namespace mbgl
