## Билет 82. Вспомогательные инструменты метапрограммирования

#### `std::apply` для вызова функции с параметрами из кортежа

`std::apply` позволяет вызывать функцию, аргументами функции станут элементы кортежа, переданного в `std::apply`.
Никакой динамической типизации нет, типы известны на этапе компиляции.
```c++
void foo(int a, string b) {
    assert(a == 10);
    assert(b == "hello");
}

int main() {
    auto t = std::make_tuple(10, "hello");
    std::apply(foo, t);  // You can call a function
}
```

#### Ссылки внутри `std::tuple`, функция `std::tie`

Внутрь `std::tuple` можно складывать ссылки, lvalue, rvalue, константые и всякие другие, и эти ссылки начинают себя вести "может быть немножко странно".
```c++
std::tuple<int, string> foo() {
    return {30, "baz"};
}

int main() {
    {
        int a = 10; string b = "foo";
        std::tuple<int&, string&&> t(a, std::move(b));  // хранит lvalue на a и rvalue на b
        t = std::make_tuple(20, "bar");  // теперь a = 20, b = "bar"
        assert(a == 20);
        assert(b == "bar");

        std::tie(a, b) = foo();  // creates a temporary tuple<int&, string&>
        assert(a == 30);
        assert(b == "baz");

        auto [c, d] = foo();  // structured binding (C++17), always new variables.
    }
}
```

`std::tie` возвращает tuple из ссылое на свои переменные. Таким образом можно в несколько переменных одновременно что-то записать.
'Structured binding для бедных'. `std::tie` умеет модифицировать старые, а structured binding всегда создает новые.

#### Необходимость правильной расстановки `noexcept` для эффективной работы. Пример: копирование `std::vector<T>` может использовать разные алгоритмы.
```c++
template<typename T>
struct vector {
    T *data = nullptr;
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
```

Благодаря проверке, выкидывает ли тип `T` исключения при копировании смогли добиться строгой гарантии исключений в операторе копирования вектора, реализовав в зависимости от случая разные алгоритмы. Тем не менее всё ещё есть проблема с `std::optional`. Иногда оператор копирования должен быть помечен `noexcept`, а иногда нет. Решается так:
```c++
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
```
Обратим внимание на копирующий опреатор присваивания `optional<T>`, там есть то, что нам надо. Метод помечается `noexcept` в зависимости от значения выражения в скобках.

#### Условный оператор `noexcept`, конструкция `noexcept(noexcept(expr))`
```c++
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
Оператор `noexcept` позволяет проверять, верно ли, что выражение никогда не кидает исключений (помечено `noexcept`). Он не вычисляет выражение, поэтому никаких утечек быть не может. Все проверки происходят на этапе компиляции.

```c++
template<typename T>
void foo(T &a, const T &b) noexcept(std::is_nothrow_copy_assignable_v<T>) {
    a = b;
}

template<typename T>
void bar(T &a, const T &b) noexcept(noexcept(a = b)) {
    a = b;
}

// We can write complex expressions: noexcept(std::is_nothrow_copy_assignable_v<T> && 2 * 2 == 4)
// In C++20 there is a similar construct: `requires requires { ..... }`

int main() {
    int a = 10, b = 20;
    static_assert(noexcept(foo(a, b)));
    static_assert(noexcept(bar(a, b)));

    std::vector<int> v1, v2;
    static_assert(!noexcept(foo(v1, v2)));
    static_assert(!noexcept(bar(v1, v2)));
}
```
по типу requires requires. Можно проверять более сложные выражения

#### `std::declval`, где можно вызывать, зачем
`std::declval` можно настраивать, чтобы он возвращал rvalue, lvalue или xvalue.
`is_nothrow_copy_assignable_1` плох, так как проверяет, что выражение валидно.
Избавляемся от вызова конструктора `T` с помощью `is_nothrow_copy_assignable_2` и `std::declval`
```c++
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

Ну а еще можем использовать для sfinae. \
Converts any type T to a reference type, making it possible to use member functions in the operand of the decltype specifier without the need to go through constructors.

std::declval is commonly used in templates where acceptable template parameters may have no constructor in common, but have the same member function whose return type is needed.

Note that std::declval can only be used in unevaluated contexts and is not required to be defined; it is an error to evaluate an expression that contains this function. Formally, the program is ill-formed if this function is odr-used. 














