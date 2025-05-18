#include <algorithm> // std::copy_n()
#include <iostream>
#include <string_view>
#include <vector>

#include "Dada.hpp"
#include "InlineVector.hpp"
#include "helper.hpp"

#define TRACE(...) _TRACE(__PRETTY_FUNCTION__ __VA_OPT__(,) __VA_ARGS__)

template <typename... Args>
static void _TRACE(Args... args) {
  (std::cerr << ... << args) << std::endl;
}

#define DISPLAY(X) (std::cout << (#X) << " = " << (X) << std::endl)

int main(void) {
  TR_USE_NAMESPACE()
  DISPLAY(sizeof(InlineVector<char, 1>));

  #define TR_FAFA(ID) using Fafa = Dada<ID>;
  #define TR_ASSERT(...) Fafa::AssertTrace({ __VA_ARGS__ });

  {
    TR_FAFA("N-Default Ctor") {
      InlineVector<Fafa, 6> a(3);
      (void) a;
    }

    TR_ASSERT(
      Fafa::DefaultConstructor(1, 0),
      Fafa::DefaultConstructor(2, 0),
      Fafa::DefaultConstructor(3, 0),
      Fafa::Destructor(1, 0),
      Fafa::Destructor(2, 0),
      Fafa::Destructor(3, 0),
    );
  }

  {
    TR_FAFA("N-Copy Ctor") {
      Fafa b(789);
      InlineVector<Fafa, 6> const a(3, b);
      (void) a;
    }

    TR_ASSERT(
      Fafa::ParameterizedConstructor(1, 789),
      Fafa::CopyConstructor(2, {}, 1, 789),
      Fafa::CopyConstructor(3, {}, 1, 789),
      Fafa::CopyConstructor(4, {}, 1, 789),
      Fafa::Destructor(2, 789),
      Fafa::Destructor(3, 789),
      Fafa::Destructor(4, 789),
      Fafa::Destructor(1, 789),
    );
  }

  {
    TR_FAFA("Implicit N-Move Ctor") {
      // InlineVector(3, Dada&&) is called.
      //   Doesn't work with `explicit Dada(std::size_t)`.
      InlineVector<Fafa, 6> const a(3, 42);
      (void) a;
    }

    TR_ASSERT(
      Fafa::ParameterizedConstructor(1, 42),
      Fafa::CopyConstructor(2, {}, 1, 42),
      Fafa::CopyConstructor(3, {}, 1, 42),
      Fafa::MoveConstructor(4, {}, 1, 42),
      Fafa::Destructor(1, 42),
      Fafa::Destructor(2, 42),
      Fafa::Destructor(3, 42),
      Fafa::Destructor(4, 42),
    );
  }

  {
    TR_FAFA("Explicit N-Move Ctor") {
      InlineVector<Fafa, 6> const a(3, Fafa(64));
      (void) a;
    }

    TR_ASSERT(
      Fafa::ParameterizedConstructor(1, 64),
      Fafa::CopyConstructor(2, {}, 1, 64),
      Fafa::CopyConstructor(3, {}, 1, 64),
      Fafa::MoveConstructor(4, {}, 1, 64),
      Fafa::Destructor(1, 64),
      Fafa::Destructor(2, 64),
      Fafa::Destructor(3, 64),
      Fafa::Destructor(4, 64),
    );
  }

  {
    TR_FAFA("std::initializer_list<std::size_t> Ctor") {
      InlineVector<Fafa, 3> a({ 1, 2, 3 });
      (void) a;
    }

    TR_ASSERT(
      Fafa::ParameterizedConstructor(1, 1),
      Fafa::ParameterizedConstructor(2, 2),
      Fafa::ParameterizedConstructor(3, 3),
      Fafa::Destructor(1, 1),
      Fafa::Destructor(2, 2),
      Fafa::Destructor(3, 3),
    );
  }

  {
    TR_FAFA("emplace_back()") {
      InlineVector<Fafa, 4> a;
      a.emplace_back(1337);
      a.emplace_back(42);
    }

    TR_ASSERT(
      Fafa::ParameterizedConstructor(1, 1337),
      Fafa::ParameterizedConstructor(2, 42),
      Fafa::Destructor(1, 1337),
      Fafa::Destructor(2, 42),
    );
  }

  {
    TR_FAFA("Const at()") {
      InlineVector<Fafa, 6> const a(std::in_place, 3, 69);
      Fafa const* b = a.data();
      Fafa const& c = a.at(0);
      assert(b->Value() == 69);
      assert(c.Value() == 69);
      (void) b;
      (void) c;
    }

    TR_ASSERT(
      Fafa::ParameterizedConstructor(1, 69),
      Fafa::ParameterizedConstructor(2, 69),
      Fafa::ParameterizedConstructor(3, 69),
      Fafa::Destructor(1, 69),
      Fafa::Destructor(2, 69),
      Fafa::Destructor(3, 69),
    );
  }

  {
    TR_FAFA("Non-Const at()") {
      InlineVector<Fafa, 6> a(std::in_place, 3, Fafa(443));
      Fafa* b = a.data();
      Fafa& c = a.at(0);
      assert(b->Value() == 443);
      assert(c.Value() == 443);
      (void) b;
      (void) c;
    }

    TR_ASSERT(
      Fafa::ParameterizedConstructor(1, 443),
      Fafa::CopyConstructor(2, {}, 1, 443),
      Fafa::CopyConstructor(3, {}, 1, 443),
      Fafa::MoveConstructor(4, {}, 1, 443),
      Fafa::Destructor(1, 443),
      Fafa::Destructor(2, 443),
      Fafa::Destructor(3, 443),
      Fafa::Destructor(4, 443),
    );
  }

  {
    TR_FAFA("Copy operator[]") {
      InlineVector<Fafa, 6> a;
      a.emplace_back(80);
      Fafa b = a[0];
      b.Value(81);
    }

    TR_ASSERT(
      Fafa::ParameterizedConstructor(1, 80),
      Fafa::CopyConstructor(2, {}, 1, 80),
      Fafa::Destructor(2, 81),
      Fafa::Destructor(1, 80),
    );
  }

  {
    TR_FAFA("Move operator[]") {
      Fafa a(128);
      a = InlineVector<Fafa, 6>(2)[1];
      a.Value(127);
    }

    TR_ASSERT(
      Fafa::ParameterizedConstructor(1, 128),
      Fafa::DefaultConstructor(2, 0),
      Fafa::DefaultConstructor(3, 0),
      Fafa::MoveAssignment(1, 128, 3, 0),
      Fafa::Destructor(2, 0),
      Fafa::Destructor(3, 0),
      Fafa::Destructor(1, 127),
    );
  }

  {
    TR_FAFA("Reference front()") {
      InlineVector<Fafa, 2> a(std::in_place, 2, 123);
      Fafa& b = a.front();
      b.Value(321);
    }

    TR_ASSERT(
      Fafa::ParameterizedConstructor(1, 123),
      Fafa::ParameterizedConstructor(2, 123),
      Fafa::Destructor(1, 321),
      Fafa::Destructor(2, 123),
    );
  }

  {
    TR_FAFA("Reference back()") {
      InlineVector<Fafa, 5> a(std::in_place, 3, 999);
      Fafa& b = a.back();
      b.Value(1001);
    }

    TR_ASSERT(
      Fafa::ParameterizedConstructor(1, 999),
      Fafa::ParameterizedConstructor(2, 999),
      Fafa::ParameterizedConstructor(3, 999),
      Fafa::Destructor(1, 999),
      Fafa::Destructor(2, 999),
      Fafa::Destructor(3, 1001),
    );
  }

  {
    TR_FAFA("Move back()") {
      Dada a = InlineVector<Fafa, 3>(std::in_place, 2, 888).back();
      a.Value(777);
    }

    TR_ASSERT(
      Fafa::ParameterizedConstructor(1, 888),
      Fafa::ParameterizedConstructor(2, 888),
      Fafa::MoveConstructor(3, {}, 2, 888),
      Fafa::Destructor(1, 888),
      Fafa::Destructor(2, 888),
      Fafa::Destructor(3, 777),
    )
  }

  {
    TR_FAFA("push_back()") {
      InlineVector<Fafa, 3> a {};
      a.push_back(1122);
      {
        Fafa b(3344);
        a.push_back(b);
      }
      a.emplace_back(5566);
    }

    TR_ASSERT(
      Fafa::ParameterizedConstructor(1, 1122),
      Fafa::MoveConstructor(2, {}, 1, 1122),
      Fafa::Destructor(1, 1122),

      Fafa::ParameterizedConstructor(3, 3344),
      Fafa::CopyConstructor(4, {}, 3, 3344),
      Fafa::Destructor(3, 3344),

      Fafa::ParameterizedConstructor(5, 5566),

      Fafa::Destructor(2, 1122),
      Fafa::Destructor(4, 3344),
      Fafa::Destructor(5, 5566),
    )
  }

  { // TODO TMP
    TR_FAFA("ITERATOR foreach") {
      Fafa::DisplayId();
      InlineVector<Fafa, 4> const a(std::in_place, 2, 256);
      for (auto& b : a) {
        DISPLAY(b);
      }
    }

    Fafa::DisplayTrace();
  }

  {
    auto a = InlineVector<int, 5>{ 1, 2, 3, 4, 5 };
    auto b = InlineVector{ 1, 2, 3, 4, 5 };
    assert(a.capacity() == 5);
    assert(b.capacity() == 5);
  }

/*

  {
    BLOCK("ITERATOR reverse()");
    InlineVector<Dada, 2> const a(std::in_place, 2, 246); (void) a;
    // std::reverse(a.begin(), a.end());
  }

  {
    BLOCK("ITERATOR ++");
    InlineVector<Dada, 2> const a({1, 2});
    auto iterator = a.begin();
    DISPLAY(*(++iterator));
    DISPLAY(*(iterator++));
  }
*/

  // {
  //   BLOCK();
  //   Dada a(12); (void) a;
  //   Dada b(std::move(a));
  // }

  return 0;
}

// f Boost.Container
//  It also exists in Folly, Abseil, Qt, wxWidgets, ... however,
// boost::small_vector
