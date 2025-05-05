## Билет 53. Обобщённые функции

### Указатели на функции

Лекция 24-220411: [запись](https://youtu.be/bY-xdk1BgeI?list=PL8a-dtqmQc8obAqSKqGkau8qiafPRCxV7&t=4204),
[код](https://github.com/hse-spb-2021-cpp/lectures/tree/master/24-220411).

Лекция 28-220523: [запись](https://youtu.be/eok9TfiS74Q?list=PL8a-dtqmQc8obAqSKqGkau8qiafPRCxV7&t=1746), (там два файла)

#### Общие вещи

[24-220411/04-fptrs/01-fptrs.cpp](https://github.com/hse-spb-2021-cpp/lectures/blob/master/24-220411/04-fptrs/01-fptrs.cpp)

Синтаксис: `return_type (*func_name)(arg_types)`  
Можно написать `using fptr = ...`, тогда нужно передавать так:

```c++
using ApplyArgument = void(*)(int);
void apply2(ApplyArgument operation) { // cdecl.org
    std::cout << "calling with 10\n";
    operation(10);
}
```

[28-220523/02-weird-types/04-function-pointer-type.md](https://github.com/hse-spb-2021-cpp/lectures/blob/master/28-220523/02-weird-types/04-function-pointer-type.md)

Есть отдельный тип "функция", он автоматически преобразуется к указателю на функцию.

Указатель на функцию можно взять на обычную функцию, шаблонную, перегруженную, лямбду без захвата. Лямбда без захвата
умеет неявно кастоваться к указателю на функцию, потому что если просто взять на неё указатель, получим указатель на
объект без полей, в который она рассахаривается. Лямбда с захватом хранит состояние, поэтому на нее взять указатель
нельзя.

**Указатели на функции не совместимы между собой и остальными указателями.** Можно присваивать указателю только
указатель на функцию с тем же возвращаемым значением и параметрами. В GCC можно(но вообще нельзя) делать указатель на
функцию `void*`, про это ниже.

#### Проблемы с неоднозначностями

[24-220411/04-fptrs/02-fptrs-templated.cpp](https://github.com/hse-spb-2021-cpp/lectures/blob/master/24-220411/04-fptrs/02-fptrs-templated.cpp)

Принимающая функция мб шаблонной, но тогда надо ее вызывать так, чтобы тип либо выводился, либо был явно указан.

```c++
template<typename T>
void apply(void (*operation)(T), T data) {...}

template<typename T>
void apply10(void (*operation)(T)) {...}

template<typename T>
void print(T x) {...}

void print_twice(int x) {...}

template<typename T>
void (*print_ptr)(T) = print;

int main() {
   apply<int>(print, 10);
   apply(print, 10); // компилятор выведет `T` смотря на 10
   // apply10(print); // не скомпилируется, т.к оба шаблонные и непонятно, каких типов мы хотим.

   [[maybe_unused]] void (*ptr1)(int) = print;
   ptr1 = print_twice; // одинаковые возвращаемые значения и аргументы => все легально

   [[maybe_unused]] auto ptr3 = print_twice;
   //[[maybe_unused]] auto ptr4 = print; // непонятно, какой тип выводить
   [[maybe_unused]] auto ptr5 = print<int>;

   // [[maybe_unused]] void (*ptr6)(int) = print_ptr; // автовывод не работает для шаблонных переменных.
   [[maybe_unused]] void (*ptr7)(int) = print_ptr<int>;
   [[maybe_unused]] auto ptr8 = print_ptr<int>;
}
```

[24-220411/04-fptrs/03-fptrs-overload.cpp](https://github.com/hse-spb-2021-cpp/lectures/blob/master/24-220411/04-fptrs/03-fptrs-overload.cpp)

Аналогичные проблемы будут если есть перегрузки одной функции и мы пытаемся взять на одну них указатель. Все решается
явным указанием типа или `static_cast< void(*)(int) >`.

[28-220523/02-weird-types/05-function-pointer-overload.cpp](https://github.com/hse-spb-2021-cpp/lectures/blob/master/28-220523/02-weird-types/05-function-pointer-overload.cpp)
Нельзя сделать `decltype` от перегруженной или шаблонной при неоднозначностях. Так же компилятор работат только на один
уровень вложенности и не сможет догадаться, что `bar(&foo);` вызывает ее перегрузку от `int`.

```c++
template<typename Fn> void bar(Fn fn) { fn(10); }
bar(&foo);  // no matching function for call to 'bar(<unresolved overloaded function>)'
bar(static_cast<void(*)(int)>(&foo));  // Ок.
bar([](int x) { return foo(x); });     // Ок, у лямбды фиксированный параметр.
```

### void*

[24-220411/05-c-generics/01-void-ptr.cpp](https://github.com/hse-spb-2021-cpp/lectures/blob/master/24-220411/05-c-generics/01-void-ptr.cpp)

В `void*` можно сохранить(даже неявно) указатель на любой тип, но его нельзя разыменовать. Обратно в нужный указатель
можно преобразовать `static_cast`. Если преобразовали не в тот тип, то при разыменовании будет УБ (
нарушение [strict aliasing](https://www.geeksforgeeks.org/strict-aliasing-rule-in-c-with-examples/))

[24-220411/05-c-generics/02-void-ptr-extensions.cpp)](https://github.com/hse-spb-2021-cpp/lectures/blob/master/24-220411/05-c-generics/02-void-ptr-extensions.cpp)

Арифметики указателей нет, но у GCC есть расширение, в котором арифметика работает так же как для `char*` (можем смещать
по байтам).

[24-220411/05-c-generics/11-for-each-generic.cpp](https://github.com/hse-spb-2021-cpp/lectures/blob/master/24-220411/05-c-generics/11-for-each-generic.cpp)

Вообще указатель на функцию и объект это не одно и то же, они могут даже в разной памяти, но в Linux память одна,
поэтому можно преобразовать указатель на функцию к `void*`. Это некорректно, но скорее всего будет работать даже под
Windows.

Так можно написать `for_each` без шаблонов:

```c++
void for_each(void *begin, void *end, std::size_t elem_size, void (*f)(const void*)) {
    for (; begin != end; begin = static_cast<char*>(begin) + elem_size) 
        f(begin);  // Can only pass element by pointer
}

void print_int(const void *x) {
    std::cout << *static_cast<const int*>(x) << "\n";
}

int main(void) {
    int arr[10]{}; 
    for_each(arr, arr + 10, sizeof(arr[0]), print_int);
}
```

[24-220411/05-c-generics/12-for-each-generic-gcc.cpp](https://github.com/hse-spb-2021-cpp/lectures/blob/master/24-220411/05-c-generics/12-for-each-generic-gcc.cpp)

Этот код можно чуть упростить (убрать `static_cast<char*>`, и не кастовать внутри `print`) при помощи расширения
GCC (`-fpermissive`). Он даже забьет на`const void*` -> `int*`.

[24-220411/05-c-generics/13-for-each-generic-arg.cpp](https://github.com/hse-spb-2021-cpp/lectures/blob/master/24-220411/05-c-generics/13-for-each-generic-arg.cpp)

Если хотим передать состояние (в С++ для этого есть лямбды с захватом и функторы), то можем эти параметры запаковать в
структуру, передать ее через указатель на `void*`, потом распаковать (см. код).

Но делать так позволительно только в C, т.к нет шаблонов.
