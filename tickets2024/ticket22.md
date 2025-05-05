## Билет 22. Специализации

* Полные и частичные специализации шаблона класса: синтаксис, независимость специализаций
    * Специализация по типам, значениям
    * Частичная специализация с уменьшением или увеличением количества шаблонных переменных
* Использование для метапрограммирования: `is_void`, `is_reference`, `is_same`, `conditional`
* Полные специализации функций
    * Ограничения по сравнению со полными специализациями шаблонов класса
    * Влияние на автовывод типов
    * Эмуляция частичных специализаций при помощи структур
* Как использовать `std::swap`, чтобы работали сторонние контейнеры
    * Запрет на добавление перегрузок в `namespace std` и необходимость частичной специализации
    * Костыль через `using std::swap;` и ADL (Argument-Dependent Lookup)

Много кода можно найти [тут](https://github.com/hse-spb-2021-cpp/lectures/tree/master/23-220321)
Для каждого (под)пункта билета будет ссылка на объяснение этой части билета
от Егора. Выглядить будет так
##### [EgorThoughts](https://youtu.be/Wcslv_3W4jc?t=1810)


### Полные и частичные специализации шаблона класса: синтаксис, независимость специализаций

> Специализация — это создание "подшаблона" нашего шаблоного класса.

_Зачем это нужно?_ Ну, допустим, у тебя есть класс vector<T>, который является обычной реализацией вектора и ты хочешь,
чтоб у тебя поведение этого класса отличалось, если передаваемый тип — это bool (то есть поведение vector\<bool>). Тогда
тебе нужно как-то сказать компилятору, что поведение vector\<bool> будет другим.

> Полная специализация — это специализация для одного конкретного типа

> Частичная специализация — это специализация для пожмножества типов
---
__Синткасис полной специализации__
##### [EgorThoughts](https://youtu.be/Wcslv_3W4jc?t=1810)
```c++
template<typename T>
struct my_vector {  // основной класс
private:
    std::vector<T> data;
public:
    my_vector(int n) : data(n) {}
    T get(int i) { return data[i]; }
    void set(int i, T value) { data[i] = std::move(value); }
};

// my_vector<bool> v1(10);  // Здесь будет нарушаться ODR (one definition rule), так как
// тут был создан my_vector (ниже) той же структуры, что и для типа T, который не bool. 
// А затем мы ещё раз создали  класс my_vector работающий с типом bool.
// При компиляции строчка 35 превращается в: 
//struct my_vector {  
//private:
//    std::vector<bool> data;
//public:
//    my_vector(int n) : data(n) {}
//    bool get(int i) { return data[i]; }
//    void set(int i, bool value) { data[i] = std::move(value); }
//}; 

template<>
struct my_vector<bool>;  // Декларация специализации (ну, чтоб там указатели можно было объявлять)

std::unique_ptr<my_vector<bool>> ptrToVector; // it's legal, baby

template<>
struct my_vector<bool> {  // Собственно специализация для bool-a
private:
    // Теперь никакого типа Т
    std::vector<std::uint8_t> bits;
public:
    my_vector(int n) : bits((n + 7) / 8) {}
    bool get(int i) { return (bits[i / 8] >> (i % 8)) & 1; }
    void set(int i, bool value) {
        if (value) {
            bits[i / 8] |= 1 << (i % 8);
        } else {
            bits[i / 8] &= ~(1 << (i % 8));
        }
    }
    void foobarbaz() {  // Даже такое вот компилируется!
        // foobarbazbazbaz(); // До тех пор пока мы это не юзаем;)
    }
};
```

_Note:_ vector\<bool> — это
очень [плохая идея](https://github.com/hse-spb-2021-cpp/lectures/blob/master/23-220321/01-specialization/02-vector-bool-bad.cpp)
, которую в какой-то момент была в плюсах (потом, слава Богу, убрали), так как это был и не вектор, по сути. Почему не
вектор? Ну эта структура была на подобие bitset-a (любим, гордимся), по которой нельзя итерироваться, нельзя брать
ссылку на элемент и прочее. Bitset - это, безпорно, классная структура, но это не вектор
##### [EgorThoughts](https://youtu.be/Wcslv_3W4jc?t=2137) про vector\<bool>

---
__Синткасис частичной специализации__
##### [EgorThoughts](https://youtu.be/Wcslv_3W4jc?t=2551)
```c++
template<typename T>
struct is_reference {
    static inline const bool value = false;
};

template<typename T> // тип Т сохраняется, потому что мы специализируем на подмножестве типа Т
struct is_reference<T&> {  // (const U)& тоже подойдёт.
    static inline const bool value = true;
};
```

Ещё [примерчик](https://github.com/hse-spb-2021-cpp/lectures/blob/master/23-220321/01-specialization/14-partial-more-types.cpp):

```c++
template<typename T>
struct store_first {
    T data;
    store_first(const T &t) : data(std::move(t)) {}
};

template<typename TEl> // Type of Element
struct store_first</*T=*/std::vector<TEl>> {
    TEl data;
    store_first(const std::vector<TEl> &v) : data(v.at(0)) {}
};

template<typename T, typename U>
struct store_first</*T=*/std::pair<T, U>> {
    T data;
    store_first(const std::pair<T, U> &p) : data(p.first) {}
};
```

Здесь мы создали структуру, которая хранит первый элемент передаваемой структуры. И соотвественно сделали частичную
специализацию под три случая:

* Нам дали просто объект
* Нам дали вектор
* Нам дали пару

#### Иногда частичная специализация не работает
```c++
template<int I>
struct foo {
};

template<int I>
struct foo<2 * I> {  // Compilation error: компилятор не настолько умный, 
                    // чтоб решать уравнения
    static inline const bool value = I;
};
```

### Использование для метапрограммирования: is_void, is_reference, is_same, conditional
##### [EgorThoughts](https://youtu.be/Wcslv_3W4jc?t=2458) 
Ну тут всё очень просто. Давайте отдельно скажем про реализацию этих типов, а затем про использование.
---

##### Реализация

На самом деле специализация — это мощное орудие, чтобы творить что-то ~~бесполезное~~ интересное!
Давайте посмотрим на реализацию is_same<T, T>

```c++
template<typename T, typename U>
struct is_same {
    static inline const bool value = false; // по дефолту value == false, 
                                            // так как типы разные
};

template<typename T> // Заметьте, присуствует лишь одно T, так как на нём завязано 
                    // подмножество специализируемых классов  
struct is_same<T, T> { // Специализируем для двух однинаковых типов
    static inline const bool value = true; // В этом случае value == true
};

// Начиная с с++17 мы можем делать шаблонные переменные
template<typename T, typename U>
static const bool is_same_v = is_same<T, U>::value;

// 
int main() {
    std::cout << is_same_v<int, long long> << "\n"; // FALSE
    std::cout << is_same_v<int, signed> << "\n"; // TRUE (скорее всего, 
                                                // не гарантируется стандартом)
    std::cout << is_same_v<int, unsigned> << "\n"; // FALSE
}
```

Ну вот создали классную штуку, не так ли? \
is_void, is_reference — того же рода птички

##### Использование шаблонных инструментов на основе conditional

conditional_t<B, T, F> — это шаблонная переменная, которая возвращает тип T, если передаваемый B == true, и тип F иначе.

```c++
// Реализация
template<bool B, typename T, typename F>
struct conditional {
using type = T;
};

template<typename T, typename F>
struct conditional<false, T, F> {
using type = F;
};

template<bool B, typename T, typename F>
using conditional_t = typename conditional<B, T, F>::type;

int main() {
conditional<2 * 2 == 4, int, double>::type x = 10.5;
conditional<2 * 2 == 5, int, double>::type y = 10.5;
std::cout << x << "\n";
std::cout << y << "\n";
}
```

Смотрите, теперь мы можем выбирать тип на основе того, во что выразится передаваемое выражение. Правда, тонкость в том,
что всё дело происходит на этапе компиляции, поэтому мы не сможем определять тип в run-time (время исполнения программы)
. \
На всякий случай, время копиляции — это когда ваша программа собирается, run time — это когда программа уже запущена и
что-то делает.

### Полные специализации функций
##### [EgorThoughts](https://youtu.be/Wcslv_3W4jc?t=2985)
Давайте сделаем странную перегрузку функции, точнее специализацию!

```c++
template<typename T> // Объявляем. По факту уже здесь можно написать, что будет
                    // делать наша функция для неспециализированного типа
T random(const T &from, const T &to);

template<> // Специализируем, как обычно
int random<int>(const int &from, const int &to) {  // Argument types should match
                                                  // exactly: `const int &`, not just `int`.
    std::uniform_int_distribution<int> distrib(from, to - 1);
    return distrib(gen);
}
// Следующий комментарий: мы можем опускать указание типа, когда используем эту функцию,
// то есть писать random(0, 5) вместо random<int>(0, 5)
template<>
double random(const double &from, const double &to) {  // May actually omit template
                                                      // parameters if they're deduced from
                                                     // args
    std::uniform_real_distribution<double> distrib(from, to);
    return distrib(gen);
}

int main() {
    std::cout << random(1, 6) << "\n";  // T=int
    std::cout << random<int>(1, 6) << "\n";
    std::cout << random<int>(1.0, 6.5) << "\n"; // здесь всё ок, так как мы явно указали тип
                                                // правда, будет варнинг о конвертанции double в int
    std::cout << "=====\n";
    std::cout << random(0.0, 1.0) << "\n";  // T=double
    std::cout << random<double>(0.0, 1.0) << "\n";
    std::cout << random<double>(0, 1) << "\n";
    // std::cout << random(0.0, 1) << "\n";  // compilation error: непонятно T=int или T=double?
}
```

### Эмуляция частичной специализации функций
##### [EgorThoughts](https://youtu.be/Wcslv_3W4jc?t=3252)

К сожалению, частичной специализации для функций не существует. Доступна лишь полная... Ну как так? Мы ж
программисты, мы ж умные, должно быть что-нибудь. 

Та-дамс! Вашему вниманию представляется __эмуляция частичной специализации__ на функциях Хотим, чтоб функция read была частично
специализирована под вектор, которая будет считывать значение и возвращать его или в случае с вектором будет возвращать
вектор считанных значений. \
Давайте создадим структуру с статичным методом read, ну и частично специализируем эту структуру. Победа!

```c++
template<typename T>
struct reader;

template<typename T>
T read() { // наш read просто вызывает read из структуры reader
    return reader<T>::read();
}

template<typename T>
struct reader {
    static T read() { // здесь static
        T res;
        std::cin >> res;
        return res;
    }
};

template<typename T>
struct reader<std::vector<T>> {
    static std::vector<T> read() { // и здесь static, так как в зависимости от шаблонного класса у нас будет генерироваться 
        int n;                      // различные методы
        std::cin >> n;
        std::vector<T> res(n);
        for (auto &v : res) {
            v = reader<T>::read();
        }
        return res;
    }
};

int main() {
    std::cout << read<int>() << "\n";
    auto vec = read<std::vector<double>>();
    std::cout << vec.size() << "\n";
    std::cout << vec[0] << "\n";
}
```

### swap specialization
##### [EgorThoughts](https://youtu.be/Wcslv_3W4jc?t=3386)

```c++
namespace my_secret {
struct Basic {};
}

namespace std {
// void swap(my_secret&, my_secret&) {  // UB, перегрузка функции из stl.
template<> void swap(my_secret::Basic&, my_secret::Basic&) {  // Not UB, специализация 
                                                             // - так можно. УБ начиная с C++20!
                                                            // Поэтому это не оптимальный вариант 
                                                           // (не будет совместимости с с++20)
    std::cout << "swap(Basic)\n";
}
};

namespace my_secret {
template<typename T>
struct Foo {};

// Не стоит специализировать std::swap (выше описано почему). Добавь версию использующую ADL.
template<typename T> // ADL — argument dependent lookup 
                    // (поиск перегрузки/специализации в зависимости от переменных)
void swap(Foo<T> &, Foo<T> &) {
    std::cout << "swap(Foo<T>)\n";
}
};

int main() {
    {
        my_secret::Basic a, b;
        std::swap(a, b);  // OK swap. 

        using std::swap;  // Обязательная строчка, чтоб следующая компилировалась. 
                         // Так компилятор будет искать перегрузку в namescace std 
        swap(a, b);  // OK swap. Будет вызван наш swap для Basic-ов
    }
    {
        my_secret::Foo<int> a, b;
        std::swap(a, b);  // Wrong swap. Не тот, который хотели

        using std::swap;  // Необязательная строчка, так как мы и так найдём нужный swap 
                         // с помощью ADL
        swap(a, b);  // OK swap, ADL. 
    }
    // Moral: `using std::swap; swap(a, b)` — самый правильный вариант для обобщённого кода.
}
```
