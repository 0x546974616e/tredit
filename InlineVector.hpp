#ifndef TR_INLINE_VECTOR_HPP
#define TR_INLINE_VECTOR_HPP

#include <cassert> // assert()
#include <cstddef> // std::size_t
#include <iterator> // std::contiguous_iterator<>
#include <memory> // std::addressof(), std::construct_at(), std::destruct_n()
#include <new> // std::launder()
#include <type_traits> // std::aligned_storage{}, std::conditional_t<>, std::is_const_v<>, std::remove_reference_t<>
#include <utility> // std::in_place_t<>, std::forward()

#include "InlineVectorIterator.hpp"
#include "helper.hpp"

TR_BEGIN_NAMESPACE()

template <typename Source, typename Destination>
using CopyConst = std::conditional_t<
  std::is_const_v<std::remove_reference_t<Source>>,
    std::add_const_t<Destination>,
    Destination
>;

template <typename Source, typename Destination>
using CopyReference = std::conditional_t<
  std::is_rvalue_reference_v<Source>,
    std::add_rvalue_reference_t<Destination>,
    std::conditional_t<
      std::is_lvalue_reference_v<Source>,
        std::add_lvalue_reference_t<Destination>,
        Destination
    >
>;

template <typename Source, typename Destination>
using CopyConstReference = CopyReference<Source, CopyConst<Source, Destination>>;

// TODO: What about POD/Trivial?
// TODO: noexcept()
// TODO: Exception Guarentee?
// TODO: Swappable?

template <typename _Type, std::size_t _Capacity>
  requires (sizeof(_Type) > 0u) and (_Capacity > 0u)
class InlineVector final {
public:
  constexpr InlineVector(void) noexcept: m_size(0u) {}

  constexpr InlineVector(std::size_t size): m_size(size) {
    assert(m_size <= _Capacity);
    for (std::size_t position = 0u; position < m_size; ++position) {
      std::construct_at(data() + position);
    }
  }

  constexpr InlineVector(std::size_t size, _Type const& value): m_size(size) {
    assert(m_size <= _Capacity);
    for (std::size_t position = 0u; position < m_size; ++position) {
      std::construct_at(data() + position, value);
    }
  }

  constexpr InlineVector(std::size_t size, _Type&& value): m_size(size) {
    assert(m_size <= _Capacity);
    std::size_t position = 0u;

    for (; position + 1u < m_size; ++position) {
      std::construct_at(data() + position, value);
    }

    if (position < m_size) {
      std::construct_at(data() + position, std::move(value));
    }
  }

  template <typename... Args>
  constexpr InlineVector(std::in_place_t, std::size_t size, Args&&... args): m_size(size) {
    assert(m_size <= _Capacity);
    std::size_t position = 0u;

    for (; position + 1u < m_size; ++position) {
      std::construct_at(data() + position, args...);
    }

    if (position < m_size) {
      std::construct_at(data() + position, std::forward<Args>(args)...);
    }
  }

  template <typename Item>
  constexpr InlineVector(std::initializer_list<Item> list): m_size(list.size()) {
    assert(m_size <= _Capacity);
    std::size_t position = 0u;

    for (auto& item : list) {
      // TODO: Should last item be moved?
      std::construct_at(data() + position, item);
      ++position;
    }
  }

  constexpr InlineVector(InlineVector const& vector): m_size(vector.m_size) {
    for (std::size_t position = 0u; position < m_size; ++position) {
      std::construct_at(data() + position, vector[position]);
    }
  }

  constexpr InlineVector(InlineVector&& vector): m_size(vector.m_size) {
    for (std::size_t position = 0u; position < m_size; ++position) {
      std::construct_at(data() + position, std::move(vector[position]));
    }
  }

  // constexpr InlineVector& operator==(InlineVector const& vector)
  // constexpr InlineVector& operator==(InlineVector&& vector)

  ~InlineVector(void) {
    std::destroy_n(data(), m_size);
    m_size = 0u;
  }

public:
  template <class Self> // Won't compile for rvalue reference.
  constexpr CopyConst<Self, _Type>* data2(this Self&& self) noexcept {
    return std::launder(
      reinterpret_cast<CopyConst<Self, _Type>*>(
        std::addressof( // addressof(&&) = delete
          std::forward<Self>(self).m_data[0]
        )
      )
    );
  }

  template <class Self>
  constexpr auto* data(this Self&& self) noexcept {
    return std::launder(
      reinterpret_cast<CopyConst<Self, _Type>*>(
        std::forward<Self>(self).m_data
      )
    );
  }

  template <class Self>
  constexpr auto&& at(this Self&& self, std::size_t index) noexcept {
    assert(index < self.m_size);
    assert(self.m_size <= _Capacity);
    return std::forward<CopyConstReference<Self, _Type>>(
      // TODO: Is this below statement safe? `self.data()[index]` safer?
      std::forward<Self>(self).data()[index]
    );
  }

  template <class Self>
  constexpr auto&& operator[](this Self&& self, std::size_t index) noexcept {
    assert(index < self.m_size);
    assert(self.m_size <= _Capacity);
    return std::forward<Self>(self).at(index);
  }

public:
  constexpr std::size_t size(void) const noexcept {
    return m_size;
  }

  constexpr std::size_t capacity(void) const noexcept {
    return _Capacity;
  }

  constexpr std::size_t is_empty(void) const noexcept {
    return m_size == 0u;
  }

  constexpr std::size_t is_full(void) const noexcept {
    return m_size == _Capacity;
  }

public:
  template <class Self>
  constexpr auto&& front(this Self&& self) noexcept {
    assert(self.m_size > 0u);
    assert(self.m_size <= _Capacity);
    return std::forward<Self>(self).at(0);
  }

  template <class Self>
  constexpr auto&& back(this Self&& self) noexcept {
    assert(self.m_size > 0u);
    assert(self.m_size <= _Capacity);
    std::size_t index = self.m_size - 1u;
    return std::forward<Self>(self).at(index);
  }

public:
  // ╦┌┬┐┌─┐┬─┐┌─┐┌┬┐┌─┐┬─┐┌─┐
  // ║ │ ├┤ ├┬┘├─┤ │ │ │├┬┘└─┐
  // ╩ ┴ └─┘┴└─┴ ┴ ┴ └─┘┴└─└─┘

  using Iterator = InlineVectorIterator<_Type>;
  using ConstIterator = InlineVectorIterator<_Type const>;

  static_assert(std::contiguous_iterator<Iterator>);
  static_assert(std::contiguous_iterator<ConstIterator>);

  // template <class Self>
  // requires (!std::is_rvalue_reference_v<Self>)
  // constexpr InlineVectorIterator<CopyConst<Self, _Type>>
  // begin(this Self&& self) noexcept {
  //   return std::forward<Self>(self).data();
  // }

  // template <class Self>
  // requires (!std::is_rvalue_reference_v<Self>)
  // constexpr InlineVectorIterator<CopyConst<Self, _Type>>
  // end(this Self&& self) noexcept {
  //   std::size_t size = self.m_size;
  //   return std::forward<Self>(self).data() + size;
  // }

  constexpr Iterator begin(void) noexcept { return data(); }
  constexpr Iterator end(void) noexcept { return data() + m_size; }

  constexpr ConstIterator begin(void) const noexcept { return data(); }
  constexpr ConstIterator end(void) const noexcept { return data() + m_size; }

  constexpr ConstIterator cbegin(void) const noexcept { return data(); }
  constexpr ConstIterator cend(void) const noexcept { return data() + m_size; }

  // rbegin, rend, crbegin, crend

public:
  template<typename... Args>
  constexpr void emplace_back(Args&&... args) {
    assert(m_size < _Capacity);
    // ::new (data() + m_size) _Type(std::forward<Args>(args)...);
    std::construct_at(data() + m_size, std::forward<Args>(args)...);
    ++m_size;
  }

  constexpr void push_back(_Type&& value) {
    emplace_back(std::move(value));
  }

  constexpr void push_back(_Type const& value) {
    emplace_back(value);
  }

  constexpr void insert(ConstIterator position, _Type const& value) {
    // ... _Base_iterator __res = _Base::insert(__position.base(), __x);

    (void) position; (void) value;

    // What about exception?
    // How to recover from exception?

    // data[m_size] new data[m_size - 1]
    // for loop m_size -> position
      // data[i] = data[i - 1]

    // Type tmp = value
    // for loop position -> new_size - 1u
      // swap(data[i], tme)
      // ++m_size

    // data[m_size] = tmp;
    // ++m_size
  }


  // emplace, insert, ...

  /*
	  emplace(const_iterator __position, _Args&&... __args)

	  insert(const_iterator __position, _Tp&& __x) { return emplace(__position, std::move(__x)); }
    insert(const_iterator __position, size_type __n, const _Tp& __x)

    insert(const_iterator __position, initializer_list<value_type> __l)
    { return this->insert(__position, __l.begin(), __l.end()); }

    insert(const_iterator __position, _InputIterator __first, _InputIterator __last)

    erase(const_iterator __position)
    erase(const_iterator __first, const_iterator __last)

    Copy and Swap Idiom

    typedef std::reverse_iterator<iterator>		reverse_iterator;
    typedef std::reverse_iterator<const_iterator>	const_reverse_iterator;
  */

  /*
	    _GLIBCXX_ASAN_ANNOTATE_GROW(1);
	    _Alloc_traits::construct(this->_M_impl, this->_M_impl._M_finish, __x);
	    ++this->_M_impl._M_finish;
	    _GLIBCXX_ASAN_ANNOTATE_GREW(1);
  */

private:
  std::size_t m_size = 0u;

  // TODO: Add `union` (see strict-aliasing-rule).
  // alignas(_Type) std::byte m_data[sizeof(_Type[_Capacity])];
  // alignas(_Type) std::byte m_data[sizeof(_Type)][_Capacity]; ??
  typename std::aligned_storage<sizeof(_Type), alignof(_Type)>::type m_data[_Capacity];
};

// template <typename _Type, typename... _Types>
// InlineVector(_Type, _Types...) -> InlineVector<
//   _Type, 1 + sizeof...(_Types)
// >;

template <typename... _Types>
InlineVector(_Types&&...) -> InlineVector< // `&&`?
  std::common_type_t<_Types...>,
  sizeof...(_Types)
>;

TR_END_NAMESPACE()

#endif // TR_INLINE_VECTOR_HPP

