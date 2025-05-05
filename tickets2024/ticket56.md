## Билет 56. Взаимодействие с библиотеками на Си

## Содержание

* [Хранение строк в POD: `char[]`, `char*`, `std::string`](#tag1)
* [Необходимость вручную выделять буфер под строку, небезопасность `gets`](#tag2)
* [Конвенции выделения ресурсов и opaque-структуры: выделение пользователем (строки), выделение библиотекой (`fopen`)](#tag3)
* [`const_cast`](#tag4)
* [`extern "C"` и линковка программ на Си/C++, линковка со стандартной библиотекой C++, совместимые между Си/C++ заголовки](#tag5)

## <a name="tag1"></a> Хранение строк в POD

POD = Plain Old Data = StandardLayout + TriviallyCopyable

Структура или класс являются TriviallyCopyable если их конструкторы, деструкторы и операторы присваивания 
тривиальные. Простыми словами: если можно побайтово скопировать все поля и получить корректный объект.

StandardLayout - см. [cppreference](https://en.cppreference.com/w/cpp/language/classes#Standard-layout_class)

* has no non-static data members of type non-standard-layout class (or array of such types) or reference,
* has no virtual functions and no virtual base classes,
* has the same access control for all non-static data members,
* has no non-standard-layout base classes,
* has all non-static data members and bit-fields in the class and its base classes first declared in the same class, and
* given the class as S, has no element of the set M(S) of types as a base class, where M(X) for a type X is defined as:
  * If X is a non-union class type with no (possibly inherited) non-static data members, the set M(X) is empty.
  * If X is a non-union class type whose first non-static data member has type X0 (where said member may be an anonymous union), the set M(X) consists of X0 and the elements of M(X0).
  * If X is a union type, the set M(X) is the union of all M(Ui) and the set containing all Ui, where each Ui is the type of the ith non-static data member of X.
  * If X is an array type with element type Xe, the set M(X) consists of Xe and the elements of M(Xe).
  * If X is a non-class, non-array type, the set M(X) is empty. 

Грубо говоря, все поля должны быть с одним уровнем доступа (public/protected/private), 
и должны быть впервые объявлены в одном классе. И еще несколько требований. Общий смысл: StandardLayout класс 
совместим с другими языками программирования типа Си. Можно проверить с помощью `std::is_standard_layout`.


### `char[]`

```cpp
struct Person {
    char first_name[31]{};
    char last_name[31]{};
};
```

Работает, как и предполагается

См. [пример1](https://github.com/hse-spb-2021-cpp/lectures/blob/master/20-220221/02-trivially-copyable-strings/01-c-str-arr-pod.cpp),
[пример2](https://github.com/hse-spb-2021-cpp/lectures/blob/master/20-220221/02-trivially-copyable-strings/01-c-str-arr-pod-read.cpp),
[лекция с таймкодом](https://youtu.be/slBDUFiJ4vg?t=2070)

### `char*`

```cpp
struct Person {
    const char *first_name;
    const char *last_name;
};
```

Формально POD, по факту сохраняются не строки, а указатели на память, поэтому так делать бесполезно.

См. [пример](https://github.com/hse-spb-2021-cpp/lectures/blob/master/20-220221/02-trivially-copyable-strings/02-c-str-ptr-bad-pod.cpp),
[лекция с таймкодом](https://youtu.be/slBDUFiJ4vg?t=2295)

### `std::string`

```cpp
struct Person {
    std::string first_name;
    std::string last_name;
};
```

НЕ TriviallyCopyable --> НЕ POD.

Работает с короткими строками из-за реализации std::string: 
короткие строки сохраняются внутри `std::string`, 
а длинным выделяется память на куче. Что считается длинной строкой - зависит от реализации.

См. [пример](https://github.com/hse-spb-2021-cpp/lectures/blob/master/20-220221/02-trivially-copyable-strings/03-string-bad-pod.cpp),
[лекция с таймкодом](https://youtu.be/slBDUFiJ4vg?t=2405)

## <a name="tag2"></a> Необходимость вручную выделять буфер под строку, небезопасность `gets`

### Выделение буфера

Возьмем функцию `strcat`, которая конкатенирует две строки. 
Ей нужно передавать буфер, куда поместится результат, иначе УБ. Но
почему нелзя просто выделять память внутри функции? Тогда нужно результат 
записывать в аргумент типа `char**` (указатель на Си-шную строку). Значит первая 
переданная строка должна быть выделена через `malloc`, что странно, но пока нормально. 
Хуже - если передать указатель не на начало строки, то внутри функции произойдет УБ, 
так как попытаемся этот указатель освободить.

См. [пример](https://github.com/hse-spb-2021-cpp/lectures/blob/3f8756da875f2ff18bb212000f866332895268b3/25-220418/03-c-str/05-why-not-alloc.c),
[лекция с таймкодом](https://youtu.be/tqLVPeZirNI?t=5220)

### Небезопасность `gets`

`gets` считывает до перевода строки, даже если не хватает буфера. Получаем buffer overflow. 
Эту уязвимость можно эксплуатирровать, [подробнее в видео](https://ulearn.me/course/hackerdom/Perepolnenie_steka_3bda1c2c-c2a1-4fb0-9146-fccc47daf93b).

Лучше использовать `fgets`, этой функции нужно указать максимальную длину буфера +она гарантировано ставит `\0` в конце.

## <a name="tag3"></a> Конвенции выделения ресурсов и opaque-структуры: выделение пользователем (строки), выделение библиотекой (`fopen`)

[лекция с таймкодом](https://youtu.be/tqLVPeZirNI?t=5488)

### Конвенции работы с ресурсами в языке Си 

* Все можно занулить и это будет корректная инициализация, 
в крайнем случае есть специальная функция для инициализации
* Все (или почти все) можно скопировать/переместить побайтово
* Всей работой с памятью занимается либо только пользователь, либо только библиотека
  * В первом случае пользователь сам выделяет буферы нужной длины (массивы `char`-ов, `char` представляет один байт). 
  Указатели на буферы передаются в функции библиотеки и пользователь сам ответственен за то чтобы длины буфера хватило.
  * Во втором случае библиотека выделяет всю необходимую память внутри своих функций, и для очистки памяти 
  нужно вызывать соответствующие функции библиотеки. Подробнее в следующем примере.

### Работа с файлами через `fopen`

Чтобы спрятать от пользователя логику работы с памятью используются opaque-указатели. 
Они передаются в функции библиотеки, при этом сам пользователь взаимодействовать 
с объектами, на которые указывает указатель не может (по крайней мере не должен).

Например функция `fopen` конструирует объект типа `FILE` и передает 
пользователю opaque-указатель на него. Дальнейшее взаимодействие пользователя 
с этим объектом, как описано выше, ограничивается передачей указателя 
в функции для работы с файлами. Заканчивается передачей в `fclose` для освобождения памяти.

Пример:

```c
#include <stdlib.h>
#include <stdio.h>

int main() {
    FILE *f = fopen("02-opaque-pointers.c", "r");  // "constructor", resources are allocated by the library.

    // Never try to access `FILE`'s fields directly.
    char buf[20];
    fscanf(f, "%20s", buf);  // "method"
    printf("buf=|%s|\n", buf);

    fclose(f);  // "destructor"
}
```

## <a name="tag4"></a> `const_cast`

В библиотеках на Си иногда забывали ставить `const` там где это имело смысл, 
поэтому в C++ есть `const_cast` который позволяет отбросить 
константность. Программист должен быть уверен, что изменений не произойдет, 
а если они все же произойдут, то UB.

Так же иногда используют чтобы сделать неконстантные перегрузки методов:

```cpp
// https://stackoverflow.com/a/123995/767632
template<typename T>
struct my_vector {
    T *data;

    const T &operator[](std::size_t i) const {
        // A lot of 
        // code you 
        // don't want to 
        // copy and paste
        return *(data + i);
    }
    T &operator[](std::size_t i) {
        // OK: we know that `this` is actually non-const.
        return const_cast<T&>(std::as_const(*this)[i]);
    }
};
```

## <a name="tag5"></a> `extern "C"` и линковка программ на Си/C++, линковка со стандартной библиотекой C++, совместимые между Си/C++ заголовки

`extern "C"` указывает, что объект нужно линковать как Сишный объект

### C++ функция --> C

```cpp
// .cpp
#include <iostream>

extern "C" int cpp_main() {
    std::cout << "Hello from C++\n";
    return 0;
}
```

```c
// .c
int cpp_main(void);

int main(void) {
    return cpp_main();
}
```

### C функция --> C++

```c
// .c
int foo(void) {
    int arr[] = { [3] = 123 };  // Use C-specific syntax to ensure we're writing C.
    return arr[3];
}
```

```cpp
// .cpp
#include <iostream>

extern "C" int foo(); // linked as a C function int foo(void)

int main() {
    std::cout << foo() << '\n';
    return 0;
}
```

### Компиляция

```shell
gcc cfile.c -o cfile -c
gcc cppfile.cpp -o cppfile -c
g++ cfile cppfile -o executable 
# или gcc cfile cppfile -lstdc++ -o executable
```

### Совместимые заголовочные файлы

Следующий заголовок можно подключать и в программах на Си, в программах на С++
```cpp
// .h
#ifdef __cplusplus
extern "C" {
#endif

int foo(void);  // Remember to say "no arguments" in C! It's compatible with C++
int my_main();  // Bad style.

// There are other incompatibilities: e.g. default arguments, macros instead of consts, you name it...

#ifdef __cplusplus
}
#endif
```

Параметры по умолчанию не поддерживаются в Си, их нужно оборачивать
```cpp
#ifdef __cplusplus
int foo(int x = 0);
#else
int foo(int x);
#endif
```

Константы нужно писать через макросы
```c
#define SOME_CONSTANT 3
```

Какие либо другие несовместимости нужно оборачивать аналогично параметрам по умолчанию
