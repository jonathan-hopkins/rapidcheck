#pragma once

#ifdef __cpp_lib_variant

#include "rapidcheck/Gen.h"

#include <variant>

namespace rc {
namespace gen {

/// Returns a generator that constructs a `variant` using one of the provided
/// generators. The gens parameters must match the size and order of the
/// variant's alternative types. Each alternative type has equal likelihood of
/// being constructed, regardless of the (implicit) size.
template <typename... VariantElements, typename... Ts>
Gen<std::variant<VariantElements...>> variant(Gen<Ts>... gens);

} // namespace gen
} // namespace rc

#include "Variant.hpp"

#endif // __cpp_lib_variant
