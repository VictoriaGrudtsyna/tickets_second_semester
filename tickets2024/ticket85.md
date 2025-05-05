## Билет 85. SFINAE – основы 

Лекция Кристины: 31.md

### Исходное применение

*SFINAE* – substitution failure is not an error – ошибка подстановки. 

Используется, чтобы вырезать неподходящие перегрузки шаблонных функций. 

Если в какой-то момент компилятору не удалось подставить тип в сигнатуру шаблонной функции, такая перегрузка игнорируется. 


```c++
template<typename T>
void duplicate_element(T &container, typename T::iterator iter) {
    container.insert(iter, *iter);
}

template<typename T>
void duplicate_element(T *array, T *element) {
   assert(array != element);
   *(element - 1) = *element;
}

int main() 
{
    std::vector a{1, 2, 3};
    duplicate_element(a, a.begin() + 1);

    
    int b[] = {1, 2, 3};
    duplicate_element(b, b + 1);  
}
```



На моменте 

```c++
duplicate_element(b, b + 1);  
```

определяется тип `T = int`, пробуем подставить его в первую перегрузку.

`int[]::iterator` не существует, и эта перегрузка вырезается по SFINAE.







### Hard compilation error vs SFINAE

SFINAE появляется, если происходит ошибка при непосредственной подстановке типа `T` (как в примере выше) или при использовании `using ... = ...` ([type alias](https://en.cppreference.com/w/cpp/language/type_alias)).




#### Корректный код:

В первой перегрузке `GetBotva<T>` проверяется через `type alias`. Если такой тип не существует, то это всё ещё SFINAE, а не ошибка компиляции.

```c++
#include <iostream>

struct BotvaHolder {
    using botva = int;
};

template<typename T> using GetBotva = typename T::botva;

template<typename T>
void foo(T, GetBotva<T>) { 
    std::cout << "1\n";
}

template<typename T>
void foo(T, std::nullptr_t) {
    std::cout << "2\n";
}

int main() {
    foo(BotvaHolder(), 10);  // 1
    foo(BotvaHolder(), nullptr);  // 2
    foo(10, nullptr); // 2
    
    // foo(10, 10); --> CE
    // SFINAE in the first overload: `int` does not have `int::botva`
    // SFINAE in the second overload: 10 is not `nullptr_t`
}
```




#### Hard compilation error:

*Hard compilation error* - ошибка, которая раскрывается вне сигнатуры шаблонной функции и не удаляется по SFINAE.

Чтобы понять, кто такой `GetBotva<T>::type` в первой перегрузке, нужно пройти два уровня: `foo` – `GetBotva<T>` – `type`. 

Если `T::botva` не существует, то ошибка компиляции появляется не внутри шаблонной функции, поэтому это не SFINAE.

```c++
#include <iostream>

struct BotvaHolder {
    using botva = int;
};

template<typename T>
struct GetBotva {
    using type = typename T::botva;  
};

template<typename T>
void foo(T, typename GetBotva<T>::type) {
    std::cout << "1\n";
}

template<typename T>
void foo(T, std::nullptr_t) {
    std::cout << "2\n";
}

int main() {
    foo(BotvaHolder(), 10);  // 1
    foo(BotvaHolder(), nullptr);  // 2
    foo(10, nullptr); // CE, but should be 2
    // foo(10, 10);  // CE
}
```







### Варианты использования SFINAE для отсеивания перегрузок



#### По возвращаемому значению:

```c++
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

struct A { int val = 0; };
struct B { int val = 0; };
struct C { int val = 0; };
struct D { int val = 0; };



//----------------------------------------- Option 1 -----------------------------------------

template<typename T, typename U>
auto operator+(const T &a, const U &b) -> decltype(a.val + b.val) {  
    return a.val + b.val;
}

// компилятор подставляет T и U и проверяет всю сигнатуру функции на корректность: 
// есть ли поле val у структур a и b

//---------------------------------------------------------------------------------------------





struct E { int val2 = 0; };
struct F { int val2 = 0; };



//----------------------------------------- Option 2 -----------------------------------------

template<typename T, typename U>
decltype(std::declval<T>().val2 + std::declval<U>().val2) operator+(const T &a, const U &b) { 
    return a.val2 + b.val2;
}

// Проверка возвращаемого значения зависит только от типов T И U (не зависит от самих структур)

//---------------------------------------------------------------------------------------------





int main() {
    A a{10};
    C c{30};
    assert(a + c == 40);  // should work

    std::string s = "hello";
    std::vector<int> vec{1, 2, 3};
    s = s + "world";  // should work
    // s = s + vec;  // standard error message

    [[maybe_unused]] E e{40};
    [[maybe_unused]] F f{50};
    assert(e + f == 90);  // works
}
```




#### Для void функций (используя оператор `,` и тип `void`):

В `decltype` записываем выражение, которое нужно проверить на корректность, и следом пишем тип возвращаемого значения.

```c++
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

struct A { int val = 0; };
struct B { int val = 0; };
struct C { int val = 0; };
struct D { int val = 0; };

template<typename T>
auto println(const T &v) -> decltype(std::cout << v.val, void()) {  // Option 1
    std::cout << v.val << "\n";
}

struct E { int val2 = 0; };
struct F { int val2 = 0; };

template<typename T>
decltype(std::cout << std::declval<T>().val2, void()) println(const T &v) { // Option 2
    std::cout << v.val2 << "\n";
}

int main() {
    A a{10};
    println(a);  // 10

    std::string s = "hello";
    // println(s);  // standard error message

    [[maybe_unused]] E e{40};
    println(e);
}
```








 ### enable_if

Хотим уметь складывать `val` у любых двух структур из `A`, `B`, `C`, `D`.

`enable_if` – структура, зависящая от двух типов `Cond` и `T`.  Внутри неё появляется псевдоним `type`, только если `Cond = true`.

```c++
#include <cassert>
#include <type_traits>

struct A { int val = 0; };
struct B { int val = 0; };
struct C { int val = 0; };
struct D { int val = 0; };
struct Foo { int val = 0; };

template<typename T>
constexpr bool good_for_plus = std::is_same_v<A, T> ||
                               std::is_same_v<B, T> ||
                               std::is_same_v<C, T> ||
                               std::is_same_v<D, T>;

template<bool Cond, typename T>
struct enable_if {};

template<typename T>
struct enable_if<true, T> { using type = T; };

template<bool Cond, typename T>
using enable_if_t = typename enable_if<Cond, T>::type;

template<typename T, typename U>
enable_if_t<good_for_plus<T> && good_for_plus<U>, int> operator+(const T &a, const U &b) {
// std::enable_if_t<good_for_plus<T> && good_for_plus<U>, int> operator+(const T &a, const U &b) {
    return a.val + b.val;
}

int main() {
    A a{10};
    C c{30};
    [[maybe_unused]] Foo foo{50};
    assert(a + c == 40);  // should work
    assert(a + foo == 60);  // CE: no match for 'operator+'
    assert(foo + foo == 60);  // CE: no match for 'operator+'
}
```







### SFINAE в фиктивных параметрах шаблона (подробнее в билете 67)

```c++
#include <cassert>
#include <type_traits>

struct A { int val = 0; };
struct B { int val = 0; };
struct C { int val = 0; };
struct D { int val = 0; };
struct Foo { int val = 0; };

template<typename T>
constexpr bool good_for_plus = std::is_same_v<A, T> || 
			       std::is_same_v<B, T> || 
                               std::is_same_v<C, T> || 
                               std::is_same_v<D, T>;

template<bool Cond, typename T = void>  // !! void by default!
struct enable_if {};

template<typename T>
struct enable_if<true, T> { using type = T; };

template<bool Cond, typename T = void>
using enable_if_t = typename enable_if<Cond, T>::type;

// Especially useful for constructors, they do not have return types.
template<typename T, typename U, typename = enable_if_t<good_for_plus<T> && good_for_plus<U>>>
// template<typename T, typename U, typename = std::enable_if_t<good_for_plus<T> && good_for_plus<U>>>
int operator+(const T &a, const U &b) {
    return a.val + b.val;
}

int main() {
    A a{10};
    C c{30};
    [[maybe_unused]] Foo foo{50};
    assert(a + c == 40);  // should work
    // assert(a + foo == 60);  // CE: no match for 'operator+'
    // assert(foo + foo == 60);  // CE: no match for 'operator+'
}
```

Перегрузка вырезается, если не получается вычислить тип третьего параметра.

Используем тип `void` по умолчанию. Но можно и любой другой, потому что нам не важен тип третьего параметра: для существования перегрузки достаточно того, что он просто определён. 





#### Проблема с перегрузкой

```c++
template<typename T, typename U, typename = std::enable_if_t<good_for_plus<T> && good_for_plus<U>>>
int operator+(const T &a, const U &b) {
    return a.val + b.val;
}

// Compilation error: redefinition. Default values is not a part of the signature.
template<typename T, typename U, typename = std::enable_if_t<good_for_plus<T> && std::is_integral_v<U>>>
int operator+(const T &a, const U &b) {
    return a.val + b;
}
```

`CE: Muptiple definition`. 

В сигнатуру функции не входит само значение по умолчанию, поэтому компилятор считает, что один и тот же оператор `+` объявлен дважды. 



***Идея:*** 

Хотим перенести `enable_if_t` так, чтобы оно было слева от знака равенства, но всё ещё оставалось внутри шаблонных аргументов. Тогда сделаем указатель. 

```c++
#include <cassert>
#include <type_traits>

struct A { int val = 0; };
struct B { int val = 0; };
struct C { int val = 0; };
struct D { int val = 0; };
struct Foo { int val = 0; };

template<typename T>
constexpr bool good_for_plus = std::is_same_v<A, T> || 
                               std::is_same_v<B, T> || 
                               std::is_same_v<C, T> || 
                               std::is_same_v<D, T>;

// We get: `template<typename U, typename U, void* = nullptr>
template<typename T, typename U, std::enable_if_t<good_for_plus<T> && good_for_plus<U>>* = nullptr>
int operator+(const T &a, const U &b) {
    return a.val + b.val;
}

template<typename T, typename U, std::enable_if_t<good_for_plus<T> && std::is_integral_v<U>>* = nullptr>
int operator+(const T &a, const U &b) {
    return a.val + b;
}

int main() {
    A a{10};
    C c{30};
    [[maybe_unused]] Foo foo{50};
    assert(a + c == 40);  // should work
    assert(a + 20 == 30);  // should work
    // assert(a + foo == 60);  // CE: no match for 'operator+'
    // assert(foo + foo == 60);  // CE: no match for 'operator+'
}
```




#### `void_t` вместо `enable_if_t`

`void_t` – шаблонный псевдоним, который принимает произвольное количество аргументов и возвращает `void` 

Может принимать произвольные типы аргументов, в отличие от `enable_if_t` 

```c++
#include <cassert>
#include <type_traits>

struct A { int val = 0; };
struct B { int val = 0; };
struct C { int val = 0; };
struct D { int val = 0; };

template<typename...>
using void_t = void;


//                                                            <=> void*
//                                _____________________________________________________________
//                               /                                                             \
template<typename T, typename U, void_t<decltype(std::declval<T>().val + std::declval<U>().val)>* = nullptr>
int operator+(const T &a, const U &b) {
    return a.val + b.val;
}
// Similar to std::void_t


int main() {
    A a{10};
    C c{30};
    assert(a + c == 40);  // should work
    assert(a + "x" == 60);  // CE: no match for 'operator+'
}
```

Использование `void_t` подчёркивает, что третий параметр шаблона исключительно для целей метапрограммирования и нам совершено не важно, как у него тип. 
