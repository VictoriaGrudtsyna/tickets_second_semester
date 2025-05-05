## Билет 20. Шаблоны классов
* Шаблоны классов
    * Синтаксис объявления и определения
    * Параметры шаблона: типы (разница `typename`/`class`), значения простых типов (без строгого определения "простоты")
    * Параметры по умолчанию, отсутствие имени у параметра
    * Параметры-шаблоны, передача в них шаблонов с параметрами по умолчанию
    * Ключевые слова `typename`, `template` для обращения внутрь зависимых типов, в том числе при наследовании от шаблонного параметра (практика `22-220314`, задание `22-call-dependent-method.cpp`)
* Отличие шаблона `Foo` и инстанцирования шаблона с параметрами по умолчанию `Foo<>`
    * Когда можно писать `Foo` вместо `Foo<T>`, удобство с `auto Foo<T>::foo() -> Foo`
* Инстанцирование
    * Независимость по методам
    * Когда происходит неявное инстанцирование: полей, методов (статических и нестатических), вложенных типов
    * Когда тип шаблона может быть incomplete, а когда не может
    * Не было: явное инстанцирование
* Независимость инстанциаций шаблона
* Определение методов и статических членов шаблонного класса внутри и вне класса


Общая концепция — хотим что-то делать для +-произвольных разных типов. 

В Си вместо этого приходится пилить макросы.

А в плюсах есть шаблоны классов/шаблоны структур

### Синтаксис

Лекция 14-220110, время: 1:06:05

Взято из `https://github.com/hse-spb-2021-cpp/lectures/blob/master/14-220110/02-templates/02-template.cpp`:
```c++
#include <iostream>

// class template
template<typename/* class */ T, char default_value>
struct templ_foo {
    T value;
    bool exists;
};

int main() {
    [[maybe_unused]] templ_foo<int, 10> x;
    [[maybe_unused]] templ_foo<int, 10> y;
    [[maybe_unused]] templ_foo<int, 11> z;
    x = y;  // OK
    x = z;  // CE: different types

    struct Foo {};
    struct Bar {};
    Foo x1;
    Bar y1;
    x1 = y1;
}
```

Обычное объявление структуры, перед которым идёт ключевое слово `template` (шаблон), после чего в угловых скобках идут параметры шаблона. Параметры шаблона бывают разных видов:

* Параметр-тип (т.е. который может являться `int`, `std::string`, ...). Указываете `typename` или `class` (разницы нет, но вот `struct` писать нельзя) и дальше называете этот параметр. Пример: `typename T`. 
* Параметры, которые являются числами на этапе компиляции. Просто `char default_value`. 

После этого можно писать искомый шаблон класса как обычно, используя параметры шаблона, в частности, тип `T`, как если бы они были известны.

То, что в итоге будет написано — ещё не класс, но лишь шаблон класса. Нельзя ни узнать его размер, ничего, "типа" ещё нет самого по себе. Очень многое зависит от типа `T`, поэтому она проверяется довольно сомнительно, только на синтаксическую корректность.

Но вот как только этот шаблон начинает использоваться, когда где-то написали `temp_foo<int, 10>` (т.е. провели инстанцирование шаблона (неявное, кстати, что такое явное? а это мы знать не должны))  и объявили переменную такого типа, например, то класс с нужной подстановкой типов и значений генерируется, тогда уже выполняются проверки типов полей и прочее. И так, на каждую подстановку, генерируется свой класс. Полученные классы (с разными подстановками) будут являться разными, никак не связанными независимыми классами. 

Взято из `https://github.com/hse-spb-2021-cpp/lectures/blob/master/14-220110/02-templates/03-template.cpp`

```c++
#include <iostream>
#include <vector>
#include <set>

// class template
template<typename/* class */ C>  // C++20: concepts
struct templ_foo {
    typename C::iterator value;  // checked on class template instantiation
    // std::vector<int>::iterator

    // methods are checked on method instantiation only
    static void static_method() {
        C::foobarbaz();
    }

    void foo(C &c) {
        c.push_back(10);
    }

    void wtf() {
        C::hregfiuhtrghtiughgihtrg(10, 'h', "hello");
    }
};

int main() {
    {
        [[maybe_unused]] templ_foo<std::vector<int>> x;
        std::vector<int> v;
        x.value = v.begin();
        x.foo(v);

        // templ_foo<std::vector<int>>::static_method();
    }

    {
        [[maybe_unused]] templ_foo<std::set<int>> x;
        std::set<int> s;
        x.value = s.begin();
//        x.foo(s);
    }
//    [[maybe_unused]] templ_foo<int> y;
}
```

Важно! В момент инстанцирования класса необходима компилируемость только самого объявления класса. По сути, происходит инстанцирование именно класса, но не его методов. Так что в методах может быть произвольная дичь, и всё компилироваться будет нормально ровно до попытки использовать, и, как следствие, попытки проинстанцировать проблемный метод, собственно, добавив в код вызов его.

Взято из `https://github.com/hse-spb-2021-cpp/lectures/blob/master/14-220110/02-templates/04-shortcut.cpp`

```c++
#include <algorithm>
#include <utility>

template<typename T>
struct my_ptr {
private:
    T *data;

public:
    my_ptr/*<T>*/() : data(new T()) {}
    my_ptr(T value) : data(new T(std::move(value))) {}

    my_ptr(const my_ptr/*<T>*/ &other) : data(new T(*other->data)) {}
    my_ptr(my_ptr &&other) : data(std::exchange(other->data, new T())) {}

    my_ptr &operator=(const my_ptr &other) {
        *data = *other.data;
        return *this;
    }

    my_ptr &operator=(my_ptr &&other) {
        std::swap(data, other.data);
        return *this;
    }

    ~my_ptr() {
        delete data;
    }
};

int main() {
    my_ptr<int> x(10), y(12);
    my_ptr<double> z(10.5);
    x = y;
    // x = z;
}
```

В случае, если внутри шаблона класса нужно обратиться к самому этому классу, то можно писать полно (`my_ptr<T>`), явно обращаясь, или можно воспользоваться сахаром и писать просто `my_ptr`. Условились, что указание шаблона без параметров соответствует шаблону с текущими параметрами. 

Тут информация с прошлого видоса кончилась.

Далее информация из лекции 22-220314, в том порядке, в котором описано в билете (Не соответствует порядку рассказа)

#### Раздельное объявление и определение

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

Как можно заметить, для того, объявление и определение чего мы хотим разделить, нужно перед определением прописывать все `template<...>`. А если `template` вложенные, там у функций-членов, внутренних структур и т.д., то на каждый уровень вложенности нужна своя строка с `template`.

А если вы хотите объявить шаблон в `.h`, а потом определить его где-нибудь в `.cpp`, то просто не делайте так. Так сказать 

Взято из `https://github.com/hse-spb-2021-cpp/lectures/blob/master/22-220314/04-template-multiple-tu/print.h`
```c++
// General implementation of print<> should always be in the header,
// so it can be implicitly instantiated on demand. Otherwise: undefined reference.
// See https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl
```

#### Параметры шаблона

В основном были в первой части билета. Разницы между `typename` и `class` в этом месте нет (`struct` нельзя) (на самом деле до, кажется, семнадцатых плюсов было одно место, где нельзя было `typename` использовать, но сейчас уже прям совсем пофиг).

Значения простых типов — тоже было, можно использовать всякие целочисленные типы (no doubles??), некоторые `reference` и `pointer`, в общем, желающим --- `https://en.cppreference.com/w/cpp/language/template_parameters`.

#### Параметры по умолчанию, отсутствие имени у параметра

Вырезано из `https://github.com/hse-spb-2021-cpp/lectures/blob/master/22-220314/03-template-details/11-args.cpp`
```c++
// 'class' == 'typename' in the line below. 'struct' is not allowed.
template<typename C = int, typename = char, typename = double, int = 10>
struct templ_foo {
};
templ_foo<std::vector<int>, char, bool> x;
templ_foo<std::vector<int>> y;
templ_foo<> z;

// Types/default values of following parameters may depend on prior.
// Unlike functions: `void foo(int a, int b = a)` is invalid.
template<typename C, C value, typename D = std::pair<C, C>>
struct templ_bar {
};
templ_bar<int, 10> xx;
templ_bar<unsigned, 4'000'000'000> yy;
```

В общем, через `=` можно указывать значения по умолчанию, можно убирать имя у параметра, можно даже комбинировать отсутствие имени и значение по умолчанию.

#### Параметры-шаблоны, передача в них шаблонов с параметрами по умолчанию

Вырезано из `https://github.com/hse-spb-2021-cpp/lectures/blob/master/22-220314/03-template-details/11-args.cpp`

```c++
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

// You may want a template of a specific 'kind' as a paremeter. Works with argument deduction as well.
// Since C++17: works even though std::vector<T, Alloc>, see https://wg21.link/p0522r0
//              Clang disables it by default because it is 'incomplete': https://github.com/llvm/llvm-project/issues/42305
template<typename T, template<typename> typename Container = std::vector>
struct heap {
    Container<std::pair<T, int>> data;  // (value, id)
};

// Compare this with easier to write and read:
template<typename T, typename Container = std::vector<T>>
struct priority_queue {
    Container c;
    // Cannot create Container<std::pair<T, int>>
};

int main() {
    heap<std::string> h1;
    h1.data.emplace_back("hello", 20);

    heap<std::string, std::set> h2;  // Does not make much sense, but compiles.
    h2.data.emplace("hello", 20);

    // heap<int, std::map> h3;
}
```

Можно бахать вложенные `template`, если параметр шаблона сам является шаблоном, как в примере с `heap`. При этом туда можно писать параметр-шаблон по умолчанию. Но вообще лучше и удобнее писать всё же типы, а не шаблоны, как в `priority_queue`.

#### Ключевые слова `typename`, `template` для обращения внутрь зависимых типов, в том числе при наследовании от шаблонного параметра

Взято из `https://github.com/hse-spb-2021-cpp/lectures/blob/master/22-220314/03-template-details/01-typename-ambiguity.cpp`

```c++
template<typename T>
struct foo {
    int y = 1;
    void bar() {
        /*typename*/ T::x *y;
        /*typename*/ T::x * y;
    }
};

struct with_int { static inline int x = 5; };
foo<with_int> f_int;

struct with_type { using x = char; };
foo<with_type> f_type;

int main() {
    f_int.bar();
    f_type.bar();  // Does not compile, needs 'typename' before 'T::x'
}
```

Нюанс в том, что компилятор, видя `T::x *y` не может понять, является ли `T::x` типом или значением, и поэтому считает (так принято), что это значение, вот и парсит это как умножение. Поэтому, чтобы явно сказать, что `T::x` это тип, нужно прописать `typename`. 

Взято из `https://github.com/hse-spb-2021-cpp/lectures/blob/master/22-220314/03-template-details/02-template-member-call-ambiguity.cpp`
```c++
struct with_templ_member {
    template<typename T>
    static int foo() { return 10; };
};

template<typename T>
struct Foo {
    void call_foo() {
        // T::foo<int>();
        // T::foo < int      >();  // compilation error: expected primary-expression before 'int'
                                   // i.e.. "cannot compare foo with int"
        T::template foo<int>();  // needs 'template'
    }
};

int main() {
    Foo<with_templ_member>().call_foo();
}
```

И снова мы не знаем, чем является `T::foo` и считаем, что это значение, а следовательно парсим `T::foo < int...` как сравнение `T::foo` и, каким-то образом, `int`. Поэтому для обращения к такому члену нужно использовать ключевое слово `template`


Теперь про наследование от шаблонного параметра:

Дано (в упражнении `https://github.com/hse-spb-2021-cpp/exercises/blob/master/22-220314/problem/22-call-dependent-method.cpp` ):

```c++
#include <iostream>

template<typename T>
struct derive_from : T {
    void call_foo() {
        foo();  // TODO: fix compilation error
    }
};

struct with_foo {
    void foo() {
        std::cout << "OK\n";
    }
};

int main() {
    derive_from<with_foo> x;
    x.call_foo();
}
```

В чём проблема? Не видим foo, т.к. не знаем, что он должен быть в `T` (поправьте, если не так). 

Как решить? Вот так: (решение `https://github.com/hse-spb-2021-cpp/exercises/blob/master/22-220314/solution/22-call-dependent-method.cpp`)



```c++
#include <iostream>

template<typename T>
struct derive_from : T {
    void call_foo() {
        this->foo();
        // Also work:
        // T::foo();
        // derive_from::foo();
    }
};

struct with_foo {
    void foo() {
        std::cout << "OK\n";
    }
};

int main() {
    derive_from<with_foo> x;
    x.call_foo();
}
```

### Отличие шаблона `Foo` и инстанцирования шаблона с параметрами по умолчанию `Foo<>`

`Foo` — шаблон, `Foo<>` — тип, результат инстанцирования шаблона с параметрами по умолчанию (внезапно). 

По этому, в первом приближении, нельзя просто взять и написать `Foo v(10, 20)`, но человечество изобрело CTAD — class template argument deduction, и теперь если из аргументов конструктора можно вывести все нужные типы, то сгенерируется deduction guide:

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

Как можно заметить в комментах, можно писать и свои deduction guides.

Удобство с `auto Foo<T>::foo() -> Foo`: видимо, относится вот к этому:

Взято из `https://github.com/hse-spb-2021-cpp/lectures/blob/master/27.5-220519/04-return-auto/03-trailing.cpp`

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
```

### Инстанцирование

Момент, когда, собственно, генерируется сам код нужного типа, метода и т.д.

#### Независимость по методам

Если есть шаблонный класс, то его методы инстанцируются только если они где-либо используются. Поэтому, в методе может быть произвольная дичь и это будет компилироваться до тех пор, пока этот метод не попробуют использовать — тогда произойдёт инстанцирование и все умрут от ошибки компиляции. Смотри первую часть билета.

#### Когда происходит неявное инстанцирование: полей, методов (статических и нестатических), вложенных типов

Поля инстанцируются при инстанцировании класса. Методы, что статические, что нестатические, инстанцируются по требованию, т.е. при использовании. Аналогично с вложенными типами. Вроде часть была в начале, часть получена экспериментальным путём.

#### Когда тип шаблона может быть incomplete, а когда не может

Тип шаблона может быть incomplete до тех пор, пока мы не обращаемся к каким-либо полям/вложенным чему либо этого типа, не пытаемся делать `sizeof` от этого типа и как-либо создавать объекты этого класса. Вроде на этом всё, хоть в действительности может быть чуть стрёмнее. 

### Независимость инстанциаций шаблона

Каждая инстанциация шаблона с разными шаблонными параметрами — свой тип, независимый и не связанный с остальными типами от этого же шаблона, но с другими шаблонными параметрами. Как итог, например, нельзя их просто брать и сравнивать/проводить какие-либо операции — это разные типы. Вроде тоже было в первой части билета.

### Определение методов и статических членов шаблонного класса внутри и вне класса

Было в блоке `Синтаксис объявления и определения` во второй части
