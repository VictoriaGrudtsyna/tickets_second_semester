## Билет 32. Гарантии исключений
В C++ функции по-разному ведут себя по отношению к исключениям. Так различаются гарантии исключений.
### Exception safety (безопасность исключений)
* #### `noexcept`/no-throw (отсутствия исключений) 
```c++
void noexcept_guarantee() noexcept {
    assert(std::cout.exceptions() == std::ios_base::goodbit);
    std::cout << "2+2=4\n";
}
```
Гарантируем, что исключений не будет. Поэтому например нельзя выделять память (на самом деле можно, если уверены, что не будет `bad_alloc`). В примере выше проверяем, что `cout` не выкинет исключений (без проверки использовать тоже нельзя).
* #### strong (строгая)
```c++
void strong_guarantee() {
    std::vector<int> data(10);
    do_something();
    std::cout << "2+2=4\n";
}
```
В общем случае, если вылетело исключение, то состояние программы не поменялось вообще никак. То есть все изменения, которые произошли к вылету исключения, откатились назад. \
В basic ниже перед исключением что-то вывелось на экран и это можно посчитать изменением состояния (при этом базовая гарантия только гарантирует, что не поменялся инвариант).

* #### basic (базовая)
```c++
void basic_guarantee_1() {
    std::cout << "2+2=\n";
    std::vector<int> data(10);
    do_something();
    std::cout << "4\n";
}
```
Базовая гарантия - гарантия, что инвариант программы сохранится. В примере выше успеет вывестись "2+2=" и затем вылетит исключение. \
В примере ниже выведется "2+2=4" и затем вылетит исключение.
```c++
void basic_guarantee_2() {
    std::cout << "2+2=4\n";
    std::vector<int> data(10);
    do_something();
}
```

* #### no (отсутствующая)
```c++
void no_guarantee() {
    std::cout << "2+2=";
    int *data = new int[10];
    do_something();
    delete[] data;
    std::cout << "4\n";
}
```
Выделили память, потом вызвали какую-то функцию `do_something()`, почистили память. Если в функции `do_something()` вылетело исключение, то в нашей функции утечка память -> никакой гарантии. \
В общем случае, если может нарушиться инвариант программы и его не восстановить, то гарантии нет.

### Спецификатор `noexcept`, что происходит при выбрасывании исключения
`noexcept` - спецификатор, который подсказывает программисту и компилятору, что исключений в данной функции не будет.
```c++
#include <iostream>

void check_n(int n) {
    if (n < 0) {
        throw 0;
    }
}

void foo() noexcept {  // If exception: std::terminate(), no dtor calls
    std::cout << "foo() start\n";
    int n;
    std::cin >> n;
    check_n(n);
    std::cout << "foo() end\n";
}

int main() {
    try {
        foo();
    } catch (...) {
        std::cout << "Caught\n";
    }
}
```
Если запустить программу с положительным `n`, то все ок, если с отрицательным, то функция начнет выполнять и в момент исключения вызовется `std::terminate`, который в свою очередь вызывает `std::abort` - и там не вызываются деструкторы. 
```c++
#include <iostream>

int remaining = 2;

void maybe_throw() {
    if (!--remaining) {
        throw 0;
    }
}

struct Foo {
    Foo() {
        std::cout << "Foo() " << this << "\n";
    }
    ~Foo() {
        std::cout << "~Foo() " << this << "\n";
        maybe_throw();
    }
};

int main() {
    try {
        try {
            Foo a;
            Foo b;
            Foo c;
            std::cout << "before throw\n";
            throw 1;
            std::cout << "after throw\n";
        } catch (int x) {
            std::cout << "caught " << x << "\n";
        } catch (...) {
            std::cout << "caught something\n";
        }
    } catch (...) {
        std::cout << "caught outside\n";
    }
    // C++03: two exceptions simultanously => std::terminate().
    // C++11: dtors are noexcept, always std::terminate().
}
```
В примере исключение кидается дважды (сначала в main, затем в деструкторе). С `C++03` если при выкидывании одного исключения выкинулось второе, то вызывается `std::terminate`. С `C++11` к этому добавилось `noexcept` у деструкторов по умолчанию.

#### Типичная гарантия для оператора перемещения
не точно просто взял с инета
Оператор перемещения в C++ предназначен для эффективного перемещения ресурсов из одного объекта в другой. Он гарантирует, что исходный объект после перемещения переходит в валидное, но неопределенное состояние. Это означает, что исходный объект может быть использован, но его содержимое непредсказуемо

### Обеспечение базовой гарантии при помощи RAII (практика 16-220124, задание 13-please-use-vector)
принцип/идиома RAII (resource acquisition is initialization). RAII предполагает, что получение ресурса производится при инициализации объекта. А освобождение ресурса производится в деструкторе объекта.
[https://metanit.com/cpp/tutorial/5.28.php]
#### RAII
17.md Кристинины конспекты
_Resource Acquisition Is Initialization_. Захват ресурса есть инициализация.

1) При захвате ресурсов конструктор должен устанавливать инвариант. При невозможности – выбрасывать исключение.
2) Деструктор должен освобождать все ресурсы.

RAII – свойство некоторого класса. Иногда говорят: в коде используются RAII-классы.

```c++
int main() {
    // RAII: Resource Acquisition Is Initialization
    {
        std::vector<int> v(1'000'000);
        // Invariant: v.size() == 1'000'000, memory allocation succeeded.
        v[999'999] = 123;
        // RAII part 1: constructor has to establish invariant and grab resources. Throw an exception if that's impossible. No exit codes.
    }  // RAII part 2: destructor has to free all resource.

    try {
        std::vector<int> v(100'000'000'000);
        // Invariant: memory allocation succeeded.
        v[99'999'999'999] = 123;
    } catch (std::bad_alloc &) {
        // Impossible to access 'v' with incorrect invariant!
        std::cout << "caught bad_alloc\n";
    }
    // Jargon: 'RAII' may only mean part 2: destructor cleans up everything.
    // Constructing object is less strict, see ifstream.
}


Полный код смотри в ссылках ниже.
```c++
struct matrix {
private:
    int height, width;
    int *data;

public:
    matrix(int height_, int width_)
        : height(height_), width(width_), data(new int[height * width]) {}

    matrix(const matrix &other) : matrix(other.height, other.width) {
        std::copy(other.data, other.data + height * width, data);
    }

    matrix(matrix &&other) noexcept
        : height(std::exchange(other.height, 0))
        , width(std::exchange(other.width, 0))
        , data(std::exchange(other.data, nullptr)) {
    }

    matrix &operator=(const matrix &other) {
        if (this != &other) {
            *this = matrix(other);
        }
        return *this;
    }

    matrix &operator=(matrix &&other) noexcept {
        std::swap(height, other.height);
        std::swap(width, other.width);
        std::swap(data, other.data);
        return *this;
    }

    ~matrix() {
        delete[] data;
    }

    int &element(int row, int col) {
        return data[row * width + col];
    }

    const int &element(int row, int col) const {
        return data[row * width + col];
    }
};
```
Это код с ручным управлением памятью и правилом пяти. Заменим указатель на кусок памяти вектором. Этим мы избавились от необходимости реализовывать все конструкторы/деструкторы и использовали RAII для обеспечения базовой гарантии: была возможна утечка памяти в конструкторе, теперь нет. Вот что получилось:
```c++
struct matrix {
private:
    // May be better cache locality than vector<vector<int>>, but more code.
    int height, width;
    std::vector<int> data;

public:
    matrix(int height_, int width_)
        : height(height_), width(width_), data(height * width) {}

    int &element(int row, int col) {
        return data[row * width + col];
    }

    const int &element(int row, int col) const {
        return data[row * width + col];
    }
};
```

### Подвохи с базовой гарантией при реализации operator=(const&), если сделать подряд `delete[] data; data = new char[...]` (можно получить UB в деструкторе, чинить — сначала выделить, потом портить поля)
Полные куски кода можно найти [тут](https://github.com/hse-spb-2021-cpp/lectures/blob/master/30-220606/03-more-exceptions/21-assignment-strong-safety.cpp)
```c++
minivector &operator=(const minivector &other) {
        if (this == &other) {  // Needed, otherwise we delete data.
            return *this;
        }
        if (len != other.len) {
            // incorrect
            delete[] data;
            // If it throws, we have UB. Workaround: `data = nullptr; len = 0;`
            data = new int[other.len];
        }
        for (std::size_t i = 0; i < other.len; i++) {
            data[i] = other.data[i];
        }
        return *this;
    }
```
Если вылетело исключение в строке с `new int`, то получится, что `data` указывает на какой-то освобожденный кусок памяти и длина у вектора ненулевая, то есть консистентность вектора не сохранится. Инвариант сломался, а значит базовой гарантии нет. 

### Обеспечение строгой гарантии исключений при помощи:

* #### Полного копирование объекта
(например, при реализации push_back)
Это чинится, например, введением нового указателя до удаления:
```c++
template<typename T>
    void push_back(const T &value) &noexcept {
        assert(len < C);
        T copy = T(value);
        new (&data[len]) T(value);
        len++;
    }

    void push_back(T &&value) &noexcept {
        assert(len < C);
        T copy = T(std::move(value));
        new (&data[len]) T(std::move(value));
        len++;
    }
```


* #### Copy-and-swap idiom для `operator=`
```c++
minivector(std::size_t len_) : data(new int[len_]{}), len(len_) {}
    minivector(minivector &&other) noexcept : data(std::exchange(other.data, nullptr)), len(std::exchange(other.len, 0)) {}
    minivector(const minivector &other) : minivector(other.len) {
        for (std::size_t i = 0; i < len; i++) {
            data[i] = other.data[i];
        }
    }
    ~minivector() {
        delete[] data;
    }

    friend void swap(minivector &a, minivector &b) noexcept {
        std::swap(a.data, b.data);
        std::swap(a.len, b.len);
    }

    // copy-and-swap idiom is about making `operator=` strong exception safe AND simple
    minivector &operator=(minivector other) {  // copy or move to the temporary
        // swap with the temporary via our convenience method
        swap(*this, other);  // we can also write it in-place, but then std::swap will be inefficient
        // std::swap(*this, other);  // do not work, as it calls `operator=`
        return *this;
        // destroy the temporary
        // the only trouble: self-assignment is now not cheap
    }
```
Здесь мы реализуем свой `swap` и оператор копирования реализуем через него: передаем по значению и затем свопаем. При этом после свопа старое значение остается в `other` который корректно уничтожается при выходе из оператора.

### Возможное отсутствие базовой гарантии у реализации `operator=` по умолчанию при наличии инварианта класса
```c++
#include <cassert>
#include <iostream>
#include <vector>

// See exercise `16-220124`, `30-copy-no-basic-guarantee`
// https://stackoverflow.com/questions/13341456/exception-safety-of-c-implicitly-generated-assignment-operator

int throw_after = 1;

struct VectorHolder {
    std::vector<int> data;
    VectorHolder(int n, int val) : data(n, val) {}

    VectorHolder &operator=(const VectorHolder &other) {
       if (--throw_after < 0) {
           throw 0;
       }
        data = other.data;
        return *this;
    }
};

struct Foo {
private:
    VectorHolder a;
    VectorHolder b;

public:
    Foo(int n) : a(n, 1), b(n, 2) {}
    void check() {
        assert(a.data.size() == b.data.size());
    }
    // Oops: implicitly generated operator= assigns fields one-by-one.
};

int main() {
    Foo x(10);
    Foo y(20);
    x.check();
    y.check();
    try {
        x = y;
    } catch (...) {
        std::cout << "caught\n";
    }
    x.check();  // invariant violated?
}
```
Есть класс, в котором два вектора и инвариант - у них одинаковая длина. У `Foo` дефолнтый оператор копирования. У векторов после первого копирования выбрасывается исключение. То есть, когда мы пишем `x = y` в момент копирования `b` вылетает исключение, а `a` был успешно скопирован до этого. В итоге инвариант нарушается, то есть базовой гарантии в операторе копирования `Foo` нет. \
Вывод: даже правило 0 не спасает и не обеспечивает базовую гарантию и вообще про гарантии надо много думать.

### Источники (лекции Егора и практики)
* [Гарантии исключений](https://youtu.be/RtLd-dS5aNo?list=PL8a-dtqmQc8ph7sgkWKlgoAHIw2WIaNDZ&t=398)
* Практика please-use-vector: [problem](https://github.com/hse-spb-2021-cpp/exercises/blob/master/16-220124/problem/13-please-use-vector.cpp), [solution](https://github.com/hse-spb-2021-cpp/exercises/blob/master/16-220124/solution/13-please-use-vector.cpp)
* [Подвохи с базовой гарантией и обеспечение строгой и далее](https://youtu.be/EBpe6OIgnYs?list=PL8a-dtqmQc8ph7sgkWKlgoAHIw2WIaNDZ&t=3971)
