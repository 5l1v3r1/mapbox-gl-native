#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
class LatLng;
class AsyncRequest;

namespace style {

class ImageSource final : public Source {
public:
    ImageSource(std::string id, std::array<LatLng, 4>);
    ~ImageSource() override;

    optional<std::string> getURL() const;

    void setURL(const std::string& url);
    void setImage(PremultipliedImage&&);
    void setSourceData(SourceData) override;
    optional<Resource> getResource() const override;

    void setCoordinates(const std::array<LatLng, 4>&);
    std::array<LatLng, 4> getCoordinates() const;

    class Impl;
    const Impl& impl() const;

    void loadDescription(FileSource&) final;

    bool supportsLayerType(const mbgl::style::LayerTypeInfo*) const override;

    mapbox::base::WeakPtr<Source> makeWeakPtr() override { return weakFactory.makeWeakPtr(); }

    Value serialize() const override;

protected:
    Mutable<Source::Impl> createMutable() const noexcept final;

private:
    optional<std::string> url;
    std::unique_ptr<AsyncRequest> req;
    mapbox::base::WeakPtrFactory<Source> weakFactory{this};
};

} // namespace style
} // namespace mbgl
