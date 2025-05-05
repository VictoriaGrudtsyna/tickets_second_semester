## Билет 54. Отличия Си и C++

Кристина лекция 25.md
Лекция Егора C basic  с 1:05:00 
### Generic

* В C все хедеры оканчиваются на `.h`
* До С99 все переменные должны объявляться в начале блока {}

## Содержание
* [Комментарии и объявления переменных (особенно в C89)](#tag1)
* [(void) в объявлении функции, неявные объявления функций](#tag2)
* [Объявления своих структур (везде нужно `struct`), конвенция с `typedef`](#tag3)
* [Выделение и освобождение памяти через `malloc`/`free` против `new`/`delete`](#tag4)
* [Отличия между Си и C++ в преобразованиях `void*`](#tag5)
* [Альтернативы для языковых возможностей: unnamed namespace, `*_cast<>`, `int a{}`, namespace, ссылки, `bool`, операторы копирования и перемещения, шаблоны](#tag6)
* [Отсутствующие возможности: перегрузка функций и параметры по умолчанию, `std::vector`, исключения](#tag7)

## <a name="tag1"></a> Комментарии и объявления переменных (особенно в C89)

Комментарии через `//` появились в С99, до этого были только такие: `/* комментарий */`

В старом C нельзя определять переменные внутри цикла или в середине программы, CE в C89, но GCC будет норм, только
с флагом -pedantic будет нормально.

```c
#include <stdio.h>  /* All headers end with .h by convention, in C++ it's <cstdio> */
/*
In <cstdio> `std::printf` is guaranteed, `::printf` is possible.
In <stdio.h> `::printf` is guaranteed, `std::printf` is possible.
<cstdio> in C does not exist.
*/

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

## <a name="tag2"></a> `(void)` в объявлении функции, неявные объявления функций

### Функция принимает любое число аргументов
`()` – произвольное число аргументов, с которыми функция ничего не может сделать.

`(void)` – без аргументов.

```c
void foo(char* a) {
    printf("%s\n", a);
}

void bar() {   // Any arguments!
}

void baz(void) {   // No arguments
}


foo("hello");
// foo("hello", "world");  // compilation error
bar(1, 2, 3, "wow");
baz();
// baz(1, 2, 3);  // compilation error
```

### Неявное объявление

Конспект Кристины: 
В стандарте написано, что если функция не найдена, то добавь объявление функции неявно, считая, что она 
возвращает `int` и принимает сколько угодно аргументов. Поэтому, например, можно использовать `printf` без 
`#include`. А стандартная библиотека линкуется автоматически к exe-шнику (можно добавить флаг и отключить 
эту возможность).

На всех современных компиляторах это будет предупреждением. В C11 это запретили, но GCC пофиг, все еще считает 
это просто предупреждением.
```c
// #include <stdio.h>
/*

f(1, 2, 3, 4, "foo") --> int f();
void *malloc() vs int malloc().
gcc thinks it's ok in C11 still
*/

int main(void) {
    printf("2 + 2 = %d\n", 4);  // Works: implicitly adds `int printf();`
    // botva(1, 2);  // Error in C99, link error in C89 (or GCC with C99+)
}
```

Можно даже так:

```c
/* int */ x = 10;
/* int */ main() { printf("Hello World %d\n", x); }
```

Билет:

Если вызвать неизвестную функцию `foo(args)`, 
неявно объявится `int foo()`, 
поэтому `printf("2 + 2 = %d\n", 4)` работает без инклюдов 
(неявно объявляется `int printf()`)

В C99 так формально уже делать нельзя, но с GCC работает

`malloc` без инклюдов объявится как `int malloc()`, 
хотя должен быть `void *malloc()` - плохо

## <a name="tag3"></a> Объявления своих структур (везде нужно `struct`), конвенция с `typedef`

Структуры в Си - просто набор публичных полей.

Нет конструкторов, деструкторов, методов, 
нет private/protected, все структуры trivially copyable, 
нет инициализации полей

Структуры имеют тип `struct <название>`, а не `<название>` как в C++

```c
struct Foo {};
...
struct Foo foo;
```

Можно писать `typedef`
```c
struct Foo {};
typedef struct Foo Foo;
...
Foo foo;
```

Можно писать `typedef` сразу
```c
typedef struct Foo {} Foo;
...
Foo foo;
```

Можно писать `typedef` для указателя
```c
struct Foo {};
typedef struct Foo Foo, *PFoo;
...
Foo foo;
PFoo pFoo = &foo;
```

Обычно используем с `typedef` сразу
```c
typedef struct Foo{
    int x; //нет конструкторов, деструкторов, НИЧЕГО, переменные не проинициализировать по умолчанию
    int y; //структура trivially copyable
} Foo, Foopointer; // чтобы не писать struct Foo при объявлении сразу пишем так, все оки
Foo foo;
Foopointer foo2 = &foo;
```

Вместо методов
```c
int point_dist(const struct Point *p) {
    return p->x * p->x + p->y * p->y;
}
```

## <a name="tag4"></a> Выделение и освобождение памяти через `malloc`/`free` против `new`/`delete`

`malloc` возвращает `void*` который потом неявно кастуется к нужному типу.
В случае неудачи возвращает NULL/0. Чтобы занулить память можно использовать `calloc`.

```c
int *a = malloc(5 * sizeof(int)); // int array
free(a);

int *b = malloc(sizeof(int)); // int var
free(b);
```

`new` разделяет выделение массива и простой переменной. Выравнивание подбирается под тип.
В случае неудачи кидает исключение (или возвращает `nullptr` если указать `std::nothrow`)

```cpp
int *a = new int[5]; // int array
delete[] a;

int *b = new int(); // int var
delete b;
```

## <a name="tag5"></a> Отличия между Си и C++ в преобразованиях `void*`

В С `void*` приводится неявно к любому указателю, 
в C++ требуется явно приводить `void*` к указателю на нужный тип

```cpp
int *a = malloc(sizeof(int)); // C
// int *b = (int*)malloc(sizeof(int)); // Bad C
int *c = static_cast<int*>(malloc(sizeof(int))); // C++
// int *d = malloc(sizeof(int)); // Bad C++
```

## <a name="tag6"></a> Альтернативы для языковых возможностей

### Unnamed namespace

Ключевое слово `static` указывает что объект компилируется 
с internal linkage, т. е. он будет доступен 
только в своей единице трансляции.
```c
static void foo() {}
static int x;
```

### `*_cast<>`

C-style cast `(int)a`

Может быть опасно, т. к. во многих случаях нет проверки на совместимость типов,
при приведении отбрасываются квалификаторы. Подробнее на 
[cppreference](https://en.cppreference.com/w/c/language/cast).

### `int a{}`

`int a = 0;`

### Namespace
  
Дописывать название библиотеки в начале всех функций

Например `very_convenient_json_parser_lib_parse_json`

### Ссылки

Живем с указателями

### `bool`

`int` со значениями 1/0 или `_Bool` из `<stdbool.h>`

### Операторы копирования и перемещения

`memcpy` копирует побайтово

### Шаблоны

Использовать `void*` для переменных неизвестного типа (24 лекция, 1:22:20)

Пример:
```c
#include <stdio.h>

void for_each(void *begin, void *end, int elem_size, void (*f)(const void*)) {
    for (; begin != end; begin = (char*)(begin) + elem_size) {
        f(begin);  // Can only pass element by pointer
    }
}

void print_int(const void *x) {
    printf("%d ", *(int*)x);
}

int main(void) {
    int arr[10] = {};
    arr[3] = 100;
    arr[5] = 200;
    for_each(arr, arr + 10, sizeof(arr[0]), print_int);
}
```

Еще бывают макросы (не углублялись)

## <a name="tag7"></a> Отсутствующие возможности: перегрузка функций и параметры по умолчанию, `std::vector`, исключения

Их нет. Deal with it.

Для перегрузки функций можно посмотреть `_Generic` (не углублялись)

Параметры по умолчанию можно сделать макросами (не углублялись)

Обработка ошибок через return code: 
функции возвращают int с кодом ошибки, их можно обработать. 
Если нужно еще что-то вернуть, одним из параметров передают указатель на аргумент, 
в который записывается результат.

