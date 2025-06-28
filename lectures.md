# Cловарь

- CRTP - curiously recurring template pattern (Лекция 1)
- EOB - Empty base optimization (Лекция 1)
- diamond-problem - проблема при множественном наследовании (Лекция 1)

- delegate-to-sister (Лекция 2)
- no unique final overrider (Лекция 2)
- препроцессор и макросы (Лекция 2)
- диграфы/триграфы (когда там в давние времена не было каких то символов - заменяли на какие то сочетания более доступных символов) (Лекция 2)
- variadic_macros (Лекция 2)
- исключения (Лекция 3)
- wide/narrow contract (Лекция 3)
- exception-ptr (Лекция 4)
- exception safety / гарантии исключений (Лекция 4)
- RAII - Resourse Acquisition Is Initialisation (Лекция 4)
- noexcept (Лекция 4)
- copy-swap idiom (Лекция 4)
- fubction-try block (Лекция 5)
- сети (Лекция 5)
- conditional variable (Лекция 6)
- promise future (Лекция 6)
- const-thread-safe (Лекция 6)
- TOC-TOU (Лекция 6)
- thread-local (Лекция 6)
- неявный inline в шаблонах (Лекция 6)
- volatile (Лекция 6)
- aligment (Лекция 7)
- padding (Лекция 7)
- strict-aliasing rule (Лекция 7)
- trivially-copible (Лекция 7)
- standart-layout (Лекция 7)
- designation initializer (Лекция 9)
- union (anonimous/unnamed) (Лекция 10)
- ref-qualifier (Лекция 11)
- placement-new (Лекция 11)
- copy-swap idiom (Лекция 11)
- template-freinds (Лекция 11)
- allocator (Лекция 12)
- typename, template (Лекция 12)
- deduction - выведение типов (Лекция 12)
- ctad (Лекция 12)
- метапрограммирование (Лекция 13)
- специализации (Лекция 13)
- variadic (Лекция 13)
- fold expression (Лекция 13)
- perfect-forwarding (Лекция 14)
- type display (Лекция 14)
- decltype (Лекция 14)
- sfinae (Лекция 14)
- member detection (Лекция 16)
- type-list (Лекция 16)
- function_traits (Лекция 16)
- guaranteed-copy-elision (Лекция 17)
- RVO (return value optimization) (Лекция 17)
- NRVO (named return value optimization) (Лекция 17)
- temporary materialization (Лекция 17)
- qulified/unqulified name lookup (Лекция 18)
- ADL (argument dependent lookup) (Лекция 18)
- the most vexing parse (Лекция 18)
# Навигация 

- [Лекция 1](#лекция-1)
- [Лекция 2](#лекция-2)
- [Лекция 3](#лекция-3)
- [Лекция 4](#лекция-4)
- [Лекция 5](#лекция-5)
- [Лекция 6](#лекция-6)
- [Лекция 7](#лекция-7)
- [Лекция 8](#лекция-8)
- [Лекция 9](#лекция-9)
- [Лекция 10](#лекция-10)
- [Лекция 11](#лекция-11)
- [Лекция 12](#лекция-12)
- [Лекция 13](#лекция-13)
- [Лекция 14](#лекция-14)
- [Лекция 15](#лекция-15)
- [Лекция 16](#лекция-16)
- [Лекция 17](#лекция-17)
- [Лекция 18](#лекция-18)
# Ссылочки на папки с лекциями 
- Лекция 1 - https://github.com/hse-spb-2023-cpp/lectures/tree/main/14-240116
- Лекция 2 - https://github.com/hse-spb-2023-cpp/lectures/tree/main/15-240123
- Лекция 3 - https://github.com/hse-spb-2023-cpp/lectures/tree/main/16-240130
- Лекция 4 - https://github.com/hse-spb-2023-cpp/lectures/tree/main/17-240206
- Лекция 5 - https://github.com/hse-spb-2023-cpp/lectures/tree/main/18-240213
- Лекция 6 - https://github.com/hse-spb-2023-cpp/lectures/tree/main/19-240220
- Лекция 7 - https://github.com/hse-spb-2023-cpp/lectures/tree/main/20-240227
- Лекция 8 - https://github.com/hse-spb-2023-cpp/lectures/tree/main/21-240305
- Лекция 9 - https://github.com/hse-spb-2023-cpp/lectures/tree/main/22-240313
- Лекция 10 - https://github.com/hse-spb-2023-cpp/lectures/tree/main/24-240409
- Лекция 11 - https://github.com/hse-spb-2023-cpp/lectures/tree/main/24-240409
- Лекция 12 - https://github.com/hse-spb-2023-cpp/lectures/tree/main/25-240416
- Лекция 13 - https://github.com/hse-spb-2023-cpp/lectures/tree/main/26-240423
- Лекция 14 - https://github.com/hse-spb-2023-cpp/lectures/tree/main/27-240430
- Лекция 15 - https://github.com/hse-spb-2023-cpp/lectures/tree/main/28-240514
- Лекция 16 -https://github.com/hse-spb-2023-cpp/lectures/tree/main/29-240521
- Лекция 17 - https://github.com/hse-spb-2023-cpp/lectures/tree/main/30-240528
- Лекция 18 - https://github.com/hse-spb-2023-cpp/lectures/tree/main/31-240604
# Лекция 1

виртуальные функции немножечко тормозят, потому что каждый раз компилятору нужно понимать какой у нас на самом деле тип объекта, искать где у нео метод и вызывать. Компилятору по этому поводу сложно оптимизировать виртуальные функции

хотим с помощью другой техники (CRTP), сделать так, чтобы компилятору при вызове функции было понятно от кого что вызывается. В примере ниже - чтобы компилятор в функции writeHello понимал сразу какой writer у него и где искать метод writeString.

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

    std::cout << sizeof(OstreamWriter) << "\n";
    std::cout << sizeof(CountingWriter) << "\n";
}
```


Как это сделать с помощью CRTP? - при помощи шаблонов

сделаем writeHello шаблонным - это уже работает и уже немного поможет - компилятор будет точно знать от какого класса вызвались.


Помимо чего, чтобы совсем всё хорошо было, хотим избавиться от virtual functions

richwriter сделаем шаблонным классом и при наследовании будем говорить кто наследуется от richwriter, чтобы тот при необходимости смог воспользоваться методом из нас самих

нужно чтобы вместо вызова виртуальных функций делать всё на этапе компиляции + объекты занимают меньше места(не надо помнить кто мы на самом деле)

```c++
#include <iostream>
#include <string_view>

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

    std::cout << sizeof(OstreamWriter) << "\n";
    std::cout << sizeof(CountingWriter) << "\n";

    // Pros: probably faster, objects take less space
    // Cons: less clear code
}
```


##### EOB

в с++ есть требование: любой объект занимает хотя бы один байт(даже если объект пустой) + объекты разного типа не могут пересекаться.

если используется наследование, то такого уже не происходит - если есть empty base, то ему можно не выделять отдельный байт памяти


##### Реализация виртуальных функций

- самое простое - std::function плюс функция как поле класса
- можно использовать указатели на функции (при этом методе объект меньше размера(указатель занимает 8 байт, а не 32))

```c++
#include <functional>
#include <iostream>
#include <vector>

struct Base {
    int x = 10;

    // https://cdecl.org
    using print_impl_ptr = void (*)(Base *);  // std::function<void(Base*)>, no captures
    static void print_impl(Base *self) {  // self ~ this
        std::cout << "x = " << self->x << "\n";
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

    static void print_impl(Base *self_b) {
        Derived *self = static_cast<Derived *>(self_b);
        std::cout << "x = " << self->x << ", y = " << self->y << "\n";
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
Если виртуальных функций много, то грустно, потому что нужно ровно столько же указателей на функцию


Поэтому теперь хотим не для каждой функции хранить еще поле, а хотим в целом хранить одно поле, которое знает всю информацию про тип

будет хранить таблицу виртуальных функций! baseVtable
а в каждом объекте base будем хранить один указатель(vptr) на vtable - теперь независимо от того, сколько у нас виртуальных функций, все наши объекты раздулись только на 8 байт

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
    std::string type_name;
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
const BaseVtable Base::BASE_VTABLE{Base::print_impl, "Base"};

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
const BaseVtable Derived::DERIVED_VTABLE{Derived::print_impl, "Derived"};

int main() {
    Base b;     // vptr == &BASE_VTABLE, x
    Derived d;  // vptr == &DERIVED_VTABLE, x, y
    b.print();
    d.print();

    Base &db = d;
    db.print();
    std::cout << db.vptr->type_name << "\n";  // Derived

    std::cout << sizeof(Base) << ", " << sizeof(Derived) << "\n";

    [[maybe_unused]] std::vector<Derived> vec(1'000'000);
}
```

что делать если derived добавил свои собственный виртуальные функции? - наследование vtable-ов!! Для каждого класса будем заводить свои тип vtable, и они еще будут наследоваться друг от друга


итого у нас две иерархии наследования - одна для классов, другая для их таблиц

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
        std::cout << "megaprint! y = " << b->y << "\n";
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

#### множественное наследование 

можно взять и отнаследовать класс от двух одновременно: в памяти сначала будет первый родитель, потом второй и тд, только потом наши личные поля

если поле определено однозначно - то всё гуд, если же не однозначно, то надо подсказать из какого именно родителя брать метод

схема, как это выглядит в памяти

```c++
#include <chrono>
#include <string>
#include <iostream>

struct PieceOfArt { std::chrono::time_point<std::chrono::steady_clock> date; };
struct Music : PieceOfArt { int bpm; };
struct Lyrics : PieceOfArt { std::string text; };
struct Song : Music, Lyrics {  // sizeof(Song) >= 2.
     std::string album;

     // Order of construction: Music, Lyrics (same order as after :), fields of Song.
     // Full order: Music::PieceOfArt, Music, Lyrics::PieceOfArt, Lyrics, Song.

     // Song(....) : Music(.....), Lyrics(....), album(....) { ... }
};

/*
Class diagram:

PieceOfArt   PieceOfArt
    ^            ^
    |            |
  Music        Lyrics
    ^            ^
      \        /
        \    /
         Song

Layout is guaranteed, exact sizes and padding vary:

0                  8      16               24      56       88
+---------------------------------------------------------+
| +---------------------+ +----------------------+        |
| | +------------+      | | +------------+       |        | //music and lyrics от разны PieceOfArt 
| | | PieceOfArt |      | | | PieceOfArt |       |        |
| | | +date      |      | | | +date      |       |        |
| | +------------+      | | +------------+       |        |
| |   Music             | |   Lyrics             |        |
| |                +bpm | |                +text |        |
| +---------------------+ +----------------------+        |
| Song                                                    |
|                                                  +album |
+---------------------------------------------------------+
*/

int main() {
   alignas(0x100) Song s;
   [[maybe_unused]] auto f1 = s.bpm;
   [[maybe_unused]] auto f2 = s.text;
   // [[maybe_unused]] auto x = s.date;  // ambiguous member, clang++ has good error message, `using Music::date` won't help for some reason.
   [[maybe_unused]] auto y = s.Music::date;
   [[maybe_unused]] auto z = s.Lyrics::date;
   // [[maybe_unused]] PieceOfArt &p = s;  // ambiguous base, upcast is impossible

   std::cout << &s << " " << sizeof(s) << "\n";

   Music &m = s;
   PieceOfArt &mp = m;
   std::cout << "Music:\n";
   std::cout << &m << " " << &mp << " " << sizeof(m) << " " << sizeof(mp) << "\n";
   std::cout << &m.date << " " << &m.bpm << "\n";
   // [[maybe_unused]] Song &ms1 = static_cast<Song&>(mp);  // ambiguous base, downcast is impossible
   [[maybe_unused]] Song &ms2 = static_cast<Song&>(m);  // unambiguous base, downcast is possible
   std::cout << "  downcast: " << &m << " --> " << &ms2 << "\n";

   Lyrics &l = s;
   PieceOfArt &lp = l;
   std::cout << "Lyrics:\n";
   std::cout << &l << " " << &lp << " " << sizeof(l) << " " << sizeof(lp) << "\n";
   std::cout << &l.date << " " << &l.text << "\n";
   std::cout << "  downcast: " << &l << " --> " << &static_cast<Song&>(l) << "\n";
   // !!! static_cast changes value of the pointer
}
```

## multiple interfaces

класс, который реализует множественный интерфейс. Если у нас множественное наследование и виртуальные функции, то эта проблема не решаемая - объекты сильно сильно раздуваются


```c++
#include <iostream>

// Extra reading: https://isocpp.org/wiki/faq/multiple-inheritance#mi-not-evil
struct IntWriter {
    virtual void write(int value) = 0;
    virtual ~IntWriter() {}
};
struct IntReader {
    virtual int read() = 0;
    virtual ~IntReader() {}
};

// Implements single interface
struct Printer : IntWriter {
    void write(int value) override { std::cout << value; }
};
// Implements single interface
struct Scanner : IntReader {
    int read() override {
        int value;
        std::cin >> value;
        return value;
    }
};
// Implements multiple interfaces
struct Variable : IntWriter, IntReader {
private:
    int m_value = 0;
public:
    void write(int value) override { m_value = value; }
    int read() override { return m_value; }
};

void readAndPrint(IntReader &r) {
    std::cout << r.read() << "\n";
}

int main() {
    std::cout << sizeof(IntWriter) << "\n";  // vptr
    std::cout << sizeof(IntReader) << "\n";  // vptr
    std::cout << sizeof(Variable) << "\n";
    /*                            
    ┌──────────────────────────────┐
    │┌─────────┐                   │
    ││IntReader│                   │
    ││+vptr    │                   │
    │└─────────┘                   │
    │Scanner                       │
    │                              │
    └──────────────────────────────┘
    ┌──────────────────────────────┐
    │┌─────────┐┌─────────┐        │
    ││IntWriter││IntReader│        │
    ││+vptr    ││+vptr    │        │
    │└─────────┘└─────────┘        │
    │Variable                      │
    │                      +m_value│
    └──────────────────────────────┘
    */
    Scanner s;  // vptr
    readAndPrint(s);

    Variable v;
    v.write(10);
    readAndPrint(v);
}
```

boost-operators** - как один из примеров множественного наследования


#### side-cast/cross-cast 
каст в бок(главное чтобы у исходного объекта были виртуальные функции)

каст к completely unrelatable - nullptr

```c++
#include <iostream>
#include <cstddef>

struct Base1 { int x = 1; virtual ~Base1() {} };
struct Base2 { int y = 2; virtual ~Base2() {} };
struct Base3 { int z = 3; virtual ~Base3() {} };
struct Derived123 : Base1, Base2, Base3 {};
/*
Base1  Base2  Base3
  ^      ^      ^
   \     |     /
    \    |    /
     Derived123
*/

struct CompletelyUnrelated {};

void foo(Base1 &b1) {
    std::cout << dynamic_cast<Base3*>(&b1) << "\n";  // side-cast/cross-cast
    if (auto *b3 = dynamic_cast<Base3*>(&b1)) {
        std::cout << "  z=" << b3->z << ", &z=" << &b3->z << "\n";
    }
    std::cout << dynamic_cast<CompletelyUnrelated*>(&b1) << "\n";
}

int main() {
    Derived123 a;
    std::cout << "Derived123 " << &a << ", &z=" << &a.z << "\n";
    foo(a);

    std::cout << "=====\n";

    struct Derived123C : Derived123, CompletelyUnrelated {};
    /*
Base1  Base2  Base3
  ^      ^      ^
   \     |     /
    \    |    /
     Derived123  CompletelyUnrelated
         ^        ^
          \      /   
         Derived123C
*/
    Derived123C b;
    std::cout << "Derived123C " << &b << "\n";
    foo(b);
}
```

#### dynamic-cast к void* - самый вложенный объект

____
что если в двух базовых классах есть два одинаковых метода с одинаковым набором переменных, что тогда происходит при override - override-ид обе!!! (если функции не совместимы - то ошибка компиляции)

```c++
#include <iostream>

struct Base1 {
    virtual void foo() {
        std::cout << "Base1\n";
    }
};

struct Base2 {
    virtual void foo() {
        std::cout << "Base2\n";
    }
};

struct DerivedSimple : Base1, Base2 {
};

struct DerivedOverride : Base1, Base2 {
    void foo() override {  // Overrides both. Should be compatible in return value.
        // Base1::foo();
        // Base2::foo();
        std::cout << "DerivedOverride\n";
    }
    // Cannot override 'Base1::foo()' and 'Base2::foo()' independently.
};

int main() {
    {
        std::cout << "===== DerivedSimple =====\n";
        DerivedSimple ds;
        ds.Base1::foo();  // non-virtual call
        ds.Base2::foo();  // non-virtual call
        // ds.foo();  // ambiguous
        static_cast<Base1&>(ds).foo();
        static_cast<Base2&>(ds).foo();
    }
    {
        std::cout << "===== DerivedOverride =====\n";
        DerivedOverride dorr;
        dorr.Base1::foo();  // non-virtual call
        dorr.Base2::foo();  // non-virtual call
        dorr.foo();  // non-ambiguous: the one from DerivedOverride
        static_cast<Base1&>(dorr).foo();
        static_cast<Base2&>(dorr).foo();
    }
}
```

#### diamond problem 

хотим наследование множественное без клонирования классов

виртуальное наследование!!!! - наследование, которое помогает нам избавиться от дубликатов

нам тогда в классе, который множественно отнаследовался, нужно еще вызвать конструктор от базового класса тех классов, от которых мы отнаследовались

```c++
#include <string>
#include <iostream>

/*
Class diagram:

      Person
       ^  ^
    v./    \v.
     /      \
 Employee Student
    ^        ^
     \      /
      \    /
   MagicStudent

Proposed layout (careful: in reality Person is in the end):
+-----------------------------------------------------------------------------+
| +--------+  +------------------------------+  +---------------------------+ |
| | Person |  | +---------------+            |  | +---------------+         | |
| | +name  |  | | PersonVirtual |            |  | | PersonVirtual |         | |
| +--------+  | | +person       |            |  | | +person       |         | |
|             | +---------------+            |  | +---------------+         | |
|             | EmployeeWithVirtual          |  | StudentWithVirtual        | |
|             |                    +employer |  |                    +group | |
|             +------------------------------+  +---------------------------+ |
| MagicStudent                                                                |
+-----------------------------------------------------------------------------+
or:
+-------------------------------------------+
| +--------+  +---------------------------+ |
| | Person |  | +---------------+         | |
| | +name  |  | | PersonVirtual |         | |
| +--------+  | | +person       |         | |
|             | +---------------+         | |
|             | StudentWithVirtual        | |
|             |                    +group | |
|             +---------------------------+ |
| Student                                   |
+-------------------------------------------+
*/

struct Person {
    std::string name;
    Person(std::string name_) : name(std::move(name_)) {}
};
struct PersonVirtual {
    Person *person;
    PersonVirtual(Person *person_) : person(person_) {}
};
struct EmployeeWithVirtual : PersonVirtual {
    std::string employer;
    EmployeeWithVirtual(Person *person_, std::string employer_) : PersonVirtual(person_), employer(std::move(employer_)) {}
};
struct StudentWithVirtual : PersonVirtual {
    std::string group;
    StudentWithVirtual(Person *person_, std::string group_) : PersonVirtual(person_), group(std::move(group_)) {}
};

struct MagicStudent : Person, EmployeeWithVirtual, StudentWithVirtual {
    MagicStudent(std::string name_, std::string employer_, std::string group_)
        : Person(std::move(name_))
        , EmployeeWithVirtual(this, std::move(employer_))
        , StudentWithVirtual(this, std::move(group_)) {
    }
};

struct Student : Person, StudentWithVirtual {
    Student(std::string name_, std::string group_)
        : Person(std::move(name_))
        , StudentWithVirtual(this, std::move(group_)) {
    }
};

void hi_employee(EmployeeWithVirtual &e) {
    std::cout << "Hi, " << e.person->name << " employed by " << e.employer << "!\n";
}

void hi_student(StudentWithVirtual &s) {
    std::cout << "Hi, " << s.person->name << " from group " << s.group << "!\n";
}

void hi_magic(MagicStudent &ms) {
    std::cout << "Wow, " << ms.name << " does not exist!\n";
}

int main() {
    {
        MagicStudent ms("Egor", "HSE", "MBD181");
        hi_employee(ms);
        hi_student(ms);
        hi_magic(ms);
    }
    {
        std::cout << "=====\n";
        Student s("Egor", "MBD181");
        hi_student(s);
    }
}
```

но обычно вначале идут невиртуалные отцы, потом виртуальные

#### приколы с множественным наследованием 

все приколы из-за ромбика

static cast не работает с ромбиком, потому что он на этапе компиляции не знает на сколько ему сдвигать указатель при множественном наследовании (динамик работает, так как он знает реальные типы объекта, (чтобы он смог это сделать, нужно чтобы сам базовый класс был полиморфным))


нельзя делать статик каст из виртуального базового класса в какого-то наследника (не знаем на сколько сдвигать на этапе компиляции), от невиртуального базового класса в наследника статик каст делать надо



___________________________

# Лекция 2

заканчиваем множественное наследование

что происходит, если один класс, от которого мы отнаследовались, виртуально наследуется от базового публично, а другой приватно?

базовый класс станет доступным как и из публичного наследования, так и из приватного!! Правило: если можете получить доступ хоть каким-то способом, то можете получить доступ любым образом

если виртуальное наследование убрать - то всё равно поля из приватного наследования все равно остаются доступными

```c++
struct Base {
    int data;
};

struct X : private virtual Base {
};

struct Y : public virtual Base {
};

struct X1 : X {
    void foo() {
        // data = 10;  // access through X, private
    }
};

struct Derived : X, Y {
    void foo() {
        data = 10;  // access is public through Y
        Y::data = 20;  // accessible as well, not surprising
        X::data = 30;  // accessible as well, wow; even with non-virtual inheritance
    }
};

int main() {
}
```

#### delegate to sister

возможность вызывать функции из как бы сестренских структур: какой-то класс отнаследовался от двух структур, у которых общий предок, тогда в этом классе метод из одного класса родителя может вызывать реализацию метода другого класса родителя. (виртуальные функции и наследвания тут играют роль! важно что у родителей именно общий base!!). То есть при виртуальном наследовании только один класс наследник может реализовать метод, и если мы потом отнаследуемся от них всех, то получится что эта реализация теперь не только для одного класса, но и для всех его сестер 



```c++
#include <iostream>

struct Base {
    virtual void foo() = 0;
    virtual void bar() = 0;
};

struct X : virtual Base {
    void foo() override {
        std::cout << "X::foo()\n";
        bar();  // https://isocpp.org/wiki/faq/multiple-inheritance#mi-delegate-to-sister
    }
};

struct Y : virtual Base {
    void bar() override {
        std::cout << "Y::bar()\n";
    }
};

struct Derived : X, Y {  // virtual is important!
};

int main() {
    Derived a;
    a.foo();  // X::foo() --> Base::bar() ~~ Y::bar()
}
```


#### no unique final overrider

когда родители наследуются от одного класса и оверрайдят один и тот же метод, то мы не знам какую реализацию выбрать и возникает эта ошибка компиляции. Если в нас самих переопределить метод, то всё будет гуд. При чем нам так же будут доступны реализации наших родителей


если в самом базовом классе есть реализация метода, но потом еще кто-то один из родителей его оверрайдит, то берем именно оверрайженную версию, потому что она считается более приоритетной

```c++
#include <iostream>

struct Base {
    virtual void foo() = 0;
    virtual void bar() = 0;
};

struct X : virtual Base {
    void foo() override {
        std::cout << "X::foo()\n";
    }
};

struct Y : virtual Base {
    void foo() override {
        std::cout << "Y::foo()\n";
    }
    void bar() override {
        std::cout << "Y::bar()\n";
    }
};

struct Derived : X, Y {  // ok: Derived::foo() calls X::foo() + Y::foo(), Y::bar()
    void foo() override {
        std::cout << "Derived::foo()\n";
        X::foo();  // non virtual call
        Y::foo();  // non virtual call
    }
};

int main() {
    Derived d;
    d.foo();
    std::cout << "=====\n";
    d.X::foo();  // non virtual call
    std::cout << "=====\n";
    d.bar();
    std::cout << "=====\n";
    static_cast<Base&>(d).foo();  // Derived::foo()
}
```

#### смешиваем виртуальное и невиртуальное наследование 

будет много неоднозначностей..., даже c base

в общем случаем - если виртуальное наследования, то предок один, если сколько-то невиртуальных наследовний, то ровно столько же копий базового класса. 

Порядок инииализации такой: сначала всё виртуальное, потом всё не виртуальное
Порядок уничтожения - обратный 


но обычно либо всегда виртуально, либо всегда невирутально

```c++
#include <iostream>

struct Base { int data = 0; };
struct X : virtual Base {};
struct Y : Base {};
struct Z : virtual Base {};
struct Derived : X, Y, Z {};  // warning: multiple 'Base'

/*
    Base 
  v.|  |v.
  -/   \
 /      -----\
X             Z
|     Base   /
|      |    /
|      Y   /
 \--\  |  /
    Derived
*/

int main() {
    Derived d;
    // d.data = 5;  // ambiguous
    // Base &b = d;  // ambiguous
    d.X::data = 10;
    d.Y::data = 20;
    // d.Base::data = 123;  // ambiguous

    std::cout << d.X::data << "\n";
    std::cout << d.Y::data << "\n";
    std::cout << d.Z::data << "\n";

    [[maybe_unused]] Base &bxz = static_cast<X&>(d);
    [[maybe_unused]] Base &by = static_cast<Y&>(d);

    // In general:
    // 1. For each base B: at most one virtual B, arbitrary number of non-virtual.
    // 2. Order of initialization: all virtuals (from base to derived), everything else
    // 3. Order of deinitialization: reverse.
}
```

## Препроцессор и макросы

**чтобы посмотреть только на результат препроцессора используйте ключи:**
- **-Е для g++**
- **/P - для вижака**

препроцессор - стадия до компиляции - первые шесть фаз перед компиляции - потом компляции, инстанцирование шаблонов, линковка

препроцессор -> компилятор -> линковка 

препроцессор вообще ничего не знает про я сам язык с++, корректен код на с++ или нет - препроцессору пофиг

все команды препроцессора начинаются с #, каждая команда должна быть на отдельной строчке


- если в конце строчки написать \ - то препроцессор уберет следующий перевод строки - иногда нужно для длинных комментариев 

- конкатенация - две строчки с двойными кавычками подряд - он их склеивает вместе

- диграфы

- объявление переменных / макросов препроцессора.

- есть предопрделенные макросы (например для с++ макрос __cplusplus)

можно с помощью макросов подавить предопреждения 

```c++
int main() {
#ifdef __GNUC__  // Everything is very compiler-specific, see doctest.h for an example
    // `#pragma` is a compiler command, not preprocessor
    #pragma GCC diagnostic push  // save current warnings state to a stack
    #pragma GCC diagnostic ignored "-Wunused-variable"
#elif _MSC_VER
    // TODO
#endif
    /* [[maybe_unused]] */ int x = 10;
#ifdef __GNUC__
    #pragma GCC diagnostic pop  // restore warnings state
#elif _MSC_VER
    // TODO
#endif
}
```

можно еще вот так, чтобы было короче

```c++
int main() {
#ifdef __GNUC__
    // `_Pramga` does not even look like preprocessor and can be embedded into a macro
    _Pragma("GCC diagnostic push")
    _Pragma("GCC diagnostic ignored \"-Wunused-variable\"")
#elif _MSC_VER
    // TODO
#endif
    /* [[maybe_unused]] */ int x = 10;
#ifdef __GNUC__
    _Pragma("GCC diagnostic pop")
#elif _MSC_VER
    // TODO
#endif
}
```

- макросы раскрываются супер тупо - просто подставляет одно вместо другого, заменяет одну строчку на другую и не вычисляет ничего сам, там только компилятор уже разбирается 

define любого ключевого слова в с++ - UB!!

приколы!

https://github.com/menahishayan/rickroll.h

- asserts можно реализовать только макросами!!!- вывести файл, строчку и тд. #expr - заменится на строчку, то как этот аргумент был передан 

```c++
#include <cassert>
#include <iostream>

void my_assert_1(bool expr) {
    if (!expr) {
        std::cout << "Assertion failed: ???????\n";
    };
}

#define my_assert_2(expr) \
    do { \
        if (!(expr)) { \
            std::cout << "Assertion failed: " #expr " at " __FILE__ ":" << __LINE__ << "\n"; \
        } \
    } while(0)

int main() {
    my_assert_1(2 * 2 == 5);
    my_assert_2(2 * 2 == 5);
    assert(2 * 2 == 5);
}
```

- лямбды в макросах

```c++
#include <iostream>

template<typename F>
void call_twice(F f) {
    f();
    f();
}

struct Foo {
    int x = 10;

    void print_x() {
        std::cout << x << "\n";
    };
};

void print_hello() {
    std::cout << "print_hello\n";
}

int main() {
    Foo f;
    print_hello();
    call_twice(print_hello);

    f.print_x();
    // call_twice(f.print_x);
    // call_twice(&Foo::print_x);
    call_twice([&]() { f.print_x(); });

#define call_twice_m(f) call_twice([&]() { f(); })
    call_twice_m(print_hello);
    call_twice_m(f.print_x);
}
```
- соединять вместе слова в макросах с помощью ## - иногда нужно для генерации каких-то имен для переменных

```c++
#include <iostream>

#define FOO(a, b) st##a##b
#define NEW_VAR(id) int x##id

int main() {
    FOO(d::c, out) << "Hello World\n";
    // std::cout 
    NEW_VAR(123) = 10;  // int x123 = 10;
    std::cout << x123 << "\n";
}
```

тут какие-то сложные правила в вычислении аргументов при раскрытии макросов? поэтому нужны доп макросы concat1 и concat2

```c++
#include <iostream>

#define CONCAT2(a, b) a##b
#define CONCAT1(a, b) CONCAT2(a, b)
#define new_var int CONCAT1(var_, __COUNTER__)

int main() {
    new_var = 500;
    new_var = 600;
    new_var = 700;
    std::cout << var_0 << "\n";
    std::cout << var_1 << "\n";
    std::cout << var_2 << "\n";
}
```

еще работа с enum и макросами
передаем макрос внутрь макроса

```c++
#include <iostream>

// X macros: https://en.wikipedia.org/wiki/X_macro

#define LIST_E(f) f(Option1) f(Option2) f(Option3)
enum E {
   #define OPTION(e) e, // заменить enum на макрос? - легко
   LIST_E(OPTION)
   // OPTION(Option1) OPTION(Option2) OPTION(Option3)
   // Option1, Option2, Option3,
   #undef OPTION
};

int main() {
    E e = Option2;
    switch (e) {
    case Option1: std::cout << "Option1\n"; break;
    case Option2: std::cout << "Option2\n"; break;
    case Option3: std::cout << "Option3\n"; break;
    }

    switch (e) {
    #define PRINT_OPTION(e) case e: std::cout << #e "\n"; break;
    LIST_E(PRINT_OPTION)
    #undef PRINT_OPTION  // undefine, remove macro
    }
}
```

можно с помощью макросов отключить вычисление аргументов или вызовов каких-то функций вообще


```c++
#include <iostream>

int foo() {
    std::cout << "Works a lot!\n";
    return 10;
}

bool logging_enabled = true;

#define log(x) if (logging_enabled) { std::cout << x << "\n"; }
// LOTS OF TROUBLES DO NOT USE

int main(int argc, char*[]) {
    if (argc > 1) {
        logging_enabled = false;
    }

    if (logging_enabled) {
        std::cout << foo() << "\n";
    }
    log(foo());
}
```
еще вариант
```c++
#include <iostream>

int foo() {
    std::cout << "Works a lot!\n";
    return 10;
}

bool logging_enabled = true;

#define LOG if (logging_enabled) std::cout

int main(int argc, char*[]) {
    if (argc > 1) {
        logging_enabled = false;
    }

    if (logging_enabled) {
        std::cout << foo() << "\n";
    }
    LOG << foo() << "\n";
}
```

проблемы с макросами:
- игнорируют любые скобки, скоупы и тд 

```c++
#include <vector>

namespace foo_ns {
void foo() {
    std::vector<int> v;
#define add(x) v.push_back(x)
    add(10);
    add(20);
    add(30);
    add(40);
// #undef add
};
}

/*int add(int a, int b) {
    return a + b;
}*/
int add(int a) {
    return a + 10;
}

int main() {
    foo_ns::foo();
}
```

- использовать макросы как замену функциям - очень плохо!!!


#### variadic macros 
есть первые два параметра, а потом сколько угодно \__VA_ARGS\__

```c++
// Compile with `g++ -E` to see preprocessed output.

// variadic macro
#define foo(a, b, ...) bar(b, a, __VA_ARGS__)

int main() {
    foo(1, 2, 3, 4);
    foo(1, 2, 3);

    // Oopses:
    foo({1, 2, 3, 4});
    foo(({1, 2, 3, 4}), 5, 6);
    foo(1, 2);  // Can be fixed with C++20's __VA_OPT__ or GCC's extension of ##.
    foo(std::map<int, int>(), std::map<int, int>());
    foo((std::map<int, int>()), (std::map<int, int>()));
}
```

пример проблем поведения из-за , с ассертом:

```c++
#include <cassert>
#include <vector>
int main() {
    // Example: assert
    std::vector<int> v;
    assert((v == std::vector{1, 2, 3})); // ассерт получил три аргумента, а хочет один
}
```


# Лекция 3

Макросы так просто рекурсию не поддерживают, так как они полностью должны раскрываться на этапе компиляции, и макросы ничего не знаю про значение или вычисление таким образом 

```c++
#include <iostream>

#define FOO(x) if (x > 0) { std::cout << x; FOO(x - 1) }

// Extra reading: Boost::PP (preprocessor).

// TODO: actually kind of recursion is possible, see Habr article

int main() {
    FOO(5);
}
```


### Обработка ошибок!

разные ссылочки от Егора:

http://joeduffyblog.com/2016/02/07/the-error-model/

John Lakos "Defensive Programming Done Right": https://www.youtube.com/watch?v=1QhtXRMp3Hg https://www.youtube.com/watch?v=tz2khnjnUx8

Bloomberg error reporting: https://bloomberg.github.io/bde-resources/doxygen/bde_api_prod/group__bsls__assert.html#4.1 https://bloomberg.github.io/bde-resources/doxygen/bde_api_prod/group__bsls__assert.html#4.2

think-cell's error reporting: https://youtu.be/Cmud1jO__VA?t=2318


Про обработку ошибок всегда нужно думать заранее!!

Классификация ошибок: 
- те ошибки которые мы можем предугадать + то, что мы собираемся обрабатывать
- необработанные предсказуемые ошибки 


широкие контракты - возвращение какого-то специального значения при возникновении ошибки, спихивание проблемы решения ошибки на того, кто вызвал этот метод(например)

```c++
int max(const std::vector<int> &v) {
    if (v.empty()) return -1;
    .....
}
std::vector<int> readVector(std::istream &is) {
    int n;
    if (!(is >> n)) return {};  // bad
    std::vector<int> v(n);
    // .....
    return v;
}
```

Если очень хочется широкие контракты - через nullopt и optional

- -1 может быть корректным значением в векторе.
- Эти условия может быть несложно проверить перед вызовом функции.
- Обычно пользователи не думают про то, что вектор может быть пустой. Даже если написано в документации. Поэтому у функции проблем нет, а у программы в целом есть.


Узкие контракты - функция может работать только в минимальном количестве случаев, если не попадаем ни в один из них - то функция роняет программу

```c++
int max(const std::vector<int> &v) {
    assert(!v.empty());  // standard C++
    _ASSERT(!v.empty());  // think-cell
    BSLS_ASSERT(!v.empty());  // Bloomberg
    .....
}
std::vector<int> readVector(std::istream &is) {
    int n;
    is >> n;
    assert(is);  // standard C++
    _ASSERT(is);  // think-cell
    BSLS_ASSERT(is);  // Bloomberg
    std::vector<int> v(n);
    // .....
    return v;
}
```

+ Непредсказуемые ошибки(либо проверять(ставить assert), не проверять - когда ошибка случится - будет больно)
+ Ошибки, которые обработать нельзя(что то типа UB, нарушение инвариантна)


#### Разные способы обработки ошибки 

- ошибка привязана к объекту, у объекта есть операции которые можно делать, у объекта есть состояние

object-state-flag

```c++
#include <fstream>
#include <iostream>
#include <string>

void check_file(const std::string &filename) {
    std::ifstream f(filename);
    std::cout << "Reading from " << filename << "\n";
    // Easy to forget to check.
    std::cout << "    is_open: " << f.is_open() << "\n";
    for (;;) {
        // https://en.cppreference.com/w/cpp/io/ios_base/iostate
        // State, not result of operation
        std::cout << "    status: " << f.rdstate() << "; "
                  << std::ios_base::badbit << " "  // irrecoverable error
                  << std::ios_base::failbit << " "  // format/extract error
                  << std::ios_base::eofbit << "\n";  // EOF reached
        if (!f) {
            break;
        }
        int x = -239;
        // Careful handling is actually required: https://en.cppreference.com/w/cpp/named_req/FormattedInputFunction
        f >> x;
        std::cout << "    x = " << x;
    }
}

int main() {
    check_file("numbers-01-simple.in");
    check_file("numbers-02-immediate-eof.in");
    check_file("does-not-exist.in");
}
```


- Глобальное состояние ошибок (сишный подход), errno(не совсем глобальная переменная, а какой то макрос, который хранит информацию про возникающие ошибки в коде)

```c++
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>

void check_file(const std::string &filename) {
    FILE *f = std::fopen(filename.c_str(), "r");
    std::cout << "Reading from " << filename << "\n";
    std::cout << "    f: " << f << ", errno: " << errno << " " << std::strerror(errno) << "\n";  // POSIX (not C/C++) requires that errno macro is set.
    // Problem: have to check immediately after each function. Does not propagate up.
    if (f == nullptr) {
        return;
    }
    for (;;) {
        int x = -239;
        int read = std::fscanf(f, "%d", &x);  // Still easy to forget to check.
        std::cout << "    x = " << x << ", read = " << read << ", errno = " << errno << " " << std::strerror(errno) << "\n";
        if (read != 1) {
            break;
        }
    }
    std::fclose(f);
}

int main() {
    check_file("numbers-01-simple.in");
    check_file("numbers-02-immediate-eof.in");
    check_file("does-not-exist.in");
}
```

- функции возвращают всегда код ошибки (например SQLITE3)
- концепция из rust: не можем вызвать функции и без обработки ошибок получить её результат, надо разобрать случай


Исключения! - механизм, который позволяет в любом месте программы кинуть ошибку, а обработать её где-то в другом месте

throw - новая операция, которая принимает произвольное значение, которое описывает произошедшую ошибку, функция аварийно завершается.

ошибки пролетают c нижнего уровня на верхний 

обрабатываются с помощью try catch

```c++
#include <cstddef>
#include <vector>
#include <iostream>

struct invalid_vector_format {};

std::vector<int> read_vector() {
    int n;
    if (!(std::cin >> n)) {
        throw invalid_vector_format();
    }
    std::vector<int> result(n);
    for (int i = 0; i < n; i++) {
        if (!(std::cin >> result[i])) {
            throw invalid_vector_format();
        }
    }
    return result;
}

void write_answer(const std::vector<int> &vec) {
    for (std::size_t i = 0; i < vec.size(); i++) {
        if (i) std::cout << " ";
        std::cout << vec[i];
    }
    std::cout << "\n";
}

void solve() {
    std::vector<int> a = read_vector();  // if exception is thrown, it's propagated
    std::vector<int> b = read_vector();  // if exception is thrown, it's propagated; local variables are cleaned up (stack unwind, раскрутка стека).
    
    // Тут какие-то вычисления
    std::vector<int> answer = a;
    answer.insert(answer.end(), b.begin(), b.end());

    write_answer(answer);
}

int main() {
    try {
        solve();  // if exception is thrown, it's propagated
    } catch (const invalid_vector_format &err) {  // looking for the first catch
        std::cout << "Invalid vector format\n";
        // Print good error message is hard:
        // 1. How exactly the format is invalid? We could add it to the exception.
        // 2. We don't have much context here: what exactly failed? A or B?
        // 3. We don't have that context in read_vector() either.
        // Consequence: this is probably not a great error handling.
        //              Not because of exceptions per se, but because we did not think _in advance_.
    }
}
```

catch(...) - поймать любые виды исключений(только где явно написано throw)
\+ catch работает с наследованием (только с наследованием (или конктреными типами), не работает с неявным преобразованием типов)
Как работает с наследованием ошибок? Важно ловить в таком случае по ссылке! а не по значению, дабы избежать slicing!!!

```c++
#include <iostream>

struct Base {  // std::exception
    virtual const char *who() const {  // std::exception::what()
        return "Base";
    }
};

struct Derived : Base {
    const char *who() const override {
        return "Derived";
    }
};

struct Magic {
    operator Base() { return {}; }
};

int main() {
    try {
        throw Derived();
    } catch (const Base &e) {
//    } catch (Base e) {
        std::cout << "Caught: " << e.who() << "\n";
    }

    try {
        [[maybe_unused]] const Base &b = Magic();
        throw Magic();
    } catch (const Base &e) {  // Does not work, needs ~type match or inheritance.
        std::cout << "Caught Base\n";
    }
}
```

Важно! try ловит только то, что находится между фигурных скобок!!!

большой примерчик:

```c++
#include <iostream>

struct err1 {};
struct err2 {};

void bar() {
    try {
        try {
            std::cerr << "3\n";
            throw err1();
            std::cerr << "x3\n";
        } catch (const err1 &e) {
            std::cerr << "4\n";
            throw err2();
            std::cerr << "x4\n";
        } catch (const err2 &e) {
            std::cerr << "x5\n";
        }
    } catch (int e) {
        std::cerr << "caught int???\n";
    } catch (const err2 &e) {
        std::cerr << "caught err2!\n";
        throw e;
    }
    std::cerr << "bar end\n";
}

void foo() {
    try {
        try {
            std::cerr << "2\n";
            bar();  // std::cerr << "3\n4\n"; throw err2();
            std::cerr << "x2\n";
        } catch (const err1 &e) {
            std::cerr << "z\n";
        }
        std::cerr << "x21\n";
    } catch (int e) {
        std::cerr << "x22\n";
    }
    std::cerr << "foo end\n";
}

int main() {
    try {
        std::cerr << "1\n";
        foo();
        std::cerr << "xxx\n";
    } catch (const err2 &) {
        std::cerr << "5\n";
    }
    std::cerr << "main continues\n";
}
```

исключения вызывают деструкторы!!! - это очень полезно
все локальные переменные, которые пропускаются при переходе от throw к catch, у них вызывается деструктор(в обратном порядке) (париться **почти** не о чем)

можно запустить, посмотреть 

```c++
#include <iostream>

struct err1 {};
struct err2 {};

struct with_destructor {
    int key;
    with_destructor(int key_) : key(key_) {}
    ~with_destructor() {
        std::cerr << "Destructed " << key << "\n";
    }
};

with_destructor wd111(111);

void bar() {
    with_destructor wd31(31);  // Implementation-defined stack unwind.
    try {
        with_destructor wd40(40);
        std::cerr << "3\n";
        throw err1();
        std::cerr << "x3\n";
    } catch (const err1 &e) {
        std::cerr << "4\n";
        throw err2();
        std::cerr << "x4\n";
    }
    std::cerr << "bar end\n";
}

void foo() {
    with_destructor wd20(20);  // Implementation-defined stack unwind.
    try {
        with_destructor wd30(30);  // Implementation-defined stack unwind.
        std::cerr << "2\n";
        bar();
        std::cerr << "x2\n";
    } catch (const err1 &e) {
        std::cerr << "z\n";
    }
    std::cerr << "foo end\n";
}

int main() {
    std::cerr << "1\n";
    {
        with_destructor wd10(10);
    }
    try {
        foo();
    } catch (...) {
        std::cerr << "caught!\n";
    }
    std::cerr << "main end\n";
}
```

Если исключения в итоге никто не поймал, то вызыв деструкторов(как для локальных, так и для глобальных) никто не гарантирует, так как программа понимает, что программа аварийно завершится

```c++
#include <iostream>

struct err1 {};
struct err2 {};

struct with_destructor {
    int key;
    with_destructor(int key_) : key(key_) {}
    ~with_destructor() {
        std::cerr << "Destructed " << key << "\n";
    }
};

with_destructor wd111(111);

void bar() {
    with_destructor wd31(31);  // Implementation-defined stack unwind.
    try {
        with_destructor wd40(40);
        std::cerr << "3\n";
        throw err1();
        std::cerr << "x3\n";
    } catch (const err1 &e) {
        std::cerr << "4\n";
        throw err2();
        std::cerr << "x4\n";
    }
    std::cerr << "bar end\n";
}

void foo() {
    with_destructor wd20(20);  // Implementation-defined stack unwind.
    try {
        with_destructor wd30(30);  // Implementation-defined stack unwind.
        std::cerr << "2\n";
        bar();
        std::cerr << "x2\n";
    } catch (const err1 &e) {
        std::cerr << "z\n";
    }
    std::cerr << "foo end\n";
}

int main() {
    std::cerr << "1\n";
    {
        with_destructor wd10(10);
    }
    //try {
        foo();
    //} catch (...) {
    //    std::cerr << "caught!\n";
    //}
    std::cerr << "main end\n";
}
```


Безопасность исключений: (exception safety)
- нет гарантий
- базовая 
- строгая 
подробнее позже, но вот пример базовой гарантии, когда хотелось строгую, дабы избежать покацанных данных

```c++
#include <iostream>
#include <stdexcept>

struct cannot_read_int {
    cannot_read_int() {}
};

int readInt() {
    int x;
    std::cin >> x;
    if (!std::cin) {
        throw cannot_read_int();
    }
    return x;
}

struct Point {
    int x = 0, y = 0;
    void read() {  // what exception safety? (безопасность исключений)
        x = readInt();
        y = readInt();
    }
};

int main() {
    Point p;
    for (;;) {
        try {
            p.read();
        } catch (const cannot_read_int &) {
            std::cout << "Cannot read int\n";
            break;
        }
        std::cout << "Read point " << p.x << " " << p.y << "\n";
    }
    std::cout << "Last point is " << p.x << " " << p.y << "\n";
}
```

###### стандартные исключения
все наследуются от std::exception

какие есть 
- std::bad_alloc - закончилась память (вектор от -1)
- std::bad_cast - например, не корректный dynamic_cast
- std::invalid_argument - например, std::stoi передали не число на самом деле
- std::out_of_range - например std::stoi передали слишком большое число, или выход за границы массива

std::runtime_error и std::logic_error - промежуточный уровень между std::exception и всеми перечисленными выше

catch может не сработать при наследовании если мы делаем наследование не через struct, а через class(приватное наследование всё портит)

Основной посыл от Егора: 

    Do not blindly use exceptions. Think about error handling first. Use exceptions only when it's easier than error codes.

    Especially do not use exceptions with programming errors. You can always easily go from error codes to exceptions, vice-versa is harder. Exceptions should be part of a contract.


# Лекция 4

#### exception ptr

с его помощью мы можем "сохранить" тип текущего исключения 
после того как мы его сохранили/поймали, и оно у нас теперь хранится в переменной типа std::exception_ptr по указателю (скорее всего shared_ptr). Мы не можем узнать его тип, но! мы можем сохраненное исключение кинуть еще раз! std::rethrow_exception(err) - ловиться так же обычными try catch

```c++
#include <exception>
#include <iostream>

// https://habr.com/ru/post/160955/

struct my_exception {
    int value;
};

void foo(int x) {
    throw my_exception{x + 10};
}

int main() {
    try {
        foo(0);
    } catch (my_exception &e) {
        std::cout << "e=" << e.value << "\n";
    }

    std::exception_ptr err;  // shared_ptr<???>
    // Can be created using std::make_exception_ptr or std::current_exception.
    auto save_exception = [&]() {
        // Can be called at any point in the programm, not necessarily right inside `catch`.
        // If there is no "current" exception, returns `exception_ptr{}`.
        err = std::current_exception();
    };

    try {
        std::cout << "before foo(1)\n";
        foo(1);
        std::cout << "after foo(1)\n";
    } catch (...) {
        std::cout << "saving exception...\n";
        save_exception();
        // Out-of-scope: one can build nested exceptions: https://en.cppreference.com/w/cpp/error/nested_exception
    }

    try {
        if (err) {
            // The only way to 'read' `exception_ptr`.
            std::rethrow_exception(err);  // Requires non-empty `err`.
        } else {
            std::cout << "no exception\n";
        }
    } catch (my_exception &e) {
        std::cout << "e=" << e.value << "\n";
    }
}
```


##### throw 
- просто перебрасывает текущее исключение!! + может работать не прямо в catch, а например в каком нибудь спец методе (как rethrow тут)

```c++
#include <exception>
#include <stdexcept>
#include <vector>
#include <iostream>

struct invalid_vector_format : std::runtime_error {
    invalid_vector_format() : std::runtime_error("Invalid vector format") {}
};

std::vector<int> read_vector() {
    int n;
    if (!(std::cin >> n)) {
        throw invalid_vector_format();
    }
    std::vector<int> result(n);
    for (int i = 0; i < n; i++) {
        if (!(std::cin >> result[i])) {
            throw invalid_vector_format();
        }
    }
    return result;
}

void write_answer(const std::vector<int> &vec) {
    for (std::size_t i = 0; i < vec.size(); i++) {
        if (i) std::cout << " ";
        std::cout << vec[i];
    }
    std::cout << "\n";
}

void rethrow() {
    std::cout << "Rethrowing...\n";
    throw;
}

void solve() {
    std::vector<int> a;
    try {
        a = read_vector();
    } catch (const invalid_vector_format &e) {
        std::cout << "Caught error while reading vector a, propagating...\n";
        // throw e;  // rethrow exception 'e', but what about other types? + slicing!! - do not write 
        // throw;  // does the same
        rethrow();  // does the same, but indirectly
    }
    std::vector<int> b;
    try {
        b = read_vector();  // maybe 'bool read_vector()' is better.
    } catch (...) {
        std::cout << "Caught error while reading vector b, propagating...\n";
        throw;  // works with `...`
    }
    
    std::vector<int> answer = a;
    answer.insert(answer.end(), b.begin(), b.end());

    write_answer(answer);
}

int main() {
    try {
        solve();
    } catch (const std::exception &err) {
        std::cout << err.what() << "\n";
    }
}
```

Надо просто throw, не throw e, потому что throw e - slicing!!


##### исключения межно кидать между единицами трансляции

файл раз:

```c++
#include <exception>
#include <iostream>

void foo();

int main() {
    try {
        foo();
    } catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << "\n";
    }
}
```

файл два 
```c++
#include <iostream>

void bar(int i);

void foo() {
    for (int i = 0; i < 5; i++) {
        std::cout << "i=" << i << "\n";
        bar(i);
    }
}
```

файл три
```c++
#include <exception>

struct my_error : std::exception {
    const char *what() const noexcept override {
        return "my_error";
    }
};

void bar(int i) {
    if (i == 3) {
        throw my_error();
    }
}
```

### Гарантии исключений 
- нет гарантии(может быть утечка памяти, UB и тд)
```c++
void no_guarantee() {
    std::cout << "2+2=";
    int *data = new int[10];
    do_something(); // что если тут было исключение
    delete[] data;
    std::cout << "4\n";
}
```
- базовая гарантия (хотя бы нет UB, нет утечек памяти, но объекты могут менятся)
```c++
void basic_guarantee_2() {
    std::cout << "2+2=4\n";
    std::vector<int> data(10);
    do_something();
}
```
- строгая гарантия (базавое + ничего не поменялось)

```c++
void strong_guarantee() {
    std::vector<int> data(10);
    do_something();
    // What is "nothing"? Can we output?
    std::cout << "2+2=4\n";
}
```
- не кидает исключений (обозначется словом noexcept)

```c++

void noexcept_guarantee() {
    assert(std::cout.exceptions() == std::ios_base::goodbit);
    std::cout << "2+2=4\n";
}
```

### RAII

- если захватываем какой-то ресурс (например, память) - делаем это в конструкторе. Если конструктор успешно завершился то гарантируется что объект в корректном состоянии
- деструктор - полное освобождение всех ресурсов

```c++
#include <iostream>
#include <vector>

int main() {
    // RAII: Resource Acquisition Is Initialization
    {
        std::vector<int> v(1'000'000);
        // Invariant: v.size() == 1'000'000, memory allocation succeeded.
        v[999'999] = 123;
        // RAII part 1: constructor has to establish invariant and grab resources. Throw an exception if that's impossible. No exit codes.
    }  // RAII part 2: destructor has to free all resource.

    try {
        std::vector<int> v(100'000'000'000);
        // Invariant: memory allocation succeeded.
        v[99'999'999'999] = 123;
    } catch (std::bad_alloc &) {
        // Impossible to access 'v' with incorrect invariant!
        std::cout << "caught bad_alloc\n";
    }
    // Jargon: 'RAII' may only mean part 2: destructor cleans up everything.
    // Constructing object is less strict, see `ifstream`.

    // Other languages: `finally` block or special 'try-with-resources'
    // syntax (Java) or `with` statement (Python). In Go: `defer` statement.
}
```


Как поймать исключения из конструкторов и отфильтровать его от других возможных? 

```c++
#include <iostream>
#include <vector>

void may_throw(std::vector<int> &) {
    // maybe throw exception
}

int main() {
    try {
        std::vector<int> v(100'000'000'000);
        try {
            v[99'999'999'999] = 123;
            may_throw(v);
        } catch (...) {
            std::cout << "caught exception not inside v ctor\n";
        }
    } catch (std::bad_alloc &) {
        std::cout << "caught bad_alloc inside v ctor\n";
    }
}
```


### noexcept 
- гарантированно не бросает исключения

если из noexcept функции вылетает исключение, то вызывается std::terminate - завершает программу аварийно

(если исключение не покидает пределы функции, то она может быть noexcept, всё ок)

noexcept функции можем спокойно вызывать из любого места программы и быть увереным, что исключения не будет

```c++
#include <iostream>

void check_n(int n) {
    if (n < 0) {
        throw 0;
    }
}

void foo() noexcept {  // If exception: std::terminate(), no dtor calls
    std::cout << "foo() start\n";
    int n;
    std::cin >> n;
    check_n(n);
    std::cout << "foo() end\n";
}

// There was no difference with/without noexcept with this example, but it may happen:
// 1. Say hello to no-noexcept-move and strong exception safety (will be later).
// 2. One can optimize code like `i++; foo(); i++;` => `foo(); i += 2;`
// 3. There is no need to prepare for stack unwind.

int main() {
    try {
        foo();
    } catch (...) {
        std::cout << "Caught\n";
    }
}
```

Типичный пример noexcept - деструктор:

Что если деструктор может вызывать два исключения? - аварийное завершение 
с с++ 11 все деструкторы неявно помечены noexcept, c++03 - падаем.

```c++
#include <iostream>

void maybe_throw() {
    static int remaining = 2;
    if (!--remaining) {
        throw 0;
    }
}

struct Foo {
    Foo() {
        std::cout << "Foo() " << this << "\n";
    }
    ~Foo() {
        std::cout << "~Foo() " << this << "\n";
        maybe_throw();
    }
};

#if __cplusplus > 200301L
#error This file requires C++03
#endif

int main() {
    try {
        try {
            Foo a;
            Foo b;
            Foo c;
            std::cout << "before throw\n";
            // throw 1;  // TODO: try uncommenting
            std::cout << "after throw\n";
        } catch (int x) {
            std::cout << "caught " << x << "\n";
        } catch (...) {
            std::cout << "caught something\n";
        }
    } catch (...) {
        std::cout << "caught outside\n";
    }
    // C++03: two exceptions simultanously => std::terminate().
    // C++11: dtors are noexcept, always std::terminate().
}
```

бонусы noexcept:
- документация для функции 
- наличие noexcept можно проверять во время выполнения
- компилятор может не париться 

минусы: 
- нет гарантий что это реально так 
- запрещает negative testing?
- что то типа wide contract 

типичный noexcept - move-constructors, move-assignment

##### гарантии исключения для копирования

плохо

```c++
    IntWrapper& operator=(const IntWrapper &other) {
        if (this == &other) {
            return *this;
        }
        if (!other.data) {
            delete data;
            data = nullptr;
        } else {
            // 1. no exception safety :(
            // 2. data is always re-allocated, unlike with move below.
            delete data;
            data = new int(*other.data);
        }
        return *this;
    }
```

нормально

```c++
  IntWrapper& operator=(const IntWrapper &other) {
        if (this == &other) {
            return *this;
        }
        if (!other.data) {
            delete data;
            data = nullptr;
        } else {
            // The only operation that may throw.
            int *new_data = new int(*other.data);
            // Operations that do not throw.
            delete data;
            data = new_data;
        }
        return *this;
    }
```

##### copy-swap idiom

позволяет написать оператор присваивания короче и безопасно с точки зрения исключений - пишем один оператор присваивания который принимает other по значению, дальше меняемся с ним местами(позволяет писать вместо правила пяти "правило четырёх"), но неэффективно с самоприсваиванием например + всегда выделит память

```c++
 IntWrapper& operator=(IntWrapper other) /* TODO: noexcept */ {  // copy or move to the temporary
        // swap with the temporary
        std::swap(data, other.data);
        // std::swap(*this, other);  // do not work, as it calls `operator=`
        return *this;
        // destroy the temporary `other`
    }
```

#### скорость

throw в программе замедляет программу(даже если он никогда не кидается, то все равно чуть чуть тормозит), если кидается и ловится много исключений, то очень долго работает. Исключения тормозят программу примерно в 1000 раз.


можно поэксперементировать с этим кодом

```c++
#include <chrono>
#include <iostream>

// Conclusion: exceptions are for exceptional cases only! Sad path is much, much slower.

// TODO: disable some optimizations?

bool foobar(int a, int b, bool please_throw) {
    if (please_throw) {
        if (a % b) {
            throw 0;  // todo: commenting out makes program faster
            return false;
        }
        return true;
    }
    return a % b == 0;
}

const int STEPS = 1'000'000; // Note: much fewer steps.

int main() {
    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < STEPS; i++) {
        try {
            foobar(12, 3, true);
        } catch (...) {
        }
    }

    auto duration = std::chrono::steady_clock::now() - start;
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms\n";
}
```

### exceptions everywhere

Почему pop() у stack<> ничего не возвращает? потому что нельзя так задать разумную гарантию исключений

напишем свой стэк. Проблема возникает тогда, когда мы пытаемся использовать наш pop(). Может возникнуть исключение при операторе присваивания(если нет move semantics), невозможно проконтролировать наличие искючение и корректность присваивания. 

```c++
#include <vector>

template<typename T>
struct stack {
    std::vector<T> data;

    T pop() noexcept {
        T result = std::move(data.back());
        data.pop_back();
        return result;
    }
};

struct Foo {
    int x;
    Foo(int x) : x(x) {}
};

int main() {
    stack<Foo> s;
    s.data = std::vector<Foo>{1, 2, 3, 4, 5};

    Foo x(123);
    x = s.pop();  // If exception is thrown by `operator=` (C++03), `s` cannot restore stack state.
                  // Looks like strong guarantee violation.
}
```

как чинить?

```c++
#include <vector>

template<typename T>
struct stack {
    std::vector<T> data;

    void pop_into(T &result) noexcept {
        result = std::move(data.back());
        data.pop_back();
    }
};

int main() {
    stack<int> s;
    s.data = std::vector{1, 2, 3, 4, 5};

    int x = 10;
    s.pop_into(x);  // All exceptions are handled by `pop` itself.
}
```

#### разные места откуда вылетают исключения и что тогда делать?

далее везде используется такая структура:


```c++
#include <iostream>

template<int X, bool SHOULD_THROW = false>
struct with_destructor {
    // If throws, object is dead, basic safety ~ strong safety.
    with_destructor(bool should_throw = false) {
        std::cout << "constructing " << X << " " << this << "\n";
        if (SHOULD_THROW || should_throw) {
            throw 0;
        }
    }
    ~with_destructor() {
        std::cout << "destructing " << X << " " << this << "\n";
    }
};

int main() {
    try {
        with_destructor<0> x;
        with_destructor<1> y(true);
//        with_destructor<1, true> y;
        with_destructor<2> z;
    } catch (...) {
        std::cout << "Caught\n";
    }
}
```



1) если исключение кинул конструктор, то объект считается не созданным и деструктор не вызывается

что если массив? - так же, если при создании очередного элемента массива выскочило исключение, то уже созданные ранее объекты вызывают деструктор(кажется, гарантированно в обратном порядке, у вектора кажется, порядок не гарантирован, но это не точно).

```c++
int main() {
    try {
        with_destructor<1> arr[]{false, false, true, false};
        // TODO? Guarantee: order of destruction is reverse of construction
    } catch (...) {
        std::cout << "Caught\n";
    }
    std::cout << "==========\n";
    try {
        std::vector<with_destructor<1>> vec{false, false, true, false};
    } catch (...) {
        std::cout << "Caught\n";
    }
}
```

что если поле класса не смогло создаться? то уже созданные также удаляются в обратном порядке.(конструкторы, десткрукторы структуры еще даже не вызываются, так как не созданы поля)

```c++
#include <iostream>

template<int X, bool SHOULD_THROW = false>
struct with_destructor {
    with_destructor(bool should_throw = false) {
        std::cout << "constructing " << X << "\n";
        if (SHOULD_THROW || should_throw) {
            throw 0;
        }
    }
    ~with_destructor() {
        std::cout << "destructing " << X << "\n";
    }
};

struct Foo {
    with_destructor<1> a;
    with_destructor<2> b;
    with_destructor<3, true> c;
    with_destructor<4> d;

    Foo() {
        std::cout << "Foo()\n";
    }
    ~Foo() {
        std::cout << "~Foo()\n";
    }
};

int main() {
    try {
        with_destructor<0> x;
        Foo f;
    } catch (...) {
        std::cout << "Caught\n";
    }
}
```

с базовыми классами всё тоже гуд, так как сначала создается базовый класс, а после уже наследники, но всё равно все нормально, только у base уже будет вызываться деструктор.

делегирующие конструкторы: считается, что объект создан, когда завершен один конструктор

нет деструктора

```c++

struct Foo {
    with_destructor<1> a;
    with_destructor<2> b;
    with_destructor<3> c;
    with_destructor<4> d;

    Foo() {
        std::cout << "Foo()\n";
        std::cout << "throwing\n";
        throw 0;
    }
    Foo(int) : Foo() {  // After a single ctor is completed, object is created.
        std::cout << "Foo(int)\n";
    }
    ~Foo() {
        std::cout << "~Foo()\n";
    }
};
```

есть деструктор

```c++

struct Foo {
    with_destructor<1> a;
    with_destructor<2> b;
    with_destructor<3> c;
    with_destructor<4> d;

    Foo() {
        std::cout << "Foo()\n";
    }
    Foo(int) : Foo() {  // After a single ctor is completed, object is created.
        std::cout << "Foo(int)\n";
        std::cout << "throwing\n";
        throw 0;
    }
    ~Foo() {
        std::cout << "~Foo()\n";
    }
};
```


аргументы функции - если при создании чего-то выскочило исключение, то уже созданные аргументы удаляются, до самой функции информация об исключении не доходит, функция не вызывается



что, если параметры сложные?
юники? что с delete - до с++17 компилятор имел право вычислять аргументы вперемежку и из-за этого могли быть странные баги

```c++
#include <iostream>
#include <memory>

template<int X, bool SHOULD_THROW = false>
struct with_destructor {
    // If throws, object is dead, basic safety ~ strong safety.
    with_destructor(bool should_throw = false) {
        std::cout << "constructing " << X << " " << this << "\n";
        if (SHOULD_THROW || should_throw) {
            throw 0;
        }
    }
    ~with_destructor() {
        std::cout << "destructing " << X << " " << this << "\n";
    }
};

template<typename T1, typename T2, typename T3>
void foo(
    [[maybe_unused]] std::unique_ptr<T1> a,
    [[maybe_unused]] std::unique_ptr<T2> b,
    [[maybe_unused]] std::unique_ptr<T3> с
) {
    std::cout << "foo\n";
}

int main() {
     try {
         // OK only at and after C++17:
         foo(
             std::unique_ptr<with_destructor<1>>(new with_destructor<1>(false)),
             std::unique_ptr<with_destructor<2>>(new with_destructor<2>(true)),
             std::unique_ptr<with_destructor<3>>(new with_destructor<3>(false))
         );
         /*
         // Before C++17:
         1. new with_destructor<3>(false)
         2. new with_destructor<2>(true)
         3. new with_destructor<1>(false)
         4. unique_ptr(....)
         5. unique_ptr(....)
         6. unique_ptr(....)
         // Before C++17:
         1. make_unique<with_destructor<3>>(false) - один вызов функции
         2. make_unique<with_destructor<2>>(true)
         3. make_unique<with_destructor<1>>(false)

         // https://herbsutter.com/gotw/_102/
         */
     } catch (...) {
         std::cout << "caught\n";
     }
}
```

# Лекция 5

продолжаем разговор про исключения

##### что происходит при исключении, возникающем при возврате значения из функции 


return может выкинуть исключение внутри функции, если ему не удалось скопировать значение туда, куда надо, и это исключение можно поймать прямо в функции.

то есть при возврате можно поймать исключение 

```c++
#include <iostream>

int remaining = 1;

struct Foo {
    Foo() {
        std::cout << "Foo() " << this << "\n";
    }
    Foo(const Foo &) {
        std::cout << "Foo(const Foo&) " << this << "\n";
        if (!--remaining) {
            throw 0;
        }
    }
    Foo &operator=(const Foo&) = delete;
    ~Foo() {
        std::cout << "~Foo " << this << "\n";
    }
};

Foo func(int x) {
    Foo a, b;
    // We need some convoluted code so the compiler cannot use RVO/NRVO (optimizations).
    if (x == 0) {
        try {
            return a;
        } catch (...) {
            std::cout << "exception in func\n";
            return Foo();
        }
    } else {
        return b;
    }
}

int main() {
    Foo f = func(0);
    std::cout << "&f=" << &f << "\n";
}
```

при присваивании возвращаемого значения может возникнуть исключение именно при копировании в переменную результата. И хотя кажется, что это ошибка при возврате функции, это не так, и ловится оно уже не в самой функции, а в месте, где возвращаемое значение копировалось повторно

```c++
#include <iostream>

int remaining = 2;

struct Foo {
    Foo() {
        std::cout << "Foo() " << this << "\n";
    }
    Foo(const Foo &) {
        std::cout << "Foo(const Foo&) " << this << "\n";
        if (!--remaining) {
            throw 0;
        }
    }
    Foo &operator=(const Foo &) {
        std::cout << "operator=(const Foo&) " << this << "\n";
        if (!--remaining) {
            throw 0;
        }
        return *this;
    }
    ~Foo() {
        std::cout << "~Foo " << this << "\n";
    }
};

Foo func([[maybe_unused]] int x) {
    Foo a, b;
    // We need some convoluted code so the compiler cannot use RVO/NRVO (optimizations).
    if (x == 0) {
        try {
            return a;
        } catch (...) {
            std::cout << "exception in func\n";
            return Foo();
        }
    } else {
        return b;
    }
}

int main() {
    Foo f;
    try {
        std::cout << "&f=" << &f << "\n";
        f = func(0);
        std::cout << "after f\n";
    } catch (...) {
        std::cout << "exception in main\n";
    }
}
```


как ловить исключения, которые вылетели из конструкторов полей или из конструкторов базового класса? Есть специальный синтаксис - function-try block

что можно сделать после того как поймали такое исключение? (гарантированно объект создать не получилось) - Нужно обязательно кинуть исключение дальше (если мы этого не сделаем, за нас это сделает компилятор). То есть в конструкторе мы можем поймать исключение и понять что мы его поймали, но ничего больше не можем.

```c++
struct Foo : Base {
    std::vector<int> a, b;

    Foo(const std::vector<int> &a_, const std::vector<int> &b_)
    try
        : a(a_)
        , b(b_)
    {
        std::cout << "constructor called\n";
    } catch (const std::bad_alloc &) {
        // Catching exceptions from: member initialization list (even omittied), constructor's body.
        std::cout << "Allocation failed\n";
        // All fields and bases are already destroyed. The object may be destroyed as well (e.g. delegating ctor).
        // We do not know what exactly has failed.
        // No way to resurrect the object. `throw;` is added implicitly.
    } catch (int x) {
        std::cout << "Oops\n";
        // We can change the exception, though.
        throw 10;
    }
};
```

Такую же штуку можно провернуть с деструктором(да, они с с++11 noexcept неявно, но можно это отменть явно дописав noexcept(false))

```c++
#include <iostream>

struct Base {
    ~Base() noexcept(false) {
        throw 0;
    }
};

struct Foo : Base {
    ~Foo() try {
        std::cout << "destructing Foo\n";
    } catch (...) {
        // Catching exceptions from: destructors of fields and bases.
        // All fields and bases are already destroyed.
        // We do not know what exactly has failed.

        std::cout << "destruction exception\n";
        // `throw;` is added implicitly, but if we `return`, the destructor is considered not throwing.
        return;
    }
};

int main() {
    try {
        Foo f;
    } catch (int x) {
        std::cout << "exception " << x << "\n";
    }
}
```

так же можно писать для просто функций 

```c++

Foo func(Foo a, Foo, Foo) try {
    std::cout << "func\n";
    return a;
} catch (...) {
    // Does not catch exceptions on argument creation.
    // The rest can be caught with the usual `try`-`catch`,
    // including `return`. So: useless.
    std::cout << "exception in func!\n";
    // No implicit `throw;`, we `return` normally by default.
    throw;
}
```

не нужно полагаться на что-то сгенерированное по умолчанию, они могут не предоставлять никаких гарантий исключений 

простой способ получить строгую гарантию исключений, если есть базовая - копируемся, делаем всё что надо с кпией, потом меняемся с ней местами


## Сети

TCP-IP

IP - кидается пакетом в одну сторону

TCP - соединение 
ip:port - socket

netcut(nc) - для всякого просмотра подключений или для самого подключения - говорим слушаем или подключаемся 

nc -l -p 10000 - прикидывается сервером 

telnet - аналог

напишем сервер через boost-asio 

io_context - инициализирует сам boost asio
tcp::acceptor - сам сервер - умеет принимать соединение - надо передать io_context и tcp::endpoint - какой ip и порт

accept - возвращает tcp::socket - работает с байтами

tcp::iostream - крутая штука - обертка над передающимися байтами

пишем сервер

```c++
#include <boost/asio.hpp>
#include <cassert>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <utility>

using boost::asio::ip::tcp;

int main(int argc, char *argv[]) {
    assert(argc == 2);

    boost::asio::io_context io_context;  // Thread-safe, ok to create once per app.
    tcp::acceptor acceptor(
        io_context, tcp::endpoint(tcp::v4(), std::atoi(argv[1]))
    );
    std::cout << "Listening at " << acceptor.local_endpoint() << "\n";

    tcp::iostream client([&]() {
        tcp::socket s = acceptor.accept();
        std::cout << "Connected " << s.remote_endpoint() << " --> "
                  << s.local_endpoint() << "\n";
        return s;
    }());

    while (client) {
        std::string s;
        client >> s;
        client << "string: " << s << "\n";
    }
    std::cout << "Completed\n";
}
```

пишем клиента

socket пытается подключаться к удаленному серверу

```c++
#include <boost/asio.hpp>
#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <utility>

using boost::asio::ip::tcp;

int main(int argc, char *argv[]) {
    assert(argc == 3);

    boost::asio::io_context io_context;

    auto create_connection = [&]() {
        tcp::socket s(io_context);
        boost::asio::connect(
            s, tcp::resolver(io_context).resolve(argv[1], argv[2])
        );
        return tcp::iostream(std::move(s));
    };
    tcp::iostream conn = create_connection();
    std::cout << "Connected " << conn.socket().local_endpoint() << " --> "
              << conn.socket().remote_endpoint() << "\n";

    conn << "hello world 123\n";
    conn.socket().shutdown(tcp::socket::shutdown_send);  // говорим что данные закончились
    std::cout << "Shut down\n";

    int c;
    while ((c = conn.get()) != std::char_traits<char>::eof()) {
        std::cout << static_cast<char>(c);
    }

    std::cout << "Completed\n";
    return 0;
}
```

побайтово - не надо!!!

через socket напрямую не нужно общаться

с исключениями в boost asio плохо 

NAT

# Лекция 6

### promise-future

promise - в неё можно когда нибудь что-то положить 
future - что-то, что может отдать когда то нам 

они потокобезопасные

get у future - блокирующий!!

если просто через optional делать то у нас нет get - не очень удобно, нужно постоянно проверять наличие и брать мьютексы

удобная потокобезопасная штука чтобы обменяться сообщениями между двумя потоками (чем то одним ровно)

```c++
#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>

int main() {
    std::promise<std::string> input_promise;
    std::future<std::string> input_future = input_promise.get_future();

    std::thread producer([&]() {
        std::string input;
        std::cin >> input;
        input_promise.set_value(std::move(input));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "producer after set_value-1\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::cout << "producer after set_value-2\n";
    });

    std::thread consumer([&]() {
        std::cout << "Consumer started\n";
        std::string input = input_future.get();
        std::cout << "Consumed!\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        std::cout << "Processed string: " << input << "\n";
    });

    consumer.join();
    producer.join();
}
```

### очереди 

spsc - single-producer-single-consumer
mpmc - multi-producer multi-consumer 

### реализация promise-future под капотом 

conditional_variable - умеет будить потоки
wait - ждем пока нас кто-то не разбудит - надо передавать лок!!!
как используется?

```c++
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

int main() {
    std::mutex m;
    std::string input;
    bool input_available = false;
    std::condition_variable cond;

    std::thread producer([&]() {
        while (true) {
            std::cin >> input; // формально - это UB
            std::unique_lock l(m);
            input_available = true;
            cond.notify_one();  // Разбудит один случайный поток. Важно, чтобы это было ему по делу.
            // cond.notify_all();  // Разбудит все потоки.
        }
    });

    std::thread consumer([&]() {
        while (true) {
            std::unique_lock l(m);
            while (!input_available) {  // while, не if! spurious wakeup
                cond.wait(l);
            }
            // Эквивалентная while конструкция, чтобы было сложнее набагать и были понятнее намерения.
            // cond.wait(l, []() { return input_available; });  // "ждём выполнения предиката".
            std::string input_snapshot = input;
            input_available = false;
            l.unlock();

            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            std::cout << "Got string: " << input_snapshot << "\n";
        }
    });

    consumer.join();
    producer.join();
}
```


Какая идея стоит за этим condvar? Это оптимизация для такого кода: есть условие, которого мы ждем. Вместо того чтобы в бесконечном цикле бегать  проверять выполнилось условие или нет, мы говорим потоку подождать (cond.wait(l)), где l - lock, его нужно передать чтобы отпустился мьютекс (чтобы вообще было возможно изменить условие).

spiriuis wakeup - поток может проснуться случайно, поэтому нужен while, а не if


проблема голодания: когда одному из потоков не достается мьютекса 

std::async - принимает лямбду, как то её запускает, возвращает то, что вернуля lambda. - не очень хорошо - две непредсказуемые ветки, программа не предсказуемая

conditional_variable всегда сочетается с каким-то условием и с каким-то мьютексом

где лучше вызывать notify? считается, что лучше вызывать под мьютексом, причин не помним((


### mutable 
lock/unlock - не const-qualified 
что если хотим взять под мьютек const метод или переменную?

mutable - разрешает менять даже в const-qualified методах

```c++
#include <mutex>

struct atomic_int {
    int get() const {
        std::unique_lock l(m);
        return value;
    }

    void set(int new_value) {
        std::unique_lock l(m);
        value = new_value;
    }

private:
    mutable std::mutex m;  // Can be changed even in const-qualified methods.
    int value;
};

int main() {
}
```

### взаимный deadlock 

возникает когда два мьтекса - один поток захватил один мьютекс, второй захвати другой и они теперь друг друга ждут и программа зависает навсегда. Достаточно устрить любой цикл потоков которые друг друга ждут 

```c++
#include <cassert>
#include <cstdlib>
#include <mutex>
#include <thread>

int main(int argc, char *argv[]) {
    assert(argc == 2);
    const int N = std::atoi(argv[1]);

    std::mutex m1, m2;
    std::thread t1([&]() {
        for (int i = 0; i < N; i++) {
            std::unique_lock a(m1);
            std::unique_lock b(m2);
        }
    });
    std::thread t2([&]() {
        for (int i = 0; i < N; i++) {
            std::unique_lock b(m2);
            std::unique_lock a(m1);
        }
    });
    // N=10'000: уже deadlock: пусть сначала каждый поток схватил себе m1/m2, а потом ждёт второй. А он занят.
    t2.join();
    t1.join();
}
```
решение: 
- scoped_lock (забавный)

```c++
  std::scoped_lock a(m1, m2);
            // Под капотом вызывает std::lock.
            // Он пытается избежать deadlock, например:
            // while (true) {
            //     m1.lock();
            //     if (m2.try_lock()) break;
            //     m1.unlock();
            // }
```

- сделать у мьютексов глобальный порядок (берем сначала один мьютекс, потом второй - всегда!!)



### design-api для многопоточных штук

ссылка на набор терминов 

https://bloomberg.github.io/bde-resources/doxygen/bde_api_prod/group__bsldoc__glossary.html#3.1.5

const-thread-safe - если вызываете на одном объекте два конст метода то это окей, если какой-то из них уже не конст, то не окей

странности из-за неатомарных операций (потоки разбиваются)

ниже один мьютекс не поможет (это будет deadlock), два мьютекса могут помочь, но не полностью

```c++
#include <iostream>
#include <mutex>
#include <thread>

struct Foo {
private:
    std::mutex m;

public:
    void foo(int x) {  // Атомарная операция, atomic.
        std::unique_lock l(m);
        std::cout << "foo(" << x << ")\n";
    }

    void double_foo(int x) {  // Неатомарная операция :(
        foo(x);
        foo(x + 1);
    }
};

int main() {
    const int N = 100'000;
    Foo f;
    std::thread t([&f]() {
        for (int i = 0; i < N; i += 10)
            f.double_foo(i);
    });
    for (int i = 0; i < N; i += 10)
        f.double_foo(N + i);
    t.join();
}
```


лучшее решение - сделать приватный метод:

```c++
#include <iostream>
#include <mutex>
#include <thread>

struct Foo {
private:
    std::mutex m;

    void foo_lock_held(int x) {
        std::cout << "foo(" << x << ")\n";
    }

public:
    // Публичный интерфейс из атомарных операций.
    // Над ним надо очень хорошо думать, потому что комбинация двух атомарных
    // операций неатомарна.
    void foo(int x) {
        std::unique_lock l(m);
        foo_lock_held(x);
    }

    void double_foo(int x) {
        std::unique_lock l(m);
        foo_lock_held(x);
        foo_lock_held(x + 1);
    }
};

int main() {
    const int N = 100'000;
    Foo f;
    std::thread t([&f]() {
        for (int i = 0; i < N; i += 10)
            f.double_foo(i);
    });
    for (int i = 0; i < N; i += 10)
        f.foo(N + i);
    t.join();
}
```

проблема с вектором: два метода - push_back и [], если два потока, то один может сделать push_back и вектор переедет, а второй попробует взять [], и у него окажется невалидная ссыль и всё плохо :(

```c++
#include <cstddef>
#include <mutex>
#include <utility>
#include <vector>

template <typename T>
struct atomic_vector {
private:
    mutable std::mutex m;
    std::vector<T> v;

public:
    void push_back(T x) {
        std::unique_lock l(m);
        v.push_back(std::move(x));
    }

    // UB is imminent!
    const T &operator[](std::size_t i) const {
        std::unique_lock l(m);
        return v[i];
    }
};

void print_head(const atomic_vector<int> &v) {
    std::cout << v[0] << "\n";
}

// How to improve: return by value!

// TODO: full example
```

### TOC-TOU

time to check time to use
между моментов проверки и между моментом действий прошло какое-то время

проблема с balance, как только она вернула какое-то значение, баланс уже мог поменяться, поэтому любые геттеры в многопоточной программе не имеют почти никакого смысла, чтобы никого не провоцировать, лучше вообще не писать геттеры в многопоточке 

```c++
#include <iostream>
#include <mutex>
#include <thread>

struct User {
private:
    mutable std::mutex m_;
    int balance_ = 1'000'000;

public:
    // TOC-TOU is imminent! Just by looking at the API.
    int balance() const {
        std::unique_lock l(m_);
        return balance_;
    }

    void decrease_balance(int decrease_by) {
        std::unique_lock l(m_);
        balance_ -= decrease_by;
    }
};

int main() {
    User u;
    std::thread t([&u]() {
        while (u.balance() >= 3) {  // time of check
            u.decrease_balance(3);  // time of use
        }
    });
    while (u.balance() >= 4) {  // time of check
        u.decrease_balance(4);  // time of use
    }
    t.join();
    std::cout << u.balance() << "\n";  // Should be >= 0
}
```


как лучше?? делать и проверку и измнениме ровно под одним мьютексом

```c++
bool decrease_balance(int decrease_by) {
        std::unique_lock l(m_);
        if (balance_ < decrease_by) {
            return false;
        }
        balance_ -= decrease_by;
        return true;
    }
```

### shared-ptr 
const-thread-safety добиться не тривиально 
у него несколько объектов
конструктор копирования - потокобезопасный, так как у него счетчик ссылок atomic 

```c++
#include <iostream>
#include <memory>
#include <thread>

int main() {
    std::shared_ptr<int> p = std::make_unique<int>(10);

    // Each shared_ptr<int> p is actually three objects with different semantics:
    // 1. `p` itself is not thread-safe, but you can a) read-only; b) use mutex; or c) use std::atomic_* functions.
    // 2. Reference counter inside `p` is thread-safe (probably via atomic)
    // 3. `*p` is not thread-safe, but you can use mutex

    std::thread t1([p]() {
        for (int i = 0; i < 100'000; i++) {
            // std::shared_ptr<int>(const std::shared_ptr<int>&)
            std::shared_ptr<int> p2 = p;  // Thread-safe, even though it increases reference counter.
            ++*p;  // Non thread-safe.
        }
    });

    std::thread t2([p]() {
        for (int i = 0; i < 100'000; i++) {
            auto p2 = p;
            ++*p;
        }
    });

    t1.join();
    t2.join();

    std::cout << *p << std::endl;  // race-y.
    std::cout << p.use_count() << std::endl;  // non-race-y, always 1 here.
    p = nullptr;  // No leaks.
}
```

### thread-local 

новый вид storage duration (+ automatic, static, dynamic)

thread-local - как глобальная, но создается отдельно для каждого потока

переменная создается в момент первого обращения в новом потоке (не гарантируется)

зачем надо? чтобы создавать что-то один раз на поток, например в тестировании(чтобы корректно работало тестирование в нескольких потоках)



```c++
#include <chrono>
#include <iostream>
#include <thread>

struct Foo {
    Foo(int id) {
        std::cout << "Foo(" << id << ")@" << std::this_thread::get_id() << "\n";
    }
    ~Foo() {
        std::cout << "~Foo()@" << std::this_thread::get_id() << "\n";
    }
};

thread_local Foo foo(10);  // new kind of storage duration.

int main() {
    std::cout << "T1 &foo=" << &foo << "\n";  // Компилятор может проинициализировать лениво, но может и в момент начала потока.
    std::thread t2([&]() {
        thread_local Foo bar(20);  // Инициализирует при проходе через эту строчку.
        std::cout << "Before wait\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        std::cout << "T2 &foo=" << &foo << "\n";  // Компилятор может проинициализировать лениво, но может и в момент начала потока.
        std::cout << "T2 &bar=" << &bar << "\n";  // Уже точно проинициализировано выше.
    });
    std::cout << "Waiting for it...\n";
    t2.join();
    std::cout << "Joined\n";
    return 0;
}

/*
Зачем можно использовать: отслеживать стэк вызовов в каждом потоке.

TEST_CASE() {  // Запомнили в thread-local переменную текущий test case
    SUBCASE() {  // Запомнили текущий SUBCASE
       CHECK(....)  // Можем выводить красивое сообщение
    }
}
*/
```

### templates 

если у компилятора определение функции в какой-то единице трансляции, то только в этой единице трансляции он это определение может раскрыть, подставить некоторое T и тд и тп.

Поэтому нужно определять шаблонные типы и функции в заголовках!! слово inline добавляется неявно!!!

### снова про многопоточность чуть-чуть

практические приколы, по сути разные проявления UB

поигрался оптимизатор: он знает что доступ к одной переменной из разных потоков без каких либо мьютексов это UB и оптимизирует код так, как хочет (убирает циклы, например)

```c++
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#pragma GCC optimize("-O2")

int main() {
    int data = 0;
    bool stop = false;

    std::thread t([&]() {
        while (!stop) {  // Hmm: stop is always the same => loop is either infinite or never starts.
            data++;
        }
    });

    while (data < 100) {}  // Hmm: data is always the same => loop is either infinite or never starts.
    assert(data >= 100);
    std::cout << "done " << data << "\n";
    stop = true;

    // Hmm: optimizations like this are important. The alternative is to always re-read from memory, memory is slow.

    t.join();
}
```

**reordering**

компилятор имеет право переставлять строчки местами, если ему так хочется, это может происходить даже на уровне процессора


volatile - отрубает оптимизации на уровне компилятора, причем творчески, на процессор никак не влияет

```c++
#include <chrono>
#include <iostream>
#include <thread>

#pragma GCC optimize("-O2")

int main() {
    volatile int data = 0;
    volatile bool finished = false;

    std::thread t([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // Hmm: compiler cannot reorder two writes below. But CPU still can!
        data = 123;
        finished = true;
    });

    while (!finished) {}
    // Hmm?
    std::cout << data << "\n";

    t.join();
}
```

# Лекция 7

### aligment 
- закрепляется к типу, число, степень двойки, минимум единица, любой адрес такого типа должен делиться на его aligment

**размер типа всегда делится на его выравнивание** - чтобы было просто делать сишные массивы(чтобы не добавлять зазоры или еще что-то придумывать)

что если памыть не выровнена?

на некоторых процессорах это просто работает дольше, на некоторых это в целом не работает

```c++
#include <cassert>
#include <iostream>

int main() {
    alignas(0x100) char data[sizeof(long long) + 1]{};
    long long *x = (long long*)(data + 1);
    std::cout << x << "\n";
    *x = 10;
    std::cout << *x << "\n";
}
```
### как это влияет на структуры?

aligment структур - наибольший из aligment-ов типов её полей. Если какой-то тип имеет меньший aligment, то к нему добавляется padding - какие-то пустые байты

```c++
#include <iostream>

struct JustThreeChars {
    char c1 = 0;
    char c2 = 0;
    char c3 = 0;
};

struct Foo {
    char c = 0;
    // padding
    int i = 0;
};

struct Bar {
    int i = 0;
    char c = 0;
    // padding
};

int main() {
    std::cout << sizeof(JustThreeChars) << " " << alignof(JustThreeChars)
              << "\n";

    Foo f;
    std::cout << sizeof(Foo) << " " << sizeof(f.c) << " " << sizeof(f.i)
              << "\n";
    std::cout << alignof(Foo) << " " << alignof(f.c) << " " << alignof(f.i)
              << "\n";
    std::cout << static_cast<void *>(&f.c) << "\n";
    // std::cout << &f.c << "\n";
    std::cout << &f.i << "\n";
}
```

порядок полей класса иногда влияет!!

```c++
#include <iostream>

struct Foo {
    char c = 0;
    // padding
    int i = 0;
    char c2 = 0;
    // padding
};

struct Bar {
    int i = 0;
    char c = 0;
    char c2 = 0;
    // padding
};
```

даже при наследовании оптимизирует!!!

```c++
#include <iostream>

struct Bar {
    int i = 0;
    char c = 0;
    char c2 = 0;
    // padding
};

struct BarDerived : Bar {
    char c3 = 0;  // can be put into padding in `Bar`
    int i2 = 0;
};

int main() {
    std::cout << sizeof(Bar) << "\n";
    std::cout << sizeof(BarDerived) << "\n";
}
```

### Бинарные форматы

как правило все языки программирования работают с байтами(октет).
байт - одно число (от 0 до 255, или от -128 до 127) на один конкретный байт смотрим только как на число, узнать его конкретный бит - не можем. Как байт представляется внутри машины- личное дело машины.

отрицательные числа - по модулю 256 

- -128 = 128 (mod 256)
- -1 = 255 (mod 256)

многобайтовые числа бьют на байты. Две конвенции их хранения - little endian, big endian

-  little endian (x86): a[0] + 256 * a[1] 0x1234 ~ 0x34 0x12
-  big endian ("network byte order"): a[0] * 256 + a[1] 0x1234 ~ 0x12 0x34


### строки
тут всё плохо из-за огромного зоопарка кодировок(латиница в этих кодировках на одних и тех же местах - ASCII)

UNICODE - занумеровали все символы встречающиеся на планете (codepoints на самом деле)

но там тоже куча проблем...

UTF-8
только латиница - в точности совпадает с ASCII

всегда нужно знать кодировку!!! поэтому во всех протоколах передачи данных указывается кодировка

### aliasing and reinterpret_cast

цель: записать структурку, как она лежит в памяти, в файл

reinterpret_cast - берет указатель на что угодно и преобразуется в указатель на что угодно (через void*).

```c++
#include <cstddef>
#include <iostream>

int main() {
    int x = 123456;
    char *xc = reinterpret_cast<char*>(&x);
    // Becomes the following:
    // char *xc = static_cast<char*>(static_cast<void*>(&x));
    for (std::size_t i = 0; i < sizeof(x); i++) {
        std::cout << static_cast</*unsigned*/ int>(xc[i]) << "\n";
    }
    // 64  -30  1  0
    // 64 226   1  0
    // 64 + 256*(226 + 256*(1 + 256*0))
}
```
легально ли это? не всегда
strict-aliasing rule
- Можно через указатель p типа T1 обращаться к объекту типа T2 только если:
- 1. T1 == T2 (но T1 может быть более const)
- 2. T1 --- базовый класс T2
- .....
- 10. T1 == char, unsigned char, std::byte


```c++
#include <iostream>

int main() {
    float f = 1.0;
    static_assert(sizeof(float) == sizeof(int));

    int *x = reinterpret_cast<int*>(&f);
    std::cout << std::hex << *x /* UB */ << "\n";

    // Выше нарушаем: T1 == int, T2 == float.

    // Тут тоже нарушение с точки зрения C++: https://en.wikipedia.org/wiki/Fast_inverse_square_root
    // Но, возможно, не с точки зрения C.
}
```

проблемы:

```c++
#include <iostream>

int func(int *a, float *b) {
   *a = 10;
   *b = 123.45;
   return *a;  // --> return 10;
}

int main() {
    {
        int a = 15;
        float b = 456.78;
        int res = func(&a, &b);
        std::cout << "res=" << res << "\n";
        std::cout << "a=" << a << "\n";
        std::cout << "b=" << b << "\n";
    }
    {
        int a = 15;
        int res = func(&a, reinterpret_cast<float*>(&a));
        std::cout << "res=" << res << "\n";
        std::cout << "a=" << a << "\n";
    }
}
```

Strict aliasing rule часто нарушают.
Много кода требует себе `-fno-strict-aliasing`.

---


Можно из одной переменной перекладывать байты в другую переменную (только по байтам!!)

```c++
#include <iostream>

int main() {
    float x = 1.0;
    int y;

    static_assert(sizeof(x) == sizeof(y));
    // Аналог std::memcpy. Не UB.
    // Начиная с C++20 есть bit_cast<>.
    for (int i = 0; i < 4; i++) {
        reinterpret_cast<char*>(&y)[i] = reinterpret_cast<char*>(&x)[i];
    }

    std::cout << std::hex << y << "\n";
}
```

### Trivially copible 

https://stackoverflow.com/questions/4178175/what-are-aggregates-and-pods-and-how-why-are-they-special (C++03 and C++11)
https://en.cppreference.com/w/cpp/named_req/TriviallyCopyable


Trivially copyable: scalar types or a class such that:
1. Destructor is trivial (for class, bases, members)
2. There is copy/move ctor/assignment operator
3. All copy/move are trivial (for class, bases, members)
4. No virtual functions, no virtual base classes.


пример: (write)

```c++
struct MyTriviallyCopyable {
    int x = 10;
    char y = 20;
    // Compiler may add padding: 3 bytes so 'z' is 4-bytes aligned.
    float z = 30;
};

static_assert(std::is_trivially_copyable_v<MyTriviallyCopyable>);

int main() {
    MyTriviallyCopyable p;
    std::ofstream f("01.bin", std::ios_base::out | std::ios_base::binary);
    // Not UB.
    f.write(reinterpret_cast<const char*>(&p), sizeof(p));
}
```
std::ios_base::binary - важно - чтобы 10 считалось не переводом строки, а просто байтом 10
read


```c++
#include <iostream>
#include <fstream>
#include <type_traits>

struct MyTriviallyCopyable {
    int x;
    char y;
    // Compiler may add padding: 3 bytes so 'z' is 4-bytes aligned.
    float z;
};

static_assert(std::is_trivially_copyable_v<MyTriviallyCopyable>);

int main() {
    MyTriviallyCopyable p;
    std::ifstream f("01.bin", std::ios_base::in | std::ios_base::binary);
    // Not UB.
    f.read(reinterpret_cast<char*>(&p), sizeof(p));
    std::cout << p.x << " " << static_cast<int>(p.y) << " " << p.z << "\n";
}
```

запомнить текущее требование на паддинг для структуры можно с помощью(cупер плотно упаковали)

    #pragma pack(push, 1)
    ...
    #pragma pack(pop)

если компилятор такое поддерживает - то как правило это не UB

UB возникает, когда берем ссылку на такой элемент невыровненный 
пример (компилятор обычно не реализует тип невыровненный тип ссылки, с указателями - сэйм)

```c++
#include <algorithm>
#include <iostream>
#include <cstddef>

#pragma pack(push, 1)
struct S {
    char c;
    // no padding [bytes], alignment [of fields] is invalid
    int a = 10, b = 20;
};
#pragma pack(pop)

int main() {
    std::cout << sizeof(int) << " " << alignof(int) << "\n";
    std::cout << sizeof(std::uint32_t) << " " << alignof(std::uint32_t) << "\n";
    std::cout << sizeof(int*) << " " << alignof(int*) << "\n";

    // https://stackoverflow.com/questions/8568432/is-gccs-attribute-packed-pragma-pack-unsafe
    // One should not create a reference/pointer to a wrongly aligned object.
    // May fail even on x86_64 because of optimizations: https://stackoverflow.com/a/46790815/767632

    S s;
    s.a = 30;  // No references, the compiler knows that `s.a` is unaligned.
    std::cout << s.a << " " << s.b << "\n";  // No references because operator<< takes by value.
    std::swap(s.a, s.b);  // Unaligned references to `int`: UB, undefined sanitizer is right.
    std::cout << s.a << " " << s.b << "\n";  // No references because operator<< takes by value.
    [[maybe_unused]] int *aptr = &s.a;  // Unaligned pointer to `int`: UB, no undefined sanitizer warning :(
    *aptr = 40;
    std::cout << s.a << " " << s.b << "\n";  // No references because operator<< takes by value.
}
```

вот так делать вообще не стоит!!! вектор скомпилирован с предположением что он нормально выровенен, но по факту нет, и может приввести к страшным траблам

```c++
#include <vector>
#include <iostream>

#pragma pack(push, 1)
struct Foo {
    char x;
    std::vector<int> v;
};
#pragma pack(pop)

int main() {
    Foo f;
    std::cout << alignof(f) << "\n";
    std::cout << alignof(std::vector<int>) << "\n";
    std::cout << static_cast<void*>(&f.v) << "\n";
}
```

StandartLayout - все поля имеют одинаковый тип доступа + все поля standart layout (имеют какое-то гарантированное расположение в памяти)


StandardLayout: https://en.cppreference.com/w/cpp/named_req/SndardLayoutType
https://docs.microsoft.com/en-us/cpp/cpp/tvial-standard-layout-and-pod-types?view=msvc-170

No virtual functions/bases, no base duplicates, some extra reqs for base csses vs first members.
All non-static members (fields):
1. Are in the same struct
2. Has the same access.
3. StandardLayout themselves or scalar types (no references).

No requirements about destructors/copy ctors...

StandardLayout means "interoperable with other languages such as C".
StandardLayout && TriviallyCopyable == POD (Plain Old Data).


pod - standart layout + trivially copible

```c++
#include <cstddef>
#include <iostream>
struct EmptyBase {
    void foo() {}
};

struct IsStandardLayout : EmptyBase {
protected:
    int x = 1;
    char y = 2;
    int z = 3;
};

struct AlsoStandardLayout : IsStandardLayout {
    void bar() {}
};

struct NotStandardLayout1 : AlsoStandardLayout {
    int foo = 4;
};

struct NotStandardLayout2 {
private:
    int x = 1;
public:
    int y = 2;
private:
    int z = 3;
};

void print_bytes(unsigned char *bytes, std::size_t length) {
    for (std::size_t i = 0; i < length; i++) {
        if (i > 0) {
            std::cout << ' ';
        }
        std::cout << static_cast<int>(bytes[i]);
    }
    std::cout << '\n';
}

int main() {
    EmptyBase e;
    IsStandardLayout isl;
    AlsoStandardLayout asl;
    NotStandardLayout1 nsl1;
    NotStandardLayout2 nsl2;
    print_bytes(reinterpret_cast<unsigned char*>(&e), sizeof e);
    print_bytes(reinterpret_cast<unsigned char*>(&isl), sizeof isl);
    print_bytes(reinterpret_cast<unsigned char*>(&asl), sizeof asl);
    print_bytes(reinterpret_cast<unsigned char*>(&nsl1), sizeof nsl1);
    print_bytes(reinterpret_cast<unsigned char*>(&nsl2), sizeof nsl2);
}
```


# Лекция 8

что происходит, когда структуру, содержащуюю строчки, записываем в файл?

данные могут остаться (лежит иван закрывающий константина), но при выводе полей всё равно всё будет хорошо.

```c++
#include <cstring>
#include <iostream>
#include <fstream>

struct Person {
    char first_name[31]{};
    char last_name[31]{};
};

int main() {
    Person p;
    std::strcpy(p.first_name, "Konstantin");
    std::strcpy(p.first_name, "Ivan");
    std::strcpy(p.last_name, "Ivanov");

    {
        std::ofstream f("01.bin", std::ios::binary);
        f.write(reinterpret_cast<const char*>(&p), sizeof p);
    }
}
```


```c++
#include <cstring>
#include <iostream>
#include <fstream>

struct Person {
    char first_name[31]{};
    char last_name[31]{};
};

int main() {
    Person p;
    {
        std::ifstream f("01.bin", std::ios::binary);
        f.read(reinterpret_cast<char*>(&p), sizeof p);
    }
    std::cout << p.first_name << "\n";
    std::cout << p.last_name << "\n";
}
```

если храним как char * - все плохо, смысла

std::string - тоже плохо, оно совмещает указатель(если длинные строчки), а если короткие, то нормально(вероятно, стринг вообще не тривиалли копибл)

### std::array

Плюсовые массивы, которые ведут себя как сишный массив (фикс размер массива)

## СИ

### указатели на указатели

- многомерные массивы

первый способ [][][] - элементы хранятся подряд в памяти

как такое передается в функции? в функцию всегда передается адресс нулевого элемента(но так как это многомерный массив, то есть передается по сути адрес нулевого среза многомерного массива)
Нужно, чтобы функция знала все имерение многомерного массива, кроме первого

второй способ - передавать по ссылкам(в с++)

```c++
#include <iostream>

using arr45 = int[4][5];

// All the same:
//void foo(int arr[3][4][5]) {
//void foo(int arr[][4][5]) {
//void foo(int (*arr)[4][5]) {  // What if: (*arr) --> *arr
void foo(arr45 *arr) {
    std::cout << arr[1][2][3] << "\n";
    // static_cast<int*>(arr) + 1 * (4 * 5) + 2 * 5 + 3
}

template<std::size_t N, std::size_t M, std::size_t K>
void foo2(int (&arr)[N][M][K]) {
    std::cout << arr[1][2][3] << "\n";
    std::cout << N << " " << M << " " << K << "\n";
}

int main() {
    int arr[3][4][5]{};
    std::cout << sizeof(arr) << "\n";
    // Consecutive elements.
    std::cout << &arr[0][0][0] << "\n";
    std::cout << &arr[0][0][1] << "\n";
    std::cout << &arr[0][0][2] << "\n";
    std::cout << &arr[0][0][3] << "\n";
    std::cout << &arr[0][0][4] << "\n";
    std::cout << &arr[0][1][0] << "\n";
    std::cout << &arr[0][1][1] << "\n";
    arr[1][2][3] = 123;  // 1 * 4 * 5 + 2 * 5 + 3
    // arr[1, 2, 3] = 123;  // Not like in Pascal, see operator,
    foo(arr);
    foo2(arr);
}
```


- второй способ - симулируем вектор векторов (через указатели всё)

int ***arr2 - указатель на массив из int\*\*

синтаксис в точности такой же как и раньше при обращении к элементу, но памяти занимает больше

но передавать в функции так проще, компилятору не нужно знать другие измерения
!!нужно delete если всё было через new!!

```c++
#include <iostream>

void bar(int ***arr2) {
    std::cout << arr2[1][2][3] << "\n";
}

int main() {
    int arr[3][4][5]{};
    arr[1][2][3] = 123;
    std::cout << sizeof(arr) << "\n";

    // int ***arr2 = arr;  // Not the same!
    int ***arr2 = new int**[3];  // Extra memory for pointers
    for (int i = 0; i < 3; i++) {
        arr2[i] = new int*[4];  // More extra memory for pointers
        arr2[i][0] = new int[5]{};
        arr2[i][1] = new int[5]{};
        arr2[i][2] = new int[5]{};
        arr2[i][3] = new int[5]{};
    }
    arr2[1][2][3] = 123;

    /*
     int***         int**
    +------+     +---------+---------+---------+
    | arr2 | --> | arr2[0] | arr2[1] | arr2[2] |
    +------+     +----|----+---------+---------+
             +--------+
             v               int*
    +------------+------------+------------+------------+
    | arr2[0][0] | arr2[0][1] | arr2[0][2] | arr2[0][3] | (x3)
    +------|-----+-------|----+------------+------------+
           |             +-------+
           v   int               v    int
        +---+---+---+---+---+  +---+---+---+---+---+
        |000|001|002|003|004|  |010|011|012|013|014|  (x3 x4)
        +---+---+---+---+---+  +---+---+---+---+---+
    */
    std::cout << "=====\n";
    // Elements in the same "line" are consecutive
    std::cout << &arr2[0][0][0] << "\n";
    std::cout << &arr2[0][0][1] << "\n";
    std::cout << &arr2[0][0][2] << "\n";
    std::cout << &arr2[0][0][3] << "\n";
    std::cout << &arr2[0][0][4] << "\n";
    // Lines are independent
    std::cout << &arr2[0][1][0] << "\n";
    std::cout << &arr2[0][1][1] << "\n";
    bar(arr2);

    for (int i = 0; i < 3; i++) {
        delete[] arr2[i][0];
        delete[] arr2[i][1];
        delete[] arr2[i][2];
        delete[] arr2[i][3];
        delete[] arr2[i];
    }
    delete[] arr2;
}
```

принципиально разные структуры данных, но с одинаковым названием и синтаксисом

### двойные звездочки

не только двумерный массив

указатель на указатель

const char ** - массив сишных строк

```c++
#include <iostream>

//void foo(const char *strs[]) {
void foo(const char **strs) {
    std::cout << strs[0] << " " << strs[1] << "\n";
}

int main(int argc, char *argv[]) {
    const char *strs[] = {
        "Hello from ",
        "World"
    };
    foo(strs);

    for (int i = 0; i < argc; i++) {
        std::cout << argv[i] << "\n";
    }
}
```


в си отсутствуют ссылки
передать указатель на char * - char **
указатель на строку которую нам надо поменять - поменять куда строчка указывает

```c++
#include <cassert>
#include <iostream>

bool read_line(char **s_out) {
    *s_out = new char[10];  // Assume noexcept.
    // *s = new char[10]{};
    std::cin.read(*s_out, 9);  // Does not add zero terminator
    (*s_out)[std::cin.gcount()] = 0;
    return static_cast<bool>(std::cin);
}

bool read_line_buffered(char **s) {
    static char buffer[10];
    *s = buffer;
    std::cin.read(*s, 9);
    (*s)[std::cin.gcount()] = 0;
    return static_cast<bool>(std::cin);
}

int main() {
    [[maybe_unused]] bool result;

    char *s = nullptr;
    result = read_line(&s);
    assert(result);
    std::cout << s << "\n";
    delete[] s;

    result = read_line_buffered(&s);
    assert(result);
    std::cout << s << "\n";
    // delete[] s;  // No need, it points to a buffer.

    // Moral: easy to confuse what to clean up and how. Used very rarely.
}
```

промежуточный способ понять - подставить int

```c++
#include <cassert>
#include <iostream>

bool read_int(int *i_out) {
    std::cin >> *i_out;
    return static_cast<bool>(std::cin);
}

int main() {
    [[maybe_unused]] bool result;

    int i = 0;
    result = read_int(&i);
    assert(result);
    std::cout << i << "\n";
}
```


Наследование с двойными звездочками

что когда двойные указатели?

нельзя базовый** преобразовывать к наследнику** неявно - ожидаемо

внезапно! наследник** к базовому** - тоже не окей!! - потому что такая операция разрешит тоже самое что pd = &d2

```c++
struct Base {};
struct Derived : Base {};
struct Derived2 : Base {};

int main() {
    Base b;
    Derived d;
    [[maybe_unused]] Derived2 d2;

    {
        [[maybe_unused]] Base *pb = &b;
        [[maybe_unused]] Derived *pd = &d;
        pb = pd;  // Derived* --> Base*: OK

        pb = &d2; // OK
        // pd = pb;  // Base* --> Derived*: error, otherwise pd points to Derived2.
    }
    {
        Base *pb = &b;
        [[maybe_unused]] Base **ppb = &pb;
        Derived *pd = &d;
        [[maybe_unused]] Derived **ppd = &pd;

        *ppb = &d;  // OK: Derived* -> Base*: pb = &d;

        // ppd = ppb;  // Base** --> Derived**: error, otherwise *ppd points to Derived2.

        // Surprise: https://isocpp.org/wiki/faq/proper-inheritance#derivedptrptr-to-baseptrptr
        // ppb = ppd;  // Derived** --> Base**: error, otherwise assertion fails:

        *ppb /* Base* */ = &d2;  // OK: pd = &d2 ???
        // assert(*ppd /* Derived* */ != &d2 /* Derived2* */);
    }
}
```


примерно такие же приколы с const 

может быть как константный объект(нельзя менять объект) так и константный указатель(нельзя менять указатель)

```c++
#include <iostream>
#include <type_traits>

int main() {
    int x[10]{};
    const int cx[10]{};

    static_assert(std::is_same_v<int *, decltype(&x[0])>);
    {
        int *px = &x[0];  // not &cx[0]
        px++;
        px--;
        std::cout << *px << "\n";
        *px = 10;
        px = nullptr;
    }
    {
        const int *px = &cx[0];  // or &x[0]
        px++;
        px--;
        std::cout << *px << "\n";
        // *px = 10;
        px = nullptr;
    }
    {
        int *const px = &x[0];  // not &cx[0]
        // px++;
        // px--;
        std::cout << *px << "\n";
        *px = 10;
        // px = nullptr;
    }
    {
        [[maybe_unused]] const int *const px = &cx[0];  // or &x[0]
        // px++;
        // px--;
        std::cout << *px << "\n";
        // *px = 10;
        // px = nullptr;
    }

    {
        [[maybe_unused]] int *px = &x[0];  // not &cx[0]
        [[maybe_unused]] int *const cpx = px;
        [[maybe_unused]] const int *pcx = px;
        [[maybe_unused]] const int *const cpcx = px;
    }
    {
        const int a = 10;
        [[maybe_unused]] int b = a;

        [[maybe_unused]] int *const cpx = &x[0];
        [[maybe_unused]] int *px = cpx;
        [[maybe_unused]] const int *pcx = cpx;
        [[maybe_unused]] const int *const cpcx = cpx;
    }
    {
        [[maybe_unused]] const int *pcx = &cx[0];
        // [[maybe_unused]] int *px = pcx;
        // [[maybe_unused]] int *const cpx = pcx;
        [[maybe_unused]] const int *const cpcx = pcx;
    }
    {
        [[maybe_unused]] const int *const cpcx = &cx[0];
        // [[maybe_unused]] int *px = cpcx;
        // [[maybe_unused]] int *const cpx = cpcx;
        [[maybe_unused]] const int *pcx = cpcx;
    }

    {
        int *px = &x[0];
        [[maybe_unused]] int *py = &x[1];
        // константность в двойных указателях
        [[maybe_unused]] const int *const *const cpcpcx = &px;
        cpcpcx = py;
        *cpcpcx = &x[2];  // px = &x[2]
        **cpcpcx = 10;
    }
}
```

когда можем отбрасывать const, когда речь идет про двойные указатели?


```c++
#include <cassert>

int main() {
    int x[10]{};
    const int cx[10]{};

    int *px = &x[0];  // not &cx[0]
    const int *pcx = &cx[0];  // or &x[0]

    // https://isocpp.org/wiki/faq/const-correctness#constptrptr-conversion

    [[maybe_unused]] int **ppx = &px;  // ok: int** --> int**
    // ppx = &pcx;  // const int** --> int**: error, otherwise `**ppx = 10;` modifies the const array

    const int **ppcx = &pcx;
    // ppcx = &px;  // int** --> const int**: error, otherwise:
    *ppcx = &cx[0];  // px = &cx[0];  // makes `int*` point to `const int*`.
    if (px == &cx[0]) {
        *px = 10;  // Oops, modified const array.
    }

    [[maybe_unused]] const int *const *pcpcx = &px;
    // int** --> const int * const *: ok
    // *pcpcx = &cx[0];
}
```

### printf-scanf

scanf - считавает что-то с входного потока

пробел - пропусти любое количество пробельных символов
%- сейчас что то считаем 
[^-] - считай строку до первого - 
один символ непробельный - считай ровно этот символ
%19s - произвольная строчка длины 19 без пробелов


%f - для float
%lf - указатель для double! важно, иначе лютое UB

в scanf передавать куда прочитанное записывать!- передавать через указатели.

scanf-у нужно говорить какого размера буфер, иначе UB(((

```c++
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    // https://en.cppreference.com/w/c/io/fscanf
    char buf1[10], buf2[20];
    int x;
    int *px = &x;
    int read1 = scanf(" %9[^-]-%19s%*d%d", buf1, buf2, px);
    printf("read1=%d, *px=%d\n", read1, *px);

    float a;
    double b;
    int read2 = scanf("%f%lf", &a, &b);  // types should match!
    printf("read2=%d, a=%f, b=%f\n", read2, a, b);

    // https://en.cppreference.com/w/c/io/fprintf
    printf("buf1=|%s|\nbuf2=|%s|\nx=%d\n", buf1, buf2, x);
    printf("     01234567890123456789\nbuf1=%9s\nbuf2=%19s\nx=%05d\n", buf1,
           buf2, x);
    printf("\n% d\n% d\n% d\n", 0, 5, -5);
    printf("\n%+d\n%+d\n%+d\n", 0, 5, -5);
    printf("%010.3f\n", a);
    printf("%010.3f\n", b);  // not %lf! ellipsis conversions.
    printf("100%% done!\n");
}
```

prinf - симметрично

snprintf (куда записывать, сколько еще свободно, что записывать)
возвращает количество символов сколько ей хотелось(могло не получиться) записать, надо аккуратно, чтобы не выйти за размер буфера

```c++
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char buf[10];
    int pos = 0, out;

    out = snprintf(buf, sizeof buf, "%d+", 10);
    printf("out=%d\n", out);
    assert(out >= 0);
    pos += out;

    out = snprintf(buf + pos, sizeof buf - pos, "%d", 12345);
    printf("out=%d\n", out);
    assert(out >= 0);
    pos += out;

    out = snprintf(buf + pos, sizeof buf - pos, "+%d", 426);
    printf("out=%d\n", out);
    assert(out >= 0);
    pos += out;

    // OOPS: UB is close by because of (buf + pos)

    printf("buf=|%s|, pos=%d\n", buf, pos);
}
```

симметричная конструкция для чтения

sscanf умеет считывать из строчки (%n - верни сколько символов прочитал!!)

```c++
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char buf[] = " 123+   45+8 79+4";
    int pos = 0;
    while (buf[pos]) {
        if (buf[pos] == '+') {
            pos++;
            continue;
        }
        int x, read;
        // May take linear time! Like in GTA: https://nee.lv/2021/02/28/How-I-cut-GTA-Online-loading-times-by-70/
        int res = sscanf(buf + pos, "%d%n", &x, &read);
        assert(res == 1);
        printf("pos=%d; read=%d; x=%d\n", pos, read, x);
        pos += read;
    }
}
```
что не работает? - цикл может быть квадратичным sscanf работает за линию (почти во всех реаизациях за линейное время от длины входа, а не количество прочитанных символов)

безопасный scanf - scanf_s - которому нужно передавать размер буфера типом rsize_t - реализовано только в вижаке!!!

# Лекция 9

### как в си устроены шаблоны функции(точнее, как сделать аналог)

в си нет шаблонов вообще никаких, как их заменить? - потребовать, чтобы нам на вход дали указатель на обычную шаблонную функцию (лямбду с состоянием не передать к сожалению)

```c++
#include <iostream>

template<typename F>
void for_each_cpp(int *begin, int *end, F f) {
    while (begin != end)
        f(*begin++);
}

void for_each_c(int *begin, int *end, void (*f)(int)) {
    while (begin != end)
        f(*begin++);
}

void print_int(int x) {
    std::cout << " " << x;
}

int main(void) {
    int arr[10]{};
    arr[3] = 100;
    arr[5] = 200;

    for_each_cpp(arr, arr + 10, print_int);
    std::cout << "\n";

    for_each_cpp(arr, arr + 10, [](int x) { std::cout << " " << x; });
    std::cout << "\n";

    for_each_cpp(arr, arr + 10, [first = true](int x) mutable {
        if (!first) std::cout << " ";
        std::cout << x;
        first = false;
    });
    std::cout << "\n";

    for_each_c(arr, arr + 10, print_int);
    std::cout << "\n";

    for_each_c(arr, arr + 10, [](int x) { std::cout << " " << x; });
    std::cout << "\n";

    /*for_each_c(arr, arr + 10, [first = true](int x) mutable {
        if (!first) std::cout << " ";
        std::cout << x;
        first = false;
    });
    std::cout << "\n";*/
}
```

то есть если нам в языке си нужно передать куда то функцию - делаем это через указатель на функцию 


расширим пример - хотим использовать произвольный тип аргумента - делаем через void *

+ нужно передавать размер элемента, чтобы корректно работало с передвижением указателя

```c++
#include <iostream>
#include <string>

template<typename T, typename F>
void for_each_cpp(T *begin, T *end, F f) {
    while (begin != end)
        f(begin++);
}

// 1. `template<typename T>` --> everything by pointer
// 2. `void*` instead of `T*`
// 3. Take `elem_size` together with `void*`
void for_each_c(void *begin, void *end, std::size_t elem_size, void (*f)(void*)) {
    while (begin != end) {
        f(begin);
        begin = static_cast<char*>(begin) + elem_size;
    }
}

void print_int(void *x_void) {
    int *x = static_cast<int*>(x_void);
    std::cout << " " << *x;
}

void print_string(void *x_void) {
    std::string *x = static_cast<std::string*>(x_void);
    std::cout << " " << *x;
}

int main(void) {
    int arr[10]{};
    arr[3] = 100;
    arr[5] = 200;

    for_each_cpp(arr, arr + 10, print_int);
    std::cout << "\n";

    for_each_c(arr, arr + 10, sizeof arr[0], print_int);
    std::cout << "\n";

    std::string arr2[]{"hello", "world"};

    for_each_cpp(arr2, arr2 + 2, print_string);
    std::cout << "\n";

    for_each_c(arr2, arr2 + 2, sizeof arr2[0], print_string);
    std::cout << "\n";
}
```

если пишем под gcc, то можно воспользоваться его расширениемя и скомпилировать с ключом -fpermissive, то можем вот так написать

```c++
#include <iostream>
#include <string>

void for_each_c(void *begin, void *end, std::size_t elem_size, void (*f)(void*)) {
    while (begin != end) {
        f(begin);
        begin += elem_size;  // GCC extension: arithmetics for `void*` is same as for `char*`
    }
}

void print_int(int *x) {  // GCC extension `-fpermissive`: function pointers are compatible. Sometimes.
    std::cout << " " << *x;
}

void print_string(std::string *x) {
    std::cout << " " << *x;
}

int main(void) {
    int arr[10]{};
    arr[3] = 100;
    arr[5] = 200;

    for_each_c(arr, arr + 10, sizeof arr[0], print_int);
    std::cout << "\n";

    std::string arr2[]{"hello", "world"};
    for_each_c(arr2, arr2 + 2, sizeof arr2[0], print_string);
    std::cout << "\n";
}
```


если хотим передать лямбду с состоянием? Нужно передавать еще одну void *, 

как это можно использовать? - передавать адрес переменной

то есть чтобы получить всё т, что мы получаем из коробки в плюсах, нам надо явно самим передавать всё состояние по указателю

```c++
#include <iostream>
#include <string>

void for_each_c(
        void *begin,
        void *end,
        std::size_t elem_size,
        void (*f)(void * /*farg*/, void* /*element*/),
        void *farg
) {
    while (begin != end) {
        f(farg, begin);
        begin = static_cast<char*>(begin) + elem_size;
    }
}

void print_int(void *farg, void *x_void) {
    bool *first = static_cast<bool*>(farg);
    int *x = static_cast<int*>(x_void);

    if (!*first) {
        std::cout << " ";
    }
    std::cout << *x;
    *first = false;
}

void print_string(void *farg, void *x_void) {
    bool *first = static_cast<bool*>(farg);
    std::string *x = static_cast<std::string*>(x_void);

    if (!*first) {
        std::cout << " ";
    }
    std::cout << *x;
    *first = false;
}

int main(void) {
    int arr[10]{};
    arr[3] = 100;
    arr[5] = 200;

    {
        bool first = true;
        for_each_c(arr, arr + 10, sizeof arr[0], print_int, &first);
        std::cout << "\n";
    }

    std::string arr2[]{"hello", "world"};

    {
        bool first = true;
        for_each_c(arr2, arr2 + 2, sizeof arr2[0], print_string, &first);
        std::cout << "\n";
    }
}
```

#### дебри 

разлиновывать void* нельзя.

каст из void* к несоответствующему типу в принципе окей до тех пор пока мы этот некорректный указатель не разлиновываем и не нарушаем strict aliasing rule

```c++
#include <iostream>
#include <vector>

int main() {
    int a = 10;
    char b = 'X';
    std::vector<int> v{1, 2, 3};

    [[maybe_unused]] void *p0 = nullptr;
    void *p1 = &a;  // implicit conversion: void* points to any object
    void *p2 = &b;
    void *p3 = &v;

    std::cout << p2 << "\n";
    // *p2;  // compilation error
    std::cout << *static_cast<int*>(p1) << "\n";
    std::cout << *static_cast<char*>(p2) << "\n";
    std::cout << static_cast<std::vector<int>*>(p3)->size() << "\n";

    // static_cast<short*>(p1);  // OK
    // *static_cast<short*>(p1);  // UB, strict aliasing violation, like reinterpret_cast<short*>(&a).

    // Impossible to know where does void* point.
}
```


void * возникает когда хотим вывести char*
char* - воспринимается как сишная строка, поэтому просто вывод обычный указатель на char - UB, выход за границы массива, помогает каст к void *

```c++
#include <iostream>

int main() {
    int i = 123;
    char a = 'X';

    std::cout << i << "\n";  // 123
    std::cout << &i << "\n";  // address of 'i'

    std::cout << a << "\n";  // X
    // std::cout << &a << "\n";  // UB, because `char*` is interpreted as ASCIIZ string
    std::cout << static_cast<void*>(&a) << "\n";  // ok, address of 'a'
}
```

указатели на функции 
взять указатель на шабонную функцию нельзя (они же по сути не скомпилены), если хочется, надо конкретный тип шаблона передавать

```c++
#include <iostream>

void apply(void (*operation)(int)) { // cdecl.org (C DECLaration)
    std::cout << "calling with 10\n";
    operation(10);
    (*operation)(10);  // the same.
}

using ApplyArgument = void(*)(int);
using ApplyArgumentAlsoWorks = void(*)(int argument);
void apply2(ApplyArgument operation) { // cdecl.org
    std::cout << "calling with 10\n";
    operation(10);
}

void print_twice(int x) {
    std::cout << x << ", " << x << "\n";
}

template<typename T>
void print(T x) {
    std::cout << x << "\n";
}

int main() {
   apply(&print_twice);
   apply(print_twice);  // implicit decay: function --> function pointer
   apply2(&print_twice);
   apply2(print_twice);

   // Doing the same:
   apply(&print<int>);
   apply(&print);  // Automatically: T=int.
   apply(print);

   apply([](int x) { std::cout << "lambda: " << x << "\n"; });  // lambdas with no captures can be converted
}
```

еще приколы где компилятор может вывести шаблонный тип параметра и не может вывести 

```c++
#include <iostream>

template<typename T>
void apply(void (*operation)(T), T data) { // cdecl.org
    operation(data);
}

template<typename T>
void apply10(void (*operation)(T)) { // cdecl.org
    operation(10);
}

void print_twice(int x) {
    std::cout << x << ", " << x << "\n";
}

template<typename T>
void print(T x) {
    std::cout << x << "\n";
}

template<typename T>
void (*print_ptr)(T) = print;

int main() {
   apply<int>(print_twice, 20);
   apply<int>(print, 10);

   apply(print_twice, 20);
   apply(print, 10);

   apply10(print_twice);
   // apply10(print);

   [[maybe_unused]] void (*ptr1)(int) = print;
   [[maybe_unused]] void (*ptr2)(double) = print;
   ptr1 = print_twice;

   [[maybe_unused]] auto ptr3 = print_twice;
   [[maybe_unused]] auto ptr4 = print<int>;
   // [[maybe_unused]] auto ptr5 = print;

   [[maybe_unused]] void (*ptr6)(int) = print_ptr<int>;
   [[maybe_unused]] auto ptr7 = print_ptr<int>;
   [[maybe_unused]] void (*ptr8)(int) = print_ptr;
}
```


что с перегрузками - в си их нет, но поговорим для с++

```c++
#include <iostream>

template<typename T>
void apply(void (*operation)(T), T data) { // cdecl.org
    operation(data);
}

template<typename T>
void apply10(void (*operation)(T)) { // cdecl.org
    operation(10);
}

void print(int x) {
    std::cout << x << "\n";
}

void print(double x) {
    std::cout << x << "\n";
}

int main() {
   apply<int>(print, 10);
   apply<int>(print, 20);

   apply(print, 10);

   apply10<int>(print);
   // apply10(print);

   [[maybe_unused]] void (*ptr1)(int) = print;
   [[maybe_unused]] void (*ptr2)(double) = print;

   // auto ptr4 = print;
   [[maybe_unused]] auto ptr5 = static_cast< void(*)(int) >(print);
}
```


несхожие типы указателей на функции

```c++
using FInt = void(*)(int);
using FShort = void(*)(short);
using FNone = void(*)();

void fooi(int) {}
void foos(short) {}
void bar() {}

int main() {
    [[maybe_unused]] FInt fi = fooi;
    [[maybe_unused]] FShort fs = foos;
    [[maybe_unused]] FNone b = bar;

    // Incompatible even though 'int' and 'short' are compatible.
    // fi = fs;
    // fs = fi;

    // Cannot "ignore" arguments:
    // b = fi;
    // fi = b;  // fi(123);

    // Technically incorrect: functions are not objects, but used in Linux a lot.
    // E.g. 'man dlsym' returns function pointer inside a shared library: https://man7.org/linux/man-pages/man3/dlopen.3.html

    // [[maybe_unused]] void *pfunc1 = static_cast<void*>(fi);
    [[maybe_unused]] void *pfunc2 = reinterpret_cast<void*>(fi);
}
```

указатели на функции вообще ничего не знают про аргументы по умолчанию!!
```c++
void foo(int = 10) {}

int main() {
    void (*f1)(int) = foo;
    // f1();

    // No support for default arguments at all:

    // void (*f2)() = foo;
    // void (*f3)(int = 10) = foo;
}
```


### конкретные отличия си от с++

- в си все заголовки заканчиваются на .h
#include <cstdio> vs #include <stdio.h>

In <cstdio> `std::printf` is guaranteed, `::printf` is possible.
In <stdio.h> `::printf` is guaranteed, `std::printf` is possible.
<cstdio> in C does not exist.

// - is a C++-style comment, available since C99 only.

Oldest standard: C89 (used in Postgres 11 from 2018 and earlier).
GCC's default: C99. There are C11 and C17 (defects only), no C14 or C20.
Next expected standard: C23.
Language is much smaller than C++, but is not a subset.

We look at C17, but mostly C99 stuff.
I'm no specialist by any means, refer to https://www.manning.com/books/modern-c

в с89 все переменные нужно объявлять вначале функции/блока
поэтому в с89 такие мы не можем писать стандартно изветсный нам for

в с89 в целом мы не можем объявить переменную в середине блока

```c
#include <stdio.h>

int main(void) {
    /*
    for (int i = 0; i < 10; i++) {  // Available since C99 only
    }
    */

    int i;
    for (i = 0; i < 10; i++) {
        printf("i=%d\n", i);  /* no `std::` */
    }

    // int j;  /* Error in C89, but not in modern GCC, -pedantic only yields warning */
    {
        int j = 50;  /* You can declare variables at the beginning of a block. */
        printf("j=%d\n", j);
    }
}
```


##### bar () - функция с неограниченным числом параметров - wtf
bar(void) - функция без параметров


```c
#include <stdio.h>

void foo(char* a) {
    printf("%s\n", a);
}

void bar() {   // Any arguments!
}

void baz(void) {   // No arguments
}

int main(void) {
    foo("hello");
    // foo("hello", "world");  // compilation error
    bar(1, 2, 3, "wow");
    baz();
    // baz(1, 2, 3);  // compilation error
}
```

структуры - нет слова class, есть только struct
struct - просто набор полей(нет конструкторов, деструкторов, методов, правила пяти тд и тп, все trivially copyble)

название - struct point1 - поэтому обычно через typedef (потому что using-a нет) можно псевдоним завести

```c++
// Everything here is valid C++ as well

struct point1 {  // no 'class'
    int x;  // = 0  // no member initializers
    int y;

    // No: constructors, destructors, rule of five (everything is trivially copyable/movable), methods, operator overloading
    // No: private/public, inheritance
};
// Type name is 'struct point1', not 'point1'.
struct point1 p1a;  // Valid C++ as well.

// Create an alias or multiple. There is no `using`.
typedef struct point1 point1, *ppoint1;  // cdecl.org
// Similar to `int a, *b;`
// using point1 = struct point1;
// using ppoint1 = struct point1*;

point1 p1b;
ppoint1 pp1b = &p1b;

// More common idiom
typedef struct point2 {
    int x;
    int y;
} point2, *ppoint2;
point2 p2;
ppoint2 pp2 = &p2;

// Instead of methods.
int point2_dist2(const struct point2 *p) {
    return p->x * p->x + p->y * p->y;
}

int main() {
    // point2_dist2(&p2);
}
```

#### malloc - free
в си нет new delete, вместо этого malloc free 
malloc - выделяет память под определенное количество элементов определенного типа, память не инициализирует!! (аналог - calloc - который инициализирует нолями)
malloc(0) - сам компилятор опред


```c
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *a = malloc(5 * sizeof(int));  // new int[5];
    // malloc returns `void*`, implicitly casted to `int*` in C (not in C++)
    assert(a);  // returns NULL/0 on failure, `nullptr` is missing
    printf("%d\n", a[0]);  // Uninitialized, UB. Typically zero on Linux.
    a[0] = 10;
    printf("%d\n", a[0]);
    free(a);  // delete[] a

    int *b = malloc(0);  // implementation-defined
    printf("%p\n", b);
    free(b);

    int *c = calloc(5, sizeof(int));  // zeroes memory, new int[5]{}
    printf("%d\n", c[0]);
    free(c);
}
```

чего еще нет в си)))

#### Alternatives
* `*_cast` --> C-style-cast `(float)1`.
* `int a{}` --> `int a = 0;`
* `nullptr` --> `NULL`/`0` from the standard library: https://en.cppreference.com/w/c/types/NULL (no troubles with overloads: no overloads)
* namespaces --> prepend library name to all functions
* references --> pointers
* `bool` --> `int` + `0`/`1` OR `<stdbool.h>`
* `using pint = int*` --> `typedef int *pint;`
* `new`/`delete` --> `malloc`/`free`
* move/copy constructor/assignment operator --> `memcpy` or custom functions
* templates --> function pointers, `void*`, complicated macros

#### Fully missing
* functions: overloading, default parameters
   * partially emulated: macro (`_Generic` and other magic)
* `std::vector`
* exceptions --> return code

прикольная штука - инициализация структуры designation initializer

##### realloc 
- перевыделяет память

если памяти не хватает и он переезжает, то данные переезжают вместе с ним

realloc может случайно не сработать и вернуть void, поэтому realloc нужно всегда делать в новый указатель (чтобы не было утечки памяти)

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char *s = malloc(5);
    strcpy(s, "xyz");
    printf("%p %s\n", s, s);

    s = realloc(s, 4);
    printf("%p %s\n", s, s);

    // FIXME: what is the problem?
    // https://pvs-studio.com/en/w/v701/
    // https://habr.com/ru/company/pvs-studio/blog/343508/
    s = realloc(s, 6);
    printf("%p %s\n", s, s);

    s = realloc(s, 100);
    printf("%p %s\n", s, s);

    s = realloc(s, 1000);
    printf("%p %s\n", s, s);

    s = realloc(s, 2);
    printf("%p %s\n", s, s);  // Technically UB.

    s = realloc(s, 1000000);
    printf("%p %s\n", s, s);

    free(s);
}
```

unnamed namespace - отсутствует, поэтому если мы хотим функцию с internal linkage - то нужно дописывать static

если restrict нарушен - UB, когда не UB - все кул

memcpy - с пометкой restrict
memmove - без restrict

```c
// Instead of unnamed namespace
static void foo(void) {  // internal linkage
}

void bar(void) {
}

static int x;  // internal linkage
int y;
```


#### restrict 
- можно повесить на указатель, говорит, что это указатель на память, которая не пересекается с другими переменными, видными компилятору

```c++
#include <stdio.h>
#include <stdlib.h>

void my_memcpy1(void *restrict dst, const void *restrict src, size_t n) {
    // src:  0 1 2 3 4 5
    // dst:      2 3 4 5 6 7

    // restrict: dst/src do not intersect and do not point to &n.
    while (n-- > 0)
        *(char *)dst++ = *(const char *)src++;
}

void my_memcpy2(void *restrict dst, const void *restrict src, size_t n) {
    for (size_t i = n; i > 0; i--) {
        ((char *)dst)[i - 1] = ((const char *)src)[i - 1];
    }
}

int main(void) {
    {
        int arr[] = {1, 2, 3, 4, 5};
        my_memcpy1(arr, arr + 2, sizeof(int) * 3);  // Bad call: restrict is violated
        for (int i = 0; i < 5; i++) {
            printf("%d%c", arr[i], "\n "[i + 1 < 5]);
        }
    }
    {
        int arr[] = {1, 2, 3, 4, 5};
        my_memcpy2(arr + 2, arr, sizeof(int) * 3);  // Bad call: restrict is violated
        for (int i = 0; i < 5; i++) {
            printf("%d%c", arr[i], "\n "[i + 1 < 5]);
        }
    }
    // memcpy: no intersections
    // memmove: allows intersections
}
```

#### всякая хрень которая осталась в си

- странное объявление функции

```c
#include <stdio.h>

/* K&R (old-style) function definitions are allowed (deleted in C23). Use never. */
void foo(a, b)
int a; char b;
{
    printf("a=%d, b=%c\n", a, b);
}

void bar(int a, char b) {
    printf("a=%d, b=%c\n", a, b);
}

int main() {
    foo(123, 'x');
    bar(456, 'y');
}
```

- если не указали тип функции или тип переменной - автоматически подставляется int

из-за этого можно случайно вызвать функцию без инклудов и это будет ошибка линковки а не компиляции

```c
// #include <stdio.h>

/*
f(1, 2, 3, 4, "foo") --> int f();
gcc thinks it's ok in C11 still
*/

int main(void) {
    printf("2 + 2 = %d\n", 4);  // Works: implicitly adds `int printf();`
    // botva(1, 2);  // Error in C99, link error in C89 (or GCC with C99+)
}
```


всё грустно с malloc потому что он возвращает void *(8 байт), а не int\*(4 байта). иначе там что-то всё обрезается и всё грустно



и еще два прикола

```c
#include <stdlib.h>
#include <stdio.h>

int main(void) {
    // 1.
    int *a = malloc(5 * sizeof(int));  // new int[5];
//  int *a = (int*)malloc(5 * sizeof(int)); // very bad style: TODO
    free(a);  // delete[] a

    // 2.
    // 'A' is int in C, char in C++.
    printf("%d %d %d\n", (int)sizeof('A'), (int)sizeof((char)'A'), (int)sizeof(char));
}
```

```c++
#include <stdlib.h>
#include <stdio.h>

int main(void) {
    // 1.
//  int *a = malloc(5 * sizeof(int));  // new int[5];
    int *a = static_cast<int*>(malloc(5 * sizeof(int)));  // new int[5];
    free(a);  // delete[] a

    // 2.
    // 'A' is int in C, char in C++.
    printf("%d %d\n", (int)sizeof('A'), (int)sizeof(char));
}
````

# Лекция 10

### goto 
- перебрыгивает в нужно место в программе (можно прыгать как назад, так и вперед по меткам)
метку межно писать в любом месте программы, где после будет идти команда (то, что заканчивается точкой-запятой)

оригинальное использование(когда не было циклов, сложных if-ов и тд и тп) - использовать так сейчас - очень плохо плохо плохо!!!

```c
#include <stdio.h>

// Go To Statement Considered Harmful:
// https://homepages.cwi.nl/~storm/teaching/reader/Dijkstra68.pdf

// GOTOphobia considered harmful in C:
// https://blog.joren.ga/gotophobia-harmful or https://web.archive.org/web/20230101000000*/https://blog.joren.ga/gotophobia-harmful
// https://news.ycombinator.com/item?id=34943952

// Even worse: https://archive.org/details/32_BASIC_Programs_for_the_PET_Computer_1979 (e.g. page 23 in PDF, page 7)

int main(void) {
    int n = 10;
    int i = 0;
label:
    printf("%d\n", i);
    i++;
    int x = 5;
    printf("  x=%d\n", x);
    x++;
    if (i < n)
        goto label;
    printf("end\n");

    {
    foo:;
    }
    {
    bar:
        int x;
    }
}
```

```
если при прыжке впере было определение переменной - то в си это ок, но переменная непроинициилизиирована. В с++ так уже нельзя, компилятор не разрешит.


хорошее использование goto даже в нынешних реалиях.

- несколько вложенные циклов - хотим выйти сразу из всех

```c
#include <cstdio>
#include <utility>

int main() {
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++) {
            std::string temporary(10000, 'x');
            if (i * j == 24) {
                std::printf("%d %d\n", i, j);
                goto after_loop;  // No leaks!
            }
        }
    after_loop:;

    auto [a, b] = []() {
        for (int i = 0; i < 10; i++)
            for (int j = 0; j < 10; j++) {
                if (i * j == 24) {
                    return std::make_pair(i, j);
                }
            }
    }();
    std::printf("%d %d\n", a, b);
}

/* Java:
outer_loop:
for (int i = 0; i < 10; i++)
    for (int j = 0; j < 10; j++) {
        if (...) {
            break outer_loop;
        }
    }
*/
```
- корректная очистка ресурсов (некоторая эсуляция деструкторов).

```c++
#include <stdio.h>
#include <stdlib.h>

// TODO: showcase Linux kernel

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    int ret = 1;

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        printf("Unable to open file\n");
        goto f_closed;
    }

    int n;
    if (fscanf(f, "%d", &n) != 1) {
        printf("Unable to read n\n");
        goto f_opened;
    }
    int *arr = malloc(n * sizeof(int));
    if (!arr) {
        printf("Unable to allocate array for %d ints\n", n);
        goto buf_freed;
    }

    for (int i = 0; i < n; i++) {
        if (fscanf(f, "%d", &arr[i]) != 1) {
            printf("Unable to read element %d\n", i + 1);
            goto err;
        }
    }

    // WONTFIX: check printf result. No assert() because it can be removed.
    printf("Result: ");
    for (int i = n - 1; i >= 0; i--) {
        printf("%d", arr[i]);
        if (i > 0)
            printf(" ");
    }
    printf("\n");
    ret = 0;

err:
    free(arr);
buf_freed:
f_opened:
    fclose(f);
f_closed:
    return ret;
}
```

## Union 
- либо одно, либо другое, занимает столько байт, сколько занимает самое большое событие 

все поля начинаются ровно в одном и том же месте и мы обязаны использовать ровно одно из них в определенный момент времени

главное не записать сначала одно событие - а потом читать другое событие (в с++ - UB, в C - implementation defined)

некоторая возможность экономии памяти

```c
#include <stdio.h>

enum EventType { MOUSE, KEYBOARD };

struct MouseEvent {  // ~12 bytes
    int x;
    int y;
    int button;
};

struct KeyboardEvent {  // ~8 bytes
    int key;
    int is_down;
};

union AnyEvent {  // ~12 bytes, ensures proper alignment for all.
    struct MouseEvent mouse;
    struct KeyboardEvent keyboard;
};

// tagged union: std::variant<MouseEvent, KeyboardEvent>
// Haskell: tagged union is called "type sum", but syntax is better and safer.
// Rust: tagged union is called "enum".
struct Event {  // ~16
    enum EventType type;
    union AnyEvent event;
};

int main(void) {
    printf("%d\n", sizeof(struct Event));
    struct Event ev;
    ev.type = MOUSE;
    ev.event.mouse.x = 11;
    ev.event.mouse.y = 22;
    ev.event.mouse.button = 1;
    // https://stackoverflow.com/a/25672839/767632
    // Implementation-defined in C (object representation), no strict aliasing violation for unions specifically.
    // UB in C++ (accessing non-active union member + strict aliasing).
    // OK in GCC in C++:
    // https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html#Type-punning
    printf("%d\n", ev.event.keyboard.key);  // UB in C++, 11 in C.
}
```


union прямо в структуре: 

```c
struct Event {  // ~16
    EventType type;
    union {  // ~12 bytes, ensures proper alignment for all.
        struct MouseEvent mouse;
        struct KeyboardEvent keyboard;
    } event;
};
```

anonymous / unnamed union

```c
#include <stdio.h>

enum EventType { MOUSE, KEYBOARD };

// tagged union: std::variant<MouseEvent, KeyboardEvent>
struct Event {  // 16
    EventType type;
    union {
        struct {
            int x;
            int y;
            int button;
        } mouse;
        struct {
            int key;
            int is_down;
        } keyboard;
    } /*event*/;  // Anonymous/unnamed union member, C11 and C++98
    // Do not confuse with `union { ... } event;`
};

int main(void) {
    printf("%d\n", sizeof(struct Event));
    struct Event ev;
    ev.type = MOUSE;
    ev /*.event*/.mouse.x = 11;
    ev /*.event*/.mouse.y = 22;
    ev /*.event*/.mouse.button = 1;
}
```

имя еще можно пропускать и у структур

```c
#include <boost/core/demangle.hpp>
#include <iostream>

union Foo {
    struct X {  // OK, a type: `Foo::X`
        int x1, x2;
    };

    struct Y {  // OK, a type and a field
        int y1, y2;
    } y;

    struct {  // OK, a field
        int z1, z2;
    } z;

    struct {
        int wtf1, wtf2;
    };
    // Anonymous/unnamed struct members: C11, not in C++
    // Supported in C++ by GCC, Clang, VS.
    // Do not confuse with `struct { ... } x`.
};

int main() {
    [[maybe_unused]] Foo f;
    [[maybe_unused]] Foo::X fx;
    [[maybe_unused]] Foo::Y fy;

    // f.x1 = 10;

    f.y.y1 = 10;
    f.y.y2 = 20;

    f.z.z1 = 10;
    f.z.z2 = 20;
    std::cout << typeid(f.z).name() << "\n";
    std::cout << boost::core::demangle(typeid(f.z).name()) << "\n";

    // Non-standard C++, standard C11
    f.wtf1 = 10;
    f.wtf2 = 20;
}
```

## смешиваем программы на си и на с++

инструкция!

extern "C" - говорит что функция живет по правилам си и нам надо искать её там
(на си украшений к именам нет!! - поэтому нужно линковщику понять что линковать надо по правилам си)

надо разными компиляторами компились - gcc не добавит базовую плюсовую библу, g++ посчитает что ему только с++ и передали и не догадается что там что-то на чистом си

правильно - компилировать отдельно си, отдельно с++, после линковать при помощи g++, либо при помощи gcc и сказать чтобы прилинковало стандартную библу с++

при правильной компиляции конструкторы и деструкторы работают нормально, даже глобальные, вызываются до main в си


a.h

```c
#include <stdio.h>

int my_main(void);

int foo(void) {
    int arr[] = { [3] = 123 };  // Use C-specific syntax to ensure we're writing C.
    return arr[3];
}

int main(void) {
    printf("hello\n");
    return my_main();
}
```

b.cpp

```c++
#include <cstdio>
#include <iostream>
#include <vector>

// Question: why we needed it for DLL?
extern "C"  // Disable name mangling, and link like C.
int foo(/* no args: we are in C++ */);

int foo(int x) {  // OK
    return x + 1;
}

namespace a {
#if 1
// Warning: same as foo(), but different set of arguments. It's ok in C, though
// (e.g. printf).
extern "C" int foo(int);
#endif
}  // namespace a

struct Foo {
    Foo() {
        std::printf("Foo\n");
    }
    ~Foo() {
        std::printf("~Foo\n");
    }
} f;

extern "C"
int my_main() {
    std::vector<int> v;
    std::cout << foo() << "\n";
    std::cout << foo(100) << "\n";
    return 0;
}
```


что с headers? 

заголовки на .h


самый простой заголовок и под си и под с++

```c
#ifndef A_H_
#define A_H_

#ifdef __cplusplus
extern "C" {
#endif

int foo(void); // void очень очень важный 

#ifdef __cplusplus
}
#endif

#endif  // A_H_
```

еще несовместимости которые надо учитывать в заголовках

- константы (только  через макросы, так как с точки си const int N = 100 - переменная которую нельзя менять -> с ней нельзя создать сишный массив, требующий констант)

- если написали struct и не написали typedef то всегда надо писать struct + что-то
- нет аргументов по умолчанию

```c
#ifndef A_H_
#define A_H_

#ifdef __cplusplus
extern "C" {
#endif

const int N_cpp = 100;  // C++ only
#define N_c 100  // C++ and C

struct Point {
   int x, y;
};
struct TwoPoints {
   Point a1, b1;  // C++ only
   struct Point a2, b2;  // C++ and C
};

int foo(int x = 100);  // C++ only
int foo(int x);  // C++ and C, no default arguments. At all.

#ifdef __cplusplus
}
#endif

#endif  // A_H_
```

## конвенции языка C 

 - может потребоваться проинициализировать библиотеку которую используем (WSAStartup, WSACleanup - сокеты виндовые)
 - инициализация: функция, спец значение, либо занулить структуру
 - удаление - либо спец функция, либо ничего 
 - копирование - либо никогда, либо отдельная функция 
 - мувов не выделяют, можно скопировать побайтов через memcpy (редко)
 - память выделяется либо пользователем, либо библиотекой

### примеры 

- работа с файлами

```c
#include <stdlib.h>
#include <stdio.h>

// struct FILE;  // No definition necessary.

int main() {
    FILE *f = fopen("02-opaque-pointers.c", "r");  // "constructor", (1) resources are allocated by the library.

    // Never try to access `FILE`'s fields directly.
    char buf[20];  // (2) memory is provided by the caller.
    fscanf(f, "%20s", buf);  // "method". FIXME: why UB here?
    printf("buf=|%s|\n", buf);

    fclose(f);  // "destructor"
}
```

примеры с json 

```c
#include <assert.h>
#include <stdio.h>
#include <json-c/json.h>

int main() {
/*
{
  "a": [1,2,3],
  "b": "hello"
  "c": ["a",[1,2,3]],
}
*/
    const char *s = "{\"field\":\"value\",\"arr\":[1,2,3.5]}";

    struct json_object *obj = json_tokener_parse(s);
    // ~ shared_ptr<json_object>
    assert(obj);

    // for (auto [k, v] : obj)
    json_object_object_foreach(obj, k, v) {
        printf("%s --> %s\n", k, json_object_to_json_string_ext(v, JSON_C_TO_STRING_PRETTY));
    }
    printf("k=%s\n", k);

    json_object_put(obj);  // ~ destructor
}
```

UB 

```c
#include <assert.h>
#include <stdio.h>
#include <json-c/json.h>

int main() {
    const char *s1 = "{\"field\":\"value\",\"arr\":[1,2,3.5]}";
    const char *s2 = "[1,2,3.5]";
    printf("s1=%s, s2=%s\n", s1, s2);

    struct json_object *obj1 = json_tokener_parse(s1);
    struct json_object *obj2 = json_tokener_parse(s2);
    assert(obj1);
    assert(obj2);

    const char *str1 = json_object_to_json_string_ext(obj1, JSON_C_TO_STRING_PRETTY);  // char* is owned by `obj1`
    const char *str2 = json_object_to_json_string_ext(obj2, JSON_C_TO_STRING_PRETTY);  // char* is owned by `obj2`

    json_object_put(obj2);  // destructor
    json_object_put(obj1);  // destructor

    printf("obj1=%s\n", str1);  // UB
    printf("obj2=%s\n", str2);  // UB
}
```

пример с curl 

```c
#include <stdio.h>
#include <curl/curl.h>

// https://curl.se/libcurl/c/simple.html
 
int main(void)
{
  CURL *curl;
  CURLcode res;
 
  // FIXME: no global init for some reason?

  curl = curl_easy_init(); // инициализация библиотеки
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com"); // настроили
    /* example.com is redirected, so we tell libcurl to follow redirection */
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
 
    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl); // cделай запрос, возвращает код ошибки
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res)); 
 
    /* always cleanup */
    curl_easy_cleanup(curl);
  }
  return 0;
}
```

СИ закончилось УРА УРА УРА 
______

# Лекция 11

### ref-qualifier

хотим эффект того, что если у нас обычный объект, то какая-то функция возращает lvalue ссылку, а на временном объекте - возвращает rvalue ссылку

более мощная версия для перегрузок - ref-qualifier - обозначаем ссылочность this и получаем возможность различать временные и не временные объекты.

можно еще миксовать с const и переписать все варианты: &, &&, const &, const &&(странное)

```c++
#include <iostream>
#include <memory>
#include <optional>
#include <utility>

std::optional<std::unique_ptr<int>> create() {
    return std::make_unique<int>(10);
}

void just_print(const std::unique_ptr<int> &a) {
    if (a) {
        std::cout << "printed " << *a << std::endl;
    } else {
        std::cout << "printed nullptr" << std::endl;
    }
}

void consume_print(std::unique_ptr<int> a) {
    just_print(a);
}

struct MyOptionalUniquePtrInt {
    std::unique_ptr<int> p = std::make_unique<int>(30);

    // ref-qualifier: https://habr.com/ru/post/216783/
    std::unique_ptr<int> &value() & {
        return p;
    }

    const std::unique_ptr<int> &value() const & {
        return p;
    }

    std::unique_ptr<int> &&value() && {  // Return reference to avoid copy.
        return std::move(p);
    }
    
    // Not covered: const&&
};

MyOptionalUniquePtrInt my_create() {
    return {};
}

int main() {
    MyOptionalUniquePtrInt a;

     // a.value() is lvalue: method returns reference.
     just_print(a.value());
     // consume_print(a.value());  // CE
     consume_print(std::move(a.value()));

     // <rvalue-optional>.value() is rvalue: https://en.cppreference.com/w/cpp/utility/optional/value
     consume_print(create().value());
     consume_print(my_create().value());
     consume_print(std::move(a).value());
}
```

можно выражать одну перегрузку через другую (лучше выразить не const версию через const)(если обратно, то могут быть проблему с const_cast (можно делать только на неконстантных константных объектах))

хороший вариант 

```c++
#include <iostream>
#include <memory>
#include <optional>
#include <utility>

std::optional<std::unique_ptr<int>> create() {
    return std::make_unique<int>(10);
}

void just_print(const std::unique_ptr<int> &a) {
    if (a) {
        std::cout << "printed " << *a << std::endl;
    } else {
        std::cout << "printed nullptr" << std::endl;
    }
}

void consume_print(std::unique_ptr<int> a) {
    just_print(a);
}

struct MyOptionalUniquePtrInt {
    std::unique_ptr<int> p = std::make_unique<int>(30);

    // ref-qualifier: https://habr.com/ru/post/216783/
    std::unique_ptr<int> &value() & {
        // Here `const_cast` is always safe because we know what `value()` does.
        // https://stackoverflow.com/a/123995/767632
        // C++23: deducing this: https://devblogs.microsoft.com/cppblog/cpp23-deducing-this/
        return const_cast<std::unique_ptr<int>&>(std::as_const(*this).value());
    }

    const std::unique_ptr<int> &value() const & {
        return p;
    }

    std::unique_ptr<int> &&value() && {  // Return reference to avoid copy.
        return std::move(this->value());
    }
    
    // Not covered: const&&
};

MyOptionalUniquePtrInt my_create() {
    return {};
}

int main() {
     MyOptionalUniquePtrInt a;

     // a.value() is lvalue: method returns reference.
     just_print(a.value());
     // consume_print(a.value());  // CE
     consume_print(std::move(a.value()));

     // <rvalue-optional>.value() is rvalue: https://en.cppreference.com/w/cpp/utility/optional/value
     consume_print(create().value());
     consume_print(my_create().value());
     consume_print(std::move(a).value());
}
```

## Placement new

отделяем выделение памяти и создание объекта

только создает объект

Foo *f = new (data) Foo(); - возвращает указатель на свежесозданный объект

нужно удалить объекты!! - через деструктор!! девыделение памяти не происходит, а массив памяти data всё еще корректен

```c++
#include <iostream>
#include <memory>
#include <vector>

struct Foo {
    std::vector<int> v = std::vector{1, 2, 3};

    Foo() {
        std::cout << "Constructed at " << this << std::endl;
    }

    ~Foo() {
        std::cout << "Destructed at " << this << std::endl;
    }
};

int main() {
    alignas(alignof(Foo)) unsigned char data[sizeof(Foo)];
    std::cout << "Got memory, size=" << sizeof(data) << ", location=" << &data
              << std::endl;

    // We separate "memory allocation" from "object creation".
    // Previously `new T` did both. Now we only need it to call constructor.

    // #include <memory> for "placement new":
    Foo *f = new (data) Foo();  // Best practice.

    std::cout << f->v.size() << std::endl;
    std::cout << "bytes:";
    for (unsigned char c : data) {
        std::cout << ' ' << static_cast<int>(c);
    }
    std::cout << std::endl;

    f->~Foo();  // Explicit destructor call/pseudodestructor. No memory
                // deallocation, so it can be any memory.
    // delete f;  // Destructor + memory deallocation.

    // f->v;  // UB
    (void)data[123];  // OK
    data[123] = 50;  // OK
}
```

еще синтаксис но на самом деле - UB..., но работает

```c++
new (data) Foo();
Foo *f = reinterpret_cast<Foo *>(data);
```

### оператор присваивания на примере строчки

оператор присваивания со строгой гарантией исключений (на самом деле надо прям подумать, чтобы не было трабл, поэтому его не любят)

```c++
 str &operator=(const str &other) {
        if (this == &other) {
            return *this;
        }
        // Do all throwing operations first.
        char *new_data = other.capacity ? new char[other.capacity] : 0;

        // Do non-throwing next.
        delete[] data;
        capacity = other.capacity;
        data = new_data;
        std::strcpy(data, other.data);
        return *this;
    }
```

поэтому есть copy-swap idiom - сначала копируемся, потом меняемся или муваемся 

```c++
 str &operator=(const str &other) {
        if (this != &other) {
            str cpy(other);
            std::swap(*this, cpy);
            // *this = std::move(cpy);  // Alternative
        }
        return *this;
    }
```

правило четырех - оператор присваивания всегда копирует аргумент - но у этого есть проблемы производительности

```c++
str &operator=(str other) noexcept {
        // Cannot call `operator=`, `std::swap(*this)`.
        std::swap(capacity, other.capacity);
        std::swap(data, other.data);
        // Hence: either 1 copy ctor + swap, or 1 move ctor + swap.
        // Pros: short to implement, provides strong exception safety.
        // Cons: suboptimal performance.  What if we don't need to reallocate?
        // Self-assignment?
        return *this;
    }
```

## как функции принимают значения??

```c++
#include <string>
#include <utility>

template<typename T>
struct not_optional {
    alignas(alignof(T)) unsigned char data[sizeof(T)];

    void insert(const T &value) {
        new (&data) T(value);
    }

    void reset() {
        reinterpret_cast<T&>(data).~T();
    }
};

int main() {
    not_optional<std::string> v;
    std::string foo = "hello";

    v.insert(foo);  // 1 copy ctor (best!)
    v.reset();

    v.insert(std::string(10'000, 'x'));  // 1 ctor, 1 copy, 1 dtor
    v.reset();
}
```
можно написать только один insert по значению - в одном случае стало чуть лучше, в другом чуть лучше


третий стиль - два разных insert по const lvalue и rvalue - две перегрузки. В случае с невременными объектами мы вернулись в лучшую версию, в случае с временными - сделали получше!!

итого: как писать лучше всегда зависит от ситуации, это просто три разных стиля

четвертый стиль - спец метод emplace 

```c++
#include <string>
#include <utility>

template<typename T>
struct not_optional {
    alignas(alignof(T)) unsigned char data[sizeof(T)];

    void insert(const T &value) {
        new (&data) T(value);
    }

    void insert(T &&value) {
        new (&data) T(std::move(value));
    }

    template<typename Arg1, typename Arg2>
    void emplace(Arg1 arg1, Arg2 arg2) {
        // FIXME: use perfect forwarding + variadic templates (later)
        new (&data) T(arg1, arg2);
    }

    void reset() {
        reinterpret_cast<T&>(data).~T();
    }
};

int main() {
    not_optional<std::string> v;
    std::string foo = "hello";

    v.insert(foo);  // 1 copy ctor (best!)
    v.reset();

    v.emplace(10'000, 'x');  // 1 ctor (best!)
    v.reset();
}
```


обычно не хотим форсировать реаллокацию 

## шаблонные друзья

любой bar с любым параментром - теперь наш друг 

```c++
template<typename T>
struct Foo {
private:
    int x = 0;

    // All Bar<> are friends of us.
    template<typename/* U*/>
    friend struct Bar;
};
```


бар с конкретным набором параметров - наш друг 

```c++
#include <vector>
#include <utility>

template<typename T>
struct Bar;

template<typename T>
struct Foo {
private:
    int x = 0;

    // One specific Bar<> is a friend. No way to partially specialize.
    friend struct Bar<std::vector<T>>;
};

template<typename U /* VecT may be a better name to avoid confusion */>
struct Bar {
    void bar() {
        // Foo<int>: friend struct Bar<vector<int>>.

        Foo<int> f;  // friend struct Bar<std::vector<int>>
        // Foo<typename U::value_type> f;
        f.x = 10;

        // U is never vector<void>
        [[maybe_unused]] Foo<void> g;  // friend struct Bar<std::vector<void>>
        // g.x = 10;

        // U is never vector<U>
        [[maybe_unused]] Foo<U> h;  // friend struct Bar<std::vector<U>>
        // h.x = 10;
    }
};

int main() {
    Bar</*U=*/std::vector<int>>().bar();
    Bar</*U=*/std::vector<char>>().bar();
    Bar</*U=*/int>().bar();
}
```

### теперь разбираемся с функциями 

#### нешаблонная функция шаблонной структуры

- для каждого типа шаблонной структуры надо свою реализацию метода - wtf 

```c++
template<typename T>
struct MyTemplate {
private:
    int x = 0;

    // (1) Non-template declaration
    friend void simple_func();

    // (2) For all T: generate an independent non-template
    // non_simple_func(MyTemplate<T>&) declaration, impossible to define outside the class in general.
    // Warning.
    friend void non_simple_func(MyTemplate&);
    // friend void non_simple_func(MyTemplate<T>&);
};

void simple_func() {
    MyTemplate<int> val1;
    val1.x = 10;
}

// Template non_simple_func<T>(), does not correspond to a non-template declaration inside MyTemplate.
template<typename T>
void non_simple_func([[maybe_unused]] MyTemplate<T> &val) {
    val.x = 10;
}

// Non-template non_simple_func
void non_simple_func(MyTemplate<int> &val) {
    val.x = 10;
}

void non_simple_func(MyTemplate<char> &val) {
    val.x = 10;
}

int main() {
    MyTemplate<int> val1;
    MyTemplate<char> val2;
    MyTemplate<void> val3;

    non_simple_func(val1);  // Calls global function, ok
    non_simple_func(val2);  // Calls global function, ok.
    non_simple_func(val3);  // Calls global function by its friend declaration, undefined reference.

    non_simple_func<int>(val1);  // Attempts to call the template function, it is not a friend.
}
```

объявим функцию внутри класса (аккуратно с переопределением!!!!!!)



```c++
template<typename T>
struct MyTemplate {
private:
    int x = 0;

    // (3) For all T: generate an independent non-template
    // non_templ_friend(MyTemplate<T>, MyTemplate<void>) which is a friend of MyTemplate<T>.
    // Ok, strictly better than (2).
    friend void non_templ_friend(MyTemplate &val, [[maybe_unused]] MyTemplate<void> &weird) {
        val.x = 10;

        // Is this specific `non_templ_friend` a friend of `MyTemplate<void>`?
        // Who is a friend of `MyTemplate<void>`?
        // `void non_templ_friend(MyTemplate<void> &val, MyTemplate<void> &weird)`
        weird.x = 10;  // Should not compile unless T=void, but GCC 13 compiles it anyway. Clang does not.
        // See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=109923
    }
};

int main() {
    MyTemplate<int> val1;
    MyTemplate<char> val2;
    MyTemplate<void> weird;

    [[maybe_unused]] MyTemplate<int> val1b;
    // non_templ_friend(val1, val1b);  // val1b is not <void> g++ пофиг - это баг, clang - молодец
    non_templ_friend(val1, weird);  // T=int
    non_templ_friend(val2, weird);  // T=char
    non_templ_friend(weird, weird);  // T=void
}
```


последний прикол - нешаблонный друг - который не использует T - шаблонный параметр структуры

(тут для функции генерируется много определений - при каждом инстанцировании - получаем ODR)

```c++
template<typename T>
struct MyTemplate {
private:
    int x = 0;

    // (3)(bad) For all T: generate an independent non-template
    // non_templ_friend(MyTemplate<int>, MyTemplate<void>) which is a friend of MyTemplate<T>.
    // Redefinition, ODR violation, inline does not help.
    friend void non_templ_friend(MyTemplate<int> &val, MyTemplate<void> &weird) {
        val.x = 10;
        weird.x = 10;  // Should always compile
    }
};

int main() {
    MyTemplate<int> val1;
    MyTemplate<char> val2;
    MyTemplate<void> weird;

    MyTemplate<int> val1b;
    // non_templ_friend(val1, val1b);  // val1b is not <void>
    non_templ_friend(val1, weird);  // T=int
    // non_templ_friend(val2, weird);  // val2 is not <int>
    // non_templ_friend(weird, weird);  // val2 is not <int>
}
```

как чинить? - выносить определение из класса! 


### шаблонные функции друзья шаблонных структур 

все друзья всем 

```c++
template<typename T>
struct MyTemplate {
private:
    int x = 0;

    // (4) For all T: for all U: foo(MyTemplate<U>&, MyTemplate<void>&) is a friend of MyTemplate<T>
    template<typename U>
    friend void foo(MyTemplate<U> &, MyTemplate<void> &);
};

template<typename U>
void foo(MyTemplate<U> &val, MyTemplate<void> &weird) {
    val.x = 10;  // MyTemplate<U>: template<.....> friend void foo(.....)
    weird.x = 10;  // MyTemplate<void>: template<.....> friend void foo(.....)
}

int main() {
    MyTemplate<int> val1;
    MyTemplate<char> val2;
    MyTemplate<void> weird;

    foo(val1, weird);
    foo(val2, weird);
}
```

теперь реализуем эту функцию внутри функции - бебебе, снова нарушаем ODR(inline не помогает)

можно чтобы шаблонная функция была внутри класса - но тогда хотя бы один параметр должен зависеть от шаблона структуры, чтобы каждое инстанцирование генерировало **новую** функцию

### последнее - специализации
шаблонная функция друг только для некоторых параметров 

```c++
template<typename T>
struct MyTemplate;

template<typename U>
void foo(MyTemplate<U> &val, MyTemplate<void> &weird);

template<typename T>
struct MyTemplate {
private:
    int x = 0;

    // For any T: foo<T> is friend of MyTemplate<T>
    friend void foo<>(MyTemplate<T> &val, MyTemplate<void> &weird);  // <> or <T> is mandatory.
    // friend void foo<T>(MyTemplate<T> &val, MyTemplate<void> &weird);  // <> or <T> is mandatory.

    // For any T: foo<T*> is friend of MyTemplate<T>
    friend void foo<>(MyTemplate<T*> &val, MyTemplate<void> &weird);  // <> or <T*> is mandatory.
};

template<typename U>
void foo(MyTemplate<U> &val, [[maybe_unused]] MyTemplate<void> &weird) {
    val.x = 10;
    weird.x = 10;  // Works with U=void or U=void*, but not U=int.
}

int main() {
    MyTemplate<int*> a;
    MyTemplate<void*> b;
    MyTemplate<void> c;
    foo(a, c);  // U=int*
    foo(b, c);  // U=void*
    foo(c, c);  // U=void
}
```

___

# Лекция 12

вектор пользуется placement new

пример - через арифметику указателей 

```c++
#include <iostream>
#include <memory>
#include <string>

struct Person {
    std::string first_name, last_name;

    Person() : first_name(10'000, 'x') {
        std::cout << "Person created\n";
    }
};

int main() {
    alignas(alignof(Person)) char memory[sizeof(Person) * 4];  // Remember about proper alignment!

    Person *people = reinterpret_cast<Person*>(memory);
    Person *a = new (people) Person();
    Person *b = new (people + 1) Person();
    Person *c = new (people + 2) Person();
    // Person *d = new (people + 3) Person();  // ok, but do not want right now
    std::cout << static_cast<void*>(memory) << " " << a << " " << b << " " << c << std::endl;
    std::cout << a->first_name.size() << std::endl;

    // Order is not important, but I prefer to mimic automatic storage duration.
    c->~Person();
    b->~Person();
    a->~Person();
}
```

через оператор new мы не можем указать выравнивание объекта в векторе, поэтому через оператор new/delete не очень делать.

### allocator

- учитывает выравнивание
- dealloc надо вызывать ровно так, как вызывали allocate 
- в самом простом виде у allocator нет никакого состояния. По сути, аллокатор для какого-то класса один глобальный на всю программу, а потом мы просто создаем **интерфейсы**, что то типа std::allocator<Person>() и делаем allocate или deallocate

```c++
#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct Person {
    std::string first_name, last_name;

    Person() : first_name(10'000, 'x') {
        std::cout << "Person created\n";
    }
};

int main() {
    std::vector<Person, std::allocator<Person>> vec;
    // Does approximately the following:

    Person *people = std::allocator<Person>().allocate(4);
    Person *people2 = std::allocator<Person>().allocate(5);

    Person *a = new (people) Person();
    Person *b = new (people + 1) Person();
    Person *c = new (people + 2) Person();
    std::cout << static_cast<void*>(people) << " " << a << " " << b << " " << c << std::endl;
    std::cout << a->first_name.size() << std::endl;

    c->~Person();
    b->~Person();
    a->~Person();

    // std::allocator<Person>().deallocate(people2, 4);  // UB
    std::allocator<Person>().deallocate(people2, 5);
    std::allocator<Person>().deallocate(people, 4);

    // Better allocators with state: https://bloomberg.github.io/bde/white_papers/index.html
}
```

### реализация вектора

плохо - как только добавляем исключения - всё бэд((

```c++
#include <string>
#include <utility>

template<typename T>
struct stub_vector {
    alignas(alignof(T)) char data[sizeof(T) * 3];
    int size = 0;

    T &operator[](int i) {
        return reinterpret_cast<T&>(data[i * sizeof(T)]);
    }

    void push_back(const T &value) {
        ++size;
        new (&(*this)[size - 1]) T(value);  // If throws: `size` is incorrect.
    }

    ~stub_vector() {
        for (int i = 0; i < size; i++) {
            (*this)[i].~T();
        }
    }
};

int main() {
    stub_vector<std::string> vec;
    std::string foo = "hello";
    vec.push_back(foo);
    vec.push_back(std::string(10'000, 'x'));
}
```

copy-swap - красиво и коротко, но всегда требует перевыделения памяти, поэтому в реализации вектора так обычно не пишут


```c++
#include <cassert>
#include <string>
#include <utility>

template<typename T>
struct stub_vector {
    T *buffer = nullptr;
    int size = 0;
    int capacity = 0;

    stub_vector() {}
    stub_vector(const stub_vector &) { /* FIXME */ }
    ~stub_vector() { /* FIXME */ }
    void push_back(const T &) { /* FIXME */ }

    stub_vector &operator=(stub_vector other) {  // strong exception safety, no trouble with aliasing
        std::swap(buffer, other.buffer);
        std::swap(size, other.size);
        std::swap(capacity, other.capacity);
        return *this;
    }
};

int main() {
    stub_vector<std::string> vec1, vec2;
    vec1.push_back(std::string(100, 'a'));
    vec1.push_back(std::string(100, 'b'));
    vec2.push_back(std::string(100, 'x'));
    vec2.push_back(std::string(100, 'y'));

    vec1 = vec2;  // Ideally, no reallocation. But not now :(
}
```
решение - ручками разбирать все случаи

```c++
#include <cassert>
#include <string>
#include <utility>

template<typename T>
struct stub_vector {
    T *buffer = nullptr;
    int size = 0;
    int capacity = 0;

    stub_vector() {}
    stub_vector(const stub_vector &) { /* TODO */ }
    ~stub_vector() { /* TODO */ }
    void push_back(const T &) { /* TODO */ }

    stub_vector &operator=(const stub_vector &other) {
        if (this == &other) {
            return *this;
        }
        if (capacity >= other.size) {
            assert(size == other.size);  // FIXME: create/destroy elements
            for (int i = 0; i < other.size; ++i) {
                buffer[i] = other.buffer[i];
            }
        } else {
            // FIXME: allocate buffer, etc
        }
        return *this;
    }
    // FIXME: move constructor, move assignment
};

int main() {
    stub_vector<std::string> vec1, vec2;
    vec1.push_back(std::string(100, 'a'));
    vec1.push_back(std::string(100, 'b'));
    vec2.push_back(std::string(100, 'x'));
    vec2.push_back(std::string(100, 'y'));

    vec1 = vec2;  // No reallocation!
}
```


на самом деле вектор формально написать невозможно - будет UB(хотя у всех работает). Почему? Потому что мы юзали арифметику указателей - new (people + 1) Person() - а мы можем такое искользовать когда у нас есть массив person, но формально мы его нигде не создавали. Упс...

c++20 - починили

```c++
int main() {
    Person *people = std::allocator<Person>().allocate(4);

    Person *a = new (people) Person();

    // Technically UB before C++20: `people` is not a pointer to an array.
    // "Raw memory" (e.g. by an allocator) is not the same as "Array of `T`".
    // Hence, you cannot do pointer arithmetics with `T*` pointing inside "raw memory".
    // Same with `char*`.
    // Fixed in C++20: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p0593r6.html#dynamic-construction-of-arrays
    // Worked in practice in C++03 as well, so we do that.
    //
    // Do not try placement new of arrays: https://github.com/Nekrolm/ubbook/blob/master/pointer_prominence/array_placement_new.md
    Person *b = new (people + 1) Person();
    Person *c = new (people + 2) Person();

    c->~Person();
    b->~Person();
    a->~Person();

    std::allocator<Person>().deallocate(people, 4);

    // Other weird stuff: `std::launder` and `const`s inside `Person`
    // https://wg21.link/P0532R0
    // More recent: https://stackoverflow.com/a/70419156/767632
    // https://miyuki.github.io/2016/10/21/std-launder.html
    // One can also try changing type of the object: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=86908
}
```

еще приколы с std::launder - но там че то какая-то темная материя 

______
### странности шаблонов 

раньше такое не компилировалось (путалось с побитовым сдвигом)

```c++
#include <vector>

int main() {
    std::vector<std::vector<int>> x;  // Invalid prior to C++11 because of >> token.
    int x = 1 >> 2;
}#include <vector>

int main() {
    std::vector<std::vector<int>> x;  // Invalid prior to C++11 because of >> token.
    int x = 1 >> 2;
}
```

#### слова typename, template

хотим объявить новую переменную какого-то типа? или умножить статическую констунту на что-то? не понятно...

то есть это либо арифметическое выражение, либо объявление переменной. 
Компиляторы на это забивают и всегда считают это как арифметическую операцию.
Чтобы указать, что мы хотим не переменную а тип - надо добавлять typename!! - тогда компилятор будет ждать именно тип и именно объявление переменной 

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

template - похожая  штука для ситуаций когда хотим внутри функции вызвать шаблонную функцию. template для того чтобы подсказать компилятору что дальше у нас идет именно шаблон, и тогда он начнет воспринимать как функцию

```c++
struct with_templ_member {
    template<typename T>
    static int foo() { return 10; };
};

template<typename T>
struct Foo {
    void call_foo() {
        T::foo<int>();
        // T::foo < int      >();  // compilation error: expected primary-expression before 'int'
                                   // i.e.. "cannot compare foo with int"
        T::template foo<int>();  // needs 'template'
    }
};

int main() {
    Foo<with_templ_member>().call_foo();
}
```


### псевдонимы для типов или шаблонов 

```c++
#include <vector>
#include <utility>

using vi1 = std::vector<int>;
typedef std::vector<int> vi2;  // Like variable declaration, prepended 'typedef'.

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

### шаблонные константы и переменые (с++17)

константы нужны в метапроге, переменные - не пон(

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

### шаблоны внутри шаблонов хотим определять вне шаблонной структуры 

надо добавлять и шаблон структуры и шаблон функции

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
Foo<T> Foo<T>::create_foo() {  // careful: <T> is omitted only after ::
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

### аргументы по умолчанию у функции

```c++
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

// For both class and function templates:

// 'class' == 'typename' in the line below. 'struct' is not allowed.
template<typename C = int, typename = char, typename = double, int /*Val*/ = 10>
struct templ_foo {
};
templ_foo<std::vector<int>, char, bool, 20> all;
templ_foo<std::vector<int>, char, bool> x;
templ_foo<std::vector<int>> y;
templ_foo<> z;
templ_foo zz;  // Available because of CTAD (C++17), otherwise 'templ_foo' is a template, not a class
// vector v{1, 2, 3};  // CTAD as well

int main() {
}
```

### можно в качестве параметра для шаблона получить другой шаблон

шаблонизированный параметр template<typename>

clang такую штуку не поддрживает

```c++
#include <array>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

// For both class and function templates:

// You may want a template of a specific 'kind' as a paremeter. Works with argument deduction as well.
// Since C++17: works even though std::vector<T, Alloc>, see https://wg21.link/p0522r0
//              Clang disables it by default because it is 'incomplete': https://github.com/llvm/llvm-project/issues/42305
template<typename T, template<typename> typename Container = std::vector>
struct treap {
    Container<std::pair<T, int>> data;  // (value, priority)
};

template<typename T, template<typename, typename> typename Container = std::vector>
struct foo {};

template<typename T, template<typename, int> typename Container = std::array>
struct bar {};

// Compare this with easier to write and read:
template<typename T, typename Container = std::vector<T>>
struct priority_queue {
    Container c;
    // Cannot create Container<std::pair<T, int>>
};

int main() {
    treap<std::string> h1;
    h1.data.emplace_back("hello", 20);

    treap<std::string, std::set> h2;  // Does not make much sense, but compiles.
    h2.data.emplace("hello", 20);

    // treap<int, std::map> h3;
}
```

## вывод типов в шаблонах

когда работает автовывод шаблонного параметра?

они практически всегда могут из своих параметров вывести шаблонный тип, поэтому часто даже просто <> не пишутся

но так работает не всегда (если напрямую ниггде в параметрах сам С вывести не можем)

```c++
#include <cassert>
#include <vector>

template<typename C>
bool is_begin(typename C::iterator it, const C &c) {  // If C is known, C::iterator is also known, even though it's on the right.
    return c.begin() == it;
}

template<typename C>
bool is_begin2(typename C::iterator) {  // Impossibe to deduce C from 'C::iterator'.
    return true;
}

template<typename It>
bool is_begin2_fixed(It) {  // OK
    return true;
}

struct not_vector { using iterator = typename std::vector<int>::iterator; };

int main() {
    std::vector<int> vec;
    assert(is_begin<std::vector<int>>(vec.end(), vec));
    assert(is_begin<>(vec.end(), vec));
    assert(is_begin(vec.end(), vec));

    is_begin2<std::vector<int>>(vec.begin());
    // is_begin2<>(vec.begin());  // compilation error: template argument deduction/substitution failed
    // is_begin2(vec.begin());  // compilation error: template argument deduction/substitution failed
}
```


при наследовании тоже может зафэйлиться (если у нас несколько базовых классов(даже если от одного наследуемся приватно) - ambigous base)

```c++
#include <vector>

template<typename T>
struct Base {
    Base() {}
};

struct Derived1 : Base<int> {};
struct Derived2 : Base<double> {};
struct Derived3 : Base<int>, private Base<double> {
};

template<typename T>
void foo(const Base<T> &) {
}

int main() {
    foo(Base<int>());
    foo(Derived1());
    foo(Derived2());
    foo<int>(Derived3());
    // foo(Derived3());  // compilation error: ambiguous base, even though Base<double> is private

    // Order:
    // 1. Deduce template arguments.
    // 2. Try to call: check permissions, conversions, etc.
}
```
### компилятор вообще не умеет смотреть на преобразования

```c++
#include <vector>

template<typename T>
struct BaseConstructorTag {};

template<typename T>
struct Base {
    Base() {}
    Base(BaseConstructorTag<T>) {}
};

struct ConvertibleToBase {
    operator Base<int>() {
        return {};
    }
    // Even more fun for compiler and you, see `return-type-resolver` exercise
    // template<typename T> operator T() { return {}; }
    // https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Return_Type_Resolver
};

template<typename T>
void foo(const Base<T> &) {
}

int main() {
    BaseConstructorTag<int> t;
    [[maybe_unused]] const Base<int> &tref = t;  // Creates a temporary and binds to it.
    foo<int>(t);  // Works exactly as above.
    // foo(t);  // compilation error: cannot deduce T because compiler won't go through all possible constructors

    ConvertibleToBase x;
    [[maybe_unused]] const Base<int> &xref = x;  // Creates a temporary and binds to it.
    foo<int>(x);  // Works exactly as above.
    // foo(x);  // compilation error: cannot deduce T because compiler won't go through all possible conversions
}
```

### частичное выведение типов

- можно указать один из параметров явно, а остальное выведется из аргументов

```c++
#include <boost/lexical_cast.hpp>
#include <iostream>

template<typename TA, typename TB>
void print_two(const TA &a, const TB &b) {
    std::cout << a << ' ' << b << std::endl;
}

// Can also combine with default arguments.
template<typename To, typename From>
To my_static_cast(From x) {
    return static_cast<To>(x);
}

int main() {
    // All will call print_two<int, double>:
    print_two<int, double>(10, 23.5);
    print_two<int>(10, 23.5);
    print_two<double>(10, 23.5);  // TA=double, TB=double. const double &a = 10;
    print_two<>(10, 23.5);  // Rarely used in favor of the next line:
    print_two(10, 23.5);
    // No way to specify non-prefix.

    // Prefix of arguments is explicit, everything else is either default or deduced.
    [[maybe_unused]] auto z = my_static_cast<int/*, double*/>(10.5);
    std::cout << boost::lexical_cast<int>("123") + 50 << std::endl;
}
```

### Шаблонный методы 

- по сути всё работает как и раньше


```c++
#include <iostream>

template<typename T, int Value>
struct Foo {
    Foo() {}

    Foo(const Foo<T, Value> &) {
        std::cout << "copy constructor" << std::endl;
    }

    template<int Value2>
    Foo(const Foo<T, Value2> &) {  // `<T, Value2 + 0>` already breaks
        std::cout << Value2 << " --> " << Value << std::endl;
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
}
```

## CTAD - class template argument deduction

- когда в конструкторе не указываем вообще типа шаблонов, то ctad пытается найти подходящий конструктор и из его параметров вывести типы шаблонов

```c++
#include <utility>
#include <vector>

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

int main() {
    [[maybe_unused]] auto p1 = pair(10, 20);
    [[maybe_unused]] pair p2(10, 20);
    [[maybe_unused]] pair<double, int> p3(10, 20);
    // [[maybe_unused]] pair<double> p4(10, 20);  // Either all arguments are specified, or use full CTAD only. No partial.

    std::vector v{1, 2, 3};  // CTAD
}
```

создаются так называемое deduction guides(можно и самим создать)
что-то типа 
```c++

// template<typename TA, typename TB> pair(TA, TB) -> pair<TA, TB>;  // implicitly generated
template<typename TA> pair(TA) -> pair<TA, int>;  // (XXX) explicit deduction guide
```

с конвертациями работать не умеет!

```c++
#include <utility>

template<typename TA, typename TB>
struct pair {
    TA a;
    TB b;

    pair() {}
    pair(TA a_) : a(std::move(a_)) {}
    pair(TA a_, TB b_) : a(std::move(a_)), b(std::move(b_)) {}
};

int main() {
    struct ConvertibleToPair {
        operator pair<int, double>() { return {10, 20.5}; }
        // operator pair<int, int>() { return {10, 20.5}; }
    } magic;

    // [[maybe_unused]] pair p1 = magic;  // CTAD does not go through conversions, guides are pre-generated
    [[maybe_unused]] pair p2 = magic.operator pair<int, double>();  // CTAD works by seeing `pair<int, double>`
    [[maybe_unused]] pair<int, double> p3 = magic;  // No CTAD
}
```

раньше для с++ писали специальные функции - для них вывод работал всегда

в шаблон может быть зашита константность - в T

```c++
#include <iostream>

template<typename T>  // T=int, T=const int, T=vector<int>, T=const vector<int>
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
    // print<const int>(10);  // T=const int, const int &

    // print(10);  // Arg=int. T=int. int &x;
    // auto &a = 10;  // `auto` has same rules for deduction as templates

    print_off(20);  // Arg=int. T=int. int x;
    [[maybe_unused]] auto b = 20;

    const int x = 30;
    // print(x);  // Arg=const int. T=const int. const int &x; Compilation error inside.
    [[maybe_unused]] auto &c = x;  // auto=const int

    [[maybe_unused]] auto d = x;  // auto=int
    print_off(x);  // Arg=const int. T=int. int x;
}
// More details: approx 15m of CppCon 2014: Scott Meyers: Type Deduction and Why You Care: https://www.youtube.com/watch?v=wQxj20X-tIU
```

приколы со строчками - пипяо 

```c++
#include <iostream>
#include <string>

template<typename T>
void print_two(const T &a, const T &b) {
    std::cout << a << " " << b << std::endl;
}

int main() {
    print_two<std::string>(std::string("hello"), "world");
    // print_two<>(std::string("hello"), "world");  // compilation error: conflicting types

    print_two<>("hello", "world");  // ok, both are char[6]
    print_two<>("hello111", "world");  // not ok: char[9], char[6]. But it's ok if 'T' instead of 'const &' because 'char*'.
}
```

# Лекция 13

метапрограммирование - генерация какого-то необходимого кода. То есть программируем программу так, чтобы она генерировала нам какие-то нужные конструкции

более формально 

    Метапрограммирование — вид программирования, связанный с созданием программ, которые порождают другие программы как результат своей работы. 



## вспомогательный класс - std::tuple 

std::get\<i\> - шаблонная функция возвращающая элемент из tuple под номером i
make_tuple
decltype - возвращает тип перевенной

```c++
#include <cassert>
#include <functional>
#include <tuple>
#include <string>
#include <vector>

using std::string;
using std::vector;

void foo(int a, string b) {
    assert(a == 10);
    assert(b == "hello");
}

int main() {
    {
        std::tuple<int, vector<int>, string> t(10, vector<int>(2), "foo");
        auto t2 = std::make_tuple(10, vector<int>(2), "foo");  // t == t2
        assert(t == t2);
        int a = std::get<0>(t);  // 0 should be compile-time constant.
        string c = std::get<2>(t);
        assert(a == 10);
        assert(c == "foo");
    }

    {
        std::pair<int, string> p(10, "foo");
        std::tuple<int, string> t = p;  // Implicit conversion: pair --> tuple.
        auto tt = std::tuple_cat(t, t);  // Concatenation (cat)
        // You can get tuple element's type.
        std::tuple_element_t<0, decltype(tt)> x = std::get<0>(tt);
        static_assert(std::tuple_size_v<decltype(tt)> == 4);
        assert(x == 10);
    }

    {
        auto t = std::make_tuple(10, "hello");
        std::apply(foo, t);  // You can call a function
    }
}
```


можно в tuple складывать ссылки 


```c++
#include <cassert>
#include <tuple>
#include <string>

using std::string;

std::tuple<int, string> foo() {
    return {30, "baz"};
}

int main() {
    {
        int a = 10; string b = "foo";
        std::tuple<int&, string&&> t(a, std::move(b));  // can store arbitrary references
        t = std::make_tuple(20, "bar");  // assigns to every field
        assert(a == 20);
        assert(b == "bar");
        // Python: `a, b = (20, "bar")`

        std::tie(a, b) = foo();  // creates a temporary tuple<int&, string&>
        assert(a == 30);
        assert(b == "baz");

        auto [c, d] = foo();  // structured binding (C++17), always new variables.
    }
}
```

## Специализация шаблонов

как то поменять поведение класса при каком-то конкретном параметре. То есть при всех параметров, кроме какого-то выделеного, у нас происходит инстанцировании и используются одни и теже метода, а вот выделенный ведет себя по другому

```c++
#include <cassert>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

template<typename T>
struct my_vector {  // primary template
private:
    std::vector<T> data;
public:
    my_vector(int n) : data(n) {}
    T get(int i) { return data[i]; }
    void set(int i, T value) { data[i] = std::move(value); }
    void foobarbaz() {  // Unused methods are not compiled!
        foobarbazbazbaz();  // TODO?
    }
};

// my_vector<bool> v1(10);  // Implicit instantiation of my_vector<bool> before specialization is known, ODR violation

template<>
struct my_vector<bool>;  // Optional: specialization declaration, disables implicit instantiation.

template<>
struct my_vector<bool> {  // specialization, usual class, can be in .cpp
private:
    // There is no `T`, no relation to the primary template whatsoever
    std::vector<std::uint8_t> bits;
public:
    my_vector(int n) : bits((n + 7) / 8) {}
    bool get(int i) { return (bits[i / 8] >> (i % 8)) & 1; }
    void set(int i, bool value) {
        if (value) {
            bits[i / 8] |= 1 << (i % 8);
        } else {
            bits[i / 8] &= ~(1 << (i % 8));
        }
    }
    void foobarbaz() {  // Even unused methods are compiled!
        // foobarbazbazbaz();
    }
};

my_vector<bool> v2(10);

int main() {
    {
        my_vector<int> v(10);
        v.set(0, 100);
        v.set(8, 200);
        std::cout << v.get(0) << " " << v.get(1) << " " << v.get(8) << "\n";
    }
    {
        my_vector<bool> v(10);
        v.set(0, true);
        v.set(1, true);
        v.set(0, false);
        v.set(8, true);
        std::cout << v.get(0) << " " << v.get(1) << " " << v.get(8) << "\n";
    }
}
```

почему специализации std::vector<bool> - не очень хорошо? - пример тут, но я не прониклась 
https://github.com/hse-spb-2023-cpp/lectures/blob/main/26-240423/02-specialization/02-vector-bool-bad.cpp


в метапроге - специализации используются активно!!

например is_void_v - прикольноооо

```c++
#include <iostream>

template<typename T>
struct is_void {
    static inline const bool value = false;
};

template<>
struct is_void<void> {
    static inline const bool value = true;
};

template<typename T>
const bool is_void_v = is_void<T>::value;

template<typename T>
void foo() {
    if (is_void_v<T>) {
        std::cout << "void!\n";
    } else {
        std::cout << "not void!\n";
    }
}

int main() {
    std::cout << is_void_v<int> << "\n";
    std::cout << is_void_v<void> << "\n";
    std::cout << is_void_v<decltype(main())> << "\n";
    foo<int>();
    foo<void>();
}
```

из-за специализаций можно супер просто нарушить ODR

Moral: do not specialialize in different headers/translation units.

### Частичные специализации

```c++
#include <iostream>

template<typename T>
struct is_reference {
    static inline const bool value = false;
};

template<typename T1>
struct is_reference</*T=*/ T1&> {  // (const U)& as well.
    static inline const bool value = true;
};

template<typename T>
const bool is_reference_v = is_reference<T>::value;

int main() {
    std::cout << is_reference_v<int> << "\n";
    std::cout << is_reference_v<int&> << "\n";  // T1=int
    std::cout << is_reference_v<const int&> << "\n";  // T1=const int
}
```

```c++
#include <iostream>
#include <type_traits>

template<bool B, typename T, typename F>
struct conditional {
    using type = T;
};

template<typename T, typename F>
struct conditional<false, T, F> {
    using type = F;
};

int main() {
    conditional<2 * 2 == 4, int, double>::type x = 10.5;
    conditional<2 * 2 == 5, int, double>::type y = 10.5;
    std::cout << x << "\n";
    std::cout << y << "\n";
}
```


## специализации функций

можно даже не реализовывать базовый случай

```c++
#include <random>
#include <iostream>

std::mt19937 gen;

template<typename T>
T random(const T &from, const T &to);

template<>
int random<int>(const int &from, const int &to) {  // Argument types should match exactly: `const int &`, not just `int`.
    std::uniform_int_distribution<int> distrib(from, to - 1);
    return distrib(gen);
}

template<>
double random(const double &from, const double &to) {  // May actually omit template parameters if they're deduced from args
    std::uniform_real_distribution<double> distrib(from, to);
    return distrib(gen);
}

int main() {
    std::cout << random(1, 6) << "\n";  // T=int
    std::cout << random<int>(1, 6) << "\n";
    std::cout << random<int>(1, 6) << "\n";
    std::cout << "=====\n";
    std::cout << random(0.0, 1.0) << "\n";  // T=double
    std::cout << random<double>(0.0, 1.0) << "\n";
    std::cout << random<double>(0, 1) << "\n";
    // std::cout << random(0.0, 1) << "\n";  // compilation error: T=int or T=double?
    std::cout << random<float>(1.0f, 2.0f) << "\n";  // link error: no impl for `random<float>`
}
```


функции можно перегружать - функции с одинаковым названием но разными аргументами


у функций частичных специализации нет. Если все таки нужны частичные специализации - надо обернуть в вспомогательный класс

```c++
#include <iostream>
#include <vector>

// No partial specializations for functions, unlike classes.
// Can be emulated: delegate to a member of a class.
template<typename T>
struct reader;

template<typename T>
T read() {
    return reader<T>::read();
}

/*
template<>
int read<int>() {}  // OK

template<typename T>
vector<T> read<vector<T>>() {}  // Not OK :(
*/

template<typename T>
struct reader {
    static T read() {
        T res;
        std::cin >> res;
        return res;
    }
};

template<typename T>
struct reader<std::vector<T>> {
    static std::vector<T> read() {
        int n;
        std::cin >> n;
        std::vector<T> res(n);
        for (auto &v : res) {
            // v = reader<T>::read();  // OK
            // v = read<T>();  // Does not compile: thinks it's `reader::read`, not `::read`
            v = ::read<T>();
        }
        return res;
    }
};

int main() {
    std::cout << read<int>() << "\n";
    auto vec = read<std::vector<double>>();
    std::cout << vec.size() << "\n";
    std::cout << vec[0] << "\n";
}
```


### свой SWAP

```c++
#include <algorithm>
#include <iostream>

namespace my_secret {
struct Basic {};
}

namespace std {
// void swap(my_secret::Basic&, my_secret::Basic&) {  // UB, overload.
template<> void swap(my_secret::Basic&, my_secret::Basic&) {  // Not UB, specialization. UB since C++20.
    std::cout << "swap(Basic)\n";
}
};

namespace my_secret {
template<typename T>
struct Foo {};

// template<> swap(my_secret::Foo<T>&, my_secret::Foo<T>&);  // partial specialization :(

// Cannot specialize std::swap. Instead: add an ADL-found version.
template<typename T>
void swap(Foo<T> &, Foo<T> &) {
    std::cout << "swap(Foo<T>)\n";
}
};

int main() {
    {
        my_secret::Basic a, b;
        std::swap(a, b);  // OK swap.

        using std::swap;  // Required for the next line to compile.
        swap(a, b);  // OK swap
    }
    {
        my_secret::Foo<int> a, b;
        std::swap(a, b);  // Wrong swap.

        using std::swap;  // Optional for the next line to compile, but won't break it.
        swap(a, b);  // OK swap, ADL.
    }
    // Moral: `using std::swap; swap(a, b)` in generic code.
}
```


## variadic

- позволяет создавать шаблоны с произвольным количеством аргументов 

template parametr pack - сколько угодно параметров, каждый из которых является типом 

надо научиться распаковывать 

```c++
#include <iostream>

void print() {
}

template<typename T>
void print(const T &v) {
    std::cout << v;
}

template<typename T1, typename T2>
void print(const T1 &v1, const T2 &v2) {
    std::cout << v1 << " " << v2;
}

template<typename T1, typename T2, typename T3>
void print(const T1 &v1, const T2 &v2, const T3 &v3) {
    std::cout << v1 << " " << v2 << " " << v3;
}

// "Variadic template" is a template which has a "template parameter pack"
// https://en.cppreference.com/w/cpp/language/parameter_pack
// It is NOT a "variadic function" (C-style `printf`-like): https://en.cppreference.com/w/c/variadic

template<typename /* template parameter pack */ ...Ts>
// Mnemonics: ... on the left of declared pack.
void println(const Ts &...vs) {  // "function parameter pack"
    // Mnemonics: ... on the right of the used pack.
    // No way to "index" an element because they may have different types.
    // No simple way to force all elements to have the same type.
    print(vs...);  // pack expansion
    // print(10, 20, 'x')
    std::cout << " - " << sizeof...(vs) << " element(s)\n";
}

int main() {
    println(10, 20, 'x');
    println(10);
    println<int, double>(10, 20);  // explicit template arguments
}
```
сейчас получилась типа обертка над print - не очень умно, но синтаксис поняли

более умная распаковка - можем как то менять наши вариадики

```c++
#include <iostream>

void print(int a, int b, int c, int d) {
    std::cout << a << " " << b << " " << c << " " << d << "\n";
}

template<typename ...Ts>
void print_twice(const Ts &...vs) {
    print(239, vs..., 17, vs...);
}

template<typename ...Ts>
void print_plus_10(const Ts &...vs) {
    print((vs + 10)...);
    // print((1 + 10), (2 + 10), (3 + 10), (4 + 10))
}

template<typename ...Ts>
void print_double(const Ts &...vs) {
    print((vs + vs)...);  // expanded in parallel, should have the same length, otherwise compilation error.
    // print((1 + 1), (2 + 2), (3 + 3), (4 + 4))
}

int times_three(int x) { return x * 3; }

template<typename ...Ts>
void print_times_three(const Ts &...vs) {
    print(times_three(vs)...);  // print(times_three(1), times_three(2), times_three(3), times_three(4))
    print(times_three(vs...));  // print(times_three(1, 2, 3, 4))
}

int main() {
    print_twice(1);
    print_plus_10(1, 2, 3, 4);
    print_double(1, 2, 3, 4);
    print_times_three(1, 2, 3, 4);
}
```

Может быть несколько parametr pack

и тут по разныму происходит распределение между разными паками 

```c++
#include <iostream>
#include <tuple>

template<typename ...As, typename ...Bs>
void foo(As ...as, Bs ...bs) {
    std::cout << "foo(" << sizeof...(as) << ", " << sizeof...(bs) << ")\n";
}

template<typename ...As, typename ...Bs>
void bar([[maybe_unused]] std::tuple<As...> at, Bs ...bs) {
    std::cout << "bar(" << sizeof...(As) << ", " << sizeof...(bs) << ")\n";
}

template<typename ...As, typename ...Bs>
void baz([[maybe_unused]] std::tuple<int, As...> at, int, Bs ...bs) {
    std::cout << "baz(" << sizeof...(As) << ", " << sizeof...(bs) << ")\n";
}

template<typename ...As, typename ...Bs>
void baz_bad([[maybe_unused]] std::tuple<As..., int> at, Bs... bs, int) {
    std::cout << "baz(" << sizeof...(As) << ", " << sizeof...(bs) << ")\n";
}

int main() {
    // GCC, VS, Clang (and standard?): 0, 3
    foo(1, 2, 3);

    // Where to put the argument: As or Bs? GCC, VS, Clang: 1, 0
    foo<int>(1);

    // 2, 2
    bar(std::tuple{1, 2}, 3, 4);

    // Parameter pack should be the last one for successful deduction.
    baz(std::tuple{1, 2}, 3, 4);  // 1, 1
    // baz_bad(std::tuple{1, 2}, 3, 4);  // Neither As nor Bs are deduced, compilation error.
    // baz_bad<int>(std::tuple{1, 2}, 3);  // I don't know whether this should work or not.
}
```


раскрыть можем как то 
 - в вектор например
можно в лямбду передать 

```c++
#include <iostream>
#include <tuple>

template<typename ...Ts>
void foo(Ts...) {}

template<typename ...Ts>
void bar(Ts ...vs) {
    foo(vs...);  // function call
    // Different order of evaluation:
    [[maybe_unused]] std::tuple t1(vs...);  // direct initialization
    [[maybe_unused]] std::tuple t2{vs...};  // direct list initialization
    [[maybe_unused]] int data[] = {vs...};  // array initialization
    int x = 10;
    [x, vs...]() {};  // works with captures
    [x, &vs...]() {};  // works with captures
    // [x, vs = std::move(vs)...]() {};  // no simple way to "move" parameter pack
    [x, vs = std::make_tuple(std::move(vs)...)]() {};  // only via std::tuple (it moves/copies arguments inside a tuple), may need std::apply inside.

    for (double d : data) {
        std::cout << d << "\n";
    }
}

int main() {
    bar(12, 3.4);
}
```

### самый просто способ проитерироваться по параметрам пака

появились Fold expression - применить некоторый оператор ко всему parametr pack

правая и левая свертки - важно ()!!

```c++
#include <iostream>

// Since C++17: https://en.cppreference.com/w/cpp/language/fold
// Fold expression should always be inside its own parenthesis.

template<typename ...Ts>
auto sum1(const Ts &...vs) {
    return (vs + ...);  // unary right fold: v0 + (v1 + (v2 + v3))
    // return (... + vs);  // unary left fold: ((v0 + v1) + v2) + v3
}

template<typename ...Ts>
auto sum2(const Ts &...vs) {
    return (vs + ... + 0);  // binary right fold: v0 + (v1 + (v2 + 0))
    // return (0 + ... + vs);  // binary left fold: ((0 + v0) + v1) + v2
}

template<typename ...Ts>
auto sum_twice(const Ts &...vs) {
    return ((2 * vs) + ... + 0);  // patterns are allowed
}

template<typename ...Ts>
void print(const Ts &...vs) {
    // binary left fold: (std::cout << v0) << v1
    (std::cout << ... << vs) << "\n";
    // (std::cout << vs << ...) << "\n";  // incorrect
    // (0 + vs + ...)  // incorrect too
    // std::cout << ... << vs << "\n";  // incorrect fold expression

    // (std::cout << ... << (vs << "\n"));  // `v0 << "\n"` is not a valid expression
    // std::cout << (v0 << "\n") << (v1 << "\n") << (v2 << "\n");
}

int main() {
    // std::cout << sum1() << "\n";  // compilation error
    std::cout << sum1(1, 2, 3) << "\n";
    std::cout << sum1("hello", " ", std::string("world")) << "\n";

    std::cout << sum2() << "\n";
    std::cout << sum2(1, 2, 3) << "\n";

    std::cout << sum_twice(1, 2, 3) << "\n";

    print(1, "hello");
}
```

Можно соединить с лямбдами и получить вот что

```c++
#include <iostream>

// https://foonathan.net/2020/05/fold-tricks/

template<typename ...Ts>
void println(const Ts &...vs) {
    bool first = true;
    auto f = [&](const auto &v) {  // In C++11: use a class with templated `operator()`.
        if (first) {
            first = false;
        } else {
            std::cout << " ";
        }
        std::cout << v;
    };
    (f(vs), ...);  // C++17 lifehack.
    // (f(12), (f("hello"), f("world")));

    // int dummy[] = { f(vs)... };  // C++14 lifehack, make sure `f` returns 0.
    // int dummy[] = { f(12), f("hello"), f("world") };
    std::cout << "\n";
}

int main() {
    println(12, "hello", "world");
}
```

рекурсивно еще можно 
- через две специализации метода
- через одну + constexpr (чтобы ровно ода ветка if компилилась)
- через спец структуру

```c++
#include <iostream>

// Two overloads
template<typename T>
auto sum1(const T &v) {
    return v;
}

template<typename T, typename ...Ts>
auto sum1(const T &v, const Ts &...vs) {
    // sum1(1, 2, 3) = 1 + sum1(2, 3)
    // sum1(2, 3) = 2 + sum1(3)
    // sum1(3) = other overload, 3.
    return v + sum1(vs...);
}

// Single overload, since C++17
template<typename T, typename ...Ts>
auto sum2(const T &v, const Ts &...vs) {
    // `if constexpr` does not compile the second branch.
    if constexpr (sizeof...(vs) == 0) {
        return v;
    } else {
        return v + sum2(vs...);  // No sum2().
    }
}

// Another alternative: wrap into a class and use template specializations.
template<typename Head, typename ...Tail>
struct Summator {
    auto operator()(const Head &h, const Tail &...tail) {
        return h + Summator<Tail...>{}(tail...);
    }
};
template<typename Head>
struct Summator<Head> {
    auto operator()(const Head &h) {
        return h;
    }
};

// Another specializations
template<typename ...Ts>
struct Summator2;  // Not even defined.

template<typename Head>
struct Summator2<Head> {
    auto operator()(const Head &h) {
        return h;
    }
};

template<typename Head, typename ...Tail>
struct Summator2<Head, Tail...> {
    auto operator()(const Head &h, const Tail &...tail) {
        return h + Summator2<Tail...>{}(tail...);
    }
};

int main() {
    std::cout << sum1(1, 2, 3) << "\n";
    std::cout << sum2(1, 2, 3) << "\n";
    std::cout << Summator<int, int, int>{}(1, 2, 3) << "\n";
    std::cout << Summator2<int, int, int>{}(1, 2, 3) << "\n";
}
```


### своя функция std::apply 

- свои индексы нужно бахать - std::make_index_sequence\<sizeof...(Ts)\>()


```c++
#include <cstddef>
#include <iostream>
#include <tuple>
#include <utility>

template<typename ...Ts, std::size_t ...Is>
void print_helper(const std::tuple<Ts...> &t, std::index_sequence<Is...>) {
    // Ts = {int, double, const char*}, Is = {0, 1, 2}
    bool first = true;
    auto f = [&](const auto &v) {
        if (first) {
            first = false;
        } else {
            std::cout << " ";
        }
        std::cout << v;
    };
    (f(std::get<Is>(t)), ...);
    // f(std::get<0>(t)), (f(std::get<1>(t)), f(std::get<2>(t)));
    std::cout << "\n";
}

template<typename ...Ts>
void print(const std::tuple<Ts...> &t) {
    // Step 1: what do we want.
    std::cout << std::get<0>(t) << " " << std::get<1>(t) << " " << std::get<2>(t) << "\n";
    // Problem: get<I> needs I to be a compile-time constant.

    // Step 2: provide a compile-time list of constants in a helper type tag.
    print_helper(t, std::index_sequence<0, 1, 2>{});

    // Step 3: generate the sequence (can be done recursively)
    print_helper(t, std::make_index_sequence<sizeof...(Ts)>{});

    // Step 4 in C++20:
    auto print_helper_lambda = [&]<std::size_t ...Is>(std::index_sequence<Is...>) {
        (std::get<Is>(t), ...);
        // ...
    };
    print_helper_lambda(std::make_index_sequence<sizeof...(Ts)>{});
}

int main() {
    std::tuple t{1, 2.3, "hello"};  // tuple<int, double, const char*>
    print(t);
}
```


# Лекция 14


### вариадики в классах

```c++
#include <cstddef>
#include <string>
#include <tuple>

// "Variadic template" is a template which has a "template parameter pack"
// https://en.cppreference.com/w/cpp/language/parameter_pack
template<typename A, int N, typename ...Ts>
struct Foo {
     // Ts... xs;  // Does not work, use std::tuple instead:
     std::tuple<Ts...> xs;
};
Foo<int, 10> f1;                                        // sizeof... = 0
Foo<int, 10, int, int> f2;                              // sizeof... = 2
Foo<int, 10, int, Foo<int, 10, char>, std::string> f3;  // sizeof... = 3

// error: parameter pack 'Ts' must be at the end of the template parameter list
// template<typename... Ts, typename T> struct Bar {};
```

можно даже наследоваться от вариадиков...

```c++
#include <iostream>

struct Foo {
    Foo(int) {}
    void func(int) { std::cout << "1\n"; }
};

struct Bar {
    Bar(int);
    void func(char) { std::cout << "2\n"; }
};

template<typename ...Ts>
struct Hybrid : Ts... {  // Hybrid : Foo, Bar
    Hybrid() : Ts(10)... {}
    // Hybrid() : Foo(10), Bar(10) {}
    using Ts::func...;  // Since C++17 (TODO), not exam
};

int main() {
    Hybrid<Foo, Bar> h;
    // ....
    h.func(10);   // (1)
    h.func('x');  // (2)
}
```

#### parametr pack в специализациях

```c++
#include <tuple>

template<typename, typename>
struct Foo {
};

template<typename ...As, typename ...Bs>  // Ok, multiple parameter packs.
struct Foo<std::tuple<As...>, std::tuple<Bs...>> {  // Similar to template type deduction.
};

// We can "parse" types, like in boots::function_traits
template<typename> struct function_traits {};
template<typename Ret, typename ...Args>
struct function_traits<Ret(*)(Args...)> {
    static constexpr std::size_t arity = sizeof...(Args);
    using return_type = Ret;
};

static_assert(std::is_void_v<
    function_traits<void(*)(int, int, char)>::return_type
>);
static_assert(
    function_traits<void(*)(int, int, char)>::arity == 3
);
```

### свой tuple

рекурсивно можем

```c++
#include <cstddef>
#include <string>

// There is literally no way to "index" the parameter pack. Recursion via specialization only:
template<typename... Ts>
struct tuple {};

template<typename Head, typename... Tail>
struct tuple<Head, Tail...> {  // For non-empty lists.
    Head head{};
    tuple<Tail...> tail;  // Some use inheritance instead so EBO (Empty Base Optimization) is enabled.
};

template<std::size_t I, typename ...Ts>
const auto &get(const tuple<Ts...> &t) {
    // Since C++17
    if constexpr (I == 0) return t.head;
    else                  return get<I - 1>(t.tail);

    // Before C++17: classes only, because there are no partial specializations for functions.
}

int main() {
    tuple<int, std::string> x;
    [[maybe_unused]] auto a = x;  // tuple<int, std::string>
    [[maybe_unused]] auto b = x.head;  // int
    [[maybe_unused]] auto c = x.tail;  // tuple<std::string>
    [[maybe_unused]] auto d = x.tail.head;  // std::string
    [[maybe_unused]] auto e = x.tail.tail;  // tuple<>
    // [[maybe_unused]] auto f = x.tail.tail.head;  // compilation error

    [[maybe_unused]] int f1 = get<0>(x);
    [[maybe_unused]] std::string f2 = get<1>(x);
}
```

### perfect-forwarding

какую задачу решаем?

у нас траблы в коде ниже: хотим избегать лишних копирований плюс лишних std::ref

```c++
#include <functional>
#include <iostream>
#include <string>

void foo(int x) {
    std::cout << "  foo(" << x << ")\n";
}

void bar(int &x) {
    std::cout << "  bar(" << x << ")\n";
    x++;
}

struct Storer {
    std::string stored;
    /*
    // Inefficient: extra construction, extra destruction
    void operator()(std::string x) {
        std::cout << "  store copy\n";
        stored = std::move(x);
    }
    */
    // Efficient: only one assignment (copy or move)
    void operator()(const std::string &x) {
        std::cout << "  store copy\n";
        stored = x;
    }
    void operator()(std::string &&x) {
        std::cout << "  store move\n";
        stored = std::move(x);
    }
};

template<typename Arg, typename Fn>
void log(Fn fn, Arg arg) {
    std::cout << "start\n";
    fn(std::move(arg));
    // fn(arg);  // bad as well
    std::cout << "end\n";
}

int main() {
    log(foo, 10);

    int x = 20;
    // log(bar, x);  // compilation error :(
    log(bar, std::ref(x));  // Requires std::ref at call place
    std::cout << "x = " << x << "\n";

    std::string s = "hello world from long string";
    const std::string cs = "hello world from long const string";
    Storer baz;
    log(baz, s);  // 1 copy ctor, 1 move assignment, 1 destructor.
    log(baz, std::move(s));  // 1 move ctor, 1 move assignment, 1 destructor.
    log(baz, std::ref(s));  // 1 copy assignment
    // log(baz, std::ref(std::move(s)));  // Unsupported.
    log(baz, cs);  // 1 copy, 1 move, 1 destructor.
    log(baz, std::move(cs));  // 1 copy, 1 move, 1 destructor.

    // Two problems:
    // 1. Lots of `std::ref`/`std::cref`
    // 2. No way to do just 1 move.
    // Underlying reason: `log` creates an object, but references are enough.
    // Moreover, inspecting `fn` is impossible, it may even have overloads.
}
```

первый способ решения проблемы - раздвоить log

```c++
template<typename Arg, typename Fn>
void log(Fn fn, Arg &arg) {
    std::cout << "start (1)\n";
    fn(arg);
    std::cout << "end\n";
}

template<typename Arg, typename Fn>
void log(Fn fn, Arg &&arg) {  // not really rvalue reference, but just you wait.
    std::cout << "start (2)\n";
    fn(std::move(arg));
    std::cout << "end\n";
}
```

хотим схлопнуть log в одну функцию вместо двух: - новый синтаксис

идеально принять аргумент и идеально его передать в функцию

```c++
#include <functional>
#include <iostream>
#include <string>

void foo(int x) {
    std::cout << "  foo(" << x << ")\n";
}

void bar(int &x) {
    std::cout << "  bar(" << x << ")\n";
    x++;
}

struct Storer {
    std::string stored;
    void operator()(const std::string &x) {
        std::cout << "  store copy\n";
        stored = x;
    }
    void operator()(std::string &&x) {
        std::cout << "  store move\n";
        stored = std::move(x);
    }
};

template<typename Arg, typename Fn>
void log(Fn fn, Arg &&arg) { // Special `Arg` deduction rules: "forwarding reference".
    // Assume the argument's type is `T`. Its value category is either:
    // * Rvalue: Arg = T , Arg&& = T&&
    // * Lvalue: Arg = T&, Arg&& = T&  (reference collapse rules: & + && = &, non-temporary wins)
    std::cout << "start\n";

    // fn(arg);  // always lvalue :(
    // fn(std::move(arg));  // always rvalue :(
    fn(static_cast<decltype(arg)>(arg));  // OK, lvalue or rvalue.
    fn(static_cast<Arg&&>(arg));  // OK, lvalue or rvalue.

    // std::forward<Arg> ~ static_cast<Arg&&>:
    // * Rvalue: Arg = T , Arg&& = T&&, std::forward ~ static_cast<T&&> ~ std::move
    // * Lvalue: Arg = T&, Arg&& = T& , std::forward ~ static_cast<T&>  ~ no-op !!!
    fn(std::forward<Arg>(arg));  // OK, The Tradition! BEST

    // Notes:
    // 1. Not `std::move`: it always returns T&& (rvalue).
    // 2. `arg` is always lvalue of type `T`, so no automatic deduction is possible.
    //    Hence, the explicit template argument is required.
    // Alternative: `std::forward<Arg&&>(arg)`, `std::forward<decltype(arg)>(arg)`.

    // fn(std::forward(arg));  // Bad, compilation error.
    std::cout << "end\n";
}

int main() {
    log(foo, 10);  // Rvalue: Arg=int, Arg&&=int&&

    int x = 20;
    log(bar, x);  // Lvalue: Arg=int&, Arg&&=int&
    std::cout << "x = " << x << "\n";

    std::string s = "hello world from long string";
    const std::string cs = "hello world from long const string";
    Storer baz;
    log(baz, s);  // Lvalue: Arg=std::string&, Arg&&=std::string&
    log(baz, std::move(s));  // Rvalue: Arg=std::string, Arg&&=std::string&&
    log(baz, cs);  // Lvalue: Arg=const string&, Arg&&=const string&
    log(baz, std::move(cs));  // Rvalue: Arg=const string, Arg&&=const string&&
}
```


НЕ ПУТАТЬ!!!
```c++
template<typename T>
struct my_vector {
    void push_back_1(T &&) {  // not forwarding reference: `T` is already fixed. Just rvalue reference.
    }

    template<typename U>
    void push_back_2(U &&) {  // forwarding reference: `U` is deduced when calling
    }

    static void demo() {
        my_vector<int> v;
        v.push_back_2(10);  // forwarding reference: U=int
        int x;
        v.push_back_2(x);  // forwarding reference: U=int&
        v.push_back_2<int>(x);  // not forwarding reference
    }
};

template<typename T>
void foo(my_vector<T> &&) {  // not forwarding reference: argument is always rvalue, `T` cannot affect this
}
```

Итого: perfect forwarding это штука которая нужна если вашу функцию вызвали, она принимает какие-то параметры и мы хотим эти параметры сразу кому то идеально передать (она эти параметры нигде не сохраняет), никогда ничего не копирует, ничего не мувает, принимает ссылку, сохраняет её категорию, передает дальше

не анализирует функцию которую он будет вызывать, смотрит чисто на то как его самого вызвали (с rvalue или c lvalue)

#### perfect-forward-ить можно саму функцию

если у нас не функцию, а функциональный объект, у которого перегружены (), в зависимости от того мы сами временный объект или не временный

```c++
#include <iostream>

struct Foo {
    void operator()() & {
        std::cout << "lvalue\n";
    }
    void operator()() && {
        std::cout << "rvalue\n";
    }
};

template<typename Fn>
void call(Fn &&fn) {
    std::forward<Fn>(fn)();  // Rarely done in STL: it assumes functors are trivial. Should actually be done.
    // fn();  // always lvalue
}

int main() {
    Foo f;
    f();  // lvalue
    Foo()();  // rvalue
    call(f);  // lvalue
    call(Foo());  // should call operator() &&
}
```

#### forward-им то что функция возвращает

type display - лайфхак, во время компиляции понять что там компилятор насчитал в типах....

способ узнать какой тип у чего-то 

decltype - возвращает тип (выражения или переменной) 
2 режима:
- decltype (expression) - type + category
- decltype(name) - смотрит как была объявлена переменная!! 
добавление лишних скобочек может поменять режим...

```c++
#include <vector>

template<typename> struct TD;  // "Type Display" in a compilation error

int& foo();
int&& bar();

int main() {
    int x = 10;
    int &y = x;
    int &&z = std::move(y);

    // decltype() has two modes:
    // 1. decltype(expression)
    TD<decltype(10)>();  // int
    TD<decltype( (x) )>();  // int&
    TD<decltype( (y) )>();  // int&
    TD<decltype( (z) )>();  // int&
    TD<decltype( x + 20 )>();  // int
    TD<decltype( foo() )>();  // int&
    TD<decltype( bar() )>();  // int&&
    // Returns depending on value category: T (prvalue), T& (lvalue), T&& (xvalue)
    // Reminder: rvalue = prvalue || xvalue

    // 2. decltype(name) - a variable/field/argument with no parens
    TD<decltype(x)>();  // int
    TD<decltype(y)>();  // int&
    TD<decltype(z)>();  // int&&
    // Returns: the declaration of `name`

    // In either mode: nothing is evaluated:
    std::vector<int> v;
    decltype(*v.begin()) v_value = 10;
}
```

#### decltype(auto) 
- возвращаемый тип это decltype от того, что написано в return 

```c++
int x;

auto foo() {  // int
    return x;
}

auto &foo_ref() {  // int&
    return x;
}

auto foo_ref_caller_bad() {  // int
    return foo_ref();
}

decltype(auto) foo_ref_caller_bad() {  // int& ~ decltype(foo_ref())
    return foo_ref();
}

decltype(auto) bar() {  // int ~ decltype(x)
    return x;
}

decltype(auto) baz() {  // int& ~ decltype((x))
    return (x);
}

int main() {
}
```


как использовать? с perfect forwarding!! - если хотим чтобы наша промежуточная функция возвращала именно тот тип, который возвращает вызываемая в ней функция (с сохранением ссылочности и всего такого)


```c++
#include <iostream>
#include <utility>

int sum(int a, int b) {
    return a + b;
}

int &get_int_ref() {
    static int x = 10;
    return x;
}

template<typename Fn, typename ...Args>
decltype(auto) logged(Fn fn, Args &&...args) {
    std::cout << "started\n";
    decltype(auto) res = fn(std::forward<Args>(args)...);
    std::cout << "ended\n";
    return res;
    // return (res);  // wrong
}

int main() {
    int x = logged(sum, 2, 3);
    std::cout << "x=" << x << "\n";

    int &ref = logged(get_int_ref);
    std::cout << "ref=" << ref << "\n";
    ref += 10;
    std::cout << "ref2=" << logged(get_int_ref) << "\n";
}
```

всё это можно как лямбды писать: 

```c++
#include <iostream>
#include <utility>

int sum(int a, int b) {
    return a + b;
}

int &get_int_ref() {
    static int x = 10;
    return x;
}

int main() {
    auto logged = [](auto fn, auto &&...args) -> decltype(auto) {
        std::cout << "started\n";
        decltype(auto) res = fn(std::forward<decltype(args)>(args)...);
        std::cout << "ended\n";
        return res;
    };

    int x = logged(sum, 2, 3);
    std::cout << "x=" << x << "\n";

    int &ref = logged(get_int_ref);
    std::cout << "ref=" << ref << "\n";
    ref += 10;
    std::cout << "ref2=" << logged(get_int_ref) << "\n";
}
```

### sfinaeeeeeee

- substitution failure is not an error


изначально была нужна для шаблонных функций у которых какие-то сложные типы в аргументах. чтобы неподходящие перегрузки просто тихо пропускались

```c++
#include <cassert>
#include <vector>

/*
Substitution Failure Is Not An Error (SFINAE)

Standard jokes:
1. Segmentation Fault Is Not An Error
2. SFINAE отбивная
*/

template<typename T>
void duplicate_element(T &container, typename T::iterator iter) {
    container.insert(iter, *iter);
}

template<typename T>
void duplicate_element(T *array, T *element) {
   assert(array != element);
   *(element - 1) = *element;
}

int main() {
    std::vector a{1, 2, 3};
    duplicate_element(a, a.begin() + 1);
    assert((a == std::vector{1, 2, 2, 3}));

    int b[] = {1, 2, 3};
    duplicate_element(b, b + 1);  // Not a compilation error when we "try" the first
    // overload even though `int[]::iterator` does not exist.
    // Substitution `T=int[]` failure, not an error.
    assert(b[0] == 2);
    assert(b[1] == 2);
    assert(b[2] == 3);
}
```

#### ограничение шаблонной структуры
ставим необходимое условие для тип, чтобы он мог быть шаблоном


проще всего - requires requires- c++20

а реньше по другому делали

**declval - передаем тип, она возвращает значение этого типа** - надо вызывать только в штуках, работающих исключительно с типами

```c++
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

struct A { int val = 0; };
struct B { int val = 0; };
struct C { int val = 0; };
struct D { int val = 0; };

struct Foo {
    using Botva = int;
    void method() {}
};

template<typename T, typename U>
auto operator+(const T &a, const U &b) -> decltype(a.val + b.val) {  // !! Option 1
    return a.val + b.val;
}

struct E { int val2 = 0; };
struct F { int val2 = 0; };

template<typename T, typename U>
decltype(std::declval<T>().val2 + std::declval<U>().val2) operator+(const T &a, const U &b) {  // !! Option 2
    return a.val2 + b.val2;
}

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

# Лекция 15

## продолжаем sfinae

### оператор запятая

если мы хотим только проверить корректность выражения, но возвращаемый тип другой...

оператор запятая!!!

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
void println(const T &v) -> decltype(std::cout << v.val, void()) {
    std::cout << v.val << "\n";
}

struct E { int val2 = 0; };
struct F { int val2 = 0; };

template<typename T>
decltype(std::cout << std::declval<T>().val2, std::declval<void>()) println(const T &v) {
    std::cout << v.val2 << "\n";
}

int main() {
    A a{10};
    println(a);  // 10

    std::string s = "hello";
    // println(s);

    [[maybe_unused]] E e{40};
    println(e);
}
```


### enable if

хотим для оператора явно перечислить те структуры, которые нас устраивают

через шаблонную структуру и специализации

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

// Check out error message in GCC vs Clang

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
    //assert(a + foo == 60);  // does not compile: no match for 'operator+'
    //assert(foo + foo == 60);  // does not compile: no match for 'operator+'
}
```

есть enable_if_t - в стандартной библиотеке std::

можно этот enable_if убрать в templates - если там не корректно высчитается, то тоже вырезается под sfinae - полезно, когда есть шаблонный конструктор, которые не возвращает никакого значения, поэтому для него все предыдущие техники не подходят

```c++
#include <cassert>
#include <type_traits>

struct A { int val = 0; };
struct B { int val = 0; };
struct C { int val = 0; };
struct D { int val = 0; };
struct Foo { int val = 0; };

template<typename T>
constexpr bool good_for_plus = std::is_same_v<A, T> || std::is_same_v<B, T> || std::is_same_v<C, T> || std::is_same_v<D, T>;

template<bool Cond, typename T = void>  // !! void by default!
struct enable_if {};

template<typename T>
struct enable_if<true, T> { using type = T; };

template<bool Cond, typename T = void>
using enable_if_t = typename enable_if<Cond, T>::type;

// Especially useful for templated constructors, they do not have return types.
template<typename T, typename U, typename /* Dummy */ = enable_if_t<good_for_plus<T> && good_for_plus<U>>>
// template<typename T, typename U, typename /* Dummy */ = std::enable_if_t<good_for_plus<T> && good_for_plus<U>>>
int operator+(const T &a, const U &b) {
    return a.val + b.val;
}

int main() {
    A a{10};
    C c{30};
    [[maybe_unused]] Foo foo{50};
    assert(a + c == 40);  // should work
    // assert(a + foo == 60);  // does not compile: no match for 'operator+'
    // assert(foo + foo == 60);  // does not compile: no match for 'operator+'

    operator+<A, Foo, int>(a, foo);  // works because default for 'Dummy' is not used :(
}
```


функции считаются разными если:
- названия разные
- возвращаемый тип разный 
- набор аргументов разный 
- набор шаблонных аргументов разный (то что справа от знака равно не учитывается)

поэтому тут redefinition 

```c++

template<typename T, typename U, typename /* Dummy */ = std::enable_if_t<good_for_plus<T> && good_for_plus<U>>>
int operator+(const T &a, const U &b) {
    return a.val + b.val;
}

// Compilation error: redefinition. Default values is not a part of the signature.
template<typename T, typename U, typename /* Dummy */ = std::enable_if_t<good_for_plus<T> && std::is_integral_v<U>>>
int operator+(const T &a, const U &b) {
    return a.val + b;
}
```

как это обойти?
а вот как: переставить enable_if...* = nullptr - нашли способ написать enable_if слева от знака равно - тогда оно учитывается при принятии решения разные функции или нет

### void_t

помогает проверить корректность какого-то выражения 

```c++
#include <cassert>
#include <type_traits>

struct A { int val = 0; };
struct B { int val = 0; };
struct C { int val = 0; };
struct D { int val = 0; };

template<typename...>
using void_t = void;

// Similar to std::void_t

template<typename T, typename U, void_t<decltype(std::declval<T>().val + std::declval<U>().val)>* /* ptr */ = nullptr>
int operator+(const T &a, const U &b) {
    return a.val + b.val;
}

int main() {
    A a{10};
    C c{30};
    assert(a + c == 40);  // should work
    assert(a + "x" == 60);  // does not compile: no match for 'operator+'
}
```

### sfinae внутри класса

пусть у нас шаблоный класс. Хотим сделать так, чтобы функция внутри существовала или не существовала в зависимости от того какой у нас параметр класса.

sfinae работает только тогда когда подставляется шаблонный параметр функции!!!(а не класса)

поэтому единственный костыль который тут работает вот:

```c++
#include <iostream>
#include <type_traits>

template<typename T>
struct MyClass {
#if 0
    // Checking signature on class instantiation fails. No SFINAE: not a template
    std::enable_if_t<std::is_same_v<T, int>, int> foo_bad_1() {
        return 1;
    }
    int foo_bad_1() const {
        return 2;
    }
#endif

#if 0
    // Checking signature on class instantiation fails. No SFINAE: the error does not depend on template parameter
    template<typename = void>
    std::enable_if_t<std::is_same_v<T, int>, int> foo_bad_2() {
        return 1;
    }
    int foo_bad_2() const {
        return 2;
    }
#endif

    template<typename U = T>  // OK
    std::enable_if_t<std::is_same_v<U, int>, int> foo() {
        static_assert(std::is_same_v<U, T>);  // Recommended
        static_assert(std::is_same_v<T, int>);  // Recommended
        return 10;
    }
    int foo() const {
        return 20;
    }
};

int main() {
    {
        MyClass<int> a;
        std::cout << a.foo() << "\n";  // 10

        const auto &b = a;
        std::cout << b.foo() << "\n";  // 20
    }
    {
        MyClass<double> a;
        std::cout << a.foo() << "\n";  // 20

        std::cout << a.foo<int>() << "\n";  // 10, but fails because of static_assert

        const auto &b = a;
        std::cout << b.foo() << "\n";  // 20
    }
}
```

деструтору никак к sfinae не подогнать (в с++17)

еще примерчик про проверку каких-то условий (is_constructible_v)

```c++
#include <fstream>
#include <iostream>
#include <utility>
#include <type_traits>

template<typename T>
struct wrapper {
    T data;

    template<typename Arg/*, std::void_t<decltype(T(std::declval<Arg&&>()))>* = nullptr*/>
    wrapper(Arg &&arg) : data(std::forward<Arg>(arg)) {}
};

int main() {
    [[maybe_unused]] std::ofstream f1("a.out");
    [[maybe_unused]] wrapper<std::ofstream> f2("b.out");

    [[maybe_unused]] int i1 = 10;
    [[maybe_unused]] wrapper<int> i2 = 20;

//    [[maybe_unused]] std::string s1 = 100;
//    [[maybe_unused]] wrapper<std::string> s2 = 200;

    static_assert(!std::is_constructible_v<std::string, int>);
    static_assert(!std::is_constructible_v<wrapper<std::string>, int>);  // oops
}
```

### трюк со специализациями класса

чтобы в спеиализациях делать sfinae нам надо подготовиться заранее. Можем его делать только справа от слова struct, не слева

```c++
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

// See https://github.com/emscripten-core/emscripten/pull/9089

template<typename T, typename /* Dummy */ = void> struct serialization_traits {
    static void serialize(std::ostream &os, const T &x) {      x.serialize(os); }
    static T deserialize(std::istream &is)              { T x; x.deserialize(is); return x; }
};

// Specialize for serialization_traits<T, void> iff std::is_trivially_copyable_v<T>, SFINAE otherwise.
template<typename T> struct serialization_traits<T, std::enable_if_t<std::is_trivially_copyable_v<T>>> {
    static void serialize(std::ostream &os, const T &x) { os.write(reinterpret_cast<const char*>(&x), sizeof x); }
    static T deserialize(std::istream &is)              { T x; is.read(reinterpret_cast<char*>(&x), sizeof x); return x; }
};

int main() {
    std::stringstream s;
    serialization_traits<std::size_t>::serialize(s, 10U);
    serialization_traits<int>::serialize(s, 20);
    assert(serialization_traits<std::size_t>::deserialize(s) == 10U);
    assert(serialization_traits<int>::deserialize(s) == 20);

    // serialization_traits<std::string>::serialize(s, "hi");  // compilation error: requests 'serialize' member
}
```


### когда sfinae работает, а когда нет?

hard compilation error

в каждом из следующих случаев ошибка вычисления переменной, а не подстановки

тут ошибка компиляции GetBotva\<int\> - нет такой структуры

```c++
#include <iostream>

struct BotvaHolder {
    using botva = int;
};

template<typename T>
struct GetBotva {
    using type = typename T::botva;  // hard compilation error
};

template<typename T>
void foo(T, typename GetBotva<T>::type) {  // Compilation error: the substitution failure is inside GetBotva<T>
    std::cout << "1\n";
}

template<typename T>
void foo(T, std::nullptr_t) {
    std::cout << "2\n";
}

int main() {
    foo(BotvaHolder(), 10);  // 1
    foo(BotvaHolder(), nullptr);  // 2
    foo(10, nullptr); // 2???
    // foo(10, 10);  // CE
}
```


```c++
#include <iostream>

// Extra reading: https://www.cppstories.com/2022/sfinea-immediate-context/

struct BotvaHolder {
    static constexpr int botva = 10;
};

template<typename T>
constexpr int GetBotva = T::botva;  // hard compilation error

template<int> struct Foo {};

template<typename T>
void foo(T, Foo<GetBotva<T>>) {  // Compilation error: the substitution failure is inside GetBotva<T>
    std::cout << "1\n";
}

template<typename T>
void foo(T, std::nullptr_t) {
    std::cout << "2\n";
}

int main() {
    foo(BotvaHolder(), Foo<10>{});  // 1
    foo(BotvaHolder(), nullptr);  // 2
    foo(10, nullptr); // 2???
    // foo(10, Foo{});  // CE
}
```


SFINAE всё

___

## конвенции метапроги

traits - там лежит куча всяких полезностей

```c++
#include <cassert>
#include <cstddef>

// Convention since C++98: a "function" from a type/value to types/values/functions.
// Not like Rust traits, probably not "типаж".

// Similar to std::iterator_traits
template<typename T>
struct iterator_traits {  // General case (may be absent)
    using value_type = typename T::value_type;
    using difference_type = typename T::difference_type;

    // Not in std::iterator_traits, useless
    static constexpr int modern_magic = 3;  // Modern, can also be `const`
    enum { old_magic = 3 }; // For older buggy compilers: https://stackoverflow.com/a/205000/767632
};

template<typename T>
struct iterator_traits<T*> {  // Corner case, can be specialized
    using value_type = T;
    using difference_type = std::ptrdiff_t;

    static constexpr int modern_magic = 2;
};

template<typename It>
void foo(It begin, It end) {
    [[maybe_unused]] typename iterator_traits<It>::difference_type n = end - begin;
    [[maybe_unused]] typename iterator_traits<It>::value_type first = *begin;
    // Since C++11: `auto` is very helpful
    auto second = *(begin + 1);
}

template<typename It>
struct my_iterator_wrapper {
    typename iterator_traits<It>::value_type v;  // No `auto` fields yet.
};

int main() {
}
```

есть еще char traits - проверяет на равенстсво char??

```c++
#include <cctypes>

template<typename T>
struct char_traits {
    static bool eq(T a, T b) {
        return a == b;
    }
};

struct CaseInsensitiveChar { char c; }

template<>
struct char_traits<CaseInsensitiveChar> {
    static bool eq(CaseInsensitiveChar a, CaseInsensitiveChar b) {
        return tolower(a.c) == tolower(b.c);
    }
};

// Can also add static methods (`char_traits::eq`, `allocator_traits::allocate`) or static fields
// See https://stackoverflow.com/a/5319855/767632 for char_traits, but I've never seen it
// Typically no non-static methods.
```

## constexpr

константа на этапе компиляции

в районе функций - проверим, что эту функцию можно вызвать на этапе компиляции.


constexpr переменная - константа и вычисленная на этапе компиляции

```c++
#include <iostream>

int readInt() {
    int v;
    std::cin >> v;
    return v;
};

constexpr int sum(int a, int b) {  // is allowed to be called both in compile and run time.
    return a + b;  // Valid C++11, since C++14: more operations
}

int sum_non_constexpr(int a, int b) {
    return a + b;
}

const int N = 10;

const int X = readInt();
// constexpr int X1 = readInt();  // on a variable: forces compile-time const

const int Y = sum(2, 2);
constexpr int Y1 = sum(2, 2);  // on a variable: forces compile-time const

int arr1[N];
// int arr2[X];
int arr3[Y];
int arr4[Y1];
// int arr5[sum_non_constexpr(2, 2)];

template<int> struct Foo {};
Foo<N> foo1;
// Foo<X> foo2;
Foo<Y> foo3;
Foo<Y1> foo4;
// Foo<sum_non_constexpr(2, 2)> foo5;

static_assert(N == 10);
// static_assert(X == 123);
static_assert(Y == 4);
static_assert(Y1 == 4);
static_assert(sum(3, 4) == 7);

int main() {
    std::cout << sum(readInt(), 10) << "\n";
}
```

### constexpr struct constructor

можно создавать объект и во время выполнения и во время компиляции
constexpr становятся способы создать структуру!!

```c++
#include <iostream>

struct my_pair {
    int a, b;
    constexpr int sum() const { return a + b; }
};
static_assert(my_pair{1, 2}.sum() == 3);

struct my_pair_2 {
    int a, b;
    my_pair_2() {
        std::cin >> a >> b;
    }
    constexpr my_pair_2(int a_, int b_) : a(a_), b(b_) {}  // both run-time and compile-time
    constexpr int sum() const { return a + b; }
};
static_assert(my_pair_2{1, 2}.sum() == 3);

template<int x> struct Foo {};
Foo<my_pair_2(1, 2).sum()> f;
// Foo<my_pair_2().sum()> g;

int main() {
    my_pair_2 x;
    std::cout << x.sum() << "\n";
}
```

### парсинг выражений с помощью constexpr

```c++
#include <array>
#include <stdexcept>
#include <utility>

template<size_t N>  // Лучше не const char*, чтобы сразу знать размер.
constexpr auto parse_format(const char (&s)[N]) {
    int specifiers = 0;
    std::array<char, N> found{};
    for (size_t i = 0; i < N; i++) {  // strlen не constexpr
        if (s[i] == '%') {
            if (i + 1 >= N)
                throw std::logic_error("Expected specifier after %");
            i++;
            found[specifiers++] = s[i];
            if (!(s[i] == 'd' || s[i] == 'c' || s[i] == 's'))
                throw std::logic_error("Unknown specifier");
        }
    }
    return std::pair{specifiers, found};
}
static_assert(parse_format("hello%d=%s").first == 2);
static_assert(parse_format("hello%d=%s").second[0] == 'd');
static_assert(parse_format("hello%d=%s").second[1] == 's');
static_assert(parse_format("hello%d=%").second[1] == 's');

```

# Лекция 16

### когда sfinae полезно, даже если у нас одна функция и нам не надо как то хитро её выбирать

- сокращать ошибки компиляции

```c++
#include <algorithm>
#include <vector>

struct Foo {
};

template<typename It>
auto my_sort(It begin, It end) -> decltype(*begin < *end, void()) {
    std::sort(begin, end);
}

int main() {
    std::vector<Foo> v;
    std::sort(v.begin(), v.end());  // long compilation error
    my_sort(v.begin(), v.end());  // short compilation error
}
```

### member detection

- можно понять компилируется код или нет и в зависимости от этого вернуть true или faulse

проверяем, есть ли у структурки метод foo

... - вариадические аргументы из си, у функции с таким параметром жутко низкий приоритет перегрузки - она выберется только если ничего другое не подошло

```c++
#include <iostream>
#include <type_traits>

// https://people.eecs.berkeley.edu/~brock/blog/detection_idiom.php

struct Foo {
    void foo() {}
};

struct Bar {
};

namespace detect_foo {
// What if 'const T &'?
template<typename T>
auto impl(T &x) -> decltype(x.foo(), std::true_type{});
std::false_type impl(...);  // C-style variadic argument (ellipsis) has the lowest priority in overload resolution

template<typename T>
constexpr bool has_foo = decltype(impl(std::declval<T&>()))::value;

// In C++03:
struct yes { char x[2]; };
struct no { char x[1]; };
// decltype() --> sizeof(impl(......)) == sizeof(yes)
// declval() --> manual, no implementation
};

int main() {
    std::cout << detect_foo::has_foo<Foo> << "\n";
    std::cout << detect_foo::has_foo<Bar> << "\n";
}
```


### boost-hana

boost::hana::is_valid - получает функцию и аргументы, проверяет что мы можем вызвать данную функцию от данных аргументов - можно туда лямбду передать

## Идеи о том как писать код

type-list 

может быть удобно хранит вспомогательную структуру, необходимую только для того, чтобы хранить внутри себя некий список типов - например если хотим производить вычисление над списком типов 

можем потом еще преобразовать наш type_list в tuple (и не только в tuple)

```c++
#include <type_traits>

template<typename...> struct type_list {};

template<typename .../*Ts*/> struct tail {};
template<typename Head, typename ...Tail>
struct tail<Head, Tail...> {
    using type = type_list<Tail...>;
};
template<typename ...Ts> using tail_t = typename tail<Ts...>::type;

// More generic solution
// Base case, unused
template<template<typename...> typename /*F*/, typename /*TypeList*/>
struct apply_type_list;

// Specialization
template<template<typename...> typename F, typename ...Ts>
struct apply_type_list<F, type_list<Ts...>> {
    using type = F<Ts...>;
};
static_assert(std::is_same_v<
    typename apply_type_list<tuple, type_list<int, string>>::type,
    tuple<int, string>
);
```


### function-traits

-принимает тип указатель на функцию и разбирает его на составляющие - узнает сколько аргументов, узнает возвращаемый аргумент и список аргументов (type_list)



мета +- всё


___

## возвращаемый auto 


- как компилятор узнает возвращаемый тип в таком случае? 
находит первый return, смотрит что делает первый ретурн, начинает считать что мы возвращаем этот тип, далее проверяет все остальные return функции и если всё совпадает то всё супер, иначе ошибка компиляции

ему формально нужно чтобы все return возвращали один и тот же тип!!!(даже если return не достигается)

```c++
// Since C++14
auto fib(int n) {
    if (n <= 1) {
        return 1;  // first return, deduced int
    } else {
        return fib(n - 1) + fib(n - 2);  // ensured: it is int
    }
}

auto fib_bad(int n) {
    if (n > 1) {
        return fib_bad(n - 1) + fib_bad(n - 2);  // first return, cannot deduce
    } else {
        return 1;
    }
}

auto inconsistent_deduction() {
    return 1;
    return 'x';  // 'int' and 'char' can be implicitly converted, but are not the same type
}

struct Foo {};

auto returning_initializer_list() {
    return Foo();
    return {};  // still compilation error
}

void do_something() {}

auto return_void() {
    return do_something();  // ok, returning void, but should not be `auto&`
}
```

у лямбд принцип аналогично 

у лямбд через стрелочку можно явно указать возвращаемый тип

```c++
  auto g = [](int n) -> double {
        if (n > 1) {
           return 100;
        } else {
           return 200.0;
        }
        // return "foo";  // compilation error: cannot cast to double
    };
```

есть еще несколько случаев когда удобнее указывать тип через стрелочку справа у простых функций (дабы сократить код и не писать какие-то сложные конструкции)

```c++
// "Trailing return type", syntax sugar
auto fib(int n) -> int {
    if (n > 1) {
        return fib(n - 1) + fib(n - 2);  // first return, cannot deduce, but it's ok
    } else {
        return 1;
    }
}

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

int main() {
    my_set<int> s;
    [[maybe_unused]] auto b = s.begin();
    [[maybe_unused]] auto e = s.end();
}
```


- auto можно писать даже в объявлениях (если в одной единице трансляции, между единицами трансляции не получится)

## условный noexcept

как добавить к функции noexcept в зависимости от передаваемого типа 

noexcept(...) - условный noexcept который помечает функцию как noexcept только в том случае, если то что в скобочках вычилилось как true

```c++
#include <cstddef>
#include <memory>
#include <optional>
#include <iostream>
#include <string>
#include <type_traits>

template<typename T>
struct vector {
    T *data;
    std::size_t len = 0;

    vector() = default;
    vector(const vector &) = default;
    vector(vector &&) = default;

    vector &operator=(vector &&) = default;
    vector &operator=(const vector &other) {
        if (this == &other) {
            return *this;
        }
        // NOTE: two separate algorithms for providing a strong exception safety
        if constexpr (std::is_nothrow_copy_assignable_v<T>) {
            // Never throws (like in lab10)
            std::cout << "naive copy assignment\n";
            for (std::size_t i = 0; i < len && i < other.len; i++) {
                data[i] = other.data[i];
            }
            // ...
        } else {
            std::cout << "creating a new buffer\n";
            // May actually throw, cannot override inidividual elements, should allocate a new buffer.
            *this = vector(other);
        }
        return *this;
    }
};

template<typename T>
struct optional {
    alignas(T) char bytes[sizeof(T)];
    bool exists = false;

    optional() = default;
    ~optional() { reset(); }

    T &data() { return reinterpret_cast<T&>(bytes); }
    const T &data() const { return reinterpret_cast<const T&>(bytes); }

    void reset() {
        if (exists) {
            data().~T();
            exists = false;
        }
    }
    optional &operator=(const optional &other)
        // default is bad, optional<int> is not nothrow-copyable
        // noexcept  // optional<std::string> is not nothrow-copyable
        noexcept(std::is_nothrow_copy_constructible_v<T>)  // ok, conditional noexcept-qualifier
    {
        if (this == &other) {
            return *this;
        }
        reset();
        if (other.exists) {
            new (bytes) T(other.data());
            exists = true;
        }
        return *this;
    }
};

int main() {
    {
        vector<optional<int>> v1, v2;
        v1 = v2;  // naive copy assignment
    }
    {
        vector<optional<std::string>> v1, v2;
        v1 = v2;  // creating a new buffer
    }
    // can uncomment to test during compilation
    static_assert(std::is_nothrow_copy_assignable_v<optional<int>>);
    static_assert(!std::is_nothrow_copy_assignable_v<optional<std::string>>);
}
```

## noexcept operator 

проверяет, почемена ли операция noexcept 

```c++
#include <cassert>
#include <vector>

int foo() noexcept { return 1; }
int bar()          { return 2; }
std::vector<int> get_vec() noexcept { return {}; }

int main() {
    int a = 10;
    std::vector<int> b;
    // Simple cases
    static_assert(noexcept(a == 10));
    static_assert(!noexcept(new int{}));   // no leak: not computed
    static_assert(noexcept(a == foo()));
    static_assert(!noexcept(b == b));      // vector::operator== is not noexcept for some reason, but I don't know how it can fail
    bool x = noexcept(a == 20);
    assert(x);

    // Complex expressions
    static_assert(!noexcept(a == bar()));  // bar() is not noexcept
    static_assert(noexcept(get_vec()));  // noexcept even though copying vector may throw: return value creation is considered "inside" function
    static_assert(noexcept(b = get_vec()));  // operator=(vector&&) does not throw
    static_assert(!noexcept(b = b));  // operator=(const vector&) may throw
}
```

можем потом миксовать noexcept(noexcept(...)) - миксуем оператор noexcept и условный noexcept

## declval детально 

- можно настраивать чтобы он возвращал rvalue xvalue lvalue
если пытаемся использовать declval вне контекста, в котором код не вызывается, то ошибка компиляции

```c++
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

template<typename T>
constexpr bool is_nothrow_copy_assignable_1 = noexcept(T() = static_cast<const T&>(T()));

struct Foo {
    Foo() {}
    Foo &operator=(const Foo &) noexcept {
        return *this;
    }
};

static_assert(is_nothrow_copy_assignable_1<std::optional<int>>);
static_assert(!is_nothrow_copy_assignable_1<std::vector<int>>);
// static_assert(is_nothrow_copy_assignable_1<Foo>);  // static assertion failed: constructor is not noexcept
// static_assert(is_nothrow_copy_assignable_1<std::runtime_error>);  // compilation error: no default constructor
// static_assert(is_nothrow_copy_assignable_1<int>);  // compilation error: cannot assign to a temporary int

template<typename T>
constexpr bool is_nothrow_copy_assignable_2 = noexcept(std::declval<T&>() = std::declval<const T&>());

static_assert(is_nothrow_copy_assignable_2<std::optional<int>>);
static_assert(!is_nothrow_copy_assignable_2<std::vector<int>>);
static_assert(is_nothrow_copy_assignable_2<Foo>);  // ok
static_assert(is_nothrow_copy_assignable_2<int>);  // ok
static_assert(is_nothrow_copy_assignable_2<std::runtime_error>);  // ok

int main() {
    // [[maybe_unused]] int x = std::declval<int>();  // compilation error: std::declval<> is only used in unevaluated context
}
```

## странные типы

### initializer_list

так у вектора сделан конструктор от различных элементов - просто через initializer list от этих аргументов

от unique_ptr не работает, так как хранит внутри себя константные элементы, поэтому из initializer list нельзя замувать

```c++
#include <cassert>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <vector>

struct Foo {
    int val;
    Foo(int x, int y) : val(10 * x + y) {
    }
};

template<typename> struct TD;

struct Bar {
    Bar(std::initializer_list<int> x) {
        std::cout << x.size() << " " << *x.begin() << "\n";
    }
    Bar(std::initializer_list<Foo> x) {
        std::cout << x.size() << " " << x.begin()->val << "\n";
    }
    Bar([[maybe_unused]] std::initializer_list<std::unique_ptr<int>> x) {
        // TD<decltype(*x.begin())>{};
        // std::unique_ptr<int> y = std::move(*x.begin());  // Oops :(
    }
};

void demo_bar() {
    Bar{1, 2, 3, 4};  // Bar(std::initializer_list<int>{1, 2, 3, 4});
    Bar{{1, 2}, {3, 4}, {5, 6}};  // Bar(std::initializer_list<Foo>{Foo(1, 2), Foo(3, 4), Foo(5, 6)});
}

// Second part: recursion

struct Baz {
    int val;
    std::vector<Baz> children;

    Baz(int val_) : val(val_) {
    }
    Baz(std::initializer_list<Baz> children_)
        : val(-1), children(children_.begin(), children_.end()) {
    }

    void print() const {
        if (val >= 0) {
            assert(children.empty());
            std::cout << val;
        } else {
            std::cout << "{";
            for (const auto &c : children)
                c.print();
            std::cout << "}";
        }
    }
};

int main() {
    demo_bar();
    Baz{
        1,             //
        2,             //
        {3, 4},        //
        {{5}},         //
        {{6}, 7, {}},  //
        8              //
    }
        .print();
    std::cout << "\n";
}
```

### перегруженный operator&()

### function-type

```c++
#include <functional>

void func1();
void func2(int x);
void func3(int y);
int func4(int x, char y);

template<typename> struct TD;
TD<decltype(func1)> a; // TD<void()>
TD<decltype(func2)> b; // TD<void(int)>
TD<decltype(func3)> c; // TD<void(int)>
TD<decltype(func4)> d; // TD<int(int, char)>

decltype(func1) x;  // TODO: can create a variable of this type?!!
std::function<void(int)> y;  // frequently used as a template parameter

using func1_ptr = void(*)();
func1_ptr z = func1;  // implicitly convertible to a pointer
```

### указатель на член класса

- что то типа смещение в типе...
работает с наследование

```c++
#include <cassert>
#include <functional>
#include <memory>

// https://isocpp.org/wiki/faq/pointers-to-members

struct S { int x = 1, y = 2; };
struct S2 : S { int z = 3; };

int main() {
    S s;
    S *sptr = &s;

    int S::*a = &S::x;  // ok, remember 'a' is '0 bytes after S start'
    int (S::*b) = &S::y;  // ok, unnecessary parens
    // int S::*с = &(S::y);  // compilation error: &S::y is a whole

    assert(s.*a == 1);  // access field of name '*a'
    assert(sptr->*a == 1);
    // but actually it's a separate operator: .* ->*

    a = b;  // Now `a` "points" to a field `y` of arbitrary `S`.
    assert(s.*a == 2);
    assert(sptr->*a == 2);

    // void *wtf = (void*)a;  // invalid cast! It is not a pointer to memory, it is an offset

    // To simplify the syntax
    using IntMemberOfS = int S::*;
    [[maybe_unused]] IntMemberOfS c = &S::x;

    // Or a standard library
    std::invoke(a, s) = 3;  // s.*a = 3;
    assert(std::invoke(a, s) == 3);

    // Works with inheritance
    S2 s2;
    assert(s2.*a == 3);  // You can use pointer to a base member
    int S2::*d = a;  // There are implicit conversions
    assert(s2.*d == 3);
    [[maybe_unused]] IntMemberOfS e = static_cast<IntMemberOfS>(d);  // There are explicit conversions

    std::unique_ptr<S> p(new S);
    assert(std::invoke(a, *p) == 2);
    assert((*p).*a == 2);
    // assert(p->*a == 2);  // compilation error, not overloaded (they forgot?)

    // Works with virtual inheritance
}

template<typename, auto...> struct StructSaver {};
StructSaver<S, &S::x, &S::y> saver;

// private, protected: kinda ignored
```

тоже самое с методами...

только тут еще какие-то приколы с константностью, примерно как и раньше были проблемы в си

возникают редко, в основном для мета программировании

```c++
#include <cassert>
#include <functional>

struct S {
    int field = 100;
    int foo1(int x) { return x + 10 + field; }
    int foo2(int x) { return x + 20 + field; }
    int bar1(int x) const { return x + 30 + field; }
    int bar2(int x) const { return x + 40 + field; }
};

int main() {
    S s;
    int (S::*a)(int) = &S::foo1;
    assert((s.*a)(9) == 119);  // Parens are .* are necessary!
    a = &S::foo2;
    assert((s.*a)(8) == 128);
    assert(std::invoke(a, s, 7) == 127);

    // Be careful with const qualifiers: https://isocpp.org/wiki/faq/pointers-to-members#memfnptr-to-const-memfn
    int (S::*b)(int) const = &S::bar1;  
    assert((s.*b)(6) == 136);
    b = &S::bar2;
    assert((s.*b)(5) == 145);
    assert(std::invoke(b, s, 4) == 144);

    // ->* works as well

    // Works with virtual functions, virtual inheritance => may need multiple pointers and be big, incompatible with function pointers.
    // Be careful with overloads, just like in function pointers
}
```


# Лекция 17

### перегрузки подробнее

выбор наиболее подходящей функции - обычно нормально работает, но могут быть странности 

```c++
#include <iostream>
#include <vector>

void println(int x) {
    std::cout << x << "\n";
}

void println(double x) {  // Removing this overload resolves ambiguity
    std::cout << x << "\n";
}

void println(const std::vector<int> &vec) {
    std::cout << "{";
    bool first = true;
    for (const auto &item : vec) {
        if (!first) {
            std::cout << ", ";
        }
        first = false;
        std::cout << item;
    }
    std::cout << "}\n";
}

int main() {
    println(10);
    println(1.2);
    println(std::vector{1, 2, 3});
    println(123456789123456);
    println(1LL);
}
```

- в сишном стиле 0 - нулевой указатель корректный, в с++ из-за совместимости тоже

приколы со смешиванием namespace-ов (пример gcd, lcm)

- можно не указывать имя параметра в функции(например, теги)

```c++
#include <vector>
#include <iostream>

struct comma_separated_tag {};  // Not _t, it's reserved.
struct element_per_line_tag {};  // Not _t, it's reserved.

constexpr comma_separated_tag comma_separated;
constexpr element_per_line_tag element_per_line;

void println(const std::vector<int> &vec, comma_separated_tag) {
    bool first = true;
    for (const auto &item : vec) {
        if (!first) {
            std::cout << ", ";
        }
        first = false;
        std::cout << item;
    }
    std::cout << "\n";
}

void println(const std::vector<int> &vec, element_per_line_tag) {
    for (const auto &item : vec) {
        std::cout << item << "\n";
    }
}

int main() {
    std::vector<int> v{1, 2, 3};
    // println_comma_separated(v);
    // println_element_per_line(v);

    // type tags
    println(v, comma_separated_tag{});
    println(v, element_per_line_tag{});
    //
    println(v, comma_separated);
    println(v, element_per_line);
}
```


пример из стандартной библиотеки - std::inplace

- параметры по умолчанию (только последние)

вызов любой функции состоит из четырех шагов:
- разрешение имен(поиск всех функций подходящих по имени) - overload set
- выбирается viable function (подходящие по передаваемым параметрам) - из них выбирается best viable
- проверка доступности к функции 
- вызов функции


```c++
#include <iostream>

void foo(const int&) {
    std::cout << "1\n";
}

void foo(char) {
    std::cout << "2\n";
}

template<typename T>
void foo(T) {
    std::cout << "3\n";
}

template<typename T>
void foo(T&) {
    std::cout << "4\n";
}

int main() {
    foo(10);  // Arg=int. Overload set: 1, 2, 3 (T=int), 4 (T=int)
              // Viable: 1, 2, 3 (T=int)
              // Best viable: 1 (similar conversion with 3, but templates are worse)
    foo('1');  // Arg=char. Overload set: 1, 2, 3 (T=char), 4 (T=char)
               // Viable: 1, 2, 3 (T=char)
               // Best viable: 2 (similar conversion with 3, but templates are worse)
    foo(10.0);  // Arg=double. Overload set: 1, 2, 3 (T=double), 4 (T=double)
                // Viable: 1, 2, 3 (T=double)
                // Best viable: 3 (better argument match: no floating-integral conversions)
    foo<int>(10);  // Arg=int. Requested template with first argument=int
                   // Overload set: 3 (T=int), 4 (T=int)
                   // Viable: 3 (T=int)
                   // Best viable: 3
    foo<>(10);  // Same: T are deduced to `int`.

    [[maybe_unused]] const int x = 10;
    // foo<>(x);  // Arg=const int. Overload set: 3 (T=int), 4 (T=const int)
               // Viable: 3 (T=int), 4 (T=const int)
               // Best viable: ambiguous, similar conversions.
    foo(x);  // Arg=const int. Overload set: 1, 2, 3 (T=int), 4 (T=const int)
             // Viable: 1, 2, 3 (T=int), 4 (T=const int)
             // Best viable: 1 (same level as 3/4, but not a template).
}
```

пример Димова/Абрамса про то почему специализации может быть плохо и как это работает с выбором перегрузок

прикол что если мы перенесем одну строчку под другую, то у нас на удивление поменяется вывод(один и тот же код в зависимости от контекста может специализировать разные шаблоны)


```c++
#include <algorithm>
#include <iostream>

// Function specialization is probably a bad idea: http://www.gotw.ca/publications/mill17.htm
// Use overloads instead.

// Dimov/Abrahams example:
template<typename T> void foo(T) { std::cout << "1\n"; }

// Not overload, specialization of foo(T). Does not participate in overload resolution!
template<> void foo(int *) { std::cout << "2\n"; }  // T = int*

template<typename T> void foo(T*) { std::cout << "3\n"; }

// template<> void foo(int *) { std::cout << "4\n"; }

int main() {
    int x = 10;
    foo(x);  // 1
    foo<int*>(&x);  // 2, two candidates: foo<int*>(int*) and foo<int*>(int**)
    foo<int>(&x);  // 3, two candidates: foo<int*>(int) and foo<int*>(int*)
    foo(&x);  // 3: two candidates: foo(T) and foo(T*), latter is better
}
```


### штуки связанные с классами

- друзья члены другого класса

синтаксис

```c++
struct Bar;
struct Foo {
private:
    void foo_private(Bar *b);
public:
    void foo_public(Bar *b);
    void foo_other(Bar *b);
};

struct Bar {
private:
    int x;
    // friend void Foo::foo_private(Bar *b);  // cannot see Foo::foo_private
    friend void Foo::foo_public(Bar *b);
};

void Foo::foo_public(Bar *b) {
    b->x++;
}

void Foo::foo_other([[maybe_unused]] Bar *b) {
    // b->x++;  // is not a friend
}

int main() {}
```

теперь навесим шаблоны - можем только все всем друзья 

```c++
template<typename T> struct Bar;
template<typename T>
struct Foo {
    template<typename U>
    void foo_public(Bar<U> *b);
};

template<typename U>
struct Bar {
private:
    int x;
    template<typename T>
    template<typename U_>
    friend void Foo<T>::foo_public(Bar<U_> *b);
};

template<typename T>
template<typename U>
void Foo<T>::foo_public(Bar<U> *b) {
    b->x++;
}

int main() {
    Foo<int> f;
    Bar<char> b;
    f.foo_public(&b);
}
// TODO: what about specializations, etc?
```

проблемы с make_unique

```c++
#include <memory>

// https://abseil.io/tips/134

struct Foo {
    static std::unique_ptr<Foo> make() {
        return std::make_unique<Foo>(10);
    }

private:
    Foo(int) {}

    friend std::unique_ptr<Foo> std::make_unique<Foo>(int&&);
    // (1) No guarantee that `make_unique` calls `Foo()` itself :(
};

int main() {
    auto p1 = std::make_unique<Foo>(10);  // (2) Can call arbitrary `make_unique` :(
    auto p2 = Foo::make();
}
```

внутренний класс имеет доступ ко всем полям того класса в котором он был создан


#### is_base_of_v 
- проверит являемся ли мы наследником (игнорирует приватность)

#### noncopyable


#### пример, где компилятор генерирует конструктор копирования, перемещения, но неправильно

какое-то поле ссылается внутрь другого поля


тут разница в поведении из за того что длинные и короткие строчки хранятся по разному 

```c++
#include <cassert>
#include <iostream>
#include <iterator>
#include <list>

struct Foo {
public:
    std::list<std::string> l;
    std::list<std::string>::iterator it;

public:
#if 0
    Foo()
       : l{"hello first very very very very long string", "hello second very very very very long string"}
       , it(std::next(l.begin())) {}
    void check() {
        assert(*it == "hello second very very very very long string");
    }
#else
    Foo() : l{"hello", "world"}, it(std::next(l.begin())) {}
    void check() {
        assert(*it == "world");
        assert(it == std::next(l.begin()));
    }
#endif
};

int main() {
    Foo a;
    std::cout << "checking a" << std::endl;
    a.check();

    {
        Foo b;
        std::cout << "checking b" << std::endl;
        b.check();

        a = b;
    }
    std::cout << "checking a" << std::endl;
    a.check();
}
```

### guaranteed-copy-elision

rvo - гарантия того что при return объект создастся ровно там куда он вовращается (при возврате временного значения) (гарантируется стандартом)


nrvo - возврат переменной - можно создать прям там куда будем его возвращать(не гарантируется стандартом) (требуется мув конструктор)

если std::move делаем - то форсируем мув - так делать не надо

работает только для создания новых объектов

```c++
#include <iostream>

struct Foo {
    Foo() {
        std::cout << "Foo() " << this << "\n";
    }

#if 1
    Foo(Foo &&other) {
        std::cout << "Foo(Foo&&) " << this << " <- " << &other << "\n";
    }

    Foo& operator=(Foo &&other) {
        std::cout << "operator=(Foo&&) " << this << " = " << &other << "\n";
        return *this;
    }
#else
    Foo(Foo &&other) = delete;
    Foo& operator=(Foo &&other) = delete;
#endif

    ~Foo() {
        std::cout << "~Foo() " << this << "\n";
    }
};

// (Almost) unique situation in C++: copy elision is allowed!
// Even if there are side effects of move/copy ctors.
Foo create_foo_rvo() {
    // Since C++17: guaranteed copy elision, no need in Foo(Foo&&)
    // Before C++17: needs Foo(Foo&&), even if never called
    return Foo();  // copy/move from prvalue is elided (RVO, return value optimization)
}

#if 1
Foo create_foo_nrvo() {
    Foo f;
    return f;  // NRVO (named return value optimization), not guaranteed
    // return std::move(f);  // prevents NRVO, no guaranteed copy elision as it's an xvalue
}

Foo create_foo_no_nrvo(int x) {
    Foo f, g;
    // probably no copy elision
    if (x == 0) {
        return f;
    } else {
        return g;
    }
}
#endif

void consume(Foo f) {
    std::cout << "consumed " << &f << "\n";
}

int main() {
    consume(create_foo_rvo());  // copy/move from prvalue is elided
#if 1
    std::cout << "=====\n";
    consume(create_foo_nrvo());  // copy/move from prvalue is elided
    std::cout << "=====\n";
    consume(create_foo_no_nrvo(0));
#endif

    // For constructors only, not assignment operators.
#if 0
    Foo f;
    // .....
    f = create_foo_rvo();  // No RVO
#endif
}
```

сам факт того что мы взяли ссылку на временный объект мы форсировали temporary materialization - заставили компилятов этот объект где то создать, чтобы взять на него ссылку

```c++
#include <iostream>

struct Foo {
    Foo() {
        std::cout << "Foo() " << this << "\n";
    }

    Foo(Foo &&other) {
        std::cout << "Foo(Foo&&) " << this << " <- " << &other << "\n";
    }

    Foo& operator=(Foo &&other) {
        std::cout << "operator=(Foo&&) " << this << " = " << &other << "\n";
        return *this;
    }

    ~Foo() {
        std::cout << "~Foo() " << this << "\n";
    }
};

Foo create_foo_rvo() {
    return Foo();  // copy/move from prvalue is elided (RVO, return value optimization)
}

Foo create_foo_nrvo() {
    Foo f;
    return f;  // NRVO (named return value optimization), not guaranteed
}

Foo &&pass_ref(Foo &&f) {  // Forces "temporary materialization" by binding a reference, now it's xvalue at best
    std::cout << "arg is " << &f << "\n";
    return std::move(f);
}

Foo pass_copy(Foo f) {  // Forces "temporary materialization" by binding a reference inside Foo(Foo&&)
    std::cout << "arg is " << &f << "\n";
    return f;  // no std::move to enable NRVO
}

void consume(Foo f) {
    std::cout << "consumed " << &f << "\n";
}

int main() {
    consume(pass_ref(create_foo_rvo()));
    std::cout << "=====\n";
    consume(pass_copy(create_foo_rvo()));
    std::cout << "=====\n";
    [[maybe_unused]] Foo f = Foo(Foo(Foo(Foo(Foo()))));
    std::cout << "=====\n";
    [[maybe_unused]] Foo g = Foo(Foo(Foo(static_cast<Foo&&>(Foo(Foo(Foo()))))));
}
```

# Лекция 18


## время жизни временных объектов

висячая ссылка - пытаемся к временному объекту привязать ссылку
исключение - константная ссылка продливает время жизни временного объекта

```c++
#include <iostream>
#include <vector>

void println(const std::vector<int> &) {
}

int main() {
    // https://herbsutter.com/2008/01/01/gotw-88-a-candidate-for-the-most-important-const/
    const std::vector<int> &vec = std::vector{1, 2, 3};
    std::cout << vec.size() << "\n";

    println(std::vector{1, 2, 3});
}
```

где есть расширение и где нет:

```c++
#include <iostream>

int id_val(int x) {
    return x;
}

const int &id_ref(const int &x) {
    return x;
}

int main() {
    // Rvalue lifetime disaster, Arno Schoedl.
    {
        const int &x = id_val(10);
        std::cout << x << "\n";
    }
    {
        const int &x = id_ref(10);
        std::cout << x << "\n";
    }
}
```

частный случай из стандартной библиотеки - std::min

### range-based-for 

итерируемся по удаленному вектору - ссылка на какой-то из предыдущих временных объектов

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
    for (auto x : foo()) {
        std::cout << x << "\n";
    }
    std::cout << "=====\n";

    const VectorWrapper &vw = bar();
    for (auto x : vw.get()) {
        std::cout << x << "\n";
    }
    std::cout << "=====\n";

    // May be fixed in C++20/C++23/C++26, maybe not.
    for (auto x : bar().get()) {
        std::cout << x << "\n";
    }
}
```


## namespaces

name lookup 

foo() - unqualified name lookup 
ns1::foo() - qualified name lookup

как происходит unqualified name lookup 

сначала ищем внутр нашего нэймспэйса, а дальше поднимаемся наверх

::foo() - гарантированно в глобольном нэймспэйсе

```c++
#include <iostream>

void foo() {
    std::cout << "foo global\n";
}

void some_global() {
    std::cout << "some_global\n";
}

namespace ns1 {
void bar() {
    std::cout << "ns1::bar()\n";
}

namespace ns2 {  // ns1::ns2
void bar() {
    std::cout << "ns1::ns2::bar\n";
}
void botva_ns2() {
    std::cout << "ns1::ns2::botva_ns2()\n";
}
}  // namespace ns2

namespace ns3 {  // ns1::ns3
void botva_ns3() {
    some_global();  // ok
    ns2::botva_ns2();  // ok, find 'ns2' first
    // botva_ns2();  // only looks up in ns1::ns3, ns1::, ::, not ns1::ns2
    std::cout << "ns1::ns3::botva_ns3()\n";
}
}  // namespace ns3

namespace ns3::ns4 {
namespace ns1 {  // ns1::ns3::ns4::ns1
}

void baz() {  // ns1::ns3::ns4
    botva_ns3();  // unqualified name lookup for 'botva_ns3'
    ns2::bar();  // unqualified name lookup for 'ns2', qualified name lookup bar()

    foo();  // ok, global
    // ns2::foo();  // compilation error: no 'foo' inside 'ns2'

    ::ns1::ns2::bar();  // fully qualified lookup
    // ns1::ns2::bar();  // thinks that 'ns1' is 'ns1::ns3::ns4::ns1', 'n2' not found
}
}  // namespace ns3::ns4
}  // namespace ns1

int main() {
    ns1::ns3::ns4::baz();
    ::ns1::ns3::ns4::baz();

    ::ns1::ns2::bar();  // ok
    // ns1::ns2::bar();  // ok here, but not in ns1::ns3::ns4::baz()
}
```

внутренние namespace автоматически не смотрятся и не проверятся!!


## ADL

если мы вызываем функцию и у нас не квалифицированный поиск, то мы ищем не только в текущем нэймспейсе и его родителях, но и в неймспейсах, которые имеют отношение к нашим аргументам


```c++
// Argument-Dependent Lookup aka Koenig Lookup
#include <algorithm>
#include <vector>

namespace ns_parent {
namespace ns {
struct Foo {};

void do_something() {}
void do_something(Foo) {}
bool operator==(const Foo&, const Foo&) { return true; }
};

void do_other(ns::Foo) {}
}

int main() {
    // do_something();
    ns_parent::ns::do_something();

    // Foo f;
    ns_parent::ns::Foo f;

    f == f;
    operator==(f, f);  // ok: unqualified lookup looks in argument's type's namespaces as well (ns_parent::ns)
    ns_parent::ns::operator==(f, f);

    ns_parent::ns::do_something(f);  // ok
    do_something(f);  // unqualified name lookup, ADL enabled

    ns_parent::do_other(f);  // ok
    // do_other(f);  // do_other is in another namespace, no ADL
}
```


можем добавлять нэймспэйсы (до ближайшей фигурной скобки)

using namespace ...

## названия в с++

ограничения на имена 
- нельзя называть как ключевые слова
- нельзя начинаться с цифры
- нельзя чтобы глобальная начиналась с _
- нельзя __ нигде
- нельзя _Х..

это всё UB. чтобы компилятор мог сам спокойно использовать такие переменные и не бояться что вы сами там себе что-то такое задефайнили. А если задефайнили то вы сами виноваты ловите UB


еще нельзя чтобы имя типа оканчивалось на _t - формат POSIX против (что-то для поведения на самом линуксе)


## the most vexing parse

() - неоднозначно может как функцию воспринимать
работает как с полями, так и с локальными переменными 

std::vector\<int\> ha() - очень похоже на объявление функции!!

приколы с этим связанные:

```c++
struct Bar {
    explicit Bar() {}
};
struct Foo {
    explicit Foo(Bar) {}
};

struct Botva {
    Foo f(Bar());  // oops, function

    void foo() {
        f = f;
    }
};

int main() {
    Foo f(Bar());  // the most vexing parse: Foo f(Bar (*arg) ())
    // Foo f{Bar()};
    // Foo f(Bar{});
    // Foo f{Bar{}};
    // Foo f((Bar()));  // C++03 and before
    // Foo f(   (Bar())  );  // C++03 and before
    f = f;
}

// Foo f( (int x) ) {}
```


## hiding 
- когда есть методы и поля с одинаковым именем в базовом классе и в наследнике
влияет только на синтаксис, не на доступ

```c++
#include <iostream>

struct Base {
    int f = 10;
    void foo() {
        std::cout << "Base\n";
    }
};

struct Derived : Base {
    int f = 20;   // It 'hides' `f` from `Base`
    void foo() {  // It 'hides' `foo` from `Base`
        Base::foo();
        std::cout << "Derived " << f << " " << Base::f << "\n";
    }
};

struct SubDerived : Derived {};

int main() {
    SubDerived sd;
    sd.foo();
    std::cout << sd.f << "\n";
    std::cout << sd.Derived::f << "\n";
    std::cout << sd.SubDerived::f << "\n";  // Derived::f

    // This syntax is for naming only, it does not alter access restrictions
    // (public/protected/private).
    sd.Base::foo();
    std::cout << sd.f << " " << sd.Base::f << "\n";
}
```

перегрузка в наследнике полностью перекрывает всё функции с таким же именем в базовом классе (даже если аргументы были другие)

как чинить? 
- продублировать
- использовать using: using Base::foo


#### с помощью using можно сменить доступ

то есть не сам метод public private protected, а имя метода

```c++
struct Base {
protected:
    void magic() {
    }  // Similarly for fields.
};

struct Derived : Base {
    using Base::magic;
};

struct SubDerived : Derived {
    void magic2() {
        magic();
        Base::magic();
        Derived::magic();
    }

private:
    using Derived::magic;
};

int main() {
    [[maybe_unused]] Base b;
    // b.magic();

    Derived d;
    d.magic();
    // d.Base::magic();

    SubDerived sd;
    // sd.magic();
    // sd.Base::magic();
    sd.Derived::magic();
    sd.magic2();
}

struct SubSubDerived : SubDerived {
    // using Base::magic;  // TODO: not sure why, g++ and clang++ disagree.
    // using Derived::magic;  // TODO: not sure why.
    // using SubDerived::magic;
    void magic3() {
        Base::magic();  // TODO: not sure why.
        Derived::magic();  // TODO: not sure why.
        // SubDerived::magic();
        // magic();  // SubDerived::magic()
    }
};
```








