## Билет 33. Исключения - необычная обработка

### Непойманные исключения и раскрутка стека

_Непойманные исключения_ аварийно завершают программу, при это возможно такое, что программа поймет, что исключение никто не поймает, и завершится без раскрутки стека(тут implementation-defined).

Решение: ловить их (например, оборачивать main в try-catch блок). 

Например вот этот код ничего может ничего не вывести(как у меня локально), а может раскрутить стек и вызвать деструктор
```c++
struct sd {
    ~sd() { cout << "finally"; }
};

int main() {
    sd var;
    throw int();
}
```

_Раскрутка стека_:

Как только исключение бросается, оно летит по строчкам вверх (а не вниз, как каждая уважающая себя программа), и уничтожает всё, что встречается ему на пути (вызываются деструкторы).
Когда оно доходит до первого блока try, оно прекращает лететь вверх и идёт к блокам catch.
Там исключение ищет себе подходящий вход. Когда оно его находит - начинается обработка этого исключения. Внутри блока catch считается, что исключения уже нет.

**WARNING** Поскольку во время размотки стека вызываются деструкторы, то всякие указатели не освободят свои ресурсы (не используйте указатели, ага)


### catch (...)

Специальный синтаксис, чтобы поймать любое исключение. (а что еще сказать)

Ну вот примерчик:

```c++
struct err1 {};
struct err2 {};

int main() {
    try {
        // throw err1();
        // throw err2();
        std::vector<int> vec(-1);
    } catch (const err1 &) {
        std::cout << "1\n";
    } catch (const err2 &) {
        std::cout << "2\n";
    } catch (...) {  // Exceptions only. Not UB, not assertions!
        std::cout << "3\n";
        // throw;  // rethrow current exception
    }  // no 'finally' block!
}
```

`final` как в других языках нет(штука которая вызывается всегда после try-block)


### Перебрасывание текущего исключения при помощи throw, возможный slicing

(~50 минута 30я лекция, файлик `30-220606/03-more-exceptions/01-rethrow-slicing.cpp`)

Можно кинуть пойманное исключение(или любое другое) внутри catch. Используйте просто `throw;` чтобы бросить то, что поймали(иначе slicing, тк у нас тип принимается строгий и теряется вся информация про предыдущий тип при касте)

```c++
void foo() {
    try {
        throw std::runtime_error("Hello World");
    } catch (std::exception &a) {
        // throw a;  // oops, slicing
        throw;  // no slicing, бросим тоже самое, что поймали
    }
}

int main() {
    try {
        foo();
    } catch (std::exception &e) {
        std::cout << "caught in main(): " << e.what() << "\n";
    }
}
```

### exception_ptr, его создание/перебрасывание/хранение, пустое состояние 
(~55 минута 20 лекция; Файлик `20-220221/03-threads-misc/03-exception-ptr.cpp`)

Концептуально в одном потоке мы хотим получить исключение, а потом выкинуть и обработать в другом

(дальше текст по желанию, все это написанно в коде с комментами)

Способы создать: `std::make_exception_ptr`(по факту просто make_shared)  или `std::current_exception` - вернет exception_ptr на икслючение в текущем catch или `exception_ptr{}`, если мы не внутри catch

Узнать тип или сделать что-то разумное с `exception_ptr` у нас нет, можем только `rethrow_exception()`(оно хочет не пустое исключение), а дальше ловить и обрабатывать как обычно

```c++
struct my_exception {
    int value;
};

void foo(int x) {
    throw my_exception{x + 10};
}

int main() {
    auto f = std::async([]() { // запускает в отдельном потоке, возвращает тип future(смотри потоки)
        foo(0);
    });

    try {
        f.get(); // вернет то, что вернула лямбда или выкинет исключение, если оно было
    } catch (my_exception &e) {
        std::cout << "e=" << e.value << "\n";
    }

    // хотим понять как сделано такое c async, можно через:
    
    std::exception_ptr err;  // shared_ptr<exception>
    // Can be created using std::make_exception_ptr or std::current_exception.
    auto save_exception = [&]() {
        // Can be called at any point in the program, not necessarily right inside `catch`.
        // If there is no "current" exception, returns `exception_ptr{}`.
        err = std::current_exception();
    };

    try {
        foo(1);
    } catch (...) {
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

### Function-try-block у конструкторов, деструкторов, методов и функций

(~51 минута 30 лекция; Файлик `30-220606/03-more-exceptions`)
(ответы на подпункты билета смотри в конце этого билета)

_Function-try-block-сtor_ : Можно написать `try` перед `member initialization list` и `catch` сразу после. Нужно чтобы поймать исключение из самого листа и инициализации базовых классов(даже если явно их не инициализируем)

К сожалению, если поймали, то не знаем откуда и сделать мало что можем, тк поля там уже сдохли

```c++
struct Base {
    Base() {
        throw 0;
    }
};

struct Foo : Base {
    std::vector<int> a, b;

    Foo(const std::vector<int> &a_, const std::vector<int> &b_)
    try
        : a(a_)
        , b(b_)
    {
        std::cout << "constructor called\n"; // тело конструктора
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

int main() {
    try {
        Foo f({}, {});
    } catch (int x) {
        std::cout << "exception " << x << "\n";
    }
}
```

_Function-try-block-dtor_ : Аналогично, но надо помнить что деструктор по умолчанию `noexcept`, поэтому надо этот noexcept убрать(ну или что мы ловить собрались)

(в дикой природе не водится, но абсолютно легально)
```c++
struct Base {
    ~Base() noexcept(false) { // делаем деструктор не noexcept
        throw 0;
    }
};

struct Foo : Base {
    ~Foo() try {
        std::cout << "destructing Foo\n"; // тело деструктора
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
    } catch (...) {
        std::cout << "exception\n";
    }
}
```

_Function-try-block-func_ : Можно и на функции навесить, будет висеть просто на **теле**, но довольно бесполезно, тк создание аргументов надо ловить при вызове функции(тут не сможем), а проблема в момент `return` может быть поймана более простым try-block вокруг самого `return`

```c++
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
    Foo &operator=(const Foo&) = delete;
    ~Foo() {
        std::cout << "~Foo " << this << "\n";
    }
};

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

int main() {
    Foo a, b, c;
    try {
        std::cout << "before foo\n";
        Foo x = func(a, b, c);
        std::cout << "after foo\n";
    } catch (...) {
        std::cout << "exception\n";
    }
}
```
####Итого
1) Невозможность обращения к полям
   
В конструкторе по причине все сломалось, все потерлось, а в деструкторе по причиене уже удалили

2) Возможность (или её отсутствие) отменить выброс исключения (смотри комменты в коде)

В конструкторе всегда(либо сами, либо автоматически) `throw;` в конце, тк объект не создался. `return`нельзя, объект не получилось создать, он умирает, часть полей уже в базовом классе потерлось. 
В деструкторе по умолчанию добавить `throw`, но можно написать return и тогда ничего не выбросится.

В функциях return по дефолту, но можно что-то выкинуть, если хотим




