## Билет 24. Вызов шаблонных функций

### Автовывод шаблонных параметров функций ([лекция](https://youtu.be/nFHUie787ZM?t=418))
Шаблоны функций отличаются тем, что для них параметры могут выводиться автоматически:

```c++
template<typename T>
void swap(T &a, T &b) {
    T tmp = std::move(a);
    a = std::move(b);
    b = tmp;
}
```
Такую шаблонную функцию можно вызвать явно, как и любой шаблон:
```c++
int x, y;
swap<int>(x, y);
```
Но обычно шаблонные функции вызываются при помощи так называемого вывода типов:
```c++
swap(x, y);  // x = int, y = int => T = int
```
Тогда компилятор попытается вывести, чему равен каждый шаблонный параметр: 
посмотрит, какие формально параметры у `swap`'а. 
Как можно привязать `int` к `T &`? Только когда `T = int`.
Противоречий не возникает, вызывается `swap` от `int`'а.

А вот если попробовать вызвать функцию от разных параметров:
```c++
int x;
short z;
swap(x, z); // compilation error
```
Будет ошибка компиляции, так как компилятор не найдёт функцию ```swap``` от параметров 
`int &` и `short &`, только от `int &` и `int &` (template argument substitution failure).

В случае же следующего вызова функции:
```c++
swap<int>(x, z); // compilation error
```
Проблем с выводом типов не будет (явно указан `int`), но будет проблема в том, что компилятор не сможет привязать
`int &` к `short`.

Следующий пример корректен:
```c++
template<typename T, typename U>
void foo(const std::vector<T> &, const std::vector<U> &) {}

foo(std::vector<double>{}, std::vector<float>{});  // T = double, U = float
```

`const` — часть типа, поэтому есть следующие приколы:
```c++
template<typename T>
void print(/* const */ T &a) { // принимаем по ссылке
    a++; // a is not const!
    std::cout << a << std::endl;
}

template<typename T>
void print_off(T a) { // принимаем по значению
    a++;
    std::cout << a << std::endl;
}

print(10); // compilation error: Arg = int, T = int, 10 != int &
print_off(20); // ok: Arg = int, T = int, 20 = int
const int x = 30;
print(x);  // compilation error: Arg = const int, T = const int => const int &x
print_off(x);  // ok: Arg = const int. T = int => int x
```

Мы не можем вызвать `print(10)`, потому что не можем привязать ссылку к временному объекту (`rvalue`).
По тем же причинам, например, не скомпилируется `auto &a = 10`. При этом, если передать `const int`, то у нас получится
вызвать функцию, хотя мы явно указали `int &`. В таком случае возникнет ошибка компиляции, так как
внутри функции меняется переменная, которая `const`.

Автовывод может выводить только часть аргументов:
```c++
template<typename To, typename From>
To my_static_cast(From x) {
    return static_cast<To>(x);
}

template<typename TA, typename TB>
void print_two(const TA &a, const TB &b) {
std::cout << a << ' ' << b << std::endl;
}

[[maybe_unused]] auto z = my_static_cast<int>(10.5);
print_two<int, double>(10, 23.5);
print_two<int>(10, 23.5);
print_two<double>(10, 23.5);  // TA = double, TB = double => const double &a = 10;
print_two<>(10, 23.5); // явно вызываем именно шаблонную функцию
print_two(10, 23.5);
```
Так как тип `To` в списке аргументов первый, при вызове можно указать только его, а `From` выведется из аргумента.
То есть можно указывать любой префикс шаблонных параметров, а все остальные должны быть либо выводимы из аргументов,
либо быть параметрами по умолчанию.

### Невозможность автовывода ([лекция](https://youtu.be/nFHUie787ZM?t=2434))
При наследовании автовывод тоже работает, но бывают проблемы:
```c++
template<typename T>
struct Base {};

struct Derived1 : Base<int> {};
struct Derived2 : Base<double> {};
struct Derived3 : Base<double>, Base<int> {}; // наследуемся от нескольких!

template<typename T>
void foo(const Base<T> &) {
}

struct ConvertibleToBase {
    operator Base<int>() { // оператор (неявного) преобразования в Base<int>
        return {};
    }
};

foo(Base<int>()); // ok
foo(Derived1()); // ok: среди баз Derived1 есть Base<int> => T = int
foo(Derived2()); // ok
foo<int>(Derived3()); // compilation error: Base<T> — неоднозначный базовый класс

ConvertibleToBase x;
const Base<int> &ref = x; // ok
foo<int>(x);
foo(x);  // compilation error
```
При вызове `foo(x)` компилятору придётся перебрать все возможные операторы преобразования внутри `x`
(которые могут быть шаблонными), ну и он просто забьёт на это дело.

Автовывод вложенных типов ([лекция](https://youtu.be/nFHUie787ZM?t=3375)):
```c++
template<typename C>
bool is_begin(typename C::iterator it, const C &c) {  // Контейнер C и итератор it, в нём лежащий
    return c.begin() == it;
}

template<typename C>
bool is_begin2(typename C::iterator) {  // Impossibe to deduce C from 'C::iterator'
    return true;
}

std::vector<int> vec;
assert(is_begin<std::vector<int>>(vec.end(), vec)); // ok, явно указали тип C
assert(is_begin<>(vec.end(), vec)); // ok, сработал автовывод

is_begin2<std::vector<int>>(vec.begin()); // ok: явно указали тип C
is_begin2<>(vec.begin());  // compilation error: template argument deduction/substitution failed
```
Во второй шаблонной функции нет аргумента, из которого выводится тип `C`, — проблема. Компилятору пришлось бы
перебрать все типы и проверить, у какого из них есть `::iterator` нашего типа, что странно.

### Разрешение перегрузок с шаблонными функциями и автовыводом шаблонных параметров
1) **Name resolution** (Когда пишем имя функции, нужно понять, на какие именно функции это имя может указывать):
   - ADL (argument dependent lookup)
   - template argument deduction

На выходе получаем множество функций (`overload set`), из которых нужно выбрать наиболее подходящую перегрузку.
2) **Overload resolution**
   - Выкидываем все функции, которые в принципе не подходят, — остаются так называемые `viable functions`.
   - Выбираем `best viable function` — наиболее подходящую перегрузку.


3) Только после этого происходит **проверка доступа** — если
      выбранная функция оказалась приватной, то мы именно в этот момент получим ошибку (а публичную функцию, которая могла бы подойти,
      мы уже не выберем).

Есть правило, что при прочих равных **не**шаблонная функция выигрывает у шаблонной.

Если вызываем функцию со скобками `<>`, то есть явно указываем на шаблон, то все **не**шаблонные функции
выпиливаются из `overload set`.

Иногда специализации функций не влияют на выбор перегрузки. Пример Димова-Абрамса ([лекция](https://youtu.be/Elg1jHq-eyc?t=2586)):
```c++
template<typename T> void foo(T) { std::cout << "1\n"; }

template<> void foo(int *) { std::cout << "2\n"; } // специализация шаблонной функции

template<typename T> void foo(T*) { std::cout << "3\n"; }

// template<> void foo(int *) { std::cout << "4\n"; }

int x = 10;
foo(x);  // 1
foo<int*>(&x);  // 2, two candidates: foo<int*>(int*) and foo<int*>(int**) (во втором случае — 1) 
foo<int>(&x);  // 3, two candidates: foo<int*>(int) and foo<int*>(int*)
foo(&x);  // 3: two candidates: foo(T*) and foo(T)
```
Почему в последнем случае выбирается последняя перегрузка? Потому что у неё `T*`, что-то более конкретное и ограничивающее,
чем просто `T`, несмотря на то, что шаблон первой функции далее специализирован. А вот если специализацию перенести вниз,
то в последних двух вызовах будет выбираться именно она. Поэтому лучше не использовать специализации функций, потому что с
перегрузками они работают странно.
