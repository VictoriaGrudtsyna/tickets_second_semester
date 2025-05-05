# 70. Препроцессор: базовые макросы

## Вопросы

* Конкатенация строковых литералов
* Как пользоваться `#ifdef DEBUG`, `#define` и ключом `-DDEBUG` (в случае GCC/Clang).
* Базовый синтаксис для `#if` и `defined()`
* Определение компилятора и компилятороспецифичные `#pragma` (например, игнорирование предупреждений в GCC)
* Проблемы с аргументами макросов: приоритет операторов, повторное вычисление.
* Проблемы с макросами из нескольких statement и трюк с `do { } while(0)`
* Проблемы с `operator,` и вызовом макросов: как добиться, чтобы `assert(true), assert(false);` работало.
* Вариадический макрос: `__VA_ARGS__` и `...`, проблемы с последней запятой (не было: решение этой проблемы).
* Проблемы с `assert(v == std::vector{1, 2, 3})`, `foo({1, 2})`, `foo(map<int, int>{})` и как их решать.

---

## Конкатенация строковых литералов

**Триграфы**

Препроцессор умеет заменять символы на другие символы, триграфы (до С++17,
так как раньше на некоторых компьютерах не было []):

* `??(` ––> `[`
* `??` ––> `]`
* `<: :>` ––> `[]`
* `??/` ––> `\`
* `\` ––> убрать перевод строки

Комментарии убираются препроцессором!

```c++
std::vector<int> v(10);
v??(0??) = 10;  // v[0] = 10;
std::cout << v[0] << "\n";

// You can ignore newline characters anywhere (including in a comment, phase 2) by \
   This line is a comment as well! \
   And this too!

std::cout << "1\n";
// Does the following line compiles??/
std::cout << "2\n";
std::cout << "3\n";
```

**Контактенация строковых литералов**

Препроцессор умеет объединять два идущих подряд литерала.

```cpp
std::string s1 = "hello" "wor\
ld";
[[maybe_unused]] std::string s2 = "he\n\t\xFF" R"foo(Hello World)foo";
```

## Как пользоваться `#ifdef DEBUG`, `#define` и ключом `-DDEBUG` (в случае GCC/Clang).

Можно ограничить выполнение части кода и «запускать» ее только в режиме DEBUG.

```cpp
#ifdef DEBUG
#define debug_only
#else
#define debug_only if (false)
#endif

int main() {
    std::cout << "1\n";
    debug_only {
        std::cout << "2\n";
        std::cout << "3\n";
    }
    std::cout << "4\n";
}
```

## Базовый синтаксис для `#if` и `defined()`

`#define` определяет макрос, который во что-то раскрывается. При этом можно объявлять переменные препроцессора,
например, SOME_VAR — именно такая переменная.

```cpp
#define SOME_VAR
```

Можно также передать переменную на этапе компиляции:

```cpp
// CMakeLists.txt: target_compile_definitions(main PUBLIC -DSOME_VAR)
// g++ -DSOME_VAR
```

Теперь с помощью этих переменных и `#if`/`#ifdef` можно контролировать
поведение программы.

```cpp
#define VALUE 123

#ifdef SOME_VAR
std::cout << "SOME_VAR was defined\n";
#else
std::cout << "SOME_VAR was not defined\n";
#endif

std::cout << VALUE << "\n";
// std::cout << SOME_VAR << "\n";  // SOME_VAR is replaced with nothing

#if defined(SOME_VAR) && 1 >= 2 && VALUE == 120 + 3
std::cout << "x\n";
#endif
```

Также есть предопределенные макросы (например, `__cplusplus` — номер стандарта). Активно используются в
библиотеках/тестах.

```cpp
// There are "predefined macros"
#ifdef __cplusplus
std::cout << "We are C++\n";
#else
std::cout << "We are not C++???\n";
// Preprocessor warning/error commands
// #error Compiling with C is not supported  // Supported
// #warning Compiling with C is badly supported  // Supported, official standard since C++23
#endif
```

## Определение компилятора и компилятороспецифичные `#pragma` (например, игнорирование предупреждений в GCC)

1) Определение версии:

```cpp
std::cout << "Detecting compiler... ";
#if defined(__clang__)
    std::cout << "LLVM/Apple clang " << __clang_version__ << "\n";
#elif defined(__INTEL_LLVM_COMPILER)
    std::cout << "Intel clang-based compiler: " << __INTEL_LLVM_COMPILER 
<< " " << __VERSION__ << "\n";
#elif defined(__INTEL_COMPILER)
    std::cout << "Intel C++ Compiler Classic: " << __INTEL_COMPILER << " " 
<< __VERSION__ << "\n";
#elif defined(__GNUC__)
    std::cout << "GNU C " << __GNUC__ << "." << __GNUC_MINOR__ << "." << 
__GNUC_PATCHLEVEL__ << "\n";
#endif
#ifdef _MSC_VER
    std::cout << "Microsoft Visual C++ " << _MSC_VER << "\n";
#endif
```

2) Заглушение предупреждения на определенных компиляторах.

`#pragma` — команда компилятору. Препроцессинг оставляет это, не считая как комментарием.

`#pragma GCC diagnostic push` – сохраняем состояние, потом восстанавливаем (потому что иногда надо только в части кода
необходимо заглушить предупреждение).

```cpp
int main() {
#ifdef __GNUC__  // Everything is very compiler-specific, see doctest.h 
for an example
    // `#pragma` is a compiler command, not preprocessor
    #pragma GCC diagnostic push  // save current warnings state to a stack
    #pragma GCC diagnostic ignored "-Wunused-variable"
#endif
    /* [[maybe_unused]] */ int x = 10;
#ifdef __GNUC__
    #pragma GCC diagnostic pop  // restore warnings state
#endif
}
```

## Проблемы с аргументами макросов: приоритет операторов, повторное вычисление.

Препроцессору плевать на синтаксис! Например, тут мы ожидаем увидеть 9, а получаем 7.

```cpp
#define THREE 1 + 2
#define ZERO 1 - 1

std::cout << THREE * 3 << "\n"; // 7

#if ZERO
std::cout << "Zero is the truth\n"; // true
#else
std::cout << "Zero is the lie\n";
#endif
```

## Проблемы с макросами из нескольких statement и трюк с `do { } while(0)`

Способ, использовавшийся в С, для написания простых функций. Можно писать с параметрами. Очень легко
огрести.

```cpp
#include <iostream>

#define max(a, b) a < b ? a : b
#define mul(a, b) a * b

int foo() {
    std::cout << "foo() evaluated\n";
    return 10;
}

int main() {
    std::cout << max(2, 1) << "\n";  // No brackets => ((std::cout << 2) < 
1) ? 2 : 1 << "\n";
		// need --> (a < b ? a : b)
    std::cout << mul(2 + 1, 2) << "\n";  // No brackets => 2 + 1 * 2
		// need ((a) < (b) ? (a) : (b))

    int x = 10;
    std::cout << max(x++, 12) << "\n";  // Macro => double change of 'x' => UB.
    std::cout << max(foo(), 12) << "\n";  // Macro => double evaluation of 
foo().
}
```

**Решение:**

```c++
#define max(a, b) ((a) < (b) ? (a) : (b))
#define mul(a, b) ((a) * (b))
```

Еще пример такой функции.

```cpp
#define print_twice(x) std::cout << x; std::cout << x;

int main() {
    print_twice(500);
    if (2 * 2 == 5)
        print_twice(123); // 123 printed once !
}
```

**Решение:**

```cpp
#define
print_twice(x) do { std::cout << x; std::cout << x; } while (0)
```

## Проблемы с `operator,` и вызовом макросов: как добиться, чтобы `assert(true), assert(false);` работало.

`#expr` —> превратилось в строковый литерал.

`__FILE__` —> предопределенный макрос имя файла.

`__LINE__` —> предопределенный макрос номер строки вызова.

```cpp
#include <cassert>

void my_assert_1(bool expr) {
    if (!expr) {
        std::cout << "Assertion failed: ???????\n";
    };
}

#define my_assert_2(expr) \
    do { \
        if (!(expr)) { \
            std::cout << "Assertion failed: " #expr " at " __FILE__ ":" << 
__LINE__ << "\n"; \
        } \
    } while(0)

int main() {
    my_assert_1(2 * 2 == 5);
    my_assert_2(2 * 2 == 5);
    assert(2 * 2 == 5);
}
```

## Вариадический макрос: `__VA_ARGS__` и `...`, проблемы с последней запятой (не было: решение этой проблемы).

Функции, принимающие что угодно, несколько аргументов. Но, опять же, аккуратно.

```cpp
// Compile with `g++ -E` to see preprocessed output.

// variadic macro
#define
foo(a, b, ...) bar(b, a, __VA_ARGS__)

int main() {
    foo(1, 2, 3, 4);
    foo(1, 2, 3);
    
    // Oopses:
    foo({ 1, 2, 3, 4 }); // 4 arguments ???
    foo(({ 1, 2, 3, 4 }), 5, 6);
    foo(1, 2);  // Can be fixed with C++20's __VA_OPT__ or GCC's extension 
    of ##.
    foo(std::map<int, int>(), std::map<int, int>());
    foo((std::map<int, int>()), (std::map<int, int>()));
    
    foo((std::map<int, int>()));
}
```

## Проблемы с `assert(v == std::vector{1, 2, 3})`, `foo({1, 2})`, `foo(map<int, int>{})` и как их решать.

Проблема в том, что препроцессинг аргументом считает все, что находится в пределах `()`, но не в пределах
`{}` / `[]`. Поэтому для того, чтобы все работало корректно, надо вокруг каждого аргумента расставлять `()`.