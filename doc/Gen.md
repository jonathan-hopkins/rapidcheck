# `Gen<T>`

_This section is incomplete._

`Gen<T>` is a fundamental template class in RapidCheck. A generator creates values of type `T` given a `Random` and a [size](generators.md#Size). It has value semantics (ie. is copyable).

It has one member function:

 - `Shrinkable<T> operator()(const Random& random, int size) const` returns a `Shrinkable` (see documentation for [Shrinkable](Shrinkable.md)).

 Generators are frequently copied

 See also:
  - Basics: [Generator](generators.md)
  - Reference: [Generators](generators_ref.md)
