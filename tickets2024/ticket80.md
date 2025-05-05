## Билет 80. Parameter pack — основы

### Функции с произвольным числом аргументов: синтаксис function parameter pack

Чтобы реализовывать функции с произвольным числом аргументов, нужно использовать function parameter pack.

Его синтаксис:

```c++
template<typename /* template parameter pack */ ...Ts>
void example(const Ts &...vs){} // function parameter pack
```

Где template parameter pack - параметр шаблона, у которого есть многоточие при объявлении. В нем могут быть произвольные
типы, и их кол-во так же произвольно.
function parameter pack - некоторое кол-во аргументов от нуля и больше, которое называется vs. Это не переменные, а так
сказать последжовательность переменнных.
Parameter pack можете передавать только целиком. 

### Template parameter pack, variadic template (в том числе специализации — у них может быть несколько паков)

Синтаксис Template parameter pack выше. Элемент по номеру на базовом уровне в языке никак получить нельзя. Никак не можем контролировать, чтобы все элементы были одного типа.

Template parameter pack не является variadic function, например C-printf. Variadic function не проверяет типы. Аргументы неявно конвертируются, так же мы должны явно знать типы из строки формата. Их не следует использовать в C++. \
Егор не рекомендует использовать, если что-то такое надо, то юзаем parameter pack и радуемся жизни, потому что будет шанс проверить совпадение типов с типами аргументов при вызове.

Синтаксис variadic function такой: 
```c++
void simple_printf(const char* fmt, ...){}
```

Можно использовать несколько parameter pack в специализациях. Главное, чтобы компилятор смог догадаться чему они равны, исходя из аргументов шаблона.
И в этой ситуации мы можем использовать несколько parameter pack в шаблонах, требуем, чтобы каждый тип (As and Bs) был std::tuple.

```c++
template<typename, typename>
struct Foo {
};

template<typename ...As, typename ...Bs>  // Ok, multiple parameter packs.
struct Foo<std::tuple<As...>, std::tuple<Bs...>> {  // Similar to template type deduction.
};
```

### Значения вместо типов в template parameter pack

Иногда бывает полезно передать в шаблон значение, а из него компилятор уже выведет сам тип.

```c++
template<int ...Ns>  // Template parameter pack, but of values instead of types.
struct Foo {
    static constexpr inline std::size_t M = sizeof...(Ns);
};
Foo<> b1;            // M = 0
Foo<10, 20, 30> b2;  // M = 3

int func() { return 0; }
void foo(int a, int (*b)()) {}

template<typename ...Ts>
struct Combo {
    // Pack expansion to provide exact types of values.
    template<Ts ...Values> struct Inside {
        void doit() {
            // Just like a function parameter pack.
            // As foo is not templated, it requires specific `Ts...`.
            foo(Values...);
        }
    };
};
Combo<int, int(*)()>::Inside<10, func> x;
// Combo<int, int(*)()>::Inside<func, func> y;  // type mismatch

// Since C++17: each value can have an independent type.
template<auto Value> struct WithValue {};
WithValue<10> y;
template<auto ...Values> struct WithValues {};
WithValues<10, func, func> z;
```

### Pack expansion, в том числе параллельный для нескольких паков

Parameter pack можно раскрыть, написав (выражение)...:

```c++
example(vs...);
print((vs+10)...) // ко всем элементам прибавится 10
print(vs..., vs...) // если parameter pack {1,2,3,4}, данная строка вызовет функцию print(1,2,3,4,1,2,3,4)
```

Если у нас есть два parameter pack одинаковой длины, то можно раскрыть их параллельно:

```c++
print((vs + vs)...); // print(1,2,3,4) - результат 2,4,6,8
```

Можно узнать количество элементов, из которых он состоит:

```c++
std::cout << sizeof...(vs) << std::endl;
```

### Автовывод типов, автовывод нескольких template parameter pack из аргументов функции

Function parameter pack не работает, если parameter pack не последний в аргументах. Хотите использовать его в автовыводе типов или автовыводе аргументов из аргументов функции?
Используйте его только в конце.

```c++
//  тут на разных компиляторах по разному, но происходит в обоих случаях какая-то дичь
template<typename ...As, typename ...Bs>
void foo(As ...as, Bs ...bs) {
    std::cout << "foo(" << sizeof...(as) << ", " << sizeof...(bs) << ")\n";
}
// так делать можно -> parameter packs As и Bs корректно раскрываются, так как они находятся в конце
// As корректно раскрывается, так как единственна в типе tuple
template<typename ...As, typename ...Bs>
void bar([[maybe_unused]] std::tuple<As...> at, Bs ...bs) {
    std::cout << "bar(" << sizeof...(As) << ", " << sizeof...(bs) << ")\n";
}
// так тоже можно делать, так как As находится в конце типа tuple, Bs как в прошлом примере
template<typename ...As, typename ...Bs>
void baz([[maybe_unused]] std::tuple<int, As...> at, int, Bs ...bs) {
    std::cout << "baz(" << sizeof...(As) << ", " << sizeof...(bs) << ")\n";
}
// и так можно делать, так как parameter pack находится в конце и определяется однозначно
template<typename T, typename ...Bs>
void baz2(const T& a, Bs ...bs){
    std::cout << "baz2(" << sizeof...(As) << ", " << sizeof...(bs) << ")\n";
}

template<typename ...As, typename ...Bs>
void baz_bad([[maybe_unused]] std::tuple<As..., int> at, Bs... bs, int) {
    std::cout << "baz(" << sizeof...(As) << ", " << sizeof...(bs) << ")\n";
}

int main() {
    // GCC, Clang: 0, 3
    // Visual Studio (and standard?): compilation error
    foo(1, 2, 3);

    // Compilation error: where to put the argument: As or Bs?
    // foo<int>(1);

    // 2, 2
    bar(std::tuple{1, 2}, 3, 4);

    // Parameter pack should be the last one for successful deduction.
    baz(std::tuple{1, 2}, 3, 4);  // 1, 1
    // baz_bad(std::tuple{1, 2}, 3, 4);  // Neither As nor Bs are deduced, compilation error.
    // baz_bad<int>(std::tuple{1, 2}, 3);  // I don't know whether this should work or not.
}
```

### Fold expression (бинарный и унарный)

С C++17 добавили так называемые fold expressions - они позволяют записывать какие-то операции над элементами pack-а короче и может быть понятнее)
Fold expression бывают binary и unary.
Вот общий вид всех fold expressions (скобки везде обязательны)

Unary:
- ```(pack op ...) == ((arg1 op arg2) op arg3 /* и так далее */)```
    + пример: ```((std::cout << args), ...)``` равносильно ```(std::cout << args1), (std::cout << args2), и так далее```

      здесь ```pack == args && op == ","```
- ```(... op pack)``` равносильно ```(arg1 op (arg2 op arg3) /* и так далее */)```

Binary:
- ```(init_val op ... op pack)``` равносильно ```((init_val op args1) op args2)```
    + пример: ``` (std::cout << ... << args)``` равносильно ```std::cout << args1 << args2 << и так далее```

      здесь ```pack == args && op == "<<"```
- ```(pack op ... op init_val) == (args1 op (args2 op init_val))```
    + то же самое, только другая ассоциативность



```c++
template<typename ...Ts>
auto sum1(const Ts &...vs) {
    return (vs + ...);  // unary right fold: v0 + (v1 + (v2 + v3))
    // return (... + vs);  // unary left fold: ((v[n-4] + v[n-3]) + v[n-2]) + v[n-1]
}

template<typename ...Ts>
auto sum2(const Ts &...vs) {
    return (vs + ... + 0);  // binary right fold: v0 + (v1 + (v2 + 0))
    // 0 - элемент, с которого начинаем
}

template<typename ...Ts>
auto sum_twice(const Ts &...vs) {
    return ((2 * vs) + ... + 0);  // паттерны и выражение разрешены
}

template<typename ...Ts>
void print(const Ts &...vs) {
    // binary left fold: (std::cout << v0) << v1
    (std::cout << ... << vs) << "\n";
    // (std::cout << vs << ...) << "\n";  // некорректно
    // std::cout << ... << vs << "\n";  // некорректный fold expression
    // std::cout << ... << (vs << "\n");  // `v0 << "\n"` невалидное выражение, так как побитовый сдвиг на "\n" wtf???
}
```

### Трюк с лямбда-функцией для эмуляции цикла по элементам parameter pack
В C++17 появились fold expression с запятой и стало возможно применить какую-либо операцию к каждому операнду:

```c++
template <typename... T>
void print(int n, const T &...vals) {
    auto f = [&](const auto &val) -> void {
        if (n > 0) {
            std::cout << typeid(val).name() << " : " << val << std::endl;
            n--;
        }
    };

    (f(vals), ...); // (1)
}
```

(1) раскроется в вызов `f` на элементах пака через оператор запятая, гарантирующий порядок выполнения.


До C++17 подобных fold expression не было и их приходилось имитировать, пользуясь тем, что можно было распаковать
пак через запятую в инициализации массивов. При этом было важно, чтобы функтор, который вызывался в цикле возвращал
значение такого же типа, как и у элементов массива.

```c++
template <typename... T>
void print(int n, const T &...vals) {
    auto f = [&](const auto &val) -> int {
        if (n > 0) {
            std::cout << typeid(val).name() << " : " << val << std::endl;
            n--;
        }
        return 0;
    };

    [[maybe_unused]] int dummy[] = {f(vals)...};
}
```
Здесь импользуется pack expansion - шаблон повторяется через запятую для всех элементов пака (Точно такой же используется
при передаче всего пака в качестве агрументов к другой функции)