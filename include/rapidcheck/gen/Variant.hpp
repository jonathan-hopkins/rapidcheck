#pragma once

namespace rc {
namespace gen {
namespace detail {

// not actually as restrictive as the standard requires.
// see ctor: template<class T> constexpr variant(T&& t)
// at https://eel.is/c++draft/variant.ctor
template <typename Variant, size_t Idx, typename T>
struct IsConstructorForVariantElement : public std::false_type {};

template <typename... VariantElements, size_t Idx, typename T>
struct IsConstructorForVariantElement<std::variant<VariantElements...>, Idx, T>
    : public std::is_constructible<
          std::variant_alternative_t<Idx, std::variant<VariantElements...>>,
          T> {};

template <typename Variant, size_t Idx, typename... Ts>
struct GenVariantSelector;

template <typename... VariantElements, size_t Idx, typename... Ts>
struct GenVariantSelector<std::variant<VariantElements...>, Idx, Ts...>
    : public GenVariantSelector<std::variant<VariantElements...>,
                                Idx - 1,
                                Ts...> {
  GenVariantSelector(std::tuple<Gen<Ts>...> g) noexcept
      : GenVariantSelector<std::variant<VariantElements...>, Idx - 1, Ts...>(
            g) {}

  // A compile error here means that you failed to pass a generator that creates
  // a variant holding the appropriate alternative type. In general, the Ith
  // parameter to variant<Ts...>() must be a generator for the Ith variant
  // alternative.
  // eg. variant<float, int *>(gen::just(1.0f), gen::just("fail"));
  // In the example, "fail" is not a valid construction of either an float or an
  // int*.
  std::enable_if_t<IsConstructorForVariantElement<
                       std::variant<VariantElements...>,
                       Idx,
                       std::tuple_element_t<Idx, std::tuple<Ts...>>>::value,
                   Gen<std::variant<VariantElements...>>>
  operator()(size_t idx) {
    if (idx == Idx)
      return gen::construct<std::variant<VariantElements...>>(
          std::get<Idx>(gens));
    else
      return GenVariantSelector<std::variant<VariantElements...>,
                                Idx - 1,
                                Ts...>::operator()(idx);
  };
};

template <typename... VariantElements, typename... Ts>
struct GenVariantSelector<std::variant<VariantElements...>, 0, Ts...> {
  GenVariantSelector(std::tuple<Gen<Ts>...> g) noexcept
      : gens(g) {}

  // A compile error here means that you failed to pass a generator that creates
  // a variant holding the appropriate alternative type. In general, the Ith
  // parameter to makeVariant() must be a generator for the Ith variant
  // alternative.
  // eg. makeVariant<float, int *>(gen::just(1.0f), gen::just("fail"));
  // In the example, "fail" is not a valid construction of either an float or an
  // int*.
  std::enable_if_t<IsConstructorForVariantElement<
                       std::variant<VariantElements...>,
                       0,
                       std::tuple_element_t<0, std::tuple<Ts...>>>::value,
                   Gen<std::variant<VariantElements...>>>
  operator()(size_t idx) {
    return gen::construct<std::variant<VariantElements...>>(std::get<0>(gens));
  };

protected:
  std::tuple<Gen<Ts>...> gens;
};

template <typename... Ts>
struct DefaultArbitrary<std::variant<Ts...>> {
  static Gen<std::variant<Ts...>> arbitrary() {
    return gen::variant<Ts...>(gen::arbitrary<Ts>()...);
  }
};

} // namespace detail

template <typename... VariantElements, typename... Ts>
Gen<std::variant<VariantElements...>> variant(Gen<Ts>... gens) {
  constexpr size_t num_elements =
      std::tuple_size_v<std::tuple<VariantElements...>>;
  typedef detail::GenVariantSelector<std::variant<VariantElements...>,
                                     num_elements - 1,
                                     Ts...>
      selector_t;
  // generate a variant of any type with equal likelihood:
  auto gen_element_idx =
      gen::resize(kNominalSize, gen::inRange<size_t>(0, num_elements));
  return gen::mapcat(
      gen_element_idx,
      [gs = std::make_tuple((gens)...)](size_t element_idx_selected)
          -> Gen<std::variant<VariantElements...>> {
        return selector_t{gs}(element_idx_selected);
      });
}

} // namespace gen
} // namespace rc
