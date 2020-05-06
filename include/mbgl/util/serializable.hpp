#pragma once

#include <mapbox/compatibility/value.hpp>

namespace mbgl {

using Value = mapbox::base::Value;

class Serializable {
public:
    virtual ~Serializable() = default;
    virtual Value serialize() const = 0;
};

} // namespace mbgl
