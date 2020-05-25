#pragma once

#include <mapbox/geometry/box.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/util/optional.hpp>

#include <array>

namespace mbgl {

class OpacityState {
public:
    OpacityState(bool placed, bool skipFade);
    OpacityState(const OpacityState& prevState, float increment, bool placed);
    bool isHidden() const;
    float opacity;
    bool placed;
};

class JointOpacityState {
public:
    JointOpacityState(bool placedText, bool placedIcon, bool skipFade);
    JointOpacityState(const JointOpacityState& prevOpacityState, float increment, bool placedText, bool placedIcon);
    bool isHidden() const;
    OpacityState icon;
    OpacityState text;
};

class VariableOffset {
public:
    std::array<float, 2> offset;
    float width;
    float height;
    style::TextVariableAnchorType anchor;
    float textBoxScale;
    optional<style::TextVariableAnchorType> prevAnchor;
};

class JointPlacement {
public:
    JointPlacement(bool text_, bool icon_, bool skipFade_) : text(text_), icon(icon_), skipFade(skipFade_) {}

    bool placed() const { return text || icon; }

    const bool text;
    const bool icon;
    // skipFade = outside viewport, but within CollisionIndex::viewportPadding px of the edge
    // Because these symbols aren't onscreen yet, we can skip the "fade in" animation,
    // and if a subsequent viewport change brings them into view, they'll be fully
    // visible right away.
    const bool skipFade;
};

struct PlacedSymbolData {
    // Contents of the label
    std::u16string key;
    // If symbol contains text, text collision box in viewport coordinates
    optional<mapbox::geometry::box<float>> textCollisionBox;
    // If symbol contains icon, icon collision box in viewport coordinates
    optional<mapbox::geometry::box<float>> iconCollisionBox;
    // Symbol text was placed
    bool textPlaced;
    // Symbol icon was placed
    bool iconPlaced;
    // Symbol text or icon collision box intersects tile borders
    // (initialized only in tile mode)
    optional<bool> intersectsTileBorder;
    // Viewport padding ({viewportPadding, viewportPadding} is a coordinate of the tile's top-left corner)
    float viewportPadding;
    // Layer id (leader of the symbol layout group)
    std::string layer;
    // In Continous mode, contains the previous opacity of the symbol,
    // in case it was already placed.
    optional<JointOpacityState> prevOpacity;
};

} // namespace mbgl