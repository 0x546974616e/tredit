#ifndef TR_DADA_HPP
#define TR_DADA_HPP

#include <cassert>
#include <cstddef>
#include <variant>
#include <vector>

#include "helper.hpp"

// #define TR_DADA_CHECK "🗹" // U+1F5F9 Ballot box with bold check
// #define TR_DADA_CROSS "🗷" // U+1F5F7 Ballot box with bold script X

TR_BEGIN_NAMESPACE()

template<size_t N>
struct DadaId final {
  char value[N];

  constexpr DadaId(const char (&string)[N]) {
    std::copy_n(string, N, value);
  }

  friend std::ostream& operator<<(std::ostream& stream, DadaId const& id) {
    return stream << std::string_view(id.value, N); // Is id.value null-terminated?
  }
};

namespace details {
  struct DadaValue final {
    std::size_t id {}, value {};

    constexpr DadaValue WithValue(std::size_t newValue) const noexcept {
      return { .id = id, .value = newValue };
    }

    friend constexpr bool operator==(DadaValue a, DadaValue b) noexcept = default;
    friend std::ostream& operator<<(std::ostream& stream, DadaValue dada) {
      // return stream << "(Id" << dada.id << ':' << dada.value << ')';
      return stream << '[' << dada.id << "](" << dada.value << ')';
    }
  };

  template <DadaId Id, std::size_t N>
  requires (N == 1 or N == 2)
  struct DadaTrace final {
    DadaValue values[N];

    friend constexpr bool operator==(DadaTrace a, DadaTrace b) noexcept = default;
    friend std::ostream& operator<<(std::ostream& stream, DadaTrace trace) {
      stream << Id << ' ' << trace.values[0];
      if constexpr (N >= 2) stream << " <- " << trace.values[1];
      return stream;
    }
  };

  #define TR_DADA_TRACE(ID, N) using ID = DadaTrace<(#ID), N>
  TR_DADA_TRACE(DefaultConstructor, 1);
  TR_DADA_TRACE(ParameterizedConstructor, 1);
  TR_DADA_TRACE(CopyConstructor, 2);
  TR_DADA_TRACE(MoveConstructor, 2);
  TR_DADA_TRACE(CopyAssignment, 2);
  TR_DADA_TRACE(MoveAssignment, 2);
  TR_DADA_TRACE(Destructor, 1);
  #undef TR_DADA_TRACE
}

using DadaTrace = std::variant<
  details::DefaultConstructor,
  details::ParameterizedConstructor,
  details::CopyConstructor,
  details::MoveConstructor,
  details::CopyAssignment,
  details::MoveAssignment,
  details::Destructor
>;

std::ostream& operator<<(std::ostream& stream, DadaTrace const& trace) {
  auto print = [&stream](auto& trace) -> std::ostream& { return stream << trace; };
  return std::visit(print, trace);
}

///
/// Usage:
///
/// (Not Thread-Safe)
///
/// ```cpp
/// using Fafa = Dada<"Test">;
///
/// {
///   Fafa a;       // Default Constructor
///   Fafa b(1337); // Parameterized Constructor
///   Fafa c(a);    // Copy Constructor
///   a = b;        // Copy Assignment
///   b = Fafa(42); // Move Assignment
///                 // Destructors
/// }
///
/// Fafa::DisplayTrace();
/// Fafa::AssertTrace({
///   Fafa::DefaultConstructor(1, 0),
///   Fafa::ParameterizedConstructor(2, 1337),
///   Fafa::CopyConstructor({3, 0}, {1, 0}),
///   Fafa::CopyAssignment({1, 0}, {2, 1337}),
///   Fafa::ParameterizedConstructor(4, 42),
///   Fafa::MoveAssignment({2, 1337}, {4, 42}),
///   Fafa::Destructor(4, 42),
///   Fafa::Destructor(3, 0),
///   Fafa::Destructor(2, 42),
///   Fafa::Destructor(1, 1337),
/// });
/// ```
///
template <DadaId Id>
class Dada final {
public:
  static inline std::size_t s_id = 0u;
  static inline std::vector<DadaTrace> s_trace {};

  static constexpr void DisplayId(void) {
    #define TR_DADA_SEPARATOR " ==================== "
    std::cerr << "\n" TR_DADA_SEPARATOR << Id << TR_DADA_SEPARATOR "\n\n";
    #undef TR_DADA_SEPARATOR
  }

  static constexpr void DisplayTrace(void) {
    DisplayId();
    for (auto& trace : s_trace) {
      std::cerr << trace << std::endl;
    }
  }

  static constexpr void AssertTrace(std::initializer_list<DadaTrace> trace) {
    #define TR_DADA_RED   "\x1B[91m"
    #define TR_DADA_GREEN "\x1B[92m"
    #define TR_DADA_BLUE  "\x1B[94m"
    #define TR_DADA_NONE  "\x1B[0m"

    auto found = s_trace.begin();
    auto expected = trace.begin();
    bool error = false;

    DisplayId();

    while (found != s_trace.end() && expected != trace.end()) {
      if (*found == *expected) {
        std::cerr << TR_DADA_GREEN "V" TR_DADA_NONE;
        std::cerr << " " << *found;
        std::cerr << std::endl;
        ++found; ++expected;
        continue;
      }

      std::cerr << TR_DADA_RED;
      std::cerr << "- " << *expected;
      std::cerr << TR_DADA_NONE;
      std::cerr << std::endl;

      std::cerr << TR_DADA_BLUE;
      std::cerr << "+ " << *found;
      std::cerr << TR_DADA_NONE;
      std::cerr << std::endl;

      std::cerr << std::endl;
      ++found; ++expected;
      error = true;
    }

    while (expected != trace.end()) {
      std::cerr << TR_DADA_RED;
      std::cerr << "- " << *expected;
      std::cerr << TR_DADA_NONE;
      std::cerr << std::endl;

      error = true;
      ++expected;
    }

    while (found != s_trace.end()) {
      std::cerr << TR_DADA_RED;
      std::cerr << "+ " << *found;
      std::cerr << TR_DADA_NONE;
      std::cerr << std::endl;

      error = true;
      ++found;
    }

    assert(!error && "Unexpected trace.");

    #undef TR_DADA_RED
    #undef TR_DADA_GREEN
    #undef TR_DADA_BLUE
    #undef TR_DADA_NONE
  }

public:
  #define TR_DADA_TRACE_1(CLASS)                                              \
    static constexpr auto CLASS(std::size_t id, std::size_t value) noexcept { \
      return details::CLASS {{ { id, value } }};                              \
    }                                                                         \
                                                                              \
    static constexpr auto CLASS(details::DadaValue value) noexcept {          \
      return details::CLASS { value };                                        \
    }

  #define TR_DADA_TRACE_2(CLASS)                                    \
    static constexpr auto CLASS(                                    \
      std::size_t id1, std::size_t value1,                          \
      std::size_t id2, std::size_t value2                           \
    ) noexcept {                                                    \
      return details::CLASS {{ { id1, value1 }, { id2, value2 } }}; \
    }                                                               \
                                                                    \
    static constexpr auto CLASS(                                    \
      details::DadaValue value1,                                    \
      details::DadaValue value2                                     \
    ) noexcept {                                                    \
      return details::CLASS { value1, value2 };                     \
    }

  TR_DADA_TRACE_1(DefaultConstructor);
  TR_DADA_TRACE_1(ParameterizedConstructor);
  TR_DADA_TRACE_2(CopyConstructor);
  TR_DADA_TRACE_2(MoveConstructor);
  TR_DADA_TRACE_2(CopyAssignment);
  TR_DADA_TRACE_2(MoveAssignment);
  TR_DADA_TRACE_1(Destructor);

  #undef TR_DADA_TRACE_1
  #undef TR_DADA_TRACE_2

public:
  constexpr operator details::DadaValue(void) const noexcept {
    return m_inner;
  }

  constexpr Dada(void): m_inner({ ++s_id, std::size_t {} }) {
    s_trace.emplace_back(details::DefaultConstructor { m_inner });
  }

  // explicit
  constexpr Dada(std::size_t value): m_inner({ ++s_id, value }) {
    s_trace.emplace_back(details::ParameterizedConstructor { m_inner });
  }

  constexpr Dada(Dada const& other): m_inner({ ++s_id, std::size_t {} }) {
    s_trace.emplace_back(details::CopyConstructor { m_inner, other });
    m_inner.value = other.Value();
  }

  constexpr Dada(Dada&& other): m_inner({ ++s_id, std::size_t {} }) {
    s_trace.emplace_back(details::MoveConstructor { m_inner, other });
    m_inner.value = other.Value();
  }

  constexpr Dada& operator=(Dada const& other) {
    s_trace.emplace_back(details::CopyAssignment { m_inner, other });
    m_inner.value = other.m_inner.value;
    return *this;
  }

  constexpr Dada& operator=(Dada&& other) {
    s_trace.emplace_back(details::MoveAssignment { m_inner, other });
    m_inner.value = other.m_inner.value;
    return *this;
  }

  ~Dada(void) {
    s_trace.emplace_back(details::Destructor { m_inner });
  }

public:
  constexpr std::size_t Value(void) const noexcept {
    return m_inner.value;
  }

  constexpr void Value(std::size_t value) noexcept {
    m_inner.value = value;
  }

  friend std::ostream& operator<<(std::ostream& stream, Dada const& dada) {
    return stream << dada.m_inner;
  }

private:
  details::DadaValue m_inner;
};

TR_END_NAMESPACE()

#endif // TR_DADA_HPP
