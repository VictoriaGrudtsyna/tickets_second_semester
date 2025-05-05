# 62. Трюки с наследованием классов (необязательно множественными)

## Вопросы

* Короткие факты
    * Наследование `private`/`protected`/`public`, отличия слов `struct`/`class`.
    * slicing (срезка объектов) при присваивании полиморфного класса в переменную типа "базовый класс", как защититься (хранить только ссылки/умные указатели)
    * Получение most derived class при помощи `dynamic_cast`
    * Делегирующие конструкторы (поведение при исключениях — отдельный билет)
* Один из способов реализации виртуальных функций: таблица виртуальных функций, в том числе с наследованием
* CRTP (`27-230511/01-inheritance-tricks`)

---

## Короткие факты

### Наследование `private`/`protected`/`public`, отличия слов `struct`/`class`.

Также, как и с полями – `private`, `protected` и `public`. Для классов наследование по умолчанию `private`,
у структур – `public`. Влияет на то, кто понимает, что класс отнаследован от базового класса.

```cpp
struct Base {
    void foo() {
    }
};

struct Derived1 : public Base {};  // Default for 'struct', the most popular.
struct Derived2 : protected Base {};
struct Derived3 : private Base {  // Default for 'class'.
    void baz() {
        foo();
        Derived3 d3;
        [[maybe_unused]] const Base &b = d3;
    }
};
struct Derived22 : /* public */ Derived2 {
    void bar() {
        foo();
        Derived2 d2;
        [[maybe_unused]] const Base &b2 = d2;
    }
};

struct Derived33 : /* public */ Derived3 {
    void bar() {
    // foo();
    [[maybe_unused]] ::Base b;  // :: is important
    // [[maybe_unused]] const ::Base &b2 = *this;
    // [[maybe_unused]] const ::Base &b3 = static_cast<const ::Base &>(*this);
    [[maybe_unused]] const ::Base &b4 =
        (const ::Base
            &)*this;  // meh, C-style cast ignores access modifiers
    }
};
```

`private` – Derived3 знает, что он отнаследован от Base.

Derived33 не знает, что Derived3 наследован от Base --> нельзя вызывать методы Base, создавать поле
такого типа (лайфхак: ::Base сработает (не из-за наследования, а просто поле)), нельзя взять ссылку на себя как на Base.

`protected` – Derived2 знает, что он отнаследован от Base. Наследники не могут вызывать методы Base.

```c++
int main() {
    Base b;
    Derived1 d1;
    Derived2 d2;
    Derived3 d3;

    b.foo();
    d1.foo();
    // d2.foo();
    // d3.foo();

    [[maybe_unused]] const Base &b1 = d1;

    // [[maybe_unused]] const Base &b2x = d2;
    // [[maybe_unused]] const Base &b2y = static_cast<const Base &>(d2);
    [[maybe_unused]] const Base &b2z =
        (const Base &)d2;  // meh, C-style cast ignores access modifiers

    // [[maybe_unused]] const Base &b3x = d3;
    // [[maybe_unused]] const Base &b3y = static_cast<const Base &>(d3);
    [[maybe_unused]] const Base &b3z =
        (const Base &)d3;  // meh, C-style cast ignores access modifiers
}
```

**NB:** важно помнить, что у `class` наследование приватное по умолчанию.

### slicing (срезка объектов) при присваивании полиморфного класса в переменную типа "базовый класс", как защититься (хранить только ссылки/умные указатели)

**Slicing**

Есть класс и есть наследник.

```c++
struct Base {
    int x = 10;
    void foo() const {
        std::cout << "x=" << x << "\n";
    }
};

struct Derived : Base {
    int y = 20;
    void bar() const {
        foo();
        std::cout << "y=" << y << "\n";
    }
};

void bar(Base b) {  // Slicing: we create a new object
    std::cout << "bar(" << b.x << ")\n";
    const Derived &d = static_cast<const Derived&>(b);  // derivedcast, UB
    std::cout << ".y=" << d.y << "\n";  // UB
    &d.y;  // UB
}
```

Вызываем функцию, принимающую по значению, а не по ссылке. Тогда из Derived создастся объект типа Base и при попытке
сделать derived_cast возникнет UB.

```c++
{
    Derived d;
    d.x = 123;
    // Base(const Base &other) : x(other.x)
    bar(d);  // Always UB.
}
{
    Base b;
    bar(b);  // Always UB.
}
```

**Защита от slicing**

Запретить в базовом классе копирование и перемещение. Тогда код не сломается (чаще всего), а при попытке принять Derived
по значению
будет ошибка компиляции (нет копирования).

```c++
struct Base {
    int x = 10;
    void foo() const {
        std::cout << "x=" << x << "\n";
    }

    Base() {}
    Base(const Base &) = delete;
    Base(Base &&) = delete;
    Base &operator=(const Base &) = delete;
    Base &operator=(Base &&) = delete;
};
```

### Получение most derived class при помощи `dynamic_cast`

Хотим узнать реальный адрес объекта. Можно сделать `static_cast<Derived>(&y)`, а можно `dynamic_cast<void*>(&y)` –
специальный синтаксис, чтобы сказать «посмотри на самый вложенный класс и верни ссылку». Для того, чтобы `dynamic_cast`
работал,
нужен хотя бы один виртуальный метод (например, деструктор).

В предыдущем случае был такой каст под капотом.

```c++
#include <string>
#include <iostream>

/*
 X        Y
 ^        ^
  \      /
   \    /
  Derived
*/

struct X { int x_data = 0; virtual ~X() {} };
struct Y { int y_data = 0; virtual ~Y() {} };
struct Derived : X, Y { };

int main() {
    alignas(0x100) Derived d;
    X &x = d;
    Y &y = d;

    std::cout << &d << "\n";
    std::cout << &x << " " << dynamic_cast<void*>(&x) << "\n";  // "most derived object"
    std::cout << &y << " " << dynamic_cast<void*>(&y) << "\n";  // "most derived object"
}
```

### Делегирующие конструкторы (поведение при исключениях — отдельный билет)

Конструктор вызывает собственный конструктор типа, а потом может еще что-то доделать, если надо.

```c++
bigint(const std::string &s) {
    // TODO: ...
    std::cout << "constructing from string " << s << "\n";
}
bigint(int x) : bigint(std::to_string(x)) {  // Delegating constructor
    // bigint(std::to_string(x));  // bad attempt :(
    std::cout << "constructing from int " << x << "\n";
}
bigint() : bigint(0) {}
```

## Один из способов реализации виртуальных функций: таблица виртуальных функций, в том числе с наследованием

### Виртуальные функции

Попробуем сделать виртуальные функции самостоятельно без слова `virtual`.

Пусть есть класс и его наследник. Хотим, что `print` печатало разное в зависимости от того в базовом классе и
наследнике. Можно создать поле типа `std::function<void()> print`.

Получили виртуальные функции на минималках. Но теперь надо для каждой функции хранить поле в каждом объекте. А на это
тратится много байт. Например, тут у нас размер структуры с одним `int` равен 40 байтам! А если еще функций добавить...

```c++
#include <functional>
#include <iostream>
#include <vector>

struct Base {
    int x = 10;

    std::function<void()> print = [&]() { std::cout << "x = " << x << "\n"; };
    // std::function<void()> pretty_print = ....;
    // std::function<void()> read = ....;
};

struct Derived : Base {
    int y = 20;

    Derived() {
        print = [&]() { std::cout << "x = " << x << ", y = " << y << "\n"; };
    }
};

int main() {
    Base b;
    Derived d;
    b.print();
    d.print();

    Base &db = d;
    db.print();

    std::cout << sizeof(Base) << ", " << sizeof(Derived) << "\n";
}
```

Попробуем оптимизировать. Будем хранить указатели на статическую функцию в каждом классе. В наследнике поменяем
указатель и будем вызывать нужную функцию по указателю в методе `print`.

Размер стал меньше (24 байта), но проблема осталась – все еще при добавлении функций увеличивается размер структуры.

```c++
#include <functional>
#include <iostream>
#include <vector>

struct Base {
    int x = 10;

    using print_impl_ptr = void (*)(Base *);
    static void print_impl(Base *b) {
        std::cout << "x = " << b->x << "\n";
    }

    print_impl_ptr print_ptr = print_impl;
    // pretty_print_impl_ptr pretty_print_ptr;
    // read_impl_ptr read_ptr;

    void print() {
        print_ptr(this);
    }
};

struct Derived : Base {
    int y = 20;

    static void print_impl(Base *b) {
        Derived *d = static_cast<Derived *>(b);
        std::cout << "x = " << d->x << ", y = " << d->y << "\n";
    }

    Derived() {
        print_ptr = print_impl;
    }
};

int main() {
    Base b;
    Derived d;
    b.print();
    d.print();

    Base &db = d;
    db.print();

    std::cout << sizeof(Base) << ", " << sizeof(Derived) << "\n";

    [[maybe_unused]] std::vector<Derived> vec(1'000'000);
}
```

### Таблица виртуальных функций

Заметим, что все функции разбиваются на группы и вызывают или функции из `Base`, или функции из `Derived`.

Создадим для базового класса и наследника по таблице, в которой будем хранить указатели на функции. Тогда можно объявить
таблицу один раз и в каждом классе добавить указатель на таблицу, по которому будем обращаться к нужным функциям.

Теперь мы храним только один указатель в каждой структуре, добавление новых функций не будет увеличивать размер структуры.

Примерно так реализована таблица виртуальных функций.

```c++
#include <functional>
#include <iostream>
#include <vector>

struct Base;

struct BaseVtable {  // virtual functions table
    using print_impl_ptr = void (*)(Base *);
    print_impl_ptr print_ptr;
    // pretty_print_impl_ptr pretty_print_ptr;
    // read_impl_ptr read_ptr;

    // We may also add other information about type, e.g.:
    // std::string type_name;
};

struct Base {
    static const BaseVtable BASE_VTABLE;

    const BaseVtable *vptr = &BASE_VTABLE;
    int x = 10;

    static void print_impl(Base *b) {
        std::cout << "x = " << b->x << "\n";
    }

    void print() {
        vptr->print_ptr(this);
    }
};
const BaseVtable Base::BASE_VTABLE{Base::print_impl};

struct Derived : Base {
    static const BaseVtable DERIVED_VTABLE;

    int y = 20;

    static void print_impl(Base *b) {
        Derived *d = static_cast<Derived *>(b);
        std::cout << "x = " << d->x << ", y = " << d->y << "\n";
    }

    Derived() {
        vptr = &DERIVED_VTABLE;
    }
};
const BaseVtable Derived::DERIVED_VTABLE{Derived::print_impl};

int main() {
    Base b;     // vptr == &BASE_VTABLE, x
    Derived d;  // vptr == &DERIVED_VTABLE, x, y
    b.print();
    d.print();

    Base &db = d;
    db.print();

    std::cout << sizeof(Base) << ", " << sizeof(Derived) << "\n";

    [[maybe_unused]] std::vector<Derived> vec(1'000'000);
}
```

Можно добавить в таблицу виртуальных функций для наследника новые функции.

```c++
#include <functional>
#include <iostream>
#include <vector>

struct Base;

struct BaseVtable {
    using print_impl_ptr = void (*)(Base *);
    print_impl_ptr print_ptr;

    // We may also add other information about type, e.g.:
    // std::string type_name;
};

struct Base {
    static const BaseVtable BASE_VTABLE;

    const BaseVtable *vptr = &BASE_VTABLE;
    int x = 10;

    static void print_impl(Base *b) {
        std::cout << "x = " << b->x << "\n";
    }

    void print() {
        vptr->print_ptr(this);
    }
};
const BaseVtable Base::BASE_VTABLE{Base::print_impl};

struct Derived;
struct DerivedVtable : BaseVtable {
    using mega_print_impl_ptr = void (*)(Derived *);
    mega_print_impl_ptr mega_print_ptr;
};

struct Derived : Base {
    static const DerivedVtable DERIVED_VTABLE;

    int y = 20;

    static void print_impl(Base *b) {
        Derived *d = static_cast<Derived *>(b);
        std::cout << "x = " << d->x << ", y = " << d->y << "\n";
    }

    static void mega_print_impl(Derived *b) {
        std::cout << "megapring! y = " << b->y << "\n";
    }

    Derived() {
        vptr = &DERIVED_VTABLE;
    }

    void mega_print() {
        static_cast<const DerivedVtable *>(vptr)->mega_print_ptr(this);
    }
};
const DerivedVtable Derived::DERIVED_VTABLE{Derived::print_impl,
                                            Derived::mega_print_impl};

struct SubDerivedVtable : DerivedVtable {
    // no new "virtual" functions
};
struct SubDerived : Derived {
    static SubDerivedVtable SUBDERIVED_VTABLE;

    int z = 20;

    static void mega_print_impl(Derived *b) {
        SubDerived *sd = static_cast<SubDerived *>(b);
        std::cout << "megaprint! y = " << sd->y << ", z = " << sd->z << "\n";
    }

    SubDerived() {
        vptr = &SUBDERIVED_VTABLE;
    }
};
SubDerivedVtable SubDerived::SUBDERIVED_VTABLE{Derived::print_impl,
                                               SubDerived::mega_print_impl};

int main() {
    SubDerived sd;
    sd.print();
    // Base::print() -->
    //     vptr == &SUBDERIVED_VTABLE -->
    //     vptr->print_ptr == Derived::print_impl
    sd.mega_print();

    Derived &d = sd;
    d.print();
    d.mega_print();

    Base &b = sd;
    b.print();
    Base::print_impl(&sd);
}
```

## CRTP

_CRTP (Curiously Recurring Template Pattern)_ – трюк, чтобы преобразовывать виртуальные функции в шаблоны.

Как бы мы сделали с помощью виртуальны функций: отнаследовали `OstreamWriter` от `RichWriter`, реализовали у него
только один метод `writeChar`, тогда остальные методы будут работать без изменений. Виртуальные здесь только
вызовы `writeChar`.

Также можно было бы создать класс `CountingWriter`, в котором функция `writeChar` реализована по-другому (считает
символы в строке, а не выводит).

```c++
#include <iostream>
#include <string_view>

struct RichWriter {
public:
    virtual void writeChar(char c) = 0;

    void writeInt(int value) {
        static_assert(CHAR_BIT == 8);
        static_assert(sizeof(value) == 4);
        for (int i = 0; i < 4; i++) {
            writeChar((value >> (8 * i)) & 0xFF);
        }
    }
    void writeString(std::string_view s) {
        writeInt(s.size());
        for (char c : s) {
            writeChar(c);
        }
    }
};

struct OstreamWriter : RichWriter {
private:
    std::ostream &m_os;

public:
    OstreamWriter(std::ostream &os) : m_os(os) {}

    void writeChar(char c) override {
        m_os << c;
    }
};

struct CountingWriter : RichWriter {
private:
    int m_written = 0;

public:
    void writeChar(char) override {
        m_written++;
    }
    int written() const {
        return m_written;
    }
};

void writeHello(RichWriter &w) {
    w.writeString("hello\n");
}

int main() {
    OstreamWriter ow(std::cout);
    writeHello(ow);

    CountingWriter cw;
    writeHello(cw);
    std::cout << cw.written() << "\n";
}
```

Способ сделать то же самое, но без виртуальных функций. Можно сделать `RichWriter` шаблонным классом, где параметром
передается его наследник. Тогда вместо вызова виртуальной функции `RichWriter` сделает `static_cast` к ссылке на
наследника и вызовет
у него метод `writeChar`. И теперь `OstreamWriter : RichWriter<OstreamWriter>`.

Называется Curiously Recurring Template Pattern. Суть шаблона в следующем:

* некий класс наследуется от шаблонного класса
* класс-наследник используется как параметр шаблона своего базового класса

При этом функции не виртуальные, поэтому компилятор может легко сделать эти функции `inline`.

**Плюсы:**

- Работает чуть быстрее (нет виртуальных вызовов).
- Каждый объект занимает чуть меньше памяти (не храним указатель на таблицу виртуальных функций),

**Минусы:**

- Все немного запутаннее.
- Теперь у нас нет одного базового класса, поэтому функции для вызова нужно снова писать шаблоны.
- В функциях не проверить, что нам передали именно наследника.

```c++
template<typename Derived>
struct RichWriter {
    // virtual void writeChar(char c) = 0;  // No virtual functions!

protected:
    Derived &derived() {
        // Compile-time call
        return static_cast<Derived&>(*this);
    }

public:
    void writeInt(int value) {
        static_assert(CHAR_BIT == 8);
        for (int i = 0; i < 4; i++) {
            derived().writeChar((value >> (8 * i)) & 0xFF);
        }
    }
    void writeString(std::string_view s) {
        writeInt(s.size());
        for (char c : s) {
            derived().writeChar(c);
        }
    }
};

struct OstreamWriter : RichWriter<OstreamWriter> {  // CRTP: Curiously Recurring Template Pattern
private:
    std::ostream &m_os;

public:
    OstreamWriter(std::ostream &os) : m_os(os) {}

    void writeChar(char c) {  // no 'override'
        m_os << c;
    }
};

struct CountingWriter : RichWriter<CountingWriter> {  // CRTP
private:
    int m_written = 0;

public:
    void writeChar(char) {  // no 'override'
        m_written++;
    }
    int written() const {
        return m_written;
    }
};

// Do not have a "common" type for `RichWriter`s, need templates.
template<typename RW>
void writeHello(RW &w) {
    w.writeString("hello\n");
}

template<typename T>
void writeHello2(RichWriter<T> &w) {
    w.writeString("hello\n");
}

int main() {
    OstreamWriter ow(std::cout);
    writeHello(ow);

    CountingWriter cw;
    writeHello(cw);
    std::cout << cw.written() << "\n";
}
```