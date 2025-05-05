## Билет 51. Многомерные массивы

### C-style-arrays/массивы в стиле Си

* Инициализация, невозможность копирования, связь с арифметикой указателей

Дефолтная инициализация
```c++
    // "C-arrays", "plain arrays"
    [[maybe_unused]] int var1, arr[2 * N], var2;
    // 10 uninitialized ints one after another. внутри мусор
    // Automatic storage duration
    // Size should be a compile-time expression, strictly greater than 0
```
Получать размер массива можем вот так(работает на этапе компиляции)
```c++
     assert(std::size(arr) == 10);  // C++-style
    assert(sizeof(arr) == 10 * sizeof(int));  // C style
    assert(sizeof(arr) / sizeof(arr[0]) == 10);  // C style
    arr[2] = 123;
    assert(arr[2] == 123);
```
Нету операторо присваивания и копирования
```c++
    // No push_back/pop_back/insert/operator=/copying/...
    // int arr2[2 * N] = arr;  // Just does not compile

    // range-based-for is ok, though
    for (int &x : arr) {
        x = 10;
    }
```
Но можем вставить массивы в структуру. И тогда уже копирование работает как ожидается.
```c++
const int MAX_N = 10;
struct Points {
    int n;
    std::array<int, MAX_N> xs{};  // Same as if 10 fields are defined.
    std::array<int, MAX_N> ys{};
};

int main() {
    Points a, b;
    std::cout << sizeof(a) << " " << sizeof(int) * (1 + 2 * MAX_N) << "\n";
    assert(sizeof(a) >= sizeof(int) * (1 + 2 * MAX_N));
    a.n = 1;
    a.xs[0] = 10;
    a.ys[0] = 20;
    
    b = a;  // Can reassign structs with arrays inside.
    assert(b.n == 1);
    assert(b.xs[0] == 10);
    assert(b.ys[0] == 20);
}
```
Можно просто использовать дефолтную инициализацию (В случае интов(и других примитивных типов), они будут не проинициализированы, поэтому образаться к ним UB. А у строк есть дефолтный конструктор, так что с ними все в порядке). {} делает value initialization, то есть для интов, заполним массив нулями.
Можно проинициализировать явно первые несколько элементов синтаксисом {1, 2, 3}. Не указанные элементы будут 0-ями, если мы задали размер в квадратных скобах. Если же размер мы не задали, то будет столько элементов, сколько мы указали внутри фигурных скобок.
```c++
    [[maybe_unused]] int a1[10];  // 10 default-initialized ints: uninitialized
    [[maybe_unused]] std::string a2b[10];  // constructors are still called
    [[maybe_unused]] int a2[10] = {};  // value-initialize all elements: 0.
    [[maybe_unused]] int a3[10]{};  // same

    [[maybe_unused]] int a4[10] = {1, 2, 3};  // value-initialize all elements
                                              // except first three (copy-initialized)
    int a5[10]{1, 2, 3};  // same, copy-initialized
    assert(a5[0] == 1);
    assert(a5[1] == 2);
    assert(a5[2] == 3);
    assert(a5[3] == 0);
    assert(a5[4] == 0);

    [[maybe_unused]] int a6[10] = { 0 };  // 0, 0, 0, 0, ...
    [[maybe_unused]] int a7[10] = { 1 };  // 1, 0, 0, 0, ...
    [[maybe_unused]] int a8[] = {1, 2, 3};  // size == 3
```
Арифметика указателей работает интуитивно понятно. Можем записвать себе указатель на любое место массива, прибавлять и вычитать из него. Разыменовывать указатели, которые выходят за границы UB. В последнем примере с sort автоматически data распадается до указателя.
```c++
    int data[] = {1, 2, 3, 4, 5};
    std::cout << std::size(data) << "\n";

    int *dptr = data;  // array-to-pointer decay, implicit
    std::cout << &data[0] << " " << &data << " " << dptr << "\n";  // All the same
    std::cout << data[0] << " " << *dptr << "\n";  // 1
    std::cout << data[2] << " " << *(dptr + 2) << "\n";  // 3
    dptr += 2;
    std::cout << data[1] << " " << *(dptr - 1) << "\n";  // 2

    std::sort(std::begin(data), std::end(data));
    std::sort(&data[0], &data[4] + 1);
    std::sort(data, data + 5);

    // std::cout << *(dptr - 3) << "\n";  // UB, out of bounds
    // std::cout << *(dptr + 3) << "\n";  // UB, out of bounds
```

* Расширение компилятора variable-length-arrays (VLA)

VLA нет в C++, но clang работает(то есть это некое расширение плюсов). Идея в том, что можно сосздать массив произвольного размера, который станет известен только по время исполнения. Он заводится на стеке(то есть быстрее чем на куче через std::vector или new), поэтому нужно учитывать, что стек может переполнится. Нельзя использовать шаблонную функцию std::size() (просто не скомпилится). В остальном, работают как обычные массивы.

```c++
    // GCC/Clang extension in C++ (available in C99): Variable Length Array (VLA).
    // Not available in other compilers.
    // https://gcc.gnu.org/onlinedocs/gcc/Variable-Length.html
    int n;
    std::cin >> n;
    int vla[n];  // Allocated "on stack"
    std::cout << sizeof(vla) / sizeof(vla[0]) << "\n";
    // std::cout << std::size(vla) << "\n";  // Does not compile because there is no exact "type" for VLA to instantiate std::size with.
    for (int i = 0; i < n; i++) {
        vla[i] = i * i;
    }
    for (int x : vla) {
        std::cout << " " << x;
    }
    std::cout << "\n";
```
* Динамическое выделение/освобождение при помощи new[]/delete[]

Массивы можно выделять на куче, хотя идейно это не хочется делать никогда, лучше использовать std::vector. Обычно используется только для написания библиотек. Можно оборачивать в умные указатели. Будет на один уровень оборачивания указателей меньше, чем если использовать умные указатели с std::vector.

```c++
    int n;
    std::cin >> n;

    std::string *strs = new std::string[n];  // Array of 'n' std::string's,
    // Heap-allocated, dynamic storage duration.
    // Also possible: std::unique_ptr<std::string[]>, shared_ptr<std::string[]>
    // Note [] so they call delete[] instead of delete.
    // Better, although adds level of indirection: shared_ptr<vector<string>>

    strs[0] = "hello world, this is the first string in the array";
    strs[1] = std::string(1000, 'x');
    for (int i = 0; i < n; i++) {
        std::cout << i << ": " << strs[i] << "\n";
    }
    delete[] strs;  // delete[], not just delete!

    // Inferior to std::vector, please never use.
```
### Тип "массив известного размера", тип "массив неизвестного размера", тип "указатель на массив", auto, автовывод размера в шаблонах

Если порсто передать массив, то мы внутри функции не будет знать его размер. В этом случае массив автоматически распадается в указатель на первый элемент, то есть int arr[] == int \*arr. То есть число, записанное в квадратных скобках не имеет никакого значения(можно передать массив большего или меньшего размера). Поэтому принято передавать вместе с указателем на массив его размер в отдельном аргументе. Поскольку arr это указатель, std::size не работает, а sizeof вернёт размер int*. В C++ можно передавать массив по ссылке. В этом случае можно будет передать массив только конткретного размера. К такому варианту можно добавить шаблон, и тогда уже передавать массив произвольного размера по ссылке.
```c++
#include <array>
#include <iostream>
#include <iterator>
#include <cstddef>

// void foo(int arr[]) {  // Actually void foo(int *arr)
// void foo(int arr[5]) {  // Actually void foo(int *arr)
void foo(int arr[15]) {  // Actually void foo(int *arr)
    // Careful: sizeof pointer
    std::cout << "arr[0] = " << arr[0] << ", sizeof = " << sizeof(arr) << "\n";
}

void foo_good(int arr[], int arr_len) {
    std::cout << "arr[0] = " << arr[0] << ", len = " << arr_len << "\n";
}

void foo_cpp_wtf(int (&arr)[10]) {  // C++-only, ok
// void foo_cpp_wtf(int (&arr)[5]) {  // C++-only, compilation error if size mismatched
// void foo_cpp_wtf(int (&arr)[15]) {  // C++-only, compilation error if size mismatched
    std::cout << "arr[0/10] = " << arr[0] << "\n";
}

template<std::size_t N>
void foo_cpp_templ(int (&arr)[N]) {  // Just like std::size
    std::cout << "N = " << N << ", last = " << arr[N - 1] << "\n";
}

void foo_cpp_arr(std::array<int, 10> arr) {  // C++-only, copies
    std::cout << "arr[0/10] = " << arr[0] << "\n";
}

int main() {
    int arr[10]{1, 2, 3};
    foo(arr);
    foo_good(arr, sizeof arr / sizeof arr[0]);  // C style.
    foo_cpp_wtf(arr);
    foo_cpp_templ(arr);
    {
        std::array<int, 10> arr2{1, 2, 3};
        foo_cpp_arr(arr2);
    }
}
```
### Многомерные массивы, в том числе с неизвестным нулевым измерением

В Си много мерным массивом называется следующая конструкция. По сути мы просто выделяем 3x4x5 подряд идущих элементов, и с помощью синтакцисечкого сахара можем обращаться к ним с помощью трёх скобок.
Передавать в функцию можем по разному. Например, указатель на ```int[4][5]```. Остальные варианты работают так же. То есть мы передаем указатель, но уже тип указателя, а именно ```int[4][5]``` строгий, и сним никакого преобразования из массива в указатель не происходит. На плюсах можно опять же воспользоваться щаблонами, и передавать ссылку на массив. 
```c++
#include <iostream>

using arr45 = int[4][5];

// All the same:
// void foo(int arr[3][4][5]) {
// void foo(int arr[][4][5]) {
// void foo(int (*arr)[4][5]) {  // What if: (*arr) --> *arr
void foo(arr45 *arr) {
    std::cout << arr[1][2][3] << "\n";
    // static_cast<int*>(arr) + 1 * (4 * 5) + 2 * 5 + 3
}

template<std::size_t N, std::size_t M, std::size_t K>
void foo2(int (&arr)[N][M][K]) {
    std::cout << arr[1][2][3] << "\n";
    std::cout << N << " " << M << " " << K << "\n";
}

int main() {
    int arr[3][4][5]{};
    std::cout << sizeof(arr) << "\n";
    // Consecutive elements.
    std::cout << &arr[0][0][0] << "\n";
    std::cout << &arr[0][0][1] << "\n";
    std::cout << &arr[0][0][2] << "\n";
    std::cout << &arr[0][0][3] << "\n";
    std::cout << &arr[0][0][4] << "\n";
    std::cout << &arr[0][1][0] << "\n";
    std::cout << &arr[0][1][1] << "\n";
    arr[1][2][3] = 123;
    // arr[1, 2, 3] = 123;  // Not like in Pascal, see operator,
    foo(arr);
    foo2(arr);
}
```
### Массивы массивов как многомерные массивы: выделение и освобождение за константное количество операций, использование

Другой вариант, это конструкция из нескольких уровней указателей. На примере. Для трёх слоев, нам руками надо выделить себе массив из двухуровневых указателей, потом на каждый выделить массив из указателей int \*, и потом каждый последний слой опять же выделить отдельно. В коде все проще понять, чем на словах. После использования, обязаны всю выделенную память освободить. 
```c++
#include <iostream>

void bar(int ***arr2) {
    std::cout << arr2[1][2][3] << "\n";
}

int main() {
    int arr[3][4][5]{};
    std::cout << sizeof(arr) << "\n";

    // int ***arr2 = arr;  // Not the same!
    int ***arr2 = new int**[3];  // Extra memory for pointers
    for (int i = 0; i < 3; i++) {
        arr2[i] = new int*[4];  // More extra memory for pointers
        arr2[i][0] = new int[5]{};
        arr2[i][1] = new int[5]{};
        arr2[i][2] = new int[5]{};
        arr2[i][3] = new int[5]{};
    }
    arr2[1][2][3] = 123;

    /*
     int***         int**
    +------+     +---------+---------+---------+
    | arr2 | --> | arr2[0] | arr2[1] | arr2[2] |
    +------+     +----|----+---------+---------+
             +--------+
             v               int*
    +------------+------------+------------+------------+
    | arr2[0][0] | arr2[0][1] | arr2[0][2] | arr2[0][3] | (x3)
    +------|-----+-------|----+------------+------------+
           |             +-------+
           v   int               v    int
        +---+---+---+---+---+  +---+---+---+---+---+
        |000|001|002|003|004|  |010|011|012|013|014|  (x3 x4)
        +---+---+---+---+---+  +---+---+---+---+---+
    */
    std::cout << "=====\n";
    // Elements in the same "line" are consecutive
    std::cout << &arr2[0][0][0] << "\n";
    std::cout << &arr2[0][0][1] << "\n";
    std::cout << &arr2[0][0][2] << "\n";
    std::cout << &arr2[0][0][3] << "\n";
    std::cout << &arr2[0][0][4] << "\n";
    // Lines are independent
    std::cout << &arr2[0][1][0] << "\n";
    std::cout << &arr2[0][1][1] << "\n";
    bar(arr2);

    for (int i = 0; i < 3; i++) {
        delete[] arr2[i][0];
        delete[] arr2[i][1];
        delete[] arr2[i][2];
        delete[] arr2[i][3];
        delete[] arr2[i];
    }
    delete[] arr2;
}
```

### std::array<> для упрощения операций с массивами

Если вы хотите использовать массивы, удобнее использовать array. Он появился в 11 стандарте, то есть некая новомодная штука. Можно копировать, и создавать массивы нулевого размера. Есть некоторые методы, например arr.size().

```c++
     // C arrays with C++ syntax, more consistent, strictly better
    [[maybe_unused]] std::array<int, 5> a1;  // default-initialized;
    [[maybe_unused]] std::array<int, 5> a2 = {1, 2, 3};  // 1, 2, 3, 0, 0
    assert(a2.size() == 5);  // Has some methods

    [[maybe_unused]] std::array<int, 5> a3 = a2;  // can copy

    std::array<int, 0> a4;  // Can be empty
    assert(a4.begin() == a4.end());
    assert(a4.empty());
```

[лекуия](https://youtu.be/bY-xdk1BgeI?t=581)

