#ifndef TR_INLINE_VECTOR_ITERATOR_HPP
#define TR_INLINE_VECTOR_ITERATOR_HPP

#include "helper.hpp"

TR_BEGIN_NAMESPACE()

template <typename _Type>
class InlineVectorIterator final {
private:
  // Convenient alias...
  using Iterator = InlineVectorIterator;

public:
  using value_type = _Type;
  using reference = _Type&;
  using pointer = _Type*;

  // - std::contiguous_iterator<>
  //   - std::random_access_iterator<>
  //     - std::bidirectional_iterator<>
  //       - std::forward_iterator<>
  //         - std::input_iterator<>

  using iterator_category = std::contiguous_iterator_tag;
  using difference_type = std::ptrdiff_t;

  constexpr InlineVectorIterator(void) noexcept: m_pointer(nullptr) {}
  constexpr InlineVectorIterator(pointer pointer) noexcept: m_pointer(pointer) {}
  constexpr InlineVectorIterator(Iterator const& i) noexcept: m_pointer(i.m_pointer) {}

  constexpr pointer operator->() const noexcept { return m_pointer; }
  constexpr reference operator*() const noexcept { return *m_pointer; }
  constexpr reference operator[](difference_type n) const noexcept { return m_pointer[n]; }

  constexpr Iterator& operator=(pointer pointer) { m_pointer = pointer; return *this; }
  constexpr Iterator& operator=(Iterator const& i) { m_pointer = i.m_pointer; return *this; }

  constexpr Iterator& operator++() noexcept { ++m_pointer; return *this; }
  constexpr Iterator& operator--() noexcept { --m_pointer; return *this; }

  constexpr Iterator operator++(int) noexcept { return m_pointer++; }
  constexpr Iterator operator--(int) noexcept { return m_pointer--; }

  constexpr difference_type operator+(Iterator const& i) const noexcept { return m_pointer + i.m_pointer; }
  constexpr difference_type operator-(Iterator const& i) const noexcept { return m_pointer - i.m_pointer; }

  constexpr Iterator operator+(difference_type n) const noexcept { return m_pointer + n; }
  constexpr Iterator operator-(difference_type n) const noexcept { return m_pointer - n; }

  friend constexpr Iterator operator+(difference_type n, Iterator const& i) noexcept { return n + i.m_pointer; }
  friend constexpr Iterator operator-(difference_type n, Iterator const& i) noexcept { return n - i.m_pointer; }

  constexpr Iterator& operator+=(difference_type n) noexcept { m_pointer += n; return *this; }
  constexpr Iterator& operator-=(difference_type n) noexcept { m_pointer -= n; return *this; }

  constexpr bool operator==(Iterator const& i) const noexcept { return m_pointer == i.m_pointer; }
  constexpr bool operator!=(Iterator const& i) const noexcept { return m_pointer != i.m_pointer; }

  constexpr bool operator>(Iterator const& i) const noexcept { return m_pointer > i.m_pointer; }
  constexpr bool operator<(Iterator const& i) const noexcept { return m_pointer < i.m_pointer; }

  constexpr bool operator>=(Iterator const& i) const noexcept { return m_pointer >= i.m_pointer; }
  constexpr bool operator<=(Iterator const& i) const noexcept { return m_pointer <= i.m_pointer; }

private:
  pointer m_pointer;
};

TR_END_NAMESPACE()

#endif // TR_INLINE_VECTOR_ITERATOR_HPP
