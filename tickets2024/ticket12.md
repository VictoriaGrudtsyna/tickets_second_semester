## Билет 12. Теоретическая всячина

### Возвращаемый тип auto функций и методов

У функций и методов есть возвращаемый тип. Если его лень писать, то его можно заменить на ключевое слово `auto`.
К примеру:

```c++
#include <iostream>

// Since C++14
auto foo() { // <-- deduced int
    return 1;
}

int main() {
    std::cout << foo();
}
```

> :warning: auto это не `Any` в котлине и не объект произвольного типа как в питоне, это просто синтаксический
> сахар и упрощение жизни. **Возвращаемый тип всё равно существует и должен быть вычислен на этапе компиляции**.

Собственно как происходит эта дедукция? Максимально втупую. Берётся просто тип из первого `return`.

```c++
auto different_types() {
  if (false) {
    return ""; // <-- Now auto is deduced as `const char*`
  }
  return 1; // <-- CE: already deduced as `const char*`
}

auto inconsistent_deduction() {
    return 1;
    return 'x';  // <-- CE: 'int' and 'char' can be implicitly converted, but are not the same type
}

int main() {}
```

Можно добавить модификаторов: ссылочность и `const` не входит в `auto`. Соответственно, можно писать так:

```c++
std::vector<int> a;
const auto& v() {
    return a;
}
```

Если вдруг захочется сделать рекурсивный вывод типов, то либо понадобится делать магию с шаблонами, либо чуть помочь
компилятору:

```c++
auto fib(int n) { // OK
    if (n <= 1) {
        return 1;  // first return, deduced int
    } else {
        return fib(n - 1) + fib(n - 2);  // ensured: it is int
    }
}

auto fib_bad(int n) {
    if (n > 1) {
        return fib_bad(n - 1) + fib_bad(n - 2);  // <-- CE: first return, cannot deduce, endless recursion
    } else {
        return 1;
    }
}

int main() {}
```

#### Использование `auto` и `->`, удобство при определении функций-членов снаружи класса

Можно ещё всё же написать возвращаемый тип после названия функции, если вдруг очень хочется.

```c++
// "Trailing return type", syntax sugar
auto fib(int n) -> int {
    if (n > 1) {
        return fib(n - 1) + fib(n - 2);  // first return, cannot deduce, but it's ok
    } else {
        return 1;
    }
}

int main() {
    int ans = fib(7);
}
```

Для чего это надо в жизни? Это удобно для функций-членов и шаблонов, таким образом получая сразу "обзор внутрь" класса:

```c++
template<typename T>
struct my_set {
    struct iterator {};

    iterator begin();
    iterator end();

    my_set &self();
};

// Usual
template<typename T>
typename my_set<T>::iterator my_set<T>::begin() {
    return{};
}

// Shorter version: we are "inside" my_set<T> after ::, so we can use just `iterator`
template<typename T>
auto my_set<T>::end() -> iterator {
    return {};
}

// Another alternative:
template<typename T>
// my_set<T> &my_set<T>::self() {  // Option 1
auto my_set<T>::self() -> my_set& {  // Option 2
    return *this;
}

int main() {}
```

Если разделяем declaration и definition, то нужно быть аккуратным. К моменту использования definition должен быть уже
виден.

```c++
auto foo();
auto bar();

auto foo() {
  return bar(); // <-- CE: Function 'bar' with deduced return type cannot be used before it is defined
}
auto bar() {
  return 1;
}

int main() {}
```

```c++
auto foo();
auto bar();

auto bar() {
  return 1;
}
auto foo() {
  return bar(); // OK: deduced int
}

int main() {}
```

Можно и на разные единицы трансляции разбить, но тогда возникнут проблемы, что definition не видно, а значит и тип не
вычислить:

```c++
// File: a.cpp
#include <iostream>

auto foo();
auto bar();

auto foo() {
    return 10;
}

int main() {
    std::cout << foo() << "\n";
    // std::cout << bar() << "\n";  // compilation error: `auto` is not deduced, it's in another TU.
}

// File: b.cpp
auto bar();

auto bar() {
    return 10;
}
```

Ещё можно lambda-функции указать явно тип:

```c++
int main() {
    auto g = [](int n) -> double {
    if (n > 1) {
       return 100;
    } else {
       return 200.0;
    }
    // return "foo";  // compilation error: cannot cast to double
};
}
```

### Потенциальные висячие ссылки в range-based-for по временному объекту

При range-based-for может захотеться итерироваться по полученному значению. В таком случае lifetime ссылки даже
увеличится и можно будет легально итерироваться по элементам временного объекта (см. цикл 1 и 2)

```c++
#include <iostream>
#include <vector>

std::vector<int> foo() {
    return {1, 2, 3};
}

struct VectorWrapper {
    std::vector<int> data;
    const std::vector<int> &get() const {
        return data;
    }
};

VectorWrapper bar() {
    return {{1, 2, 3}};
}

int main() {
    for (auto x : foo()) { // 1 - OK
        std::cout << x << "\n";
    }
    std::cout << "=====\n";
    const auto &vw = bar();
    for (auto x : vw.get()) { // 2 - OK
        std::cout << x << "\n";
    }
    std::cout << "=====\n";
    for (auto x : bar().get()) { // 3 - UB: Dangling reference to `VectorWrapper` from bar() 
        std::cout << x << "\n";
    }
}
```

Вывод:

```
1
2
3
=====
1
2
3
=====
5351552
0
3408216
```

В третьем же случае мы получаем `data` из `VectorWrapper`, но к сожалению мы не можем продлить время жизни этого lvalue.
Получаем UB.


### Оператор `switch`

Вместо кучи `if`, `else if`, `else if`, ... Можно сделать один маленький-сладенький дарованный из C `switch`, который
компилятор *может* соптимизировать.

Разница с `if` состоит в том, что `case ...:` работает скорее как label, то есть **при попадании в эту ветку так же мы
зайдём и во все последующие ветки**. Чтобы с этим побороться, нужно написать `break;` там, где нужно выйти из `switch`.

Почти всегда ровно так программисту и надо, поэтому GCC будет ругаться, если вы не напишите `break`. Для подавления
этого предупреждения
можно написать аттрибут `[[fallthrough]]` (то есть вы говорите компилятору, что вы действительно хотите проследовать в
следующую ветку).

```c++
#include <iostream>
#include <string>

int main() {
    for (int x = 1; x <= 5; x++) {
        std::cout << "x=" << x << "\n";

        switch (x) {  // May be a jump table, a binary search, a series of if-else...
            case 1:
            case 10:
                std::cout << "  1 or 10\n";
                [[fallthrough]];
            case 2 + 2:  // Only consts, but 2+2 is ok
                std::cout << "  4\n";
                [[fallthrough]];
            case 2:  // Although sorting `case`s alphabetically or by their semantics is more popular
                std::cout << "  2\n";
                break;
            default:  // Although it's usually the last
                std::cout << "  default\n";
                [[fallthrough]];
            case 3:
                std::cout << "  3\n";
                break;  // No warning in GCC, but it's better to write: what if we add another `case`?        }
        }
    }
    /*
    std::string s;
    switch (s) {  // Compilation Error :( Integers only.
    }
    */
}
```

Вывод:

```
x=1
  1 or 10
  4
  2
x=2
  2
x=3
  3
x=4
  4
  2
x=5
  default
  3
```

#### Проблемы с инициализацией переменных внутри `switch`

Проблема в том, что у дарования языка C очень так себе с инициализацией, так как совершенно не понятно, где и чего нужно
вызывать деструктор. Поэтому он будет пытаться чистить всё, ибо оно **в его области видимости**.

Универсальный solution: поместить весь case в фигурные скобочки `{...}`, тогда всё
инициализированное внутри успешно почистится при выходе.

В последнем можно в теории не писать, так как перепрыгнуть в состояние, где оно будет чиститься и непроинициализированно
не получится.

```c++
#include <iostream>

int main() {
    for (int x = 1; x <= 5; x++) {
        std::cout << "x=" << x << "\n";

        switch (x) {
            case 1:
                std::cout << "  1\n";
                break;
            case 4:
                std::cout << "  4\n";
                break;
            case 2: {
                [[maybe_unused]] int wtf0 = 0;
                std::cout << "  2\n";
            } break;
            default: /* { */
                [[maybe_unused]] int wtf1 = 0;  // Compilation Error in C++
                                                // May be left uninitialized in C
                std::string s;  // Even worse Compilation Error: one has to remember
                                // whether to call desturctor. Nah.
                std::cout << "  default\n";
                /* } */ break;
            case 3: // <-- Compilation Error: jump bypasses variable initialization
                [[maybe_unused]] int wtf2 = 0;  // No `case` below, so no "crosses initialization" errors.
                std::cout << "  3\n";
                break;
        }
    }
}
```

### Глобальные переменные и статические поля-члены классов; слова extern и inline
Хотим глобальные переменные. Проблема: ODR. Для обхода есть два ~~стула~~ решения: `extern` и `inline`.

`extern` – означает, что определение переменной есть, но находится где-то в другом месте программы. 

`inline` – разработчик даёт гарантию, что все definition одинаковы и можно использовать любой. 
## Слово inline иногда следует из const
Поэтому если у нас есть const, то супер использовать inline так как переменная априори не меняется
```c++
// === File: a.h
#include <string>

// WARNING: still be careful with Static Initialization Order Fiasco!

// int var1;  // multiple definition
// std::string var2 = "hello";  // multiple definition
inline std::string var3 = "world";

// just declaration
extern int var1;
extern std::string var2;

// extern int var1 = 10;  // multiple definition (defined because of initialization)
// extern std::string var2 = "hello";  // multiple definition (defined because of initialization)

void foo();

// === File: a.cpp
#include "a.h"

int var1 = 10;
std::string var2 = "hello";

int main() {}
```

`static` поле класса - поле, которое является общим для всех экземпляров класса:
```c++
struct S {
    int n;                    // defines S::n
    static int i;             // declares, but doesn't define S::i
//  static int i = 1;         // won't work. must define out of line
    inline static int x;      // defines S::x
    inline static int y = 1;  // defines S::y. we can define `inline` fields right here.
};

int S::i = 3;                 // defines S::i
```

`extern` ничего не даёт, ибо тут можно просто написать declaration.

## Отличия inline от static от unnamed namespace.

unnamed namespace и static делают видимыми внутри одной единицы трансляции 

inline все функции имеют одинаковые реализации на все единицы трансляций

### Как лучше объявлять глобальные константы и статические константы-члены классов

`constexpr` означает, что оно решится на этапе компиляции. Таким образом **гарантируется, что не будет проблем с
SIOF** (Static Initialization Order Fiasco).

`const` просто даёт константность. Проблемы с SIOF остаются (например использовать одну константу для инициализации
второй – проблема, не ясно, какая инициализируется первой)

`inline` означает, что может быть определено в разных местах и **использоваться будет какое-то одно**. Без `inline`
же **в разных TU может быть разным объектом** (т.е. жить по разному адресу).

`inline constexpr` самое лучшее, так как одно на все единицы трансляции и нет проблем с SIOF.

```c++
// Full code here: https://github.com/hse-spb-2021-cpp/lectures/tree/master/30-220606/01-globals-multiple-tu/02-consts

// === File: a.h
#include <string>

// `constexpr` means "initialize in compile-time", guarantees no problems with SIOF.

inline constexpr char str1[] = "Hello";  // The best.
// std::string is constexpr only since C++20
inline const std::string str2 = "World";  // Does not work in Visual Studio: https://abseil.io/tips/140#non-portable-mistake
constexpr char str3[] = "My";
const std::string str4 = "Dear";

struct Foo {
    static inline constexpr char str1[] = "1";
    static inline const std::string str2 = "2";
    static constexpr char str3[] = "3";  // Implies inline since C++17? I don't know. Egor said, not for exams.
    // static const std::string str4 = "4"; // <-- Doesn't work, definition must be out-of-line
    static const std::string str4;
};

void foo();

// ==== File: a.cpp
#include "a.h"

const std::string Foo::str4 = "4";
```

Адреса из разных TU:

```
From main()
Hello 0x7f9d0d6c1004
World 0x7f9d0d6c31c0
My 0x7f9d0d6c1068
Dear 0x7f9d0d6c3240
1 0x7f9d0d6c1061
2 0x7f9d0d6c31e0
3 0x7f9d0d6c1063
4 0x7f9d0d6c3160

From foo()
Hello 0x7f9d0d6c1004
World 0x7f9d0d6c31c0
My 0x7f9d0d6c1010
Dear 0x7f9d0d6c3180
1 0x7f9d0d6c1061
2 0x7f9d0d6c31e0
3 0x7f9d0d6c1063
4 0x7f9d0d6c3160
```