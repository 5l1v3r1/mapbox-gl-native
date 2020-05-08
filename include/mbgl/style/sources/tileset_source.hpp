#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {

namespace style {

class TilesetSource : public Source {
public:
    ~TilesetSource() override;

    const Tileset* getTileset() const final;
    optional<Resource> getResource() const final;
    optional<std::string> getURL() const;

    Value serialize() const override;

protected:
    TilesetSource(Immutable<Impl>, variant<std::string, Tileset> urlOrTileset);
    Value getPropertyInternal(const std::string&) const override;
    const variant<std::string, Tileset> urlOrTileset;
};

} // namespace style
} // namespace mbgl
