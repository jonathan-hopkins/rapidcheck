#include <catch2/catch.hpp>
#include <rapidcheck/catch.h>

#include "util/GenUtils.h"

#include "rapidcheck/gen/Variant.h"

using namespace rc;
using namespace rc::test;

#ifdef __cpp_lib_variant

template <typename... Fs>
struct overloaded : Fs... {
  using Fs::operator()...;
};

template <typename... Fs>
overloaded(Fs &&...f) -> overloaded<Fs...>;

TEST_CASE("gen::variant") {
  prop("passes correct size", [](const GenParams &params) {
    const auto genInt = gen::construct<int>(genSize());
    const auto genDouble = gen::cast<double>(genSize());
    const auto genFloat = gen::cast<float>(genSize());
    const auto genVariant =
        gen::variant<int, double, float>(genInt, genDouble, genFloat);
    const auto value = genVariant(params.random, params.size).value();

    std::visit(overloaded{[&params](auto v) { RC_ASSERT(v == params.size); }},
               value);

    //auto sh = rc::shrink::towards(6, 53);
    //while (auto v = sh.next()) {
    //  std::cout << *v << ", ";
    //}
    //std::cout << "\n";
    //sh = rc::shrink::towards(53, 6);
    //while (auto v = sh.next()) {
    //  std::cout << *v << ", ";
    //}
    //std::cout << "\n";
    //sh = rc::shrink::towards(-6, -53);
    //while (auto v = sh.next()) {
    //  std::cout << *v << ", ";
    //}
    //std::cout << "\n";
    //sh = rc::shrink::towards(-53, -6);
    //while (auto v = sh.next()) {
    //  std::cout << *v << ", ";
    //}
    //std::cout << "\n";

    //auto shrinkables = rc::gen::detail::integral<int>(Random(), 3).shrinks();
    //auto shrinkables = shrinkable::shrinkRecur(
    //    53,
    //    &shrink::integral<int>).shrinks();
    //while (auto shrinks = shrinkables.next()) {
    //  auto range = shrinks->shrinks();
    //  while (auto v = range.next()) {
    //    std::cout << v->value() << ", ";
    //  }
    //  std::cout << "\n";
    //}
    //std::cout << "\n";

    {
      auto sh = rc::shrink::towards(53, 0);
      while (auto v = sh.next()) {
        std::cout << *v << ", ";
      }
      std::cout << "\n";
    }

    {
      auto sh = rc::shrink::towards<unsigned int>(53, 0);
      while (auto v = sh.next()) {
        std::cout << *v << ", ";
      }
      std::cout << "\n";
    }

    {
      auto sh = rc::shrink::towards<int>(53, 6);
      while (auto v = sh.next()) {
        std::cout << *v << ", ";
      }
      std::cout << "\n";
    }

     auto shrinkables = shrinkable::shrinkRecur(
         -53, &shrink::integral<int>).shrinks();
    while (auto v = shrinkables.next()) {
      std::cout << v->value() << ", ";
    }
    std::cout << "\n";
  });

  prop("works with convertible constructors", []() {
    // generator equivalent of constructing a variant with a type different to
    // its element types. eg.
    // std::variant<double, std::string> v1{"abc"};

    const auto genFloat = gen::just(1.0f);
    const auto genCharPtr = gen::just("foo");
    const auto genVariant =
        gen::variant<double, std::string>(genFloat, genCharPtr);    
    const auto value = genVariant(Random(), 0).value();
    RC_ASSERT(std::holds_alternative<double>(value));
    RC_ASSERT(*std::get_if<double>(&value) == 1.0);
  });
}

#endif // __cpp_lib_variant
