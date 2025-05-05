## Билет 21. Использование шаблонов
* Передача функторов без `std::function` в качестве: поля, аргумента функции по значению
* Шаблоны функций, базовый автовывод параметров шаблона из аргументов
* Class Template Argument Deduction (CTAD) из конструкторов: при инициализации переменных, при создании временных объектов
* Шаблонные методы/конструкторы в обычных и шаблонных классах
    * Определение внутри и вне класса
* Шаблонные лямбда выражения (`27-220516/00-misc/01-template-lambda.cpp`)
* Проблемы с `>>` до C++11
* Псевдонимы типов: `template<> using`
* Шаблоны переменных (C++17)
* Переиспользование шаблонов между единицами трансляции, автоматический `inline`, forward declaration

Многое взято из лекции 22-220314

### Передача функторов без `std::function` в качестве: поля, аргумента функции по значению

Но конкретно эта часть взята из `https://youtu.be/bmTpRLWAO5o?t=4881`

Из `https://github.com/hse-spb-2021-cpp/lectures/blob/master/14-220110/03-functors`

В целом, мы уже умели передавать функции (функторы), но через `std::function`. Но он медленный. Хотим быстрее.

Решение --- полиморфизм во время компиляции. 

```c++
#include <iostream>

template<typename F>
void apply(F operation) {
    std::cout << "Calling with 10\n";
    operation(10);
}

struct Print {
    void operator()(int x) const {
        std::cout << x << "\n";
    }
};

struct PrintTwice {
    void operator()(int x) const {
        std::cout << x << ", " << x << "\n";
    }
};

void no_op(int) {}
// void no_op(double) {}

int main() {
    apply/*<void(*)int>*/(no_op);
    apply/*<Print>*/(Print{});
    apply/*<Print>*/(Print{});
    apply/*<PrintTwice>*/(PrintTwice{});
    // apply(12);
}
```

Теперь в `apply` можем передать всё, что скопируется и у чего есть оператор круглые скобки. И нам даже не нужно писать параметры шаблона, поскольку автовывод работает, и всё круто. Что такое автовывод? Идёт дальше по билету.

Из возможных минусов --- у нас сгенерируются три разных функции `apply`, а ещё мы не можем хранить состояние функтора, т.к. он принимается по значению, т.е. копируется. 

А ещё проблемы начинаются, когда у нас перегружаются функции, т.к. тогда у нас разные типы получаются. 

Вторая проблемя явно видна на следующем куске кода:

```c++
#include <iostream>
#include <functional>

template<typename F>
void apply(F operation) {
    std::cout << "Calling with 10\n";
    operation(10);
}

struct Print {
    int printed = 0;
    // std::ref

    void operator()(int x) {
        std::cout << x << "\n";
        printed++;
    }
};

int main() {
    Print p;
    std::cout << "printed=" << p.printed << "\n";
    apply(p);
    // apply(std::ref(p));
    std::cout << "printed=" << p.printed << "\n";
}
```

Если хотим всё же передать по ссылке --- используем `reference_wrapper` из `std` и тогда уже он там скопируется.

Хорошо, а как хранить в качестве поля структуры? Ровно так же, но в качестве поля структуры. А параметры шаблона теперь у структуры. Ограничений, до вызова `()`, на `F` и `T` нет.

```c++
#include <iostream>

template<typename F, typename T>
struct Caller {
    F operation;
    T value;

    void operator()() {
        operation(value);
    }
};

struct Printer {
    void operator()(int a) const {
        std::cout << a << "\n";
    }
};

int main() {
    Caller<Printer, int> c{Printer{}, 10};
    c();
    c();
}
```

### Шаблоны функций, базовый автовывод параметров шаблона из аргументов

Взято из `https://github.com/hse-spb-2021-cpp/lectures/blob/master/22-220314/02-function-template/01-argument-deduction.cpp`

```c++
#include <vector>
#include <utility>

// function template
template<typename T>
void swap(T &a, T &b) {
    T tmp = std::move(a);
    a = std::move(b);
    b = tmp;
}

template<typename T, typename U>
void foo(const std::vector<T> &, const std::vector<U> &) {
}

int main() {
    int x, y;
    [[maybe_unused]] short z;
    swap<int>(x, y);

    // template argument deduction
    swap(x, y);  // Arg1=int, Arg2=int => T = int
    // swap(x, z);  // compilation error: deduced conflicting types for T
    swap<int>(x, z);  // compilation error: cannot bind int& to short

    foo(std::vector<double>{}, std::vector<float>{});  // T=double, U=float
}
```

В целом всё примерно как и с шаблонами классов --- `swap` сам по себе это не функция, а лишь её шаблон, когда затрагиваем с типами происходит инстанцирование. 

При использовании можно в угловых скобках перечислить все угловые параметры, а можно и не перечислять и писать просто `swap` если из аргументов однозначно и непротиворечиво выводятся все нужные типы. Важно, чтобы типы выводились непротиворечиво, никаких преобразований на данном этапе не происходит. Так что при попытке передать `int` и `short` мы проиграли, т.к. это разные типы. И пофиг, что они кастуются друг к другу.

Как можно заметить по функции `foo`, компилятор умеет решать и чуть более сложные уравнения: "Каким должен быть `T`, чтобы можно было бы привязать `std::vector<double>` к `std::vector<T>`?". 

Нюансы: `const` это тоже часть типа. 

Взято из `https://github.com/hse-spb-2021-cpp/lectures/blob/master/22-220314/02-function-template/02-const-deduction.cpp`
```c++
#include <iostream>

template<typename T>
void print(/* const */ T &a) {
    a++;  // Assume `a` is non-const (which actually may be false).
    std::cout << a << std::endl;
}

template<typename T>
void print_off(T a) {
    a++;
    std::cout << a << std::endl;
}

int main() {
    // auto &a = 10;
    // print(10);  // Arg=int. T=int. int &x;

    // auto a = 20;
    print_off(20);  // Arg=int. T=int. int x;

    const int x = 30;
    // auto &a = x;
    // print(x);  // Arg=const int. T=const int. const int &x; Compilation error inside.

    // auto a = x;
    print_off(x);  // Arg=const int. T=int. int x;
}
// More details: approx 15m of CppCon 2014: Scott Meyers: Type Deduction and Why You Care: https://www.youtube.com/watch?v=wQxj20X-tIU
```

В частности, проблемы могут возникнуть, если вы ожидали, что передадут что-то не являющееся константным, а передали константу. Но на самом деле это не особо проблема --- просто при попытке изменить константу вы где-то внутри получим ошибку.

А ещё можно указать часть параметров шаблона, но за этим и другими подробностями обращайтесь в билет 24

### Class Template Argument Deduction (CTAD) из конструкторов: при инициализации переменных, при создании временных объектов

Class Template Argument Deduction --- появился с семнадцатых плюсов, позволяет не указывать явно параметры шаблона у шаблонных классов. Могли не указывать у функций, а теперь и у классов можем не указывать. Но есть нюансы.

Взято из `https://github.com/hse-spb-2021-cpp/lectures/blob/master/22-220314/02-function-template/05-ctad.cpp`

```c++
#include <utility>

// C++17: class template argument deduction (CTAD)
template<typename TA, typename TB>
struct pair {
    TA a;
    TB b;

    pair() {}
    pair(TA a_) : a(std::move(a_)) {}  // Will not be used by CTAD by default: unable to deduce TB.
    pair(TA a_, TB b_) : a(std::move(a_)), b(std::move(b_)) {}
};
// CTAD needs two things:
// 1. Deduce class(!) template arguments from constructor arguments.
// 2. Choose appropriate constructor. Note that constructors may be templated themselves.

// "Deduction guides" for (1) are generated from constructors by default: https://en.cppreference.com/w/cpp/language/class_template_argument_deduction
// template<typename TA, typename TB> pair(TA, TB) -> pair<TA, TB>;
// Can also add explicit deduction guide:
// template<typename TA> pair(TA) -> pair<TA, int>;  // (XXX)

template<typename TA, typename TB>
pair<TA, TB> make_pair(TA a_, TB b_) {
    return {std::move(a_), std::move(b_)};
}

int main() {
    [[maybe_unused]] auto p1 = pair(10, 20);
    [[maybe_unused]] pair p2(10, 20);
    [[maybe_unused]] pair<double, int> p3(10, 20);
    // [[maybe_unused]] pair<double, int> p4 = pair(50.0);  // Works only if there is deduction guide (XXX).
    // [[maybe_unused]] pair<double> p5(10, 20);  // Either all arguments are specified, or use full CTAD only. No partial.

    // Work around + life before C++17: using argument deduction for functions.
    [[maybe_unused]] auto p5 = make_pair(10, 20);
    [[maybe_unused]] auto p6 = make_pair<double>(10, 20);

    struct ConvertibleToPair {
        operator pair<int, double>() { return {10, 20.5}; }
    };
    // [[maybe_unused]] pair p7 = ConvertibleToPair{};  // CTAD does not work through conversions, guides are pre-generated.
    [[maybe_unused]] pair<int, double> p8 = ConvertibleToPair{};
}
```

Необходимо, чтобы по параметрам однозначно определялись шаблонные параметры для автогенерируемых. А ещё можно писать свои deduction guides. CTAD работает либо полностью, либо никак. Нельзя вывести параметры частично. В общем это на самом деле штука более мощная, чем просто вывод параметров для функции. А до семнадцатых плюсов люди жили функциями `make_pair, make_tuple` и т.д.

Можно использовать как при создании переменных, так и при создании временных объектов. 

### Шаблонные методы/конструкторы в обычных и шаблонных классах

Можно в структурах писать `template` перед методом/конструктором. Так можно получать конструкторы, операторы и не только от других шаблонных типов. Вроде всё.

Взято из `https://github.com/hse-spb-2021-cpp/lectures/blob/master/22-220314/02-function-template/04-template-members.cpp`

```c++
#include <iostream>

template<typename T, int Value>
struct Foo {
    Foo() {}

    Foo(const Foo<T, Value> &) {
        std::cout << "copy constructor" << std::endl;
    }

    template<int Value2>
    Foo(const Foo<T, Value2> &) {
        std::cout << Value2 << " --> " << Value << std::endl;
    }

    template<typename W>
    void foo(const W &) {
    }

    template<typename U, typename W>
    void bar(const W &) {
    }

    template<typename T2, int Value2>
    bool operator==(const Foo<T2, Value2> &) {
        return true;
    }
};

int main() {
    [[maybe_unused]] Foo<int, 10> x;
    [[maybe_unused]] Foo<int, 11> y;
    [[maybe_unused]] Foo<char, 11> z;

    // Cannot specify explicit template arguments for constructors.
    [[maybe_unused]] Foo<int, 10> x1 = x;
    [[maybe_unused]] Foo<int, 10> x1b(x);
    [[maybe_unused]] Foo<int, 10> x2 = y;
    [[maybe_unused]] Foo<int, 10> x2b(y);
    // [[maybe_unused]] Foo<int, 10> x3 = z;

    x.foo(z);  // argument deduction works
    x.foo<>(z);  // argument deduction works
    x.bar<int>(z);  // partial argument deduction works

    x == y;  // Oops: Foo<int, 10> == Foo<int, 11>
    x == z;  // Oops: Foo<int, 10> == Foo<char, 11>
}
```

#### Определение внутри и вне класса

Нужно писать `template` перед каждым методом, который хотим объявить вне своего шаблонного класса. А если там ещё и свои шаблонные аргументы, то их придётся писать ещё строками.

Взято из `https://github.com/hse-spb-2021-cpp/lectures/blob/master/22-220314/03-template-details/14-definition.cpp`

```c++
template<typename T>
struct Foo {
    void foo(const Foo &other);
    Foo create_foo();
    static int static_field;

    template<typename U, typename V>
    void bar();
};

template<typename T>
void Foo<T>::foo(const Foo &) {
}

template<typename T>
Foo<T> Foo<T>::create_foo() {
}

template<typename T>
int Foo<T>::static_field = 10;

template<typename T>  // Class template arguments
template<typename U, typename V>  // Function template arguments
void Foo<T>::bar() {
}

int main() {
    Foo<int> x;
    x.foo(x);

    Foo<char>::static_field = 10;
}
```

### Шаблонные лямбда выражения (`27-220516/00-misc/01-template-lambda.cpp`)

Синтаксис как у обычных лямбд, но вместо типов написано `auto`. После этого "оператор ()" становится шаблонным. Но как-то умно, вида `vector<auto>` сделать пока не получится. Да и типы уже там независимые. Зато после двадцатых плюсов всё будет круто. 

```c++
#include <iostream>

int main() {
    // Since C++14
    auto print1 = [](auto x, auto y) {  // Independent implicit template parameter for each argument.
        std::cout << x << " " << y << "\n";
    };
    print1(10, 20.0);
    print1(10, "hello");

    // Since C++20: https://stackoverflow.com/a/54126333/767632
    auto print2 = []<typename T>(const T &x, const T &y) {
        std::cout << x << " " << y << "\n";
    };
    print2(10, 20);
    // print2(10, 20.0);
}
```

### Проблемы с `>>` до C++11

До одиннадцатых плюсов `>>` парсилось только как побитовый сдвиг, и никак иначе, поэтому были проблемы с шаблонами (приходилось ставить пробелы). После же пофиксили, и теперь всё норм. А парсер плюсов стал ещё неприятнее, вероятно.

Взято из `https://github.com/hse-spb-2021-cpp/lectures/blob/master/22-220314/03-template-details/10-double-angle-bracket.cpp`

```c++
#include <vector>

int main() {
    std::vector<std::vector<int>> x;  // Invalid prior to C++11 because of >> token.
}
```

### Псевдонимы типов: `template<> using`

Можно заводить `template`-ные `using`-и, чтобы заводить себе автоматом `using`-и для шаблонов. Вещь удобная, но работает только для `using`. `typedef` не пройдёт!

Взято из `https://github.com/hse-spb-2021-cpp/lectures/blob/master/22-220314/03-template-details/12-class-alises.cpp`

```c++
#include <vector>
#include <utility>

using vi1 = std::vector<int>;
typedef std::vector<int> vi2;

// v<T> = std::vector<T>;
template<typename T>
using v = std::vector<T>;
// `using` only, not with `typedef`

template<typename T1, typename T2>
using vp = std::vector<std::pair<T1, T2>>;

int main() {
    v<int> v;
    vp<int, int> v2;
}
```

### Шаблоны переменных (C++17)

Можно заводить шаблонные переменные с семнадцатых плюсов. Удобно для синтаксиса. По сути, до этого момента для реализации, например, `default_value` пришлось бы или заводить шаблонную функцию, или делать шаблонный класс с полем в нём. А особенно полезно это в метапроге.

Взято из `https://github.com/hse-spb-2021-cpp/lectures/blob/master/22-220314/03-template-details/13-variables.cpp`

```c++
#include <iostream>
#include <type_traits>

// Since C++17
template<typename T>
const T default_value{};  // may be non-const as well.

int main() {
    // Alternative: default_value<int>()
    // Alternative: type_info<int>::default_value;
    [[maybe_unused]] auto x = default_value<int>;
    [[maybe_unused]] auto y = default_value<double>;

    // Useful in metaprogramming: function of types/compile consts.
    std::cout << std::is_trivially_copyable_v<int> << std::endl;
    // std::cout << std::is_trivially_copyable<int>::value << std::endl;
    std::cout << std::is_trivially_copyable_v<std::istream> << std::endl;
}
```

### Переиспользование шаблонов между единицами трансляции, автоматический `inline`, forward declaration

Определение и объявление шаблонной функции стоит проводить в одном файле, в `.h`. Всё будет нормально, поскольку, как можно считать, ко всем шаблонным функциям неявно дописывается `inline`. Это нужно, чтобы шаблонную функцию можно было вызывать от любых шаблонных параметров, даже от тех типов, которые видны только в своей единице трансляции (за anonymous namespace, например). Поэтому проинстанцировать нужно будет уже там.

Вывод --- шаблоны определяйте в заголовке. Объявить можно в условном `print_impl.h`, для визуального разделения объявления и определения.

Взято из `https://github.com/hse-spb-2021-cpp/lectures/tree/master/22-220314/04-template-multiple-tu`

`print.h`
```c++
#ifndef PRINT_H_
#define PRINT_H_

#include <iostream>

// Rules for template argument names are (according to my experiments) similar to functions:
// 1. Names do not matter.
// 2. Default value can only be specified in either declaration or definition, not both.
// Rules for ODR are old as well: sequences of tokens of definitions in TUs should be the same.
template<typename T>
struct my_template;  // Optional.

template<typename T>
struct my_template {
    void foo();
    void bar() {}  // Can be defined inside the struct
};

template<typename T>
void my_template<T>::foo() {  // No need in 'inline' because of template
    std::cout << "foo " << typeid(T).name() << "\n";
}

template<typename T> void print(const T &value);  // Optional.

// Can be in a separate print_impl.h to decrease visual clutter.
//
// General implementation of print<> should always be in the header,
// so it can be implicitly instantiated on demand. Otherwise: undefined reference.
// See https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl
template<typename T> void print(const T &value) {
    std::cout << value << "\n";
}

// Out-of-scope: explicit instantiations for faster compilation: https://stackoverflow.com/a/59614755/767632
//               E.g. `std::string=std::basic_string<char>`

#endif  // PRINT_H_
```

`foo.cpp`
```c++
#include "print.h"
#include <iostream>

namespace {
struct my_secret_type {};
std::ostream &operator<<(std::ostream &os, const my_secret_type &) {
    return os << "secret";
}
}

void foo() {
    print(2);
    print(2.3);
    print(my_secret_type());
    my_template<int>{}.foo();
}
```

`main.cpp`
```c++
#include "print.h"

void foo();

int main() {
    print(1);
    print(1.23);
    my_template<int>{}.foo();
    foo();
}
```
