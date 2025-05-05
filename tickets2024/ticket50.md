## Билет 50. Trivially Copyable
### Существование разных кодировок строк, разного порядка байт в числах
Лекция Кристины 22.md
#### Байты

Практически все языки и библиотеки оперируют байтами (_октет_).

Байт – число от 0 до 255, где-то от -128 до 127.
Нельзя смотреть внутрь байта, только арифметические операции (деление на два, взятие по модулю).

Интерпретация байт – вопрос творческий и вопрос договоренности.

Отрицательные числа хранятся по модулю 256 (two complement).

* -128 = 128 (mod 256)
* -1 = 255 (mod 256)

Хранятся многобайтовые числа в двух форматах:

* _little endian_ (x86): a[0] + 256 * a[1] (младший байт имеет меньший адрес) – так работает процессор
* _big endian_ ("network byte order"): a[1] * 256 + a[0] (младший байт имеет больший адрес) – записаны так, как записано
  число в десятичной системе счисления

Нужно понимать, как записаны байты в файлах. Иногда, кстати, возникает mix ending.

#### Строки

Все просто отвратительно, потому что символов не существует.

Самый простой случай – один байт = один символ – 1251, 866, KOI8-R. Главная проблема – все они разные (обычно, кстати, с латиницей особо проблем нет). Поэтому очень сильно зависит от того, как читать файл. При этом нигде нет указания, в какой кодировке все записано.

Затем возник Unicode: символов нет, есть code point – одно число от 0 до ~60000. Кодируются с помощью code units (~
байты).

Есть еще grapheme clusters

Проблемы возникли с эмодзи, которые представляют собой несколько code points. Например, "[flag A][flag U]" (Australian
flag) results in "[flag U][flag A]" (Ukranian flag).

Некоторые символы имеют разные значения в разных языках, например:

- Турецкая i – это маленькая "latin i", но большая İ, это не I. Еще есть ı, но не i.
- Немецкая буква eszett (ß) – это один code point, но при сортировке стоит воспринимать как "ss" (поэтому нельзя даже
  сказать, что строки равны, если длина равна, хе-хе).

При этом строку нельзя прочитать в каком-то формате, обычно она закодирована как-то, нет канонического вида для всех ЯП.

Например, на mac иногда буква Ё – это два code point.

Самая популярная кодировка – UTF-8:

- ASCII для латинице.
- Несколько байт для не латинских code point.
- в Python и Rust есть два типа: строка как последовательность Unicode code points (str) и строка как последовательность
  байт (bytes). В C++ такого нет :(

Legacy:

- `const char*` и `std::string` могут быть чем угодно.
- `const wchar_t*` и `std::wstring` могут быть UTF-16 или что-то еще, что использует два байта для code points.
- Поэтому _нужно_ знать, что за кодировка используется для сохранения строк.

\n in C++ - number 10.

Windows: newline: number 13 ('\r'), number 10 ('\n'). Carriage return + new line, like in typewriter Linux, modern
macOS: number 10. Old macOS: number 13.

C++, Python...: File in "text mode": replace 13 with: 13 or 13+10 or 10. File in "binary mode": write 13 as-is

### `reinterpret_cast`, strict aliasing rule и его нарушения в C++, корректное преобразование между `int/float` на уровне байт
* **`reinterpret_cast`** - штука, которая берет указатель (например на `int`) и теперь это будет точно такой же указатель, но с другим типом (например на `char *`). Даже несовместимые типы будут преобразовываться побайтово, эта штука доверяет программисту и тому, что он делает. Это не совсем сишный каст (так как сишный каст это некоторый перебор кастов). 
```c++
#include <cstddef>
#include <iostream>

int main() {
    int x = 123456;
    char *xc = reinterpret_cast<char*>(&x);
    // Becomes the following:
    // char *xc = static_cast<char*>(static_cast<void*>(&x));
    for (std::size_t i = 0; i < sizeof(x); i++) {
        std::cout << static_cast<int>(xc[i]) << "\n";
    }
}
```
В данном случае выведется 64 -30 1 0 (все в 16-ричной системе и с [little-endianess](https://en.wikipedia.org/wiki/Endianness)), как ни странно: `1 * 65536 + 226 * 256 + 64 = 123456. \
Однако, чтобы все работало и не было UB есть некоторые правила.
* [**Strict aliasing rule**](https://en.cppreference.com/w/cpp/language/reinterpret_cast): \
Можно через указатель p типа T1 обращаться к объекту типа T2 только если:
    1. T1 == T2 (но T1 может быть более const)
    2. T1 — базовый класс T2 \
    ...
    10. T1 == char, unsigned char, std::byte \
Остальные не разбирали, можно ознакомиться по ссылке выше.
* **Нарушения этого правила**:
```c++
#include <iostream>

int main() {
    float f = 1.0;
    static_assert(sizeof(float) == sizeof(int));

    int *x = reinterpret_cast<int*>(&f);
    std::cout << std::hex << *x /* UB */ << "\n";
}
```
`T1 == int`, `T2 == float` - нарушение.
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
В данном случае компилятор не ожидает, что `int *a` и `float *b` могут указывать в один кусок памяти, поэтому соптимизирует и вернет просто 10 (не будет читать из памяти). Во втором случае мы нарушили правило и теперь это UB. Вывод во втором случае будет: `res = 10` (из-за оптимизации), `a = UB`.

Еще одно нарушение: неверно предполагать, что если мы любой объект можем скастовать к `char *` и обратно (все корректно):
```c++
#include <cstddef>
#include <iostream>

int main() {
    int x = 123456;
    char *xc = reinterpret_cast<char*>(&x);
    static_assert(sizeof(int) == 4);

    xc[0] = 10;
    xc[1] = 11;
    xc[2] = 12;
    xc[3] = 13;
    std::cout << std::hex << x << "\n";
}
```
То `char *` можно скастовать к любому объекту (некорректно):
```c++
#include <cstddef>
#include <iostream>

int main() {
    char xc[] = {10, 11, 12, 13};
    static_assert(sizeof(int) == 4);

    int *xptr = reinterpret_cast<int*>(xc);
    std::cout << std::hex << *xptr /* UB */ << "\n";
    // T1 == int, T2 == char. Нельзя.
}
```
* **корректное преобразование между `int/float` на уровне байт** \
В одном из примеров выше было некорректное преобразования. Однако эту операцию можно сделать корректно, сделав преобразования по байтам (в C аналог `std::memcpy`, а в C++20 появилось `std::bit_cast<>`):
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

### Trivially Copyable структуры: определение, пример, использование для (де)сериализации
Trivially Copyable - это такие скалярные типы (`int`, `float`, `pointers`) и классы:
* Тривиальный (ничего не делает) деструктор - для класса, для базовых классов, для полей
* Хотя бы один конструктор/оператор копирования/перемещения
* Все копирующие/перемещающие кострукторы/операторы были тривиальными - для класса, для базовых классов, для полей
* Нет виртуальных функций и виртуального наследования 

Конструкторы и методы разрешены.

В стандарте есть проверка: `std::is_trivially_copyable<>`. 

Если структура trivially copyable, то можно ее побайтого скопировать в другую структурку и у неё скопируются все поля. \
Также можно использовать такую структурку для сериализации/десериализации (записать байты в файл, затем прочитать из файла).

Пример:
```c++
#include <iostream>
#include <fstream>
#include <type_traits>

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

### Padding (выравнивание) и его отключение, последствия хранения невыровненных нетривиальных типов вроде `vector<>`
Про паддинг подробнее можно почитать [в билете первого семестра](https://github.com/hse-spb-2021-cpp/questions-a/blob/master/tickets/ticket40.md#расположение-подобъектов-в-памяти-и-padding-пустое-место-для-выравнивания). Вот тот же пример паддинга:
```c++
struct MyTriviallyCopyable {
    int x = 10;
    char y = 20;
    // Compiler may add padding: 3 bytes so 'z' is 4-bytes aligned.
    float z = 30;
};
```
А пока поймем, что паддинг можно отключать:
```c++
#include <iostream>
#include <fstream>

#pragma pack(push, 1)
// Padding is disabled. We still have troubles with:
// 1. Endianness: order of bytes inside 'int'. E.g. x86 is "little-endian", some are "big-endian": old Mac, some 'mips' routers (not 'mipsel').
// 2. Not all CPUs may read unaligned memory: ""Anybody who writes #pragma pack(1) may as well just wear a sign on their forehead that says “I hate RISC”: https://devblogs.microsoft.com/oldnewthing/20200103-00/?p=103290
// 3. Exact sizes of int/float (use std::uint8_t and stuff).
struct MyPod {
    int x = 10;
    char y = 20;
    float z = 30;
};
#pragma pack(pop)

int main() {
    MyPod p;
    std::ofstream f("02.bin", std::ios_base::out | std::ios_base::binary);
    // Not UB.
    f.write(reinterpret_cast<const char*>(&p), sizeof(p));
}
```
`#pragma pack(push, 1)` говорит положи на текущий стек настройки выравнивания и теперь мы выравниваем все по одному байту. `#pragma pack(pop)` говорит забудь, что мы там клали и верни старые настройки. Теперь структурка занимает 9 байтов. Это нестандартное расширение, но его поддерживают большинство компиляторов.

Когда не надо так делать: когда в структуре сложные типпы, например вектор. Вектор может не знать, что он невыровнен и поэтому могут сломаться какие-то инварианты. Гарантии, что расположенные в любом месте памяти вектор корректно работает, нет! Ну и вторая причина: это делать бесполезно, многие сложные типы не trivially copyable, то есть даже в файл не записать.
```c++
#include <vector>
#include <iostream>

#pragma pack(push, 1)
struct Foo {
//    char x;
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

### Особенности взятия ссылок и указателей на невыровненную память (пример со `swap` полей)
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
    [[maybe_unused]] int *aptr = &s.a;  // Unaligned pointer to `int`: UB, no undefined sanitizer warning :(
    *aptr = 40;
}
```
Небольшое замечание `allignment` - это факт/процесс выравнивания полей по нужным границам в памяти, `padding` - сами байты для выравнивания. 

Пояснение к коду выше: когда мы обращаемся к полю в структуре, у которой отключено выравнивание (`s.a = 30`), то компилятор понимает, что поля невыравнены и либо корректно обрабатывает, либо говорит, что не умеет такое обрабатывать.

С ссылками сложнее, процессор не понимает выровнено/невыровнено: ссылка подразумевает, что адрес делится на `allignment` этого типа, а в невыровненных данных такого не происходит, как итог - UB. В `swap` как раз это и стреляет - он принимает аргументы по ссылкам. Указатели на невыровненную память тоже не поддерживаются.

### Standard Layout и Plain Old Data (POD) структуры: определение из C++11
[https://habr.com/ru/articles/532972/] - все на хабре (но отзыв -14:/)
**Standart Layout** - это типы, которые предназначены для взаимодействия с другими языками (например C). Означает, что у типа очень простое расположение полей в памяти.

Требования:
* Нет виртуальных функций и виртуального наследования
* Нельзя использовать класс в качестве родителя несколько раз
* Все нестатические поля во всей иерархии должны быть:
    * Объявлены в одной структуре
    * Одни и те же права доступа у всех
    * Они тоже Standart Layout

И вот пример:
```c++
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
```
Тут понятно из названия: первая - да, во второй все поля только там и одинаковый доступ - да, в третьей - да, в четвертой новая переменная - уже нет, в пятой все поля имеют разный доступ - нет.

**POD (Plain Old Data)** = StandardLayout && TriviallyCopyable. \
Как раз тот набор структур, который можно было написать в C.
### Лекции Егора
* [Reinterpret_cast, Strict aliasing rule](https://youtu.be/2tTybSDfC2I?list=PL8a-dtqmQc8ph7sgkWKlgoAHIw2WIaNDZ&t=5356)
* [Trivially Copyable](https://youtu.be/2tTybSDfC2I?list=PL8a-dtqmQc8ph7sgkWKlgoAHIw2WIaNDZ&t=6825)
* [Padding и его отключение](https://youtu.be/slBDUFiJ4vg?list=PL8a-dtqmQc8ph7sgkWKlgoAHIw2WIaNDZ&t=1088)
* [Standart Layout, POD](https://youtu.be/slBDUFiJ4vg?list=PL8a-dtqmQc8ph7sgkWKlgoAHIw2WIaNDZ&t=1759)
