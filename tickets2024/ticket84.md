## Билет 84. Вычисления на этапе компиляции — основы

Код с лекций: [`28-220523`](https://github.com/hse-spb-2021-cpp/lectures/tree/master/28-220523)

### `constexpr`-вычисления


* **`const`**: `const` означает лишь, что эту переменную менять нельзя. Никаких инструкций по поводу того, на каком этапе нужно вычислять эту переменную, `const` не даёт. В целом, константы вычисляются во время компиляции, но если мы добавляем какие-то вспомогательные штуки (например, кладём это вычисление в функцию), то они сразу перестают быть константами времени компиляции. [Лекция](https://youtu.be/eok9TfiS74Q?t=2930)
```c++
int sum(int a, int b) { 
    return a + b;  
}

const int N = 10;

const int X = readInt(); // reads smth from stdin

const int Y = sum(2, 2);

int arr1[N]; // OK
int arr2[X]; // BAD: we don't know, what's in X at compile-time
int arr3[Y]; // BAD: even though sum(2, 2) should be calculated in compile-time, compiler doesn't know what it is
```

* **`constexpr`**: говорит компилятору, что это выражение нужно вычислить на этапе компиляции. В целом, глобальные константы лучше помечать `constexpr`, а не `const`: так, если что-то сломается, мы сразу получим сообщение об ошибке. Пометка `constexpr` у функции означает, что она _может быть_ вычислена во время компиляции (но в рантайме её вызывать никто не запрещает). Базовые `constexpr`-функции появились в С++11, а в дальнейших стандартах расширялись их возможности: рекурсия, сложные операторы, шаблонность и т.д.
```c++
constexpr int sum(int a, int b) {  // is allowed to be called both in compile and run time.
    return a + b;  // Valid C++11, since C++14: more operations
}

const int N = 10;

const int X = readInt();
// constexpr int X = readInt();  // on a variable: forces compile-time const

//const int Y = sum(2, 2);
constexpr int Y = sum(2, 2);  // on a variable: forces compile-time const

int arr1[N]; // OK
// int arr2[X]; // X can't be constexpr, as readInt can't be constexpr
int arr3[Y]; // OK
```

* **Структуры**: чтобы структуры умели делать что-то во время компиляции, они должны отвечать двум логичным требованиям: [Пример](https://github.com/hse-spb-2021-cpp/lectures/blob/master/28-220523/03-constexpr/02-constexpr-struct.cpp), [Лекция](https://youtu.be/eok9TfiS74Q?t=3325)
	- Иметь `constexpr` конструктор или дефолтный конструктор и дефолтные значения полей или вызываться с direct-list-initialization (`my_struct{a, b, c}`).
	- Вызываемые методы должны быть `constexpr`.

	То есть просто хотим, чтобы на этапе компиляции либо уже были вычислены все поля, либо их можно было вычислить, про методы очевидно.
 
* **Константные строковые литералы**: умеем на этапе компиляции работать с константными строковыми литералами. Важно помнить, что `strlen` не `constexpr`, поэтому желательно как-либо явно передавать размер строки, чтобы с ней было комфортнее работать (т.к. вычислить внутри его не сможем). Вот, например, функция, которая на этапе компиляции парсит форматы для `printf`/`scanf` в формат `{amount of %, [types of %]}`. [Лекция](https://youtu.be/eok9TfiS74Q?t=3547)
```c++
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
```

### Тип "функция"

* У любой (неперегруженной, про перегрузки см. ниже) функции есть свой тип, который включает в себя возвращаемый тип и типы всех аргументов, его можно вывести через `decltype()`. [Лекция](https://youtu.be/eok9TfiS74Q?t=1638)
```c++
void func1();
void func2(int x);
void func3(int y);
int func4(int x, char y);

template<typename> struct TD;
TD<decltype(func1)> a; // TD<void()>
TD<decltype(func2)> b; // TD<void(int)>
TD<decltype(func3)> c; // TD<void(int)>
TD<decltype(func4)> d; // TD<int(int, char)>
```

* Создать переменную типа "функция" нельзя. Если попробовать это сделать, то получится не переменная, а функция, причём вызвать её будет нельзя, т.к. она не определена.  Однако этот тип часто используется, например, в шаблонах, и в целом является полноценным.
```c++
// decltype(func1) x;  // Cannot create a variable of this type.
std::function<void(int)> y;  // frequently used as a template parameter
```

* **Указатель на функцию**: между возвращаемым типом и аргументами появляется `(*)`: `void(int) --> void(*)(int)`. Тип "функция" умеет неявно приводится к типу "указатель на функцию", также можно создавать переменные этого типа. Важно, что функции и указатели на функции — это разные типы: первый чаще используется в шаблонах, второй — при создании переменных и взаимодействии с языком Си. [Лекция](https://youtu.be/eok9TfiS74Q?t=1745)
```c++
using func1_ptr = void(*)();
func1_ptr z = func1;  // implicitly convertible to a pointer
```

```c++
char foo(int);
// ....
auto x = [](int val) -> char { return val; };
auto y = [&](int val) -> char { return val; };
char (*func1)(int) = &foo;  // Ок.
char (*func2)(int) = foo;   // Ок, функция неявно преобразуется в указатель на себя.
char (*func3)(int) = x;     // Ок: лямбда без захвата — почти свободная функция.
char (*func4)(int) = y;     // Не ок: лямбда с захватом должна знать своё состояние.
```

* **`void*`**: Нельзя преобразовывать указатель на функцию в `void*`, т.к. это разные объекты: `void*` — указатель на данные, а указатель на функцию — указатель на код, они могут лежать в совершенно разных типах памяти. Однако Linux пофиг, там некоторые функции умеют возвращать `void*` вместо указателей на функции.
```c++
void *x = static_cast<void*>(&foo);  // Ошибка компиляции.
```

* **Про перегрузки**: сделать `decltype(foo)` от перегруженной функции нельзя, т.к. будет невозможно однозначно вывести её тип, получим ошибку компиляции. Однако если специфицировать тип, то перегрузка разрешится и проблем не будет: [Лекция](https://youtu.be/eok9TfiS74Q?t=1863)
```c++
void foo(int x);
char foo(char x);

TD<decltype(&foo)>{};  // Ошибка компиляции: какую перегрузку выбрать decltype?

void (*func1)(int)  = &foo;  // Если в точности указать тип, то перегрузка разрешится.
char (*func2)(char) = &foo;
```

* **Перегрузки, `auto` и шаблоны**: с `auto` и шаблонами будут проблемы, т.к. не получится вывести перегрузку. В таком случае можно пытаться явно кастится к нужному типу или использовать лямбды (здесь всё же говорим скорее об указателях, а не о типе "функция").
```c++
int foo(int x);
char foo(char x);
// ....
template<typename Fn> void bar(Fn fn) { fn(10); }
// ....
bar(&foo);  // no matching function for call to 'bar(<unresolved overloaded function>)'
bar(static_cast<void(*)(int)>(&foo));  // Ок.
bar([](int x) { return foo(x); });     // Ок, у лямбды фиксированный параметр.
```

### Traits — функции из типов или значений в наборы типов или значений

* **Кто?**: хотим получить какой-то общий расширяемый интерфейс, чтобы иметь возможность получать какие-то типы/значения/функции из каких-то других типов/значений/функций.

* **Реализация через структуры и их специализации**: имеем какое-то множество объектов, для которых хотим сделать `_traits`. Создаём соответствующую шаблонную структуру, а затем добавляем специализации для каких-то крайних случаев. Например, для `iterator_traits`, которые будут ниже, у обычных итераторов есть всё, что нужно, а для указателей потребуется специализация.

* **Конвенции `_t`, `_v`**: если у вас есть какая-то простая структура, которая делает простую операцию с типом вида "принимает тип - возвращает тип", то вы заводите себе структурку с членом `type` плюс (с C++11) делаете вспомогательную шаблонную переменную `_t`, которая крокодила `my_traits<T>::type` заменяет на `my_traits_t<T>` (фактически очень похоже на вызов функций, только другие скобочки и в качестве аргументов — типы или что-то, вычислимое на этапе компиляции). С `_v` аналогично, только вместо типа мы храним `constexpr`. [Пример целиком](https://github.com/hse-spb-2021-cpp/lectures/blob/master/28-220523/04-meta-conventions/02-type-traits-short.cpp)
```c++
// Convention for traits "returning" a single type:
template<typename T> struct remove_const {  // std::remove_const
    using type = T;  // standard name, a convention
};
template<typename T> struct remove_const<const T> {
    using type = T;
};
// Since C++11: helper has a `_t` suffix
template<typename T> using remove_const_t = typename remove_const<T>::type;

void foo() {
    [[maybe_unused]] remove_const_t<const int> x = 10;
    x++;
}
```

* **`std::integral_constant<T, v>`**: обёртка для константы типа `T` со значением `v`. Это базовый класс для всех `type_traits`, который умеет в конвертацию к `value_type`, имеет оператор `()` и специализации `true_type`/`false_type`, которые просто хранят константы `true`/`false`. Эта вся история нужна для удобства при наследовании: если хотим получить стандартный набор штук (тип, значение, операторы конвертации, круглые скобки и всю эту муть), то можем отнаследоваться от `integral_constant`, его наследников или специализаций. [Лекция](https://youtu.be/eok9TfiS74Q?t=4355)
```c++
template<typename T> struct is_const : std::false_type {
};
template<typename T> struct is_const<const T> : std::true_type {
};

static_assert(is_const<const int>::value);
static_assert(!is_const<int>::value);
```

### `iterator_traits<T>`

[Код целиком](https://github.com/hse-spb-2021-cpp/lectures/blob/master/28-220523/04-meta-conventions/01-type-traits.cpp), [Лекция](https://youtu.be/eok9TfiS74Q?t=3887)

* **`::value_type`**: проблема возникает тогда, когда у `T` нет такого члена, как `value_type` (например, если мы многострадальный указатель). То есть в общем мы ождиаем, что у итератора есть все необходимые члены, но в некоторых ситуациях может потребоваться обработка крайних случаев, подробнее см. пример в конце. 


* **Зачем `iterator_traits`, когда есть `auto`**: В общем случае: если хотим, например, сохранять в переменную разность двух итераторов, то не можем написать `Iterator::difference_type var = ...`, потому что у `Iterator` может не быть `difference_type` (если он указатель) и мы должны это учесть. `auto` справляется с выводом типов, но его не всегда можно использовать (например, в полях структурок), поэтому в некоторых местах без `iterator_traits` не обойтись. 

```c++
template<typename T>
struct iterator_traits {  // General case (may be absent)
    using value_type = typename T::value_type;
    using difference_type = typename T::difference_type;
};

template<typename T>
struct iterator_traits<T*> {  // Corner case, can be specialized
    using value_type = T;
    using difference_type = std::ptrdiff_t;
};

template<typename It>
void foo(It begin, It end) {
    [[maybe_unused]] typename iterator_traits<It>::difference_type n = end - begin;
    [[maybe_unused]] typename iterator_traits<It>::value_type first = *begin;
    // Since C++11: `auto` is very helpful
}

template<typename It>
struct my_iterator_wrapper {
    typename iterator_traits<It>::value_type v;  // No `auto` fields yet.
};
```

### Реализация `function_trais`

* **Кто?**: это штука, которая принимает указатель на функцию и умеет доставать из неё возвращаемый тип, типы аргументов, арность (число аргументов) и так далее.

* Реализация аналогично для всех остальных `_traits`: делаем шаблонную структуру-вариадик, в качестве арности задаём размер parameter-pack, возвращаемое значение — шаблонный параметр, а аргументы можем возвращать, например, через `type_list` (в целом умеем от parameter pack откусывать первый тип и рекурсивно что-то делать). В общем случае оставляем пустой `function_traits`, а для указателей на функцию делаем специализацию со всем необходимым.
```c++
template<typename...> struct type_list;

// We can "parse" types, like in boots::function_trai
template<typename> struct function_traits {};
template<typename Ret, typename ...Args>
struct function_traits<Ret(*)(Args...)> {
    static constexpr std::size_t arity = sizeof...(Args);
    using return_type = Ret;
    using args = type_list<Args...>;
};

static_assert(std::is_void_v<
    function_traits<void(*)(int, int, char)>::return_type
>);
```
