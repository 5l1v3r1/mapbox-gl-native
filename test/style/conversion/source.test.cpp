#include <mbgl/test/util.hpp>

#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion/source.hpp>
#include <mbgl/style/sources/geojson_source.hpp>

using namespace mbgl;
using namespace mbgl::style;
using namespace mbgl::style::conversion;
namespace {

std::unique_ptr<Source> parseSource(const std::string& src, const std::string& sourceName) {
    Error error;
    auto source = convertJSON<std::unique_ptr<mbgl::style::Source>>(src, error, sourceName);
    if (source) return std::move(*source);
    return nullptr;
}

void checkConstProperty(std::unique_ptr<Source>& source, const std::string& propertyName, const mbgl::Value& expected) {
    Value value = source->getProperty(propertyName);
    EXPECT_EQ(expected, value) << "get property: " << propertyName;
}

void checkSetProperty(std::unique_ptr<Source>& source, const std::string& propertyName, const JSValue& value) {
    auto error = source->setProperty(propertyName, Convertible(&value));
    EXPECT_EQ(nullopt, error) << "set property: " << propertyName << ", error: " << error->message;
}

} // namespace

TEST(StyleConversion, SetSourceGenericProperties) {
    auto source = parseSource(R"JSON({
        "type": "vector",
        "tiles": ["http://example.com/{z}-{x}-{y}.vector.pbf"],
        "scheme": "xyz",
        "minzoom": 11,
        "maxzoom": 16,
        "attribution": "mapbox",
        "bounds": [-180, -73, -120, 73]
    })JSON",
                              "vector_source");

    ASSERT_NE(nullptr, source);
    checkConstProperty(source, "volatile", false);
    checkSetProperty(source, "volatile", JSValue(true));
    checkConstProperty(source, "volatile", true);

    checkConstProperty(source, "minimum-tile-update-interval", 0.0);
    checkSetProperty(source, "minimum-tile-update-interval", JSValue(10.5));
    checkConstProperty(source, "minimum-tile-update-interval", 10.5);

    checkConstProperty(source, "prefetch-zoom-delta", NullValue());
    checkSetProperty(source, "prefetch-zoom-delta", JSValue(0));
    checkConstProperty(source, "prefetch-zoom-delta", 0u);

    checkConstProperty(source, "max-overscale-factor-for-parent-tiles", NullValue());
    checkSetProperty(source, "max-overscale-factor-for-parent-tiles", JSValue(2));
    checkConstProperty(source, "max-overscale-factor-for-parent-tiles", 2u);

    source = parseSource(R"JSON({
        "type": "geojson",
        "data": "http://127.0.0.1:3000/geojson.json"
    })JSON",
                         "geojson_source");
    auto* geojsonSource = static_cast<GeoJSONSource*>(source.get());
    ASSERT_NE(nullptr, source);
    auto url = geojsonSource->getURL();
    ASSERT_TRUE(url);
    EXPECT_EQ("http://127.0.0.1:3000/geojson.json", *url);

    checkSetProperty(source, "data", JSValue("http://127.0.0.1:3000/geojson1.json"));
    url = geojsonSource->getURL();
    ASSERT_TRUE(url);
    EXPECT_EQ("http://127.0.0.1:3000/geojson1.json", *url);

    EXPECT_FALSE(geojsonSource->getGeoJSONData());
    JSDocument document;
    document.Parse<0>(R"({
        "type": "Point",
        "coordinates": [0, 0]
    })");
    const JSValue* geometry = &document;
    checkSetProperty(source, "data", *geometry);
    EXPECT_TRUE(geojsonSource->getGeoJSONData());
}