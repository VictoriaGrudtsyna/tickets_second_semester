## Билет 83. Perfect Forwarding
См. [лекцию](https://youtu.be/V1ot4DS6wTU?t=3989)


Дословно — прямая передача. Или идеальная ссылка. Хотим написать одну шаблонную функцию, 
которая сама определяет, передали ей аргумент как `rvalue` ссылку, или же как `lvalue` ссылку. Это зашито
отдельным костылём в стандарте. Как это делается:
```c++
template<typename Arg, typename Fn>
void log(Fn fn, Arg &&arg) { // Special `Arg` deduction rules: "forwarding reference".
    // Assume the argument's type is `T`. Its value category is either:
    // * Rvalue: Arg = T , Arg&& = T&&
    // * Lvalue: Arg = T&, Arg&& = T&  (reference collapse rules: & + && = &, non-temporary wins)
    std::cout << "start\n";
    // std::forward<Arg> ~ static_cast<Arg&&>:
    // * Rvalue: Arg = T , Arg&& = T&&, std::forward ~ static_cast<T&&> ~ std::move
    // * Lvalue: Arg = T&, Arg&& = T& , std::forward ~ static_cast<T&>  ~ ничего не делаем
    fn(std::forward<Arg>(arg));
    // Notes:
    // 1. Not `std::move`: it always returns T&& (rvalue).
    // 2. `arg` is always lvalue of type `T`, so no automatic deduction is possible. Hence,
    //    the explicit template argument is required.
    // Alternative:
    // `std::forward<Arg&&>(arg)` = `std::forward<decltype(arg)>(arg)`.
    std::cout << "end\n";
}
```
Пишем функцию, пишем её шаблонный аргумент (в данном случае — `Arg`), а в аргументах функции пишем `Arg &&`. Можно подумать,
что это `rvalue` ссылка. Но для данного случая в стандарте написано, что применяются специальные правила для вывода
вот этого `Arg`. 

Эта штука (`Arg &&`) называется `forwarding reference` (именно в контексте шаблонов). Здесь компилятор
смотрит не только на тип аргумента, но ещё и на его категорию (`rvalue`/`lvalue`):
```c++
log(foo, 10);  // Rvalue: Arg=int, Arg&&=int&&

int x = 20;
log(bar, x);  // Lvalue: Arg=int&, Arg&&=int& (что-то странное...)
std::cout << "x = " << x << "\n";

std::string s = "hello world from long string";
const std::string cs = "hello world from long const string";
Storer baz;
log(baz, s);  // Lvalue: Arg=std::string&, Arg&&=std::string&
log(baz, std::move(s));  // Rvalue: Arg=std::string, Arg&&=std::string&&
log(baz, cs);  // Lvalue: Arg=const string&, Arg&&=const string&
log(baz, std::move(cs));  // Rvalue: Arg=const string, Arg&&=const string&&
```
Во втором примере имеем `Arg && = int &`, что-то странное — `rvalue` ссылка на `lvalue` ссылку. И снова
подключается костыль из стандарта под названием `reference collapse rules` (& + && = &).

Всё, что остаётся сделать после —
сохранить идеальную ссылочность: передать в функцию, которую мы вызываем внутри (`fn`), либо `lvalue`, либо `rvalue` ссылку.
Для этого есть специальная функция `std::forward`. Вызывается как `std::forward<Arg>(arg)`, где `Arg` — либо просто тип,
либо тип со ссылкой. `std::forward<Arg> ~ static_cast<Arg&&>` (см. в коде выше). `<>` писать обязательно!
Иначе компилятор не догадается, что надо делать с аргументом.

То есть, как этим всем пользоваться:
- написали шаблонный параметр
- написали `forwarding reference`
- сделали `std::forward`

Для множества параметров функции работает аналогично:
```c++
void logged(Fn fn, Args &&...args) {
    fn(std::forward<Arg>(arg)...);
}
```

`forwarding reference` возникает только тогда, когда выводится параметр при вызове.

Рассмотрим следующий пример:
```c++
template<typename T>
struct my_vector {
    void push_back_1(T &&) {  // not forwarding reference: `T` is already fixed (by vector). Just rvalue reference.
    }

    template<typename U>
    void push_back_2(U &&) {  // forwarding reference: `U` is deduced when calling
    }
};

template<typename T>
void foo(my_vector<T> &&) {  // not forwarding reference: argument is always rvalue, `T` cannot affect this
}
```

В первом `pushback`'е у нас уже
есть объект типа `vector`, уже есть какое-то зафиксированное `T`. `pushback` — **не**шаблонная функция, у неё нечего выводить
автоматически. А если мы хотим `forwarding reference`, то нужно сделать шаблонный метод `pushback`: сделать шаблонный
параметр `U`. И вот это `U` выводится в момент вызова метода.

#### Отличия ref/cref и perfect forwarding:
`ref/cref` иногда имеют смысл, например, если не хочется передавать ссылки. Поэтому, если нужно принять какие-то аргументы и не сразу
их передать в функцию, а куда-нибудь сохранить, то скорее надо сохранять значения.

`ref/cref` и `perfect forwarding` штуки перпендикулярные: вторая берет аргументы и наивно передаёт их как ссылки,
а вот первая обычно используется, когда нужны значения, но хочется ссылок.

Пример: создание нового потока.
```c++
void some_thread(std::vector<int> vec, int &x) { .... }
int y = 5;
std::thread t(some_thread, create_vector(), std::ref(y));  // temporary likely dies before the thread starts.
```
Если бы `std::thread` внутри себя делал `perfect forwarding`, то он бы вызвал функцию some_thread в новом потоке
и передал в качестве параметра `create_vector()`, то есть временное значение, ссылку на временный объект. И этот вектор бы
уже умер, когда поток начал работу.

#### Оператор `decltype` ([лекция](https://youtu.be/eok9TfiS74Q?t=125))

```c++
template<typename> struct TD;

int& foo();
int&& bar();

int main() {
    int x = 10;
    int &y = x;

    // decltype() has two modes:
    // 1. decltype(expression)
    TD<decltype(10)>();  // int
    TD<decltype( (x) )>();  // int&
    TD<decltype( (y) )>();  // int&
    TD<decltype( x + 20 )>();  // int
    TD<decltype( foo() )>();  // int&
    TD<decltype( bar() )>();  // int&&
    // Returns depending on value category: T (prvalue), T& (lvalue), T&& (xvalue)
    // Reminder: rvalue = prvalue || xvalue

    // 2. decltype(name) - a variable/field/argument with no parens
    TD<decltype(x)>();  // int
    TD<decltype(y)>();  // int&
    // Returns: the declaration of `name`
}
```
В первом режиме `decltype` возвращает тип выражения, но не только — он ещё смотрит на категорию:
`prvalue -> T`, `lvalue -> T&`, `rvalue -> T&&`.

Во втором режиме в `decltype` передаётся сущность: поле, переменная или аргумент (только одно имя, без скобочек,
без `static_cast`'ов!). И компилятор просто смотрит на то, как эта сущность была объявлена.

Рассмотрим следующий пример:
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

decltype(auto) foo_ref_caller_bad() {  // int&
    return foo_ref();
}

decltype(auto) bar() {  // int
    return x; // return decltype(x)
}

decltype(auto) baz() {  // int&
    return (x); // return decltype((x)). (x) уже считается expression
}

int main() {
}
```
`auto` сам по себе никогда ссылку не возвращает. А `decltype(auto)` запоминает всякие ссылки, константности.

Что значит `decltype(auto)`? Он работает почти так же, как просто `auto`, но вместо того, чтобы выводить 
тип по обычным правилам, он выводит его по правилам `decltype`'а. То есть навешивает `decltype` на возвращаемое
значение.

Как использовать в лямбдах?
```c++
auto logged = [](auto fn, auto &&...args) -> decltype(auto) {
    std::cout << "started\n";
    decltype(auto) res = fn(std::forward<decltype(args)>(args)...);
    std::cout << "ended\n";
    return res;  // decltype сработает во втором режиме!
};
```
Принимаем шаблонный параметр `fn`, шаблонные `variadic` `args` по `perfect forwarding`, при этом явно указан возвращаемый тип — 
`decltype(auto)`. Внутри создаём `decltype(auto)` переменную `res` (это какое-то конкретное выражение, поэтому работаем в
первом режиме). Заметим, что в `std::forward` нужно указывать `decltype(args)`.

#### Невозможность объявить переменную/поле типа `void`
Рассмотрим следующий пример ([практика](https://github.com/hse-spb-2021-cpp/exercises/blob/master/28-220523/solution/01-memorizer.h)):
```c++
template<typename Fn, typename C = typename impl::function_traits<Fn>::call_result_type>
struct Memorizer {
    Fn fn;

    std::vector<C> calls;

    template<typename ...Args>
    decltype(auto) operator()(Args &&...args) {
        std::tuple<std::remove_reference_t<Args>...> arguments(args...);
        if constexpr (std::is_same_v<void, decltype(fn(std::forward<Args>(args)...))>) {
            fn(std::forward<Args>(args)...);
            calls.emplace_back(std::move(arguments));
        } else {
            decltype(auto) result = fn(std::forward<Args>(args)...);
            calls.emplace_back(result, std::move(arguments));
            return result;
        }
    }
};
```
У нас есть шаблонный оператор. В `args` условно хранится результат и какие-то аргументы. Если результат — `void`,
мы не можем его никуда сохранить, и решение — `if constexpr` (константа времени компиляции). Разбираем отдельно случаи, когда результат есть и когда его нет.












