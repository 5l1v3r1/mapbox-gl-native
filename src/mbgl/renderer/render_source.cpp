#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_source_observer.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/util/constants.hpp>

namespace mbgl {

using namespace style;

static RenderSourceObserver nullObserver;

RenderSource::RenderSource(Immutable<style::Source::Impl> impl) : baseImpl(std::move(impl)), observer(&nullObserver) {}

RenderSource::~RenderSource() = default;

void RenderSource::setObserver(RenderSourceObserver* observer_) {
    observer = observer_;
}

void RenderSource::onTileChanged(Tile& tile) {
    observer->onTileChanged(*this, tile.id);
}

void RenderSource::onTileError(Tile& tile, std::exception_ptr error) {
    observer->onTileError(*this, tile.id, error);
}

bool RenderSource::isEnabled() const {
    return enabled;
}

uint8_t RenderSource::getMaxZoom() const {
    assert(false);
    return util::TERRAIN_RGB_MAXZOOM;
}

} // namespace mbgl
