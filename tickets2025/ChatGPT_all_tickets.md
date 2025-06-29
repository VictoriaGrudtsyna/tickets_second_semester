<details>
<summary>
10. Move-семантика и copy elision
</summary>

## Введение
Добрый день! Сегодня я расскажу о том, как в C++ движется (move-семантика) и исчезает (copy elision) «тяжёлый» копирующий код, чтобы программы работали быстрее и эффективнее. Мы разберём, какие бывают категории значений, как связываются ссылки, почему сигнатуры конструкторов перемещения именно такие, как работает `std::move`, а также разберём оптимизации копицирования — RVO и NRVO. И в конце обсудим, когда не нужно писать `return std::move(foo);`.

## I. Категории значений
1. **glvalue** (generalized lvalue) — общее понятие для lvalue и xvalue.  
2. **rvalue** — общее понятие для prvalue и xvalue.  
3. **lvalue** — значение, имеющее адрес (переменная, ссылка).  
4. **prvalue** (pure rvalue) — временное значение, не имеющее адреса (результат вызова функции по значению, литералы).  
5. **xvalue** (expiring value) — «умирающее» значение, которое можно перемещать (результат `static_cast<T&&>(obj)`, возвращаемый по `T&&`).

> В C++11/C++14 эти категории определялись раздельно; с введением C++17 упрощён новый prvalue, который уже «говорит» о возможности обязательной оптимизации копирования, но знать его глубоко не обязательно для гарантированного copy elision.

## II. Ссылки и правила привязки
- `T&` (lvalue-ссылка) может привязываться **только** к lvalue.  
- `const T&` может привязываться к любому значению (lvalue или rvalue), но не модифицирует его.  
- `T&&` (rvalue-ссылка) — **только** к rvalue (prvalue или xvalue).  

```cpp
int x = 10;
int&  lr = x;            // OK: lvalue → lvalue&
int&& rr1 = x;           // Ошибка: lvalue → rvalue&
int&& rr2 = 20;          // OK: prvalue → rvalue&
const int& cr = 30;      // OK: prvalue → const lvalue&
```

## III. Move-конструктор и move-assignment
**Сигнатуры**:  
```cpp
struct A {
    A(const A&);             // копирующий
    A(A&&) noexcept;         // перемещающий
    A& operator=(const A&);
    A& operator=(A&&) noexcept;
};
```

1. **Почему без `const` у `A&&`?**  
   - `const A&&` означало бы «невозможно изменить» временный объект → не имеет смысла, т.к. при перемещении мы обычно «воруем» внутреннее состояние (устанавливаем указатель в `nullptr` и т.п.).  
2. **Разрешение перегрузок**:  
   - При передаче lvalue → выберется копирующий конструктор (`A(const A&)`).  
   - При передаче rvalue → предпочитается move-конструктор (`A(A&&)`), т.к. он точнее соответствует аргументу rvalue.  
3. **`noexcept`**: критически важно для стандартных контейнеров (например, `vector`) — при реализации роста `vector` они перемещают элементы, и если перемещение гарантированно не бросает исключений, используется более быстрая стратегия без отката.

## IV. `std::move`
- **Что делает**: просто выполняет `static_cast<T&&>(x)`, превращая lvalue в xvalue.  
- **Почему «ничего не делает»**: это всего лишь приведение типа, без копирования или перемещения.  
- **Где используется**: когда вы хотите явно разрешить передачу внутренних ресурсов дальше, например:

```cpp
std::string foo();
std::string s = foo();           // здесь сработает RVO, но если бы не…
std::string t = std::move(s);    // «украли» буфер из s в t
```

- **Как реализовать свой**:

```cpp
template<typename T>
constexpr std::remove_reference_t<T>&& my_move(T&& t) noexcept {
    return static_cast<std::remove_reference_t<T>&&>(t);
}
```

## V. Copy elision: RVO и NRVO
### 1. Return Value Optimization (RVO)
Компилятор может **опустить** создание временного объекта при возвращении значения:

```cpp
A createA() {
    return A(…);      // объект создаётся сразу в месте назначения
}
A a = createA();      // без дополнительного копирования/перемещения
```

### 2. Named RVO (NRVO)
Аналогично, когда возвращаем **именованную** переменную:

```cpp
A createA2() {
    A tmp(…);
    // ... какие-то действия с tmp
    return tmp;       // tmp создаётся сразу в памяти a
}
```

> В C++17 и новее эти оптимизации гарантированы, даже если конструктор видимо бросает исключения.

## VI. `return std::move(foo);` — когда (не) нужно
- **Не нужно** в функциях, возвращающих по значению:
```cpp
A makeA() {
    A foo;
    // ...
    return foo;         // NRVO, а в C++17 гарантирован RVO
}
```
Если написать `return std::move(foo);`, вы **принудительно** отбросите NRVO, получите перемещение вместо идеального elision, и даже можете потерять в оптимизации.
- **Можно** использовать, если вы **на 100%** уверены, что NRVO не возможен и нужно именно перемещение (но таких случаев почти нет).

## Заключение
1. **Move-семантика** позволяет «перемещать» ресурсы без дорогостоящего копирования.  
2. **Copy elision** (RVO/NRVO) позволяет полностью **избежать** даже вызова перемещающих конструкторов при возврате по значению.  
3. **`std::move`** — это лишь приведение типа, которое дает компилятору право выбрать move-версию функции.  
4. Никогда **не** пишите `return std::move(foo);` без крайней необходимости — лучше довериться оптимизациям компилятора.

</details>

<details>
<summary>
11. Реализация своего std::vector
</summary>

## I. Размещение объектов и выравнивание
- **Placement new**: позволяет сконструировать объект в заранее выделённом буфере:
  ```cpp
  void* raw = operator new[](sizeof(T) * capacity);
  new (raw) T(value); // конструируем T в raw
  ```
- **Выравнивание**: важно, чтобы буфер был выровнен под `T`:
  ```cpp
  void* raw = operator new[](sizeof(T) * capacity, std::align_val_t(alignof(T)));
  ```
  Или использовать `std::allocator<T>::allocate`, который заботится об этом автоматически.
- **Явный вызов деструктора**:
  ```cpp
  data[i].~T();
  ```

## II. Время жизни объектов
- Объект T «жизненен» с момента конструкции (`new (ptr) T`) до вызова деструктора (`~T()`).
- Любые обращения (чтение/запись/деструктуры) за пределами этого интервала — **undefined behaviour**.

## III. Ref-qualifiers и перегрузка операторов
Референс-квалификаторы `&` и `&&` позволяют раздать разные методы для lvalue- и rvalue-объекта:
```cpp
template<typename T, typename Alloc = std::allocator<T>>
class vector {
  // const-qualified доступ
  const T& operator[](std::size_t i) const & noexcept {
    return data_[i];
  }
  // lvalue-версия: реализуем через const_cast
  T& operator[](std::size_t i) & noexcept {
    return const_cast<T&>(static_cast<const vector&>(*this)[i]);
  }
  // rvalue-версия: возвращает T&&
  T&& operator[](std::size_t i) && noexcept {
    return std::move((*this)[i]);
  }
};
```
Это избавляет от дублирования кода и реализует семантику доступа как в `std::vector`.

## IV. Перегрузка `push_back` и альтернативы
### 1. Перегрузка на `const T&` и `T&&`
```cpp
void push_back(const T& value) & {
  if(size_ == capacity_) reserve(capacity_ ? capacity_*2 : 1);
  new (&data_[size_++]) T(value);
}
void push_back(T&& value) & {
  if(size_ == capacity_) reserve(capacity_ ? capacity_*2 : 1);
  new (&data_[size_++]) T(std::move(value));
}
```
- Альтернатива **perfect forwarding** (`template<class... Args> emplace_back(Args&&... args)`): позволяет конструировать объект «in-place» без лишних копий.
- **Отличие** от приёма по значению `push_back(T value)`: приёмы по значению могут приводить к лишнему копированию при lvalue-аргументах.

### 2. Применение `const_cast` для уменьшения дублирования
Можно вынести общую логику:
```cpp
template<class U>
void push_back_impl(U&& value) {
  if(size_ == capacity_) reserve(capacity_ ? capacity_*2 : 1);
  new (&data_[size_++]) T(std::forward<U>(value));
}
void push_back(const T& v) & { push_back_impl(v); }
void push_back(T&& v) &      { push_back_impl(std::move(v)); }
```

## V. Обеспечение гарантий при исключениях
- **Базовая гарантия**: при ошибке внутреннее состояние остаётся корректным, но объекты могут быть разрушены.
- При **увеличении буфера**:
  1. Выделяем новый буфер `new_data` через `allocate`.
  2. В цикле перемещаем (или копируем) старые элементы в `new_data` с помощью `new (… ) T(...)`.
  3. Если в процессе бросилось исключение, нужно:
     - Разрушить уже сконструированные в `new_data` объекты.
     - Освободить `new_data`.
     - Оригинальный буфер и объекты остаются нетронутыми.
  4. Если всё успешно, вывести старые объекты, освободить старый буфер и присвоить `data_ = new_data`.

## VI. Полный код `vector<T>`
```cpp
#pragma once
#include <cstddef>
#include <memory>
#include <utility>
#include <new>
#include <cassert>

template<typename T, typename Alloc = std::allocator<T>>
class vector {
public:
  vector(): data_(nullptr), size_(0), capacity_(0) {}
  ~vector() {
    clear();
    if(data_) alloc_.deallocate(data_, capacity_);
  }

  // копирование
  vector(const vector& other): data_(nullptr), size_(0), capacity_(0) {
    reserve(other.size_);
    for(size_t i = 0; i < other.size_; ++i)
      new (&data_[i]) T(other.data_[i]);
    size_ = other.size_;
  }
  // перемещение
  vector(vector&& other) noexcept
    : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.size_ = other.capacity_ = 0;
  }

  // ref-qualifier оператор[]
  const T& operator[](size_t i) const & noexcept {
    return data_[i];
  }
  T& operator[](size_t i) & noexcept {
    return const_cast<T&>(static_cast<const vector&>(*this)[i]);
  }
  T&& operator[](size_t i) && noexcept {
    return std::move((*this)[i]);
  }

  size_t size() const noexcept { return size_; }
  bool empty() const noexcept { return size_ == 0; }

  void clear() noexcept {
    for(size_t i = 0; i < size_; ++i)
      data_[i].~T();
    size_ = 0;
  }

  void reserve(size_t new_cap) {
    if(new_cap <= capacity_) return;
    T* new_data = alloc_.allocate(new_cap);
    size_t i = 0;
    try {
      for(; i < size_; ++i)
        new (&new_data[i]) T(std::move_if_noexcept(data_[i]));
    } catch(...) {
      for(size_t j = 0; j < i; ++j)
        new_data[j].~T();
      alloc_.deallocate(new_data, new_cap);
      throw;
    }
    // разрушить старые и освободить
    clear();
    if(data_) alloc_.deallocate(data_, capacity_);
    data_ = new_data;
    capacity_ = new_cap;
    size_ = i;
  }

  // push_back
  template<class U>
  void push_back_impl(U&& v) {
    if(size_ == capacity_) reserve(capacity_ ? capacity_*2 : 1);
    new (&data_[size_++]) T(std::forward<U>(v));
  }
  void push_back(const T& v) & { push_back_impl(v); }
  void push_back(T&& v) &      { push_back_impl(std::move(v)); }

private:
  T* data_;
  size_t size_, capacity_;
  Alloc alloc_;
};
```
</details>

<details>
<summary>
12. Теоретическая и практическая всячина
</summary>

## I. Возвращаемый тип `auto` функций и методов

1. **Вывод простого `auto`.**  
   - В C++14 функции можно объявить с возвращаемым типом `auto` без явного указания:  
     ```cpp
     auto sum(int a, int b) {
         return a + b; // возвращаемый тип deduced as int
     }
     ```
   - Все операторы `return` должны возвращать одинаковую типизированную сущность, иначе компиляция упадёт.

2. **`auto` с модификаторами.**  
   - Можно добавлять cv-квалификаторы и ссылки:
     ```cpp
     auto& get_ref(int& x) { return x; }
     auto&& create_temp() { return std::string("tmp"); }
     ```

3. **Рекурсивные функции.**  
   - Для рекурсии нельзя полностью опустить возвращаемый тип, требуется **trailing return type**:
     ```cpp
     auto factorial(int n) -> int {
         return (n < 2 ? 1 : n * factorial(n - 1));
     }
     ```

4. **Удобство при определении методов вне класса.**  
   - В заголовке:
     ```cpp
     struct S { auto f() -> int; };
     ```
   - В cpp:
     ```cpp
     auto S::f() -> int {
         return 42;
     }
     ```

5. **Объявления функций с `auto` без определения.**  
   - Можно лишь объявить, а определить позже:
     ```cpp
     auto foo(int) -> double; // объявление
     // ...
     auto foo(int x) -> double {
       return x * 2.5;
     }
     ```

6. **Явное указание возвращаемого типа лямбды (C++14+).**  
   ```cpp
   auto lambda = [](int x) -> bool {
       return x % 2 == 0;
   };
   ```

---

## II. Оператор `switch`

1. **`break;` и `[[fallthrough]]`.**  
   - По умолчанию переход идёт «впадину» (fall-through):
     ```cpp
     switch(n) {
       case 1:
         do1();
         [[fallthrough]]; // явно разрешаем переход в case 2
       case 2:
         do2();
         break;
     }
     ```
   - `[[fallthrough]]` (C++17) сигнализирует об осознанном переходе.

2. **Проблемы с инициализацией переменных внутри `switch`.**  
   Метки `case` и `default` — это не отдельные блоки, а лишь точки перехода внутри общего блока `switch`. Без дополнительных `{}` объявление переменной в одном `case` фактически находится в том же блоке, что и другие `case`:
   ```cpp
   switch (n) {
     case 1:
       int x = compute();   // объявление и инициализация x в теле одного блока
       std::cout << x;
       break;
     case 2:
       // при n==2 управление прыгает сюда, мимо строки с int x = compute();
       std::cout << "n==2";
       break;
   }
   ```
   Здесь компилятор увидит, что `x` объявлена в области видимости всего `switch`, но при `n==2` она не инициализируется → это нарушение правил C++ (инициализация пропущена), что приводит к ошибке компиляции или UB.

   **Почему это важно?**  
   - C++ запрещает «перескок» мимо объявления переменной с инициализацией, потому что тогда объект не будет сконструирован, а его деструктор может оказаться вызван или переменная использована «впустую».  

   **Решение**: во избежание перескоков нужно в каждом `case` явно начинать новый блок:
   ```cpp
   switch (n) {
     case 1: {
       int x = compute();
       std::cout << x;
       break;
     }
     case 2: {
       std::cout << "n==2";
       break;
     }
   }
   ```
   Здесь `{}` создают новый подблок, в котором `x` локально объявлена и инициализируется **только** при входе в этот подблок. При `n==2` `case 1` инициализация `x` не видна и UB исключён.

---

## III. Глобальные переменные/функции и статические поля-члены

1. **`extern`**  
   - Расшаривает объявление:
     ```cpp
     // header.h
     extern int global_var;
     // source.cpp
     int global_var = 10;
     ```

2. **`static`** (на уровне namespace)  
   - Старый способ internal linkage:
     ```cpp
     static void helper() { /* ... */ }
     ```

3. **Неявные агенты через unnamed namespace**  
   - Аналог `static`:
     ```cpp
     namespace {
       int helper_var = 0;
     }
     ```

4. **`inline` переменные и функции (C++17).**  
   - Можно определять в header без ODR-ошибок:
     ```cpp
     inline int inline_var = 5;
     inline void inline_func() {}
     ```
   - Несколько определений в разных TU считаются одной сущностью.

5. **Static initialization order fiasco.**  
   - Порядок инициализации non-local static объектов в разных TU не определён → UB, если один использует другой в конструкторе.
   - Обход через function-local statics (инициализируются по требованию в первом вызове):
     ```cpp
     MyType& get_instance() {
       static MyType inst;
       return inst;
     }
     ```

   ### Что такое Static Initialization Order Fiasco
   * Проблема возникает, когда в разных единицах трансляции есть non-local static объекты,
     и их порядок инициализации при старте программы **не определён** стандартом.
   * Например:
     ```cpp
     // a.cpp
     #include <iostream>
     #include "b.h"
     Foo* f = new Foo;
     int main() { useFoo(); }
     ```
     ```cpp
     // b.cpp
     #include <iostream>
     extern Foo* f;
     void useFoo() {
       if (f) f->doSomething();  // UB, если f ещё не инициализирован
     }
     ```
   * Здесь объект `f` в `a.cpp` и статический член в `b.cpp` могут быть инициализированы в непредсказуемом порядке → UB.
   * Решения:
     - Использовать function-local statics (инициализируются по first use и thread-safe в C++11+):
       ```cpp
       Foo& getFoo() {
         static Foo instance;
         return instance;
       }
       ```
     - Явная функция инициализация или dependency injection.


---

## IV. Глобальные константы и статические константы-члены

1. **`constexpr` vs `const`.**  
   - `constexpr` гарантирует константную инициализацию и может использоваться в compile-time выражениях.
   - `const` на namespace-уровне имеет internal linkage по умолчанию (без `extern`).

2. **`inline constexpr`.**  
   - С C++17 статические константы можно объявить так:
     ```cpp
     inline constexpr double PI = 3.1415926535;
     ```

3. **Статические константы-члены класса.**  
   - До C++17:
     ```cpp
     struct S {
       static const int X = 42; // inline initialization allowed для integral
     };
     // Если берём адрес S::X, нужна еще одна дефиниция в cpp:
     // const int S::X;
     ```
   - C++17+:
     ```cpp
     struct S {
       inline static const int X = 42; // no separate definition
     };
     ```

4. **Порядок инициализации.**  
   - `constexpr` и inline переменные инициализируются константно до запуска любой функции.
   - Non-inline `const` или `static` могут попасть в static init fiasco.

</details>

<details>
<summary>
20. Шаблоны классов
</summary>

## I. Синтаксис объявления и определения

- **Объявление шаблона класса:**
  ```cpp
  template<typename T, int N>
  class Array;
  ```

- **Определение (implementation) внутри заголовка:**
  ```cpp
  template<typename T, int N>
  class Array {
  public:
    void fill(const T& value);
  private:
    T data[N];
  };

  template<typename T, int N>
  void Array<T,N>::fill(const T& value) {
    for(int i = 0; i < N; ++i) data[i] = value;
  }
  ```

## II. Параметры шаблона

1. **Типовые (`typename` vs `class`):**  
   Синонимы, разницы нет:
   ```cpp
   template<typename T> struct Foo1 {};
   template<class T>    struct Foo2 {};
   ```

2. **Нейтовые (non-type):**  
   Значения интегральных типов, указателей, ссылок, перечислений:
   ```cpp
   template<int N>           struct Foo3 {};
   template<std::size_t Sz>  struct Foo4 {};
   template<const char* S>    struct Foo5 {};
   ```

3. **Параметры по умолчанию:**
   ```cpp
   template<typename T = int, int N = 16>
   class Buffer {};
   ```

4. **Без имени у параметра:**
   ```cpp
   template<typename, typename Alloc = std::allocator<int>>
   class Vec; // первый параметр без имени
   ```

## III. Параметры-шаблоны (template template parameters)

- **Объявление:**
  ```cpp
  template<template<typename U, int M = 8> class Container>
  struct Wrapper {
    Container<double> c;
  };
  ```

- **Передача шаблонов с параметрами по умолчанию:**
  ```cpp
  template<template<typename, int> class C = std::vector>
  struct DefaultWrap {
    C<int, 4> v; // std::vector<int,4>
  };
  ```

## IV. `typename` и `template` для зависимых имён

- **`typename`** для dependent types:
  ```cpp
  template<typename T>
  void f() {
    typename T::value_type x; // value_type — зависимый тип
  }
  ```

- **`template`** для вызова зависимых шаблонов:
  ```cpp
  template<typename T>
  void g() {
    T::template rebind<double> ptr; // rebind — зависимый шаблон
  }
  ```

- **В наследовании от шаблонного параметра:**
  ```cpp
  template<typename Base>
  struct Derived : Base {
    void h() {
      typename Base::type x;
      this->template func<int>();
    }
  };
  ```

## V. `Foo` vs `Foo<>`

- **Шаблон без `<>`** — имя шаблона, а не тип:
  ```cpp
  template<typename T = int>
  struct Foo {};
  Foo<> f1;      // эквивалентно Foo<int>
  // Foo f2;     // ошибка: ожидается list параметров
  ```

- **Удобство в возвращаемом типе:**
  ```cpp
  template<typename T>
  auto Foo<T>::make() -> Foo; // сокращение вместо Foo<T>
  ```

## VI. Инстанцирование

1. **Независимость по методам:**  
   - Только методы, которые используются, действительно инстанцируются (implicit instantiation).
2. **Неявное инстанцирование происходит для:**
   - Полей класса (размер массива, типы).
   - Вложенных типов.
   - Специализаций методов при использовании.
3. **Incomplete-типы параметров:**
   - Можно объявить `template<typename T> struct X { T* ptr; };` даже если `T` incomplete.
   - Нельзя instantiаte методы, требующие полного определения `T` (например `sizeof(T)`).

## VII. Независимость инстанциаций шаблона

- Каждая комбинация параметров шаблона генерирует свой набор символов, не пересекающийся с другими.

## VIII. Определение методов и статических членов

- **Внутри класса (inline):**
  ```cpp
  template<typename T>
  struct S {
    static int count;
    void inc() { ++count; }
  };
  template<typename T>
  int S<T>::count = 0;
  ```

- **Вне класса:**
  ```cpp
  template<typename T>
  void S<T>::inc() {
    ++count;
  }
  ```

</details>

<details>
<summary>
21. Использование шаблонов
</summary>

## I. Передача функторов без `std::function`

- **Хранение функторов как шаблонное поле**:
  ```cpp
  template<typename F>
  struct CallbackHolder {
      F func;
      CallbackHolder(F f) : func(std::move(f)) {}
      void call() { func(); }
  };
  ```
- **Передача по значению**:
  ```cpp
  template<typename F>
  void process(F f) {
      // f может быть лямбдой, функцией или функтором
      f();
  }
  ```
- Избегаем накладных расходов `std::function`, сохраняя конкретный тип функторов в шаблонах.

---

## II. Шаблоны функций и вывод параметров

- **Автовывод типов из аргументов**:
  ```cpp
  template<typename T, typename U>
  auto add(T a, U b) {
      return a + b;
  }
  // вызов: add(1, 2.5) выведет add<int, double>
  ```
- **Неявная специализация**: компилятор сам определяет `T` и `U`.

---

## III. Class Template Argument Deduction (CTAD)

- **Пример с `std::pair` и `std::vector` (C++17+):**
  ```cpp
  std::pair p(1, 2.5);          // std::pair<int, double>
  std::vector vec = {1, 2, 3};  // std::vector<int>
  ```
- **Пользовательские CTAD:**
  ```cpp
  template<typename T>
  struct Wrapper {
      Wrapper(T v) : value(v) {}
      T value;
  };

  // Специальное руководство:
  template<typename T>
  Wrapper(T) -> Wrapper<T>;

  // Теперь:
  Wrapper w(42); // Wrapper<int>

**Зачем нужны deduction guides?**
1. **Уменьшение многословности.** Не нужно каждый раз писать `<T>`, если компилятор может вывести тип по аргументам конструктора.
2. **Консистентность со стандартной библиотекой.** Стандартные контейнеры (`std::pair`, `std::vector` и др.) используют guides, позволяя писать `std::pair p(1,2.5);` и `std::vector vec = {1,2,3};`.
3. **Гибкость.** При наличии нескольких конструкторов можно определять несколько гидов, чтобы избежать неоднозначностей и ошибок дедукции.
4. **Удобство использования.** Сокращает «шум» в коде, делая создание объектов шаблонных классов более лаконичным.

---

## IV. Шаблонные методы и конструкторы

- **Обычный класс с шаблонным методом:**
  ```cpp
  struct S {
      template<typename T>
      void foo(T x) { /*...*/ }
  };
  ```
- **Шаблонный класс с шаблонным конструктором:**
  ```cpp
  template<typename U>
  struct A {
      template<typename V>
      A(V&& v) : member(std::forward<V>(v)) {}
      U member;
  };
  ```
- **Определение вне класса:**
  ```cpp
  template<typename U>
  template<typename V>
  A<U>::A(V&& v) : member(std::forward<V>(v)) {}
  ```

---

## V. Шаблонные лямбда-выражения

- **Generic lambdas (C++14):**
  ```cpp
  auto gl = [](auto x, auto y) {
      return x * y;
  };
  gl(2, 3.5); // OK
  ```
- **Явное указание параметров шаблона (C++20):**
  ```cpp
  auto gl2 = []<typename T>(T x) {
      return x;
  };
  gl2.operator()<int>(5);
  ```

---

## VI. Проблемы с `>>` до C++11

- До C++11 требовался пробел между `>`:
  ```cpp
  // До C++11:
  std::vector<std::vector<int> > vv; // ошибка без пробела
  // C++11+:
  std::vector<std::vector<int>> vv;  // OK
  ```
**Почему это было необходимо?**  
В синтаксисе C++ до стандарта C++11 последовательность `>>` в шаблонных параметрах рассматривалась как оператор сдвига вправо, а не как закрывающие символы шаблона. Например:  
```cpp
std::vector<std::vector<int> > vv; // нужно было разделять пробелом
```  
Компилятор видел `>>` и пытался выполнить синтаксис сдвига, что приводило к ошибке.  
С введением C++11 это было исправлено: парсинг шаблонов стал контекстно-зависимым, `>>` автоматически интерпретируется как два закрывающих шаблонных скобки без необходимости пробела.  

---

## VII. Псевдонимы типов (`using`)

- **Алиасы шаблонов:**
  ```cpp
  template<typename T>
  using Vec = std::vector<T>;
  Vec<int> v; // вместо std::vector<int>
  ```
- **Полные специализации alias templates:**
  ```cpp
  template<>
  using IntVec = Vec<int>;
  ```

---

## VIII. Шаблоны переменных (C++17)

- **Пример:**
  ```cpp
  template<typename T>
  inline constexpr T pi_v = T(3.1415926535897932385);
  // Использование:
  double pi = pi_v<double>;
  ```
- **Стандартные примеры:**
  ```cpp
  // std::launder — не шаблонная переменная, но аналогично
  // std::numeric_limits<T>::max() — функция, не переменная
  ```

---

## IX. Повторное использование и forward declaration

- **Forward declaration** шаблонных классов:
  ```cpp
  template<typename T>
  class Foo;  // объявление
  ```
- **Inline** в template-функциях и CTAD позволяют определять всё в header без нарушений ODR.

</details>

<details>
<summary>
22. Специализации
</summary>

## I. Полные и частичные специализации шаблона класса

### 1. Полная специализация
Полная специализация задаёт реализацию шаблона для конкретных параметров:
```cpp
template<typename T, int N>
struct Array { /* общий вариант */ };

template<>
struct Array<int, 5> { // полная специализация для Array<int,5>
    void info() { /* ... */ }
};
```
- Специализировать можно по типам и по значимым non-type параметрам (интегральным, указателям, перечислениям).

### 2. Частичная специализация
#### Пример регулярных и нерегулярных параметров
```cpp
// Шаблон с двумя параметрами
template<typename T, int N>
struct Array { /* ... */ };

// Частичная специализация, где T — регулярный (дедуцируемый) параметр,
// а N — нерегулярный (фиксированный) параметр со значением 5:
template<typename T>
struct Array<T, 5> {
    // Здесь компилятор выводит T по первому аргументу, 
    // а значение N фиксировано в 5.
};

// При использовании:
Array<double, 5> a1;    // подходит под частичную специализацию, T=double, N=5
Array<int, 10> a2;     // не подходит -> используется primary template

```
<!-- Explanation: -->
> Параметры специализации `Array<T, 5>` требуют, чтобы второй не-type параметр был ровно `5`.
> Для `Array<int, 10>` второй параметр равен `10`, поэтому шаблон `Array<T, 5>` не подходит и используется первичный шаблон.

- **Регулярный параметр** (`T`) — тот, который остаётся шаблонным и дедуцируется компилятором.
- **Нерегулярный параметр** (`N`) — тот, который фиксируется в специализации и не дедуцируется.


### Отличия полной и частичной специализаций

- **Полная специализация** указывает шаблон **только** для конкретного набора параметров. 
  - Сигнатура шаблона полностью фиксирована и отсутствуют обобщенные параметры.
  - Пример: `template<> struct Array<int,5> { ... };`

- **Частичная специализация** определяет шаблон для **произвольного** набора параметров, 
  которые соответствуют определённому **паттерну**.
  - Можно указывать только некоторые параметры, а остальные оставлять обобщёнными.
  - Пример: `template<typename T> struct Vector<T, std::allocator<T>> { ... };`
  - При этом параметр `T` дедуцируется компилятором, а аллокатор фиксирован.

**Ключевые моменты:**
1. Полная специализация не создаёт новых шаблонных параметров — она просто предоставляет конкретную реализацию для единственного набора параметров.
2. Частичная специализация работает как правило фильтрации — она применяется ко всем инстанциациям, удовлетворяющим шаблонному паттерну, оставляя возможность дедукции.


Частичная специализация обобщает часть параметров:
```cpp
template<typename T, typename Alloc>
class Vector { /* ... */ };

// Уменьшаем количество параметров:
template<typename T>
class Vector<T, std::allocator<T>> {
    // реализация для случая с дефолтным аллокатором
};

// Специализация по указателю:
template<typename T>
struct Wrapper<T*> {
    // для T*
};
```
- Можно **увеличить** число параметров, введя дополнительные:
```cpp
template<template<typename> class C, typename T>
struct Outer<C<T>> { /* ... */ };
```
- Специализации независимы: разные комбинации создают разные классы.

## II. Метапрограммирование с помощью traits

Стандартная библиотека предоставляет набор шаблонов для проверки и выбора типов:

| Метатрафит                      | Описание                                      |
|---------------------------------|-----------------------------------------------|
| `std::is_void<T>::value`        | истинно, если `T` — `void`                    |
| `std::is_reference<T>::value`   | истинно, если `T` — ссылка                    |
| `std::is_same<T, U>::value`     | истинно, если типы `T` и `U` совпадают        |
| `std::conditional<B, T, F>::type`| `T` если `B==true`, иначе `F`                |

Пример:
```cpp
template<typename T>
using RemoveReferenceT = std::conditional_t<
    std::is_reference<T>::value,
    std::remove_reference_t<T>,
    T
>;
```

## III. Полные специализации функций

- **Полная специализация функции** предоставляет конкретную реализацию:
  ```cpp
  template<typename T>
  void func(T);

  template<>
  void func<int>(int x) {
      // для int
  }
  ```
- **Ограничения:**
  - **Нельзя** частично специализировать функции (только полное).
  - Специализация не участвует в **template argument deduction** — вызов `func(1)` сначала вызывает primary template, а специализация будет взята при полной специализации вызова `func<int>(...)`.
- **Эмуляция частичной специализации при помощи структур**:
  ```cpp
  template<typename T>
  struct FuncWrapper {
    static void call(T);
  };
  template<typename U>
  void func(U u) {
    FuncWrapper<U>::call(u);
  }

  template<>
  struct FuncWrapper<int*> {
    static void call(int* p) { /* ... */ }
  };
  ```

## IV. Пользовательская поддержка `std::swap`

- **Не добавлять** перекрытия или новые функции в `namespace std` (кроме **явных специализаций** стандартных шаблонов для пользовательских типов).
- Для корректной работы алгоритмов со `swap`:
  1. Определите свободную функцию `swap` в том же namespace, что и ваш тип:
     ```cpp
     namespace myns {
       struct X { /* ... */ };
       void swap(X& a, X& b) { /* ... */ }
     }
     ```
  2. При вызове:  
     ```cpp
     using std::swap;
     swap(a, b); // сначала рассматривается myns::swap через ADL, затем std::swap
     ```
- Такой подход гарантирует, что `swap` из вашего namespace будет найден при Argument-Dependent Lookup без нарушения ODR.
- 
</details>

<details>
<summary>
23. Шаблонные друзья
</summary>

## I. Общие вопросы по друзьям
В C++ можно объявлять следующие виды друзей:
- **Нешаблонная функция**  
- **Шаблонная функция**  
- **Нешаблонный класс**  
- **Шаблонный класс**  
- **Полная специализация шаблонного класса**  

Каждый из них при объявлении внутри класса получает доступ ко всем его `private` и `protected` членам. Однако правила видимости и области действия различаются, особенно при шаблонах и при наличии зависимых имён. При демонстрации обращайте внимание на особенности компиляторов: некоторые версии GCC или Clang могут вести себя не по стандарту.

## II. Шаблонный класс-друг произвольного класса

### 1. Общий случай
```cpp
template<typename U>
class FriendClass;

template<typename T>
class A {
private:
    T value_;
    friend class FriendClass<T>;  // шаблонный класс-друг для всех T
};

template<typename U>
class FriendClass {
public:
    void modify(A<U>& a, const U& v) {
        a.value_ = v; // доступ к приватному члену
    }
};
```

### 2. Друг — только полная специализация
```cpp
template<typename T>
class A {
private:
    int secret_ = 42;
    friend class A<int>; // только A<int> является другом
};

template<>
class A<int> {
public:
    void reveal(A<int>& a) {
        // допустимо:
        int s = a.secret_;
    }
};

// A<double> не имеет доступа к private A<double>
```

## III. Нешаблонная функция-друг шаблонного класса

### 1. Определение внутри класса
```cpp
template<typename T>
class B {
private:
    T data_;
    friend void printB(const B<T>& b) { // inline-функция, объявленная и определённая сразу
        std::cout << b.data_;
    }
};
```
- Такая функция становится **inline** и доступна для каждого инстанцирования `B<T>`.

### 2. Определение вне класса
```cpp
template<typename T>
class C {
private:
    T x_;
    friend void showC<>(const C<T>&); // объявление дружбы

    // без body
};

template<typename T>
void showC(const C<T>& c) { // определение вне класса
    std::cout << c.x_;
}
```
### 3. Тонкость с зависимым именем
```cpp
template<typename T>
class D {
    friend void funcD(T); // здесь T — зависимый тип, создаётся новая функция в каждой TU
};
```
- При таком объявлении компилятор может считать `funcD` разной для разных `T`, и нужно следить за ODR.

## IV. Шаблонная функция-друг шаблонного класса

### 1. Определение внутри класса
```cpp
template<typename T>
class E {
private:
    T val_;
    template<typename U>
    friend void tmplFriend(const E<U>& e) { // определено inline в классе
        std::cout << e.val_;
    }
};
```
- При таком объявлении для каждого `U` создаётся своя inline-функция `tmplFriend<U>`.

### 2. Определение вне класса
```cpp
template<typename T>
class F {
private:
    T v_;
    template<typename U>
    friend void extFriend(const F<U>&);
};

template<typename U>
void extFriend(const F<U>& f) { // определение вне класса
    std::cout << f.v_;
}
```

### 3. Друг — только полная специализация шаблонной функции
```cpp
template<typename T>
class G {
    friend void specFriend<int>(const G<int>&); // только для G<int>
};

void specFriend<int>(const G<int>& g) {
    // реализация
}
```

### 4. Проблема multiple definitions
```cpp
template<typename T>
class H {
    friend void helper() { /* ... */ } // не зависит от T, но объявлено в каждом инстанцировании
};
```
- Поскольку `helper` не зависит от параметра шаблона, одинаковые определения появляются в разных TU → нарушение ODR.

</details>

<details>
<summary>
24. Вызов шаблонных функций
</summary>

## I. Автовывод шаблонных параметров функций

1. **Вывод из аргументов**  
   Шаблонные параметры выводятся компилятором по передаваемым аргументам:
   ```cpp
   template<typename T>
   void f(T);
   f(10);        // T deduced as int
   f(3.14);      // T deduced as double
   ```
2. **С `const`, ссылками и наследованием**  
   ```cpp
   template<typename T>
   void g(const T&);
   int x = 0;
   g(x);         // T deduced as int
   g(1);         // T deduced as int (prvalue → T const&)
   ```
3. **Частичное указание параметров**  
   Можно явно задать некоторые параметры:
   ```cpp
   template<typename T, typename U>
   void h(T, U);
   h<int>(1, 2.5); // T=int, U deduced as double
   h<int, double>(1, 2.5); // оба указаны
   ```

## II. Невозможность автовывода

1. **Тип `C` из выражения `typename C::iterator`**  
   ```cpp
   template<typename C>
   void func(typename C::iterator it);
   // func(it); // ошибка: нет вывода C из typename C::iterator
   func<std::vector<int>>(vec.begin());
   ```
2. **Конфликтующие аргументы**  
   ```cpp
   template<typename T>
   void f(T, T);
   // f(1, 2.5); // ошибка: T не может быть одновременно int и double
   ```
3. **Неоднозначное наследование**  
   ```cpp
   struct A {};
   struct B {};
   struct D : A, B {};
   template<typename T>
   void f2(T);
   D d;
   // f2(d); // ok, T=D
   // но при указании типа-члена из D::? может быть неоднозначно
   ```
4. **Требуются пользовательские преобразования**  
   ```cpp
   struct X { X(int); };
   template<typename T>
   void p(T);
   // p(1); // может выдать warning, ведь преобразование int→X потребует user-defined conv
   ```

## III. Разрешение перегрузок с шаблонными функциями

1. **Основные термины**  
   - **overload set** — все функции с одним именем, видимые в точке вызова.  
   - **viable function** — функция, для которой возможен вызов с данным набором аргументов и шаблонных параметров.  
   - **best viable** — из viable выбирается наиболее подходящая по стандартным правилам.

2. **Этапы разрешения**  
   1. Построение overload set.  
   2. Выбор viable functions (matching parameter types/templates).  
   3. Выбор best viable.  
   4. Проверка доступа и специализаций.

3. **Приоритет**  
   Нешаблонные перегрузки имеют приоритет над шаблонными, если они viable.

4. **`foo()` vs `foo<>()`**  
   - `foo()` — компилятор рассматривает как вызов, при котором вывод шаблонных параметров возможен, но без вывода `<>()` может предпочтяться не-template overload.  
   - `foo<>()` — явно запускает вывод шаблонного шаблона, игнорируя нетemplate overload.

5. **Специализации функций не влияют на выбор**  
   - Полные специализации функций не участвуют в overload resolution.  
   - Пример (Dimov-Abrams):  
     ```cpp
     template<typename T>
     void ff(T);
     template<>
     void ff<int>(int); // полная специализация
     void test() {
       ff(1); // вызывает primary template instantiation, а не specialization
     }
     ```
</details>

<details>
<summary>
30. Исключения — основы
</summary>

## I. Предусловия, постусловия и инвариант объекта

- **Предусловия** конструктора: требования, которые должны быть выполнены перед созданием объекта.
- **Постусловия** конструктора: инвариант объекта должен быть установлен корректно, даже если бросается исключение.
- **Инвариант объекта**: логическое условие, которое должно сохраняться между вызовами публичных методов и после завершения конструктора/деструктора.
- Пример из `17-250207/01-exception-guarantees/10-raii.cpp`: класс RAII, гарантирующий освобождение ресурса.

## II. Ошибки програмирования vs. ошибки окружения

- **Ошибки программирования**:
  - Нарушение инварианта, UB: некорректное обращение к памяти, деление на ноль.
  - Обнаруживаются на этапе разработки, не подлежат перехвату через `try/catch`.
- **Ошибки окружения**:
  - Некорректный ввод пользователя, невозможность открыть файл, сбой сети.
  - Обрабатываются через исключения, их можно предсказать и восстанавливать.

## III. Обработка ошибок без исключений

1. **assert**:
   - Проверка условий только в режиме отладки.
   - Прекращает выполнение при ложном условии.
2. **exit**:
   - Завершает программу с кодом.
3. **Коды возврата**:
   - **Состояние объекта** (метод `ok()`, `error_code()`).
   - **Глобальная переменная** `errno`.
   - **Возвращаемое значение** функции.
   - **Отдельный выходной аргумент** через ссылку/указатель.
4. **Стратегии**:
   - Аварийное завершение (`assert`, `exit`).
   - Проброс кода наверх (код возврата).

## IV. Синтаксис `try`/`catch`/`throw`

```cpp
try {
    // блок, где могут возникнуть исключения
    may_throw();
} catch (const std::runtime_error& e) {
    // обработка runtime_error и его наследников
} catch (const std::exception& e) {
    // обработка остальных std::exception
} catch (...) {
    // ловим всё
    throw;  // или повторно бросаем
}
```

- Вложенные `try`/`catch` и последовательные `catch`.
- Исключения классифицируются по типу, применяется подбор наиболее точного.

## V. Слайсинг исключений

- Если бросать по значению базовый тип:
  ```cpp
  try {
      throw Derived(); // Derived -> копируется в базовый Exception
  } catch (const Base& e) {
      // информация о Derived «срезается»
  }
  ```
- **Решение**: бросать и ловить по ссылке (`throw;` сохраняет тип).

## VI. Уничтожение ресурсов при раскрутке стека

- При броске исключения выполняется **stack unwinding**:
  - Деструкторы всех объектов автоматического хранения вызываются в обратном порядке их создания.
- Гарантия освобождения ресурсов при использовании RAII.

## VII. Концепция RAII

- **Resource Acquisition Is Initialization**:
  - Захват ресурса в конструкторе, освобождение в деструкторе.
  - Исключения не приводят к утечкам.
- Примеры: `std::lock_guard`, `std::unique_ptr`.

</details>

<details>
<summary>
31. Исключения — детали
</summary>

## I. Базовые классы исключений

- **`std::exception`**  
  Базовый класс для всех стандартных исключений, определяет виртуальный метод `what()`.

- **`std::runtime_error`**  
  Производный от `std::exception`, использующийся для ошибок времени выполнения (runtime errors).

- **`std::logic_error`**  
  Производный от `std::exception`, обозначает ошибки логики программы (logic errors), которые обычно можно обнаружить на этапе разработки.

## II. Стандартные исключения

- **`std::bad_alloc`**  
  Бросается при неудаче операций выделения памяти (`new`).

- **`std::bad_cast`**  
  Бросается при неудачном приведении типа через `dynamic_cast` при отсутствии `noexcept`.

## III. Производительность исключений

- **Happy path** (когда исключений нет): почти нулевая стоимость при современных реализациях (zero-overhead), особенно если `-fno-exceptions` не используется.
- **Sad path** (когда исключение бросается): высокая стоимость из-за раскрутки стека (stack unwinding), поиска обработчиков, уничтожения локальных объектов.

## IV. Исключения в конструкторах и деструкторах

- Если конструктор бросает исключение, объект считается не сконструированным, и его деструктор **не** вызывается.
- Деструкторы, вызываемые в процессе раскрутки стека, уничтожают всё, что было сконструировано до места выброса.

## V. Исключения при разных операциях

1. **Конструирование массивов**  
   - При выбросе внутри элемента массива уничтожаются уже сконструированные элементы.

2. **Базовые классы и поля**  
   - Если в инициализаторе полей или базового класса бросается исключение, выполняется раскрутка для уже сконструированных полей и баз.

3. **Делегирующий конструктор**  
   - Исключения при делегировании приводят к тому, что делегируемый конструктор должен корректно обрабатывать неполностью сконструированное состояние.

4. **Создание параметров функции**  
   - До C++17 порядок вычисления аргументов не определён, может происходить interleaving подготовки параметров и выброс исключений.

5. **Возврат по значению**  
   - Исключение до или после RVO/NRVO влияет на то, вызывает ли деструктор временного объекта.

## VI. Причина отсутствия `stack::pop()`

- Методы контейнеров, бросающие исключения при удалении элементов, могут нарушить базовую гарантию.
- Для `std::stack` нет `pop()` без возвращения значения, чтобы избежать ситуаций, когда сначала удаляется элемент (может бросить при освобождении ресурсов), а затем возвращается его значение — это затрудняет обеспечение сильной гарантии исключений.

</details>

<details>
<summary>
32. Гарантии исключений
</summary>

## I. Уровни безопасности исключений

- **`no-throw` / `noexcept`**  
  Гарантирует отсутствие исключений. Если внутри выбрасывается исключение, происходит `std::terminate()`.

- **Strong guarantee (строгая)**  
  Операция либо полностью выполняется успешно, либо не меняет состояние объекта. Пример: copy-and-swap.

- **Basic guarantee (базовая)**  
  После исключения объект остаётся в корректном, но возможно изменённом, состоянии, без утечек ресурсов.

- **No guarantee (отсутствующая)**  
  Никаких обещаний: после исключения объект может быть в некорректном состоянии.

## II. Спецификатор `noexcept`

```cpp
void f() noexcept;
```
- Отмечает функцию как бесслучайно не выбрасывающую исключения.
- При выбросе исключения внутри `noexcept`-функции вызывается `std::terminate()`.
- **Типичная гарантия** для оператора перемещения:
  ```cpp
  A(A&&) noexcept;
  A& operator=(A&&) noexcept;
  ```
  Это важно, чтобы стандартные контейнеры могли безопасно перемещать элементы без отката.

## III. Обеспечение базовой гарантии через RAII

- **RAII-классы** автоматически управляют ресурсами в деструкторах.  
- При исключении в середине операции освобождение ресурсов гарантируется автоматическим вызовом деструкторов временных объектов.

```cpp
std::vector<T> tmp = data; // копируем старые элементы
// возможное выбрасывание при копировании внутри конструктора vector
data.swap(tmp); // swap noexcept — состояние остаётся консистентным
```

## IV. Подвохи при реализации `operator=(const&)`

Неправильный порядок операций может нарушить базовую гарантию:

```cpp
MyClass& operator=(const MyClass& other) {
    delete[] data_;           // удалили старые данные
    data_ = new char[n_];     // может бросить bad_alloc
    // При исключении — data_ указывает на освобождённую память → UB
    std::copy(other.data_, other.data_ + n_, data_);
    return *this;
}
```

**Исправление**: сначала выделение, а затем замена полей:

```cpp
MyClass& operator=(const MyClass& other) {
    char* new_data = new char[n_]; // может бросить, но старые данные целы
    std::copy(other.data_, other.data_ + n_, new_data);
    delete[] data_;
    data_ = new_data;
    return *this;
}
```

## V. Strong guarantee через Copy-and-Swap Idiom

```cpp
MyClass& operator=(MyClass other) { // other сконструирован копированием или перемещением
    swap(*this, other);             // noexcept swap
    return *this;
}
```
- При копировании/перемещении `other` может выброситься, но `*this` не изменится.
- После успешного создания `other` и swap операция гарантированно завершится без исключений.

## VI. Default `operator=` и инвариант класса

- Сгенерированный компилятором `operator=` может не обеспечивать даже базовую гарантию,  
  если класс хранит несвязанные ресурсы и имеет строгий инвариант.
- В таких случаях рекомендуется явно реализовать оператор присваивания с использованием вышеописанных техник.

</details>

<details>
<summary>
33. Исключения — необычная обработка
</summary>

## I. Непойманные исключения и раскрутка стека

- Если исключение не поймано ни одним `catch`, происходит раскрутка стека (stack unwinding) до уровня `main()`.
- Если `main()` не содержит `try/catch`, вызывается `std::terminate()`.

## II. `catch (...)`

- Ловит **любое** исключение, независимо от типа.
- Полезно для логирования или в точках «последней надежды».
- **Недостаток:** нельзя получить объект исключения напрямую (нет информации о типе или сообщении).

```cpp
try {
    mayThrow();
} catch (...) {
    std::cerr << "Unknown exception caught
";
}
```

## III. Перебрасывание текущего исключения

- Используется внутри `catch` через `throw;`
- Повторно бросает **тот самый** объект исключения, сохраняя тип и информацию.
- **Возможный слайсинг:**  
  Если вы поймали по значению базового типа, а бросили наследник:
  ```cpp
  try {
      throw Derived();
  } catch (Base e) { // поймали по значению
      throw;         // бросаем восстановленное Base e, информация о Derived утеряна
  }
  ```

## IV. `std::exception_ptr`

- **Создание и захват:**
  ```cpp
  std::exception_ptr p;
  try {
      mayThrow();
  } catch (...) {
      p = std::current_exception(); // захватываем текущее исключение
  }
  ```
- **Перебрасывание/хранение:**
  ```cpp
  try {
      if(p) std::rethrow_exception(p);
  } catch(const std::exception& e) {
      std::cout << e.what();
  }
  ```
- **Пустое состояние:** `exception_ptr` без захваченного исключения равен `nullptr`.

Пример из `17-240206/02-weird-exceptions/01-exception-ptr.cpp`.

## V. Function-try-block

- Позволяет обрабатывать исключения **во всём теле** функции или конструктора, включая инициализацию баз и полей.
- Синтаксис для конструктора:
  ```cpp
  struct S {
      S(int x) try : member(x) {
          // тело конструктора
      } catch(...) {
          // здесь невозможно обращаться к полям S, так как объект не полностью сконструирован
          throw; // можно повторно бросить или преобразовать
      }
  };
  ```
- **Невозможность обращения к полям** и вызова методов объекта до завершения инициализации.
- **Отмена выброса:**  
  - В `catch` можно перехватить исключение и не перебрасывать его, тем самым «гасить» его.
  - Однако для деструктора `function-try-block` не может предотвратить завершение программы, если исключение выброшено из деструга.

</details>

<details>
<summary>
40. Базовая многопоточность
</summary>

## I. Создание потоков в C++11

- **`std::thread`**:
  ```cpp
  #include <thread>
  void worker(int id) {
      // работа потока
  }

  // Запуск нового потока
  std::thread t(worker, 1);

  // Передача ссылок
  int x = 0;
  std::thread t2([](int& ref){ ref = 5; }, std::ref(x));

  // Ожидаем завершения
  t.join();
  t2.join();
  ```

- **Передача параметров**:
  - По значению: аргументы копируются в контекст потока.
  - По ссылке: использовать `std::ref`.

## II. Гонки (Data races)

- **При выводе на экран**:
  ```cpp
  std::cout << "Thread " << id << std::endl;
  ```
  Одновременный вызов из разных потоков может смешивать символы.
- **При доступе к данным**:
  - Совместное чтение — безопасно.
  - Одновременное чтение и запись — UB.
  - Одновременное несколько записей — UB.

## III. Борьба с гонками

- **Мьютексы (`std::mutex`)**:
  ```cpp
  std::mutex mtx;
  void safe_increment(int& counter) {
      std::lock_guard<std::mutex> lock(mtx);
      ++counter;
  }
  ```

- **Атомарные операции**:
  ```cpp
  #include <atomic>
  std::atomic<int> a{0};
  a.fetch_add(1, std::memory_order_relaxed);
  ```

- **RAII-обёртка**:
  ```cpp
  std::unique_lock<std::mutex> lock(mtx);
  // lock.unlock() автоматически вызовется при выходе из области
  ```

## IV. Частичная потокобезопасность `cout`

- `std::cout` обеспечивает безопасность отдельных операций, но не гарантирует консистентность нескольких вызовов подряд.
- Для атомарного вывода нескольких элементов:
  ```cpp
  {
      std::lock_guard<std::mutex> lock(output_mutex);
      std::cout << "Thread " << id << ": " << data << std::endl;
  }
  ```

## V. Joinable и detached потоки

- **Joinable**:
  - По умолчанию поток joinable до вызова `join()` или `detach()`.
  - `std::thread::~thread()` вызовет `std::terminate()`, если поток joinable.

- **Detached**:
  ```cpp
  std::thread t([](){ /* работа */ });
  t.detach(); // передаёт владение ОС, нельзя join
  ```

- **Особенности**:
  - После `detach` нет способа узнать о завершении потока.
  - `join` блокирует текущий поток до окончания работы дочернего.

</details>

<details>
<summary>
41. TCP-соединения при помощи блокирующего ввод-вывода в Boost::Asio
</summary>

## I. Характеристика TCP-соединения

TCP-соединение однозначно определяется **двумя** парами:
- **Local endpoint**: IP-адрес и порт на стороне клиента/сервера.
- **Remote endpoint**: IP-адрес и порт на противоположной стороне.

Каждое соединение уникально сочетанием этих двух пар.

## II. Фиксированный порт сервера и случайный порт клиента

- **Сервер**:
  - Слушает входящие соединения на заранее известном (fixed) порту, чтобы клиенты могли его найти.
  - Обычно `server.listen(порт);`.

- **Клиент**:
  - Создаёт исходящее соединение без явного задания порта: ОС назначает **ephemeral** (временный) порт динамически.
  - Это упрощает масштабирование и позволяет множеству клиентов использовать один и тот же IP, не конфликтуя по портам.

## III. `boost::asio::ip::tcp::iostream` для эхо-сервера и клиента

### Сервер (echo_server.cpp)
```cpp
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>

int main() {
    using boost::asio::ip::tcp;
    tcp::iostream acceptor_stream;
    boost::asio::io_service ios;
    tcp::acceptor acceptor(ios, tcp::endpoint(tcp::v4(), 12345));

    std::cout << "Echo server listening on port 12345
";
    for (;;) {
        tcp::socket socket(ios);
        acceptor.accept(socket); // блокирующий accept
        tcp::iostream stream(std::move(socket));

        std::string line;
        while (std::getline(stream, line)) {
            stream << line << std::endl; // эхо
        }
    }
    return 0;
}
```

### Клиент (echo_client.cpp)
```cpp
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <boost/asio/ip/tcp.hpp>

int main() {
    using boost::asio::ip::tcp;
    tcp::iostream stream("127.0.0.1", "12345"); // IP и порт сервера

    if (!stream) {
        std::cerr << "Connection failed
";
        return 1;
    }

    std::string msg;
    while (std::getline(std::cin, msg)) {
        stream << msg << std::endl;
        std::string reply;
        std::getline(stream, reply);
        std::cout << "Reply: " << reply << std::endl;
    }
    return 0;
}
```

## IV. `local_endpoint()` vs `remote_endpoint()`

- **`stream.socket().local_endpoint()`**  
  Возвращает `endpoint` (IP + порт) на **локальной** стороне соединения.

- **`stream.socket().remote_endpoint()`**  
  Возвращает `endpoint` на **удалённой** стороне (peer).

Пример:
```cpp
auto local = stream.socket().local_endpoint();
auto remote = stream.socket().remote_endpoint();
std::cout << "Local: " << local << ", Remote: " << remote << "
";
```

## V. Потоковый протокол vs сообщения

- TCP передаёт **поток байт**, а не дискретные сообщения.
- При чтении можно получить:
  - **Частичный фрагмент** длинного сообщения.
  - **Несколько сообщений** в одном приёме.

**Решения**:
1. **Делим по разделителю** (например, `'
'` или специальная метка).
2. **Используем префиксы длины**: сначала отправляем размер сообщения, затем данные.
3. **Фиксированная длина**: если все сообщения одного размера, просто читаем заданное число байт.

</details>

<details>
<summary>
42. Оповещение о событиях
</summary>

## I. Условные переменные (`std::condition_variable`)

- **Назначение**: позволяют потокам ожидать определённых условий, освобождая мьютекс и блокируясь до уведомления.
- **Основные методы**:
  ```cpp
  std::condition_variable cv;
  std::mutex mtx;
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock, []{ return condition; });  // дождаться, когда condition станет true
  // или
  cv.wait(lock);
  // после notify_one()/notify_all() нужно снова проверять условие из-за spurious wakeup
  ```
- **Spurious wakeup**: ложные пробуждения без вызова `notify`; всегда проверяйте условие в цикле или используйте перегрузку `wait(lock, pred)`.

## II. Реализация паттерна Producer-Consumer

```cpp
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

std::queue<int> buffer;
std::mutex mtx;
std::condition_variable cv;
const size_t MAX_BUFF = 10;

void producer(int id) {
    for(int i = 0; i < 20; ++i) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []{ return buffer.size() < MAX_BUFF; }); // дождаться места
        buffer.push(i);
        std::cout << "Producer " << id << " produced " << i << std::endl;
        lock.unlock();
        cv.notify_all();
    }
}

void consumer(int id) {
    for(int i = 0; i < 20; ++i) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, []{ return !buffer.empty(); }); // дождаться элемента
        int val = buffer.front();
        buffer.pop();
        std::cout << "Consumer " << id << " consumed " << val << std::endl;
        lock.unlock();
        cv.notify_all();
    }
}

int main() {
    std::thread p1(producer, 1), p2(producer, 2);
    std::thread c1(consumer, 1), c2(consumer, 2);

    p1.join(); p2.join();
    c1.join(); c2.join();
    return 0;
}
```

## III. Ключевое слово `mutable`

- Позволяет изменять данные-члены в константных методах:
  ```cpp
  class C {
    mutable std::mutex mtx_;
    int data_;
  public:
    int get() const {
      std::lock_guard<std::mutex> lock(mtx_); // mtx_ можно изменить внутри const метода
      return data_;
    }
  };
  ```
- Часто используется совместно с `std::mutex` и `std::condition_variable` для защиты состояния объекта в `const` методах.

</details>

<details>
<summary>
43. Дизайн многопоточных приложений
</summary>

## I. Deadlock и recursive mutex

- **Deadlock (взаимная блокировка)** возникает, когда два или более потока блокируют ресурсы в обратном порядке:
  ```cpp
  std::mutex m1, m2;
  // Поток A:
  std::lock_guard<std::mutex> lock1(m1);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  std::lock_guard<std::mutex> lock2(m2);

  // Поток B:
  std::lock_guard<std::mutex> lock2b(m2);
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  std::lock_guard<std::mutex> lock1b(m1);
  ```
  → затруднение, оба потока ждут друг друга.

- **`std::recursive_mutex`** позволяет одному потоку многократно блокировать один и тот же мьютекс:
  ```cpp
  std::recursive_mutex rm;
  rm.lock();
  rm.lock(); // не блокирует второй раз
  rm.unlock();
  rm.unlock();
  ```
  Полезен для рекурсивных функций, но менее эффективен, чем обычный мьютекс.

## II. Избежание взаимных блокировок

- **Контроль порядка взятия блокировок**: всегда блокировать мьютексы в одном порядке во всех потоках.
- **`std::scoped_lock` / `std::lock`** (C++17):
  ```cpp
  std::mutex m1, m2;
  std::scoped_lock lock(m1, m2); // блокирует оба без взаимных deadlock
  ```
- **`std::unique_lock`** с `std::defer_lock` и `std::lock`:
  ```cpp
  std::unique_lock<std::mutex> ul1(m1, std::defer_lock);
  std::unique_lock<std::mutex> ul2(m2, std::defer_lock);
  std::lock(ul1, ul2);
  ```

## III. `thread_local` vs глобальные переменные

- **`thread_local`**: каждая нить имеет свою копию переменной:
  ```cpp
  thread_local int counter = 0;
  ```
- **Глобальная переменная**: общая для всех потоков, требует синхронизации.
- `thread_local` безопасна для доступа без мьютексов, но не годится для обмена данными между потоками.

## IV. Частичная потокобезопасность `shared_ptr`

- **Атомарные операции на контрольном блоке**:
  - Копирование и уничтожение `std::shared_ptr` безопасно (удобно между потоками).
  - **Доступ к самому управляемому объекту** не защищён: чтение и запись содержимого требует внешней синхронизации.

## V. Проблема TOCTOU (Time-Of-Check to Time-Of-Use)

- Когда проверка условия и использование ресурса разделены временем:
  ```cpp
  if (exists(file)) {
      open(file); // между exists и open файл может быть удалён
  }
  ```
- **Решение**:
  - Использовать атомарные или трансакционные API (например, `open` с флагом `O_CREAT|O_EXCL`).
  - Минимизировать промежуток между проверкой и использованием.
  - Применять блокировки файловой системы.

</details>

<details>
<summary>
50. Trivially Copyable
</summary>

## I. Кодировки строк и порядок байт

- **Строковые кодировки**: `std::string` хранит байты (обычно в UTF-8) без семантики символов; могут использоваться и UTF-16/UTF-32.
- **Endianness**:
  - **Little-endian** (Intel): младший байт в младшем адресе.
  - **Big-endian** (Network order): старший байт в младшем адресе.
  - Влияет на побайтную сериализацию чисел:
    ```cpp
    uint32_t x = 0x11223344;
    unsigned char *p = reinterpret_cast<unsigned char*>(&x);
    // p[0] == 0x44 (LE) или 0x11 (BE)
    ```

## II. `reinterpret_cast` и strict aliasing

- **`reinterpret_cast<T*>`** позволяет смотреть на биты как на `T`, но:
  - **Strict aliasing rule** запрещает читать объект одним типом, отличным от его реального, за исключением:
    - `char*`, `unsigned char*`, `std::byte*` — разрешены для доступа к любым объектам.
  - Для безопасного копирования:
    ```cpp
    std::memcpy(&dest, &src, sizeof(src));
    ```

## III. Trivially Copyable структуры

- **Определение** (C++11):  
  Класс `T` является trivially copyable, если:
  - Тривиальный (compiler-generated) копирующий/move-конструктор, копирующее/move-присваивание и деструктор.
  - Не содержит виртуальных функций или виртуальных баз.
- **Пример**:
  ```cpp
  struct A { 
      int x; 
      double y; 
  }; // trivially copyable
  ```
- **Использование для сериализации**:
  ```cpp
  A a{1, 2.5};
  std::vector<std::byte> buf(sizeof(A));
  std::memcpy(buf.data(), &a, sizeof(A));
  // запись в файл, передача по сети
  ```

## IV. Padding и выравнивание

- **Padding** добавляется компилятором между полями и в конце структуры для выравнивания:
  ```cpp
  struct B {
      char c;    // offset 0
      // padding 3 байта 
      int i;     // offset 4
  };
  sizeof(B) == 8;
  ```
- **Отключение выравнивания**:
  ```cpp
  #pragma pack(push,1)
  struct Packed { char c; int i; };
  #pragma pack(pop)
  ```
- **Последствия**:
  - Доступ к полям может быть медленнее или вызывать UB на архитектурах с жёсткими требованиями выравнивания.
  - Нетривиальные типы (`std::vector<T>`) не могут располагаться на выровненной памяти без специальных средств.

## V. Ссылки и указатели на невыровненную память

- **Нарушение выравнивания**:
  - Приведение `char*` к `T*` и доступ к `*T_ptr` может быть UB, если указатель не выровнен.
- **Пример со `swap` полей**:
  ```cpp
  struct C { int x; double y; };
  alignas(8) char buffer[sizeof(C)];
  C* pc = reinterpret_cast<C*>(buffer);
  // если buffer адрес не кратен 8, доступ к pc->y будет небезопасен
  ```

## VI. Standard Layout и POD

- **Standard Layout** (C++11):
  - Отсутствуют виртуальные функции, все нестатические поля одного access-specifier, нет неоднородных баз и т.д.
- **POD** (`Plain Old Data`):
  - Класс или структура, удовлетворяющие как trivial, так и standard-layout.
  - Могут использоваться в C-совместном коде и сериализовываться через `memcpy`.

</details>

<details>
<summary>
51. Многомерные массивы
</summary>

## I. C-style массивы

1. **Инициализация**:
   ```cpp
   int a[3] = {1, 2, 3};
   int b[3] = {};        // все элементы 0
   int c[3] = {1, 2};    // оставшиеся обнуляются
   ```
2. **Невозможность копирования**:
   - Нельзя присвоить массивы напрямую: `int x[3], y[3]; y = x; // ошибка`.
3. **Арифметика указателей**:
   - `a` приводится к `int*`, `a + 1` указывает на следующий элемент.
   - Для многомерного: `a[i][j]` эквивалентно `*(*(a + i) + j)`.

4. **Variable-Length Arrays (VLA)** (Расширение компилятора, не ISO C++):
   ```cpp
   void foo(int n) {
       int arr[n]; // поддерживается в GCC/Clang как расширение
   }
   ```

5. **Динамическое выделение**:
   ```cpp
   int* p = new int[10];
   delete[] p;
   ```

## II. Статические и динамические массивы

1. **Массив известного размера**:
   ```cpp
   int arr[5];
   ```
2. **Массив неизвестного размера** (только в параметрах функций):
   ```cpp
   void func(int arr[], size_t n);
   ```
3. **Указатель на массив**:
   ```cpp
   int (*p)[5] = &arr;
   ```
4. **`auto` и вывод размера в шаблонах**:
   ```cpp
   template<typename T, size_t N>
   void func(T (&arr)[N]) {
       // N выведено компилятором
   }
   ```

## III. Многомерные массивы

- **Определение**:
  ```cpp
  int m[2][3] = {{1,2,3}, {4,5,6}};
  ```
- **Неизвестное нулевое измерение**:
  ```cpp
  extern int m[][3]; // объявление, первый размер не нужен
  ```
- **Доступ**: `m[i][j]`.

## IV. Массивы массивов и динамическое выделение

- **Единоразовое выделение**:
  ```cpp
  int (*m)[3] = new int[2][3];
  // ...
  delete[] m;
  ```
- **Выделение через одномерный буфер**:
  ```cpp
  int* buf = new int[2*3];
  int (*m2)[3] = reinterpret_cast<int (*)[3]>(buf);
  // использование m2[i][j]
  delete[] buf;
  ```

## V. `std::array`

- **Определение**:
  ```cpp
  std::array<std::array<int, 3>, 2> arr = {{{1,2,3}, {4,5,6}}};
  ```
- **Преимущества**:
  - Поддерживает копирование и присваивание.
  - Имеет методы `size()`, `begin()`, `end()`.
  - Совместимость с контейнерами STL.

</details>

<details>
<summary>
52. Указатели
</summary>

## I. Указатели на указатели

1. **Выделение и освобождение**, пример массива строк:
   ```cpp
   // Массив из 3 строк
   char** arr = new char*[3];
   for(int i = 0; i < 3; ++i) {
       arr[i] = new char[20];      // выделяем буфер под каждую строку
       std::strcpy(arr[i], "example");
   }
   // использование
   for(int i = 0; i < 3; ++i) {
       std::puts(arr[i]);
       delete[] arr[i];            // освобождаем каждую строку
   }
   delete[] arr;                   // освобождаем массив указателей
   ```

2. **Output-параметр функции**:
   ```cpp
   void allocate_buffer(char** out_buf, size_t n) {
       *out_buf = static_cast<char*>(std::malloc(n));
   }
   // Использование:
   char* buf = nullptr;
   allocate_buffer(&buf, 128);
   // ... использовать buf
   std::free(buf);
   ```

3. **`const` в указателях на указатели**:
   - `char* const* p` — указатель на `char* const`: нельзя изменить указатель `char*`, но можно изменить `*p`.
   - `char** const p` — константный указатель на `char*`: нельзя изменить сам `p`, но можно `*p`.
   - `const char* const* p` — указатель на константный указатель на константные `char`: нельзя изменить ни что-либо.

4. **Преобразования**:
   - `Derived**` → `Base**` **НЕ** безопасно, даже если `Derived*` наследует `Base*`.
   - Допустимы преобразования `T**` ↔ `void**` **только** через явное приведение.
   - Ковариантность указателей не распространяется на `T**`.

## II. `realloc` и его безопасное использование

- **Небезопасное**:
  ```cpp
  char* s = static_cast<char*>(std::malloc(10));
  // ...
  s = static_cast<char*>(std::realloc(s, 4));
  // если realloc вернёт nullptr, утечка старого блока, а s = nullptr утеряла исходный указатель
  ```
- **Безопасное**:
  ```cpp
  char* temp = static_cast<char*>(std::realloc(s, 4));
  if (temp) {
      s = temp;
  } else {
      // обработка ошибки, s ещё указывает на старый блок
  }
  ```

## III. `restrict` (C99/C2x)

- **Ключевое слово** (стандартно в C99, в C++ доступно как расширение в некоторых компиляторах):
  ```cpp
  void copy(int* restrict dst, const int* restrict src, size_t n) {
      for(size_t i = 0; i < n; ++i) {
          dst[i] = src[i];
      }
  }
  ```
- **Семантика**: указатели, отмеченные `restrict`, не пересекаются на время жизни функции, что позволяет оптимизировать доступ к памяти.
- Используется для повышения производительности в критичных по скорости участках кода.

</details>

<details>
<summary>
53. Обобщённые функции
</summary>

## I. Указатели на функции

1. **Синтаксис**:
   ```cpp
   // Функция
   int func(double);
   // Указатель на функцию
   int (*pf)(double) = &func;
   ```
2. **Использование как параметров**:
   ```cpp
   void callFunc(int (*f)(double), double x) {
       int r = f(x);
       // ...
   }
   ```
3. **Отсутствие типа у перегруженной/шаблонной функции**:
   - Перегруженная функция без уточнения шаблонного/не шаблонного параметров неоднозначна.
4. **Взятие указателя на перегруженную/шаблонную функцию**:
   ```cpp
   int (*pf1)(double) = static_cast<int(*)(double)>(&func);
   template<typename T>
   T (*pf2)(T) = &someTemplateFunction<T>;
   ```
5. **Лямбда-выражения в указателе на функцию**:
   ```cpp
   void (*pLam)(int) = [](int x){ /*...*/ };
   // Если лямбда без захватов, она может превращаться в указатель на функцию
   ```
6. **Преобразования между указателями на функции**:
   - Указатели разных сигнатур несовместимы, требуется `reinterpret_cast`, что является UB, но поддерживается некоторыми компиляторами.

## II. Указатель `void*`

1. **Неявные преобразования**:
   ```cpp
   int i = 10;
   void* pv = &i;      // int* → void*
   ```
2. **Явные преобразования**:
   ```cpp
   void* pv = &i;
   int* pi = static_cast<int*>(pv);
   ```
3. **Расширение компилятора**:
   - Некоторые компиляторы поддерживают арифметику указателей `void*` как `char*`, но это не стандарт C++ (GNU extension).

## III. `void*` функции как альтернатива функциональным объектам

- Пример `for_each`, принимающий функцию с `void*`:
  ```cpp
  void for_each(void* arr, size_t count, size_t elem_size, void (*func)(void*)) {
      char* data = static_cast<char*>(arr);
      for(size_t i = 0; i < count; ++i) {
          func(static_cast<void*>(data + i * elem_size));
      }
  }

  // Использование:
  int a[] = {1,2,3};
  for_each(a, 3, sizeof(int), [](void* p){
      int* ip = static_cast<int*>(p);
      std::cout << *ip << std::endl;
  });
  ```

</details>

<details>
<summary>
54. Отличия Си и C++
</summary>

## I. Комментарии и объявления переменных

- **Комментарии**:  
  - C89: только `/* ... */`  
  - C99 и C++: добавлен `// ...`

- **Объявления переменных**:  
  - C89: объявления только в начале блока (`{}`)  
  - C99/C++: объявления в любом месте кода

## II. `(void)` в объявлении функции и неявные объявления

- **C89**:  
  ```c
  int func();    // неявно func принимает неограниченные параметры
  int func(void); // явно без параметров
  ```
- **C++**:  
  ```cpp
  int func();    // означает отсутствие параметров
  ```

## III. Объявления структур и `typedef`

- **C:**  
  ```c
  struct S { int x; };
  struct S s;           // нужно ключевое слово struct
  typedef struct S S_t; // alias через typedef
  S_t t;
  ```
- **C++:**  
  ```cpp
  struct S { int x; };
  S s;                  // struct можно опустить
  // typedef необязателен
  using S_t = S;
  ```

## IV. `malloc`/`free` vs `new`/`delete`

- **C:**  
  ```c
  int *p = malloc(10 * sizeof(int));
  free(p);
  ```
- **C++:**  
  ```cpp
  int *p = new int[10];
  delete[] p;
  ```
- **Отличия:**  
  - `new` вызывает конструкторы, может бросать `std::bad_alloc`.  
  - `malloc` возвращает `nullptr` при ошибке, без конструкторов.  
  - `free` и `delete` несовместимы между собой.

## V. Преобразования `void*`

- **C:**  
  ```c
  void *pv = p;   // implicit conversion from any object*
  ```
- **C++:**  
  ```cpp
  void *pv = p;                   // OK
  int *pi = static_cast<int*>(pv); // requires explicit cast
  ```

## VI. Альтернативы языковых возможностей

| Возможность     | C                          | C++                           |
|-----------------|----------------------------|-------------------------------|
| Приватность     | нет                        | `namespace`, `class/private`  |
| Приведение типов| макросы, `void*`           | `static_cast`, `reinterpret_cast`, `const_cast` |
| Инициализация   | `=` init                   | list-initialization `{}`      |
| Логический тип  | `int`                      | `bool`                        |
| Поддержка ООП   | нет                        | классы, наследование, виртуальность |
| RAII            | нет                        | конструктор/деструктор        |
| Шаблоны         | нет                        | `template`                    |
| Перегрузка      | нет                        | функций и операторов          |

## VII. Недоступные возможности в C

- Шаблоны  
- Перегрузка функций  
- Параметры по умолчанию  
- Стандартная библиотека STL  
- Исключения  
- RAII  

</details>

<details>
<summary>
55. Особенности и идиомы Си
</summary>

## I. `printf` / `scanf`

- **Ограничение буфера для `%s`**  
  ```c
  char buf[10];
  scanf("%9s", buf); // указываем максимальную длину - 1 для завершающего '\0'
  ```

- **Вывод символа `%`**  
  ```c
  printf("Percentage: 100%%");
  ```

- **Получение текущей позиции при `scanf`**  
  ```c
  int n;
  scanf("%d%n", &n, &chars_read);
  // chars_read содержит количество прочитанных символов до %n
  ```

- **Потенциально квадратичное время**  
  Использование `scanf` в цикле для чтения строк может быть медленным из-за многократного анализа формата:
  ```c
  while (scanf("%9s", buf) == 1) { /* ... */ }
  ```

## II. Designated Initializers (C99)

- **Инициализация полей структуры**:
  ```c
  struct Point { int x, y, z; };
  struct Point p = { .y = 2, .x = 1, .z = 3 };
  ```

- **Инициализация массивов**:
  ```c
  int arr[5] = { [2] = 10, [4] = 20 };
  ```

## III. Макросы для констант

- Вместо `const int` часто используется:
  ```c
  #define PI 3.14159
  #define BUFFER_SIZE 1024
  ```

- **Особенности**:
  - Нет проверки типов.
  - Требуется осторожность с группировкой (использовать скобки в макросах).

## IV. `goto` для выхода из циклов и обработки ошибок

- **Выход из вложенных циклов**:
  ```c
  for (int i = 0; i < n; ++i) {
      for (int j = 0; j < m; ++j) {
          if (error) goto error_handler;
      }
  }
  // ...
  error_handler:
      // очистка и выход
  ```

- **Обработка ошибок**:
  ```c
  char *buf = malloc(n);
  if (!buf) goto cleanup;
  // ...
  cleanup:
      free(buf);
      return -1;
  ```

## V. Union и анонимные объединения / структуры

- **Union**:
  ```c
  union Value {
      int i;
      float f;
      char s[4];
  };
  ```

- **Анонимные union / struct** (C11):
  ```c
  struct S {
      union {
          int i;
          float f;
      }; // без имени union
  };
  struct S x;
  x.i = 10;   // доступ напрямую
  x.f = 3.14; // доступ напрямую
  ```

</details>

<details>
<summary>
56. Взаимодействие с библиотеками на Си
</summary>

## I. Хранение строк в POD

- **C-style строки**:
  - `char buf[N];` — массив символов с завершающим `\0`.
  - `char* ptr;` — указатель на динамически выделенную или статическую строку.
- **`std::string`**:
  - Не trivially copyable, но обеспечивает RAII для C-style интерфейсов.
  - Для передачи в C-функции: `c_str()` или `data()`.

## II. Выделение буфера и безопасность

- **Ручное выделение**:
  ```c
  char* buf = (char*)malloc(100);
  if (!buf) { /* ошибка */ }
  ```
- **Небезопасность `gets`**:
  - `gets(buf);` не ограничивает размер буфера → buffer overflow.
  - Использовать `fgets(buf, size, stdin)` или `getline`.

## III. Конвенции выделения ресурсов и opaque-структуры

- **Opaque структуры**:
  ```c
  // header.h
  typedef struct Opaque Opaque;
  Opaque* create();
  void destroy(Opaque*);
  ```
- **Выделение пользователем** (C API):
  ```c
  void parse(const char* input, char* output, size_t out_size);
  ```
- **Выделение библиотекой**:
  ```c
  // library возвращает структуру, выделенную внутри
  MyHandle* h = open_handle();
  close_handle(h);
  ```

## IV. `const_cast`

- Используется, чтобы убрать `const` для вызова C API:
  ```cpp
  const char* src = "hello";
  // C API ожидает char*
  char* modifiable = const_cast<char*>(src);
  c_api(modifiable);
  ```

- **Осторожно**: неизменяемые литералы в размещённой памяти менять нельзя.

## V. `extern "C"` и линковка

- **Интерфейс C**:
  ```cpp
  extern "C" {
      #include "c_library.h"
  }
  ```
- Предотвращает name mangling, обеспечивая корректную линковку с C-библиотеками.
- **Совместимые заголовки**:
  ```cpp
  #ifdef __cplusplus
  extern "C" {
  #endif

  void c_function(int);

  #ifdef __cplusplus
  }
  #endif
  ```

- **Линковка со стандартной C-библиотекой**:
  - В C++ функции `malloc`, `free`, `fopen` и т.д. находятся в глобальном пространстве имён и в `std`-пространстве.

</details>

<details>
<summary>
60. Множественное наследование
</summary>

## I. Синтаксис и пример

```cpp
struct A {
    virtual void foo() { }
};

struct B {
    virtual void bar() { }
};

struct C : A, B {
    void foo() override { }
    void bar() override { }
};
```

- `C` наследует **две** базовые подчасти `A` и `B`.

## II. Представление в памяти и `static_cast`-приведение

- В памяти объект `C` содержит два подобъекта:
  ```
  [A subobject]    // начало объекта C
  [B subobject]    // смещено после A
  [C members...]
  ```
- При `static_cast<B*>(ptrA)`, где `ptrA` указывает на A-часть `C`, корректно учитывается смещение:
  ```cpp
  C c;
  A* pa = &c;
  B* pb = static_cast<B*>(pa); // делает pointer adjustment
  ```

## III. Порядок инициализации/уничтожения

- **Инициализация** базовых классов и полей происходит в порядке объявления баз:
  ```cpp
  struct D : B, A { /* ... */ };
  // Сначала B, затем A, затем поля D
  ```
- **Деструктор** выполняется в обратном порядке: сначала поля `D`, затем `A`, потом `B`.
- **Передача параметров**: список инициализации конструктора `C` позволяет указывать конструкторы баз:
  ```cpp
  C(int av, int bv) : A(av), B(bv) { }
  ```

## IV. Дублирование баз и неоднозначности

- При наследовании от одного и того же базового более одного раза (диамант):
  ```cpp
  struct Base { int x; };
  struct Left : Base { };
  struct Right : Base { };
  struct Diamond : Left, Right { };
  // Diamond содержит два Base подобъекта
  ```
- Приведение к `Base*` из `Diamond*` неоднозначно:
  ```cpp
  Diamond d;
  // Base* pb = &d; // ошибка: неоднозначность Left::Base или Right::Base
  ```

## V. Cross-cast (side-cast)

- **Cross-cast** — приведение между двумя базовыми подчастями:
  ```cpp
  C c;
  A* pa = &c;
  B* pb = dynamic_cast<B*>(pa); // side-cast: A→C→B
  ```
- В примере `14-250117/03-multiple-inheritance/10-side-cast.cpp` показано, как `dynamic_cast` корректно выполняет такой cast.

## VI. Приведения и виртуальные функции

- **`dynamic_cast`**:
  - Поддерживает безопасное приведение между подчастями среди виртуальных баз.
  - Возвращает `nullptr` при неудаче (для указателей).
- **`static_cast`**:
  - Делает pointer adjustment без проверки времени выполнения.
  - Для cross-cast требуется сначала `static_cast` к производному, затем к другой базе.
- **Неявное преобразование**:
  - Является `static_cast` для прямых наследований, но неоднозначна при дублировании баз.
- **`override`** и одинаковые виртуальные функции:
  - Если `A` и `B` имеют методы с одинаковой сигнатурой, `C` должен переопределить явное `override` для каждой:
    ```cpp
    struct C2 : A, B {
        void foo() override; // override A::foo
        void foo_B() { B::foo(); }
    };
    ```
- **Приватное/защищённое наследование**:
  - Для неявных преобразований приватное наследование запрещает преобразование `Derived*` → `Base*` вне `Derived`.

</details>

<details>
<summary>
61. Виртуальное наследование
</summary>

## I. Виртуальный базовый класс и проблема ромба

- **Синтаксис виртуального наследования**:
  ```cpp
  struct Base { int x; };
  struct Left : virtual Base { };
  struct Right : virtual Base { };
  struct Diamond : Left, Right { };
  ```
- **Проблема ромба**: без virtual каждая ветвь имела свою копию Base, при virtual обе ветви делят единый Base-подобъект.

## II. Порядок инициализации/уничтожения

- **Конструктор Diamond** инициализирует virtual-Base первым:
  ```cpp
  Diamond(int bx, int lx, int rx)
    : Base(bx)        // единственная инициализация Base
    , Left(lx)
    , Right(rx)
  { }
  ```
- **Порядок**:
  1. virtual-базы (`Base`)
  2. небазовые прямые базы в порядке объявления (`Left`, `Right`)
  3. поля `Diamond`
- **Деструкторы** вызываются в обратном порядке.

## III. Представление в памяти (неполиморфные классы)

- При virtual-наследовании объект содержит **vtbl** указатель и/или указатели на виртуальные базы:
  ```
  [Diamond subobject]
    ├── vptr (для RTTI/virtual)
    ├── Base shared subobject
    ├── Left subobject
    └── Right subobject
  ```
- Virtual Base хранит смещение через **виа-поинтер** или таблицу смещений.

## IV. Виртуальное и невиртуальное наследование одного базового

- Если:
  ```cpp
  struct A { };
  struct B1 : A { };
  struct B2 : virtual A { };
  ```
- `B1` содержит собственную `A`, `B2` — общую виртуальную `A`.
- При множественном наследовании нужно учитывать две разные модели.

## V. `dynamic_cast` и `static_cast`

- **С виртуальными методами**: `dynamic_cast<Derived*>(basePtr)` работает благодаря наличию RTTI.
- **Без виртуальных методов**: `dynamic_cast` к указателю не скомпилируется (нет информации о типе).
- **`static_cast`** корректно меняет указатель, выполняя pointer adjustment:
  - Для прямого виртуального наследования возможно, но для cross-cast нужно сначала cast в most derived.

## VI. Delegate-to-sister (side-cast)

- Пример приведения одной ветви к другой:
  ```cpp
  Diamond d;
  Left* pl = &d;
  Right* pr = dynamic_cast<Right*>(pl); // находит общий Diamond, затем Right
  ```

## VII. Приватное и защищённое virtual-наследование

- Для private/protected virtual баз `static_cast<Derived*>(basePtr)` вне класса запрещён.
- `override` для виртуальных функций работает аналогично обычному наследованию, но через единую базу.

</details>

<details>
<summary>
62. Трюки с наследованием классов (необязательно множественным)
</summary>

## I. Наследование и спецификаторы доступа

- **`public`** наследование: «является» (is-a) — все публичные и защищённые члены базового становятся соответствующего уровня доступа в производном.
- **`protected`** наследование: всё, что было `public` в базе, становится `protected` в производном; вне класса доступ ограничен.
- **`private`** наследование: всё, что было `public` или `protected` в базе, становится `private` в производном.

- **`struct` vs `class`**:
  - По умолчанию у `struct` все члены и наследование `public`.
  - У `class` — `private`.

## II. Срезка объектов (slicing)

```cpp
struct Base { virtual void foo() {} };
struct Derived : Base { int extra; };

Derived d;
Base b = d; // slicing: поле extra теряется
b.foo();    // работает с Base-частью
```

- **Защита**:
  - Используйте **указатели** или **умные указатели** (`std::unique_ptr<Base>`, `std::shared_ptr<Base>`).
  - Храните и передавайте по ссылке (`Base&`) или по указателю.

## III. Получение наиболее производного класса

- **`dynamic_cast`**:
  ```cpp
  Base* pb = getBase();
  if (Derived* pd = dynamic_cast<Derived*>(pb)) {
      // pb действительно указывает на Derived
  }
  ```

## IV. Делегирующие конструкторы

- Позволяют одному конструктору вызывать другой в списке инициализации:
  ```cpp
  struct S {
      int x; double y;
      S(int a) : S(a, 0.0) {}       // делегирует другому
      S(int a, double b) : x(a), y(b) {}
  };
  ```
- **Исключения**: при делегировании инвариант объекта должен быть установлен только в самом полном конструкторе.

## V. Реализация виртуальных функций через V-таблицу

- Каждый класс с виртуальными методами хранит **vptr** — указатель на таблицу виртуальных функций (**vtable**).
- При вызове `obj.foo()` компилятор генерирует косвенный вызов через vtable:
  ```cpp
  // псевдокод
  vptr->foo(&obj);
  ```

## VI. CRTP (Curiously Recurring Template Pattern)

- Шаблонный паттерн, где класс наследует шаблон от производного:
  ```cpp
  template<typename Derived>
  struct BaseCRTP {
      void interface() { static_cast<Derived*>(this)->implementation(); }
  };

  struct Concrete : BaseCRTP<Concrete> {
      void implementation() { /* ... */ }
  };
  ```
- **Преимущества**:
  - Статический полиморфизм без виртуальных функций.
  - Расширение интерфейса базового шаблоном.

</details>

<details>
<summary>
70. Препроцессор: базовые макросы
</summary>

## I. Конкатенация строковых литералов

В C/C++ строковые литералы, записанные подряд, автоматически объединяются:
```c
const char* s = "Hello, "
                "world!";
```
Эквивалентно `"Hello, world!"`.

## II. Условная компиляция: `#ifdef`, `#define`, `-DDEBUG`

- **Определение макроса**:
  ```c
  #define DEBUG
  ```
- **Проверка**:
  ```c
  #ifdef DEBUG
  // код только для отладки
  #endif
  ```
- **Ключ `-DDEBUG`** при компиляции:
  ```
  gcc -DDEBUG file.c
  ```

## III. `#if`, `defined()`

```c
#if defined(DEBUG) && (VALUE > 0)
  // ...
#endif
```
Позволяет комбинировать условия и проверять значения макросов.

## IV. Определение компилятора и `#pragma`

- **Компилятороспецифичные макросы**:
  ```c
  #ifdef __GNUC__
    #pragma GCC diagnostic ignored "-Wunused-variable"
  #endif
  #ifdef _MSC_VER
    #pragma warning(disable: 4996)
  #endif
  ```

## V. Приоритет операторов и повторное вычисление в макросах

```c
#define SQUARE(x) (x)*(x)

SQUARE(1+2) // разворачивается как (1+2)*(1+2) == 9, но может быть неожиданным без скобок
```
- Следует использовать скобки вокруг параметров и всего выражения:
  ```c
  #define SQUARE(x) ((x)*(x))
  ```

## VI. Мульти-стейтментные макросы и `do { } while(0)`

```c
#define LOG(msg) do {               printf("LOG: %s
", msg);   } while(0)
```
Гарантирует корректную работу в `if`/`else`.

## VII. `operator,` и вызов макросов

Чтобы `assert(true), assert(false);` работало, нужно:
```c
#define ASSERT(expr) ((expr) ? (void)0 : assert_failed())

void assert_failed(void) {
    // обработка
}
```
Или использовать `do { ... } while(0)` для составных операторов.

## VIII. Вариадические макросы и `__VA_ARGS__`

```c
#define DEBUG_PRINT(fmt, ...)     printf("DEBUG: " fmt "
", __VA_ARGS__)
```
- **Проблема последней запятой**:  
  ```c
  DEBUG_PRINT("Hello"); // ошибка: лишняя запятая
  ```
  Решение (GCC/Clang extension):
  ```c
  #define DEBUG_PRINT(fmt, ...)       printf("DEBUG: " fmt "
    ", ##__VA_ARGS__)
  ```

## IX. Проблемы с инициализатором списка в макросах

При использовании:
```cpp
assert(v == std::vector{1, 2, 3});
foo({1, 2});
foo(map<int, int>{});
```
макросы могут не корректно парситься. Решения:
- Добавлять скобки вокруг параметра при вызове макроса.
- Использовать `do { ... } while(0)` и `__VA_ARGS__` с `##__VA_ARGS__`.

</details>

<details>
<summary>
71. Препроцессор: генерация кода и `inline`
</summary>

## I. Макрос как параметр макроса

- **Автоматический обход элементов `enum`**:
  ```c
  #define FOREACH_COLOR(F) \
      F(RED)               \
      F(GREEN)             \
      F(BLUE)

  #define GENERATE_ENUM(ENUM) ENUM,
  enum Color {
      FOREACH_COLOR(GENERATE_ENUM)
  };

  #define GENERATE_STRING(ENUM) #ENUM,
  const char* ColorNames[] = {
      FOREACH_COLOR(GENERATE_STRING)
  };
  ```

## II. Оператор `##` (конкатенация токенов)

- **Создание уникальных имён**:
  ```c
  #define CONCAT(a, b) a##b
  #define UNIQUE_NAME(prefix) CONCAT(prefix, __LINE__)

  int UNIQUE_NAME(var) = 5; // var23, если на строке 23
  ```
- **Несколько уровней макросов** нужны, чтобы заставить препроцессор сначала раскрыть `__LINE__`, а затем выполнить конкатенацию:
  ```c
  #define CONCAT_INDIRECT(a, b) a##b
  #define CONCAT(a, b) CONCAT_INDIRECT(a, b)
  ```

## III. `__FILE__`, `__LINE__` и создание макроса `CHECK`

```c
#define CHECK(expr)                                                    \
    do {                                                               \
        if (!(expr)) {                                                 \
            printf("%s:%d: CHECK failed: %s\n", __FILE__, __LINE__, #expr); \
        }                                                              \
    } while (0)
```

- Использует макросы `#expr` для получения текста выражения, а `__FILE__`, `__LINE__` для указания местоположения.

## IV. Фреймворк `TEST_CASE`

- **Авторегистрация тестов**:
  ```c
  typedef void (*TestFunc)();

  #define TEST_CASE(name)                                             \
      static void UNIQUE_NAME(test_)();                                \
      namespace {                                                     \
          struct UNIQUE_NAME(TestRegistrar_) {                        \
              UNIQUE_NAME(TestRegistrar_)() {                         \
                  register_test(name, UNIQUE_NAME(test_));            \
              }                                                       \
          } UNIQUE_NAME(_registrar);                                  \
      }                                                               \
      static void UNIQUE_NAME(test_)()
  ```

- **Пример использования**:
  ```c
  TEST_CASE("addition") {
      CHECK(1 + 1 == 2);
  }
  ```

## V. Ограничения макросов

- Макросы могут генерировать только фрагменты синтаксиса, которые после раскрытия образуют корректный код.
- Лучшая практика — держать макросы простыми и инкапсулировать сложность в inline-функциях или шаблонах C++.

</details>

<details>
<summary>
80. Parameter pack — основы
</summary>

## I. Function parameter packs

- **Синтаксис** функции с произвольным числом аргументов:
  ```cpp
  template<typename... Args>
  void func(Args... args) {
      // args — pack of parameters
  }
  ```
- Можно вызывать с любым количеством аргументов: `func(1, 2.5, "text");`

## II. Template parameter packs и variadic templates

- **Template parameter pack** объявляется как `typename... Ts`:
  ```cpp
  template<typename T, typename... Rest>
  struct Tuple { /* ... */ };
  ```
- **Специализации** могут содержать несколько паков:
  ```cpp
  template<typename... Ts, typename... Us>
  struct Foo { /* ... */ };
  ```

## III. Non-type parameter packs

- **Значения вместо типов**:
  ```cpp
  template<int... Ns>
  struct IntSeq { /* ... */ };
  IntSeq<1, 2, 3> seq;
  ```

## IV. Pack expansion

- **Раскрытие в разных контекстах**:
  ```cpp
  template<typename... Ts>
  void printAll(Ts... args) {
      (std::cout << ... << args) << std::endl; // fold expression
  }
  ```
- **Параллельная экспансия** нескольких паков:
  ```cpp
  template<typename... Ts, typename... Us>
  void zip(Ts... ts, Us... us) {
      ((process(ts, us)), ...);
  }
  ```

## V. Type deduction with packs

- Компилятор выводит сразу несколько паков из аргументов:
  ```cpp
  template<typename... Ts>
  void g(Ts... args);
  g(1, 'a', 2.5); // Ts = <int, char, double>
  ```
- Можно смешивать обычные и паковые параметры:
  ```cpp
  template<typename T, typename... Ts>
  void h(T first, Ts... rest);
  ```

## VI. Fold expressions (C++17)

- **Бинарный fold**:
  ```cpp
  (expr1 + ... + exprN)
  ```
- **Унарный fold**:
  ```cpp
  (... + exprN)
  (expr1 + ...)
  ```

## VII. Эмуляция цикла по pack

- Приём через **initializer list** и `operator,`:
  ```cpp
  template<typename... Ts>
  void forEach(Ts... args) {
      using expander = int[];
      (void)expander{0, ( (std::cout << args << ' '), 0 )...};
      std::cout << std::endl;
  }
  ```
- Можно использовать **lambda** внутри fold:
  ```cpp
  template<typename... Ts>
  void visitAll(Ts... args) {
      ( [ & ](auto x){ process(x); }(args), ... );
  }
  ```

</details>

<details>
<summary>
81. Parameter pack — детали
</summary>

## I. Pattern matching и специализации для рекурсивной обработки (классы)

```cpp
// Рекурсивный пример через non-type pack
template<int... Ns>
struct IntPrinter;

template<>
struct IntPrinter<> {
    static void print() {}
};

template<int N, int... Rest>
struct IntPrinter<N, Rest...> {
    static void print() {
        std::cout << N << " ";
        IntPrinter<Rest...>::print();
    }
};

// Использование:
IntPrinter<1, 2, 3>::print(); // вывод: 1 2 3
```

## II. Pattern matching + вывод типов + перегрузки (функции)

```cpp
// Функция-обработчик одного аргумента
template<typename T>
void process_one(T&& v) {
    std::cout << v << " ";
}

// Рекурсивная обработка через variadic template
void process_all() {
    std::cout << std::endl;
}

template<typename T, typename... Rest>
void process_all(T&& v, Rest&&... rest) {
    process_one(std::forward<T>(v));
    process_all(std::forward<Rest>(rest)...);
}

// Использование:
process_all(10, 3.14, "text"); // вывод: 10 3.14 text
```

## III. Реализация `std::tuple` и утилит

```cpp
template<typename... Ts>
struct Tuple;

template<>
struct Tuple<> { };

template<typename T, typename... Rest>
struct Tuple<T, Rest...> {
    T head;
    Tuple<Rest...> tail;
    Tuple() = default;
    Tuple(const T& h, const Rest&... r) : head(h), tail(r...) {}
};

// get<Index>
template<size_t I, typename TupleT>
struct TupleElement;

template<typename T, typename... Rest>
struct TupleElement<0, Tuple<T, Rest...>> {
    static T& get(Tuple<T, Rest...>& t) { return t.head; }
};

template<size_t I, typename T, typename... Rest>
struct TupleElement<I, Tuple<T, Rest...>> {
    static auto& get(Tuple<T, Rest...>& t) {
        return TupleElement<I-1, Tuple<Rest...>>::get(t.tail);
    }
};

template<size_t I, typename... Ts>
auto& get(Tuple<Ts...>& t) {
    return TupleElement<I, Tuple<Ts...>>::get(t);
}

// tuple_size
template<typename TupleT>
struct TupleSize;

template<typename... Ts>
struct TupleSize<Tuple<Ts...>> : std::integral_constant<size_t, sizeof...(Ts)> {};
```

## IV. Эмуляция цикла через lambda и `initializer_list`

```cpp
template<typename... Ts>
void for_each(Ts&&... args) {
    (void)std::initializer_list<int>{(
        (std::cout << args << " "), 0
    )...};
    std::cout << std::endl;
}
```

## V. `make_index_sequence` для печати элементов `Tuple`

```cpp
template<typename TupleT, size_t... Is>
void printTupleImpl(TupleT& t, std::index_sequence<Is...>) {
    (void)std::initializer_list<int>{(
        (std::cout << get<Is>(t) << " "), 0
    )...};
    std::cout << std::endl;
}

template<typename... Ts>
void printTuple(Tuple<Ts...>& t) {
    printTupleImpl(t, std::make_index_sequence<sizeof...(Ts)>{});
}
```

</details>

<details>
<summary>
82. Вспомогательные инструменты метапрограммирования
</summary>

## I. `std::apply`

- Вызывает функцию с аргументами из `std::tuple` или других кортеж-подобных:
  ```cpp
  #include <tuple>
  #include <utility>

  auto tup = std::make_tuple(1, 2.5, "str");
  auto result = std::apply(
      [](int a, double b, const char* s) { return std::to_string(a) + s; },
      tup
  );
  ```

## II. `std::tie`

- **Связывание ссылок** на элементы кортежа:
  ```cpp
  int x; double y;
  std::tie(x, y) = std::make_pair(42, 3.14);
  ```
- Используется для распаковки `std::tuple`:
  ```cpp
  auto [a, b, c] = std::make_tuple(1, 2, 3); // structured bindings (C++17)
  // до C++17:
  std::tie(a, b, c) = std::make_tuple(1, 2, 3);
  ```

## III. `noexcept` и производительность

- **Значение спецификатора `noexcept`** влияет на выбор алгоритмов:
  - `std::vector<T>` в `resize`/`reserve` использует move-constructor если он `noexcept`, иначе копирует.
- **Условные `noexcept`**:
  ```cpp
  template<typename T>
  void f() noexcept(noexcept(T(std::declval<T>()))) {
      // noexcept true, если конструктор T не бросает
  }
  ```

## IV. `noexcept(noexcept(expr))`

- Вложенный `noexcept` оценивает, бросает ли сам `noexcept(expr)` исключение, что всегда даёт `false` или `true`.  
- Используется для проверки гарантии выражения:
  ```cpp
  template<typename T>
  auto safeCall(T&& t) noexcept(noexcept(std::forward<T>(t)())) {
      return std::forward<T>(t)();
  }
  ```

## V. `std::declval`

- Переносит тип в unevaluated context для извлечения свойств типа:
  ```cpp
  template<typename T>
  auto test(int) -> decltype(std::declval<T>().method(), std::true_type{});

  template<typename T>
  std::false_type test(...);

  // Использование:
  // test<MyType>(0) -> std::true_type if MyType::method() exists, else std::false_type
  ```
- **Где можно вызывать**: только в unevaluated contexts (например, в `decltype`), т.к. не создаёт объект.
- **Зачем**: для SFINAE и traits, чтобы проверить корректность выражений без потребности в конструкторе.

</details>

<details>
<summary>
83. Perfect forwarding
</summary>

## I. Forwarding references и `std::forward`

- **Forwarding reference** (ранее called universal reference):
  ```cpp
  template<typename T>
  void func(T&& t); // если параметр шаблонный — это forwarding reference
  ```
- **`std::forward`**:
  ```cpp
  template<typename T>
  void wrapper(T&& t) {
      func(std::forward<T>(t)); // сохраняет value category
  }
  ```

## II. Правила вывода и collapse reference

- При подстановке:
  - Если аргумент lvalue → T deduced as `T&`, параметр становится `T& &&` → collapses to `T&`.
  - Если аргумент rvalue → T deduced as `T`, параметр `T&&`.
- **Collapse rules**:
  - `& &` → `&`
  - `& &&` → `&`
  - `&& &` → `&`
  - `&& &&` → `&&`

## III. Когда `T&&` не forwarding reference

- В методах шаблонного класса:
  ```cpp
  template<typename U>
  struct S {
      void method(U&& u); // здесь U&& — не forwarding reference, а просто rvalue reference
  };
  ```
- Для forwarding необходимо, чтобы параметр сам являлся шаблонным параметром функции.

## IV. Perfect forwarding для нескольких аргументов

```cpp
template<typename F, typename... Args>
auto invoke(F&& f, Args&&... args)
    -> decltype(std::forward<F>(f)(std::forward<Args>(args)...))
{
    return std::forward<F>(f)(std::forward<Args>(args)...);
}
```

## V. `decltype` и `decltype(auto)`

- **`decltype(expr)`**:
  - Если `expr` — имя переменной → lvalue → `T&`.
  - Если `expr` — результат функции или rvalue → `T`.

- **`decltype(auto)`** сохраняет категорию выражения:
  ```cpp
  auto x = expr;            // always decayed
  decltype(auto) y = expr;  // preserves reference/value
  ```

- **Нельзя** объявить `decltype(auto) var;` без инициализатора, и нельзя иметь переменную типа `void`.

## VI. `ref`/`cref` vs perfect forwarding

- **`std::ref`/`std::cref`** создают `std::reference_wrapper<T>`:
  ```cpp
  template<typename F, typename Arg>
  void g(F f, Arg a) {
      f(a);
  }
  g(func, std::ref(x)); // передаёт lvalue reference
  ```
- **Perfect forwarding** сохраняет точную категорию аргумента без обёртки.

</details>

<details>
<summary>
84. Вычисления на этапе компиляции — основы
</summary>

## I. `constexpr`-вычисления

- **`constexpr` константы**:
  ```cpp
  constexpr int max_value = 100;
  ```
  - Инициализируются во время компиляции.
  - Могут использоваться в контекстах, требующих константных выражений.

- **Отличия от `const`**:
  - `const` не гарантирует compile-time инициализацию.
  - `constexpr` функции могут вычисляться на этапе компиляции, если аргументы тоже константны.

- **`constexpr` функции и структуры**:
  ```cpp
  constexpr int factorial(int n) {
      return n < 2 ? 1 : (n * factorial(n - 1));
  }
  struct ConstExpr {
      int x;
      constexpr ConstExpr(int v): x(v) {}
  };
  ```

## II. Тип "функция" vs "указатель на функцию"

- **Function type**: `int(double)` — тип функции.
- **Function pointer**: `int (*)(double)` — указатель на функцию.
- Различаются при использовании `decltype`:
  ```cpp
  using FuncType = int(double);
  using FuncPtr  = int(*)(double);
  ```

## III. Метапрограммирование через структуры и специализации

- **`std::integral_constant`**:
  ```cpp
  template<typename T, T v>
  struct integral_constant {
      static constexpr T value = v;
      using value_type = T;
      using type = integral_constant;
      constexpr operator T() const noexcept { return v; }
  };
  using true_type  = integral_constant<bool, true>;
  using false_type = integral_constant<bool, false>;
  ```

- **Конвенции `_t` и `_v`**:
  ```cpp
  template<typename T>
  using remove_const_t = typename std::remove_const<T>::type;

  template<typename T>
  constexpr bool is_void_v = std::is_void<T>::value;
  ```

- **Реализация compile-time функций**:
  ```cpp
  template<typename T>
  struct is_reference : false_type {};

  template<typename T>
  struct is_reference<T&> : true_type {};
  ```

## IV. `iterator_traits<T>`

- **`iterator_traits`** позволяет извлечь свойства итератора:
  ```cpp
  template<typename Iter>
  struct iterator_traits {
      using value_type        = typename Iter::value_type;
      using difference_type   = typename Iter::difference_type;
      using pointer           = typename Iter::pointer;
      using reference         = typename Iter::reference;
      using iterator_category = typename Iter::iterator_category;
  };
  ```
- **Отличия от `T::value_type`**:
  - Специализирован для указателей (`value_type` = `T`).
- **Зачем нужен**:
  - Упрощает написание шаблонов алгоритмов до появления `auto`.

## V. `function_traits`

- **Пример реализации**:
  ```cpp
  template<typename>
  struct function_traits;

  // specialization for function types
  template<typename R, typename... Args>
  struct function_traits<R(Args...)> {
      static constexpr std::size_t arity = sizeof...(Args);
      using result_type = R;
      template<std::size_t I>
      using arg_t = std::tuple_element_t<I, std::tuple<Args...>>;
  };

  // pointer to function
  template<typename R, typename... Args>
  struct function_traits<R(*)(Args...)> : function_traits<R(Args...)> {};
  ```

- **Использование**:
  ```cpp
  using traits = function_traits<void(int, double)>;
  static_assert(traits::arity == 2);
  static_assert(std::is_same_v<traits::arg_t<1>, double>);
  ```

</details>

<details>
<summary>
85. SFINAE — основы
</summary>

## I. Определение SFINAE

- **SFINAE** (Substitution Failure Is Not An Error) — правило языка шаблонов, при котором ошибки замены параметров шаблона приводят к удалению кандидата из множества перегрузок, а не к ошибке компиляции.
- Работает для шаблонных функций и частичных специализаций.

## II. Hard compile error vs SFINAE

- **Hard error**: синтаксическая или семантическая ошибка, не связанная с подстановкой шаблона.
- **SFINAE**: ошибки при подстановке типов в шаблон (в неинициализируемых контекстах) подавляются:
  ```cpp
  template<typename T>
  auto func(T t) -> decltype(t.size()); // viable only if T::size exists
  ```

## III. SFINAE по возвращаемому типу

1. **Без запятой**:
   ```cpp
   template<typename T>
   auto f(int) -> decltype(std::declval<T>().foo());
   ```
2. **С оператором `,`**:
   ```cpp
   template<typename T>
   auto f(int) -> decltype(std::declval<T>().foo(), void());
   ```
3. **С возвратом `void`**:
   - Используется, чтобы возвращаемый тип был `void`, если выражение корректно.

## IV. `enable_if`

- **Прототип**:
  ```cpp
  template<bool B, typename T = void>
  struct enable_if { };
  template<typename T>
  struct enable_if<true, T> { using type = T; };
  ```
- **Использование в возвращаемом типе**:
  ```cpp
  template<typename T>
  typename std::enable_if_t<std::is_integral_v<T>, T>
  func(T t) { return t; }
  ```
- Позволяет исключать функции из выбора перегрузок.

## V. SFINAE в фиктивных параметрах шаблона

1. **В значении по умолчанию**:
   ```cpp
   template<typename T, typename = std::enable_if_t<Condition<T>::value>>
   void g(T);
   ```
2. **Проблемы с переопределением**: перегрузки могут быть удалены из overload set.
3. **В типе параметра**:
   ```cpp
   template<typename T>
   void h(typename std::enable_if_t<Condition<T>::value, T>* = nullptr);
   ```

## VI. `void_t` для проверки выражений

- **`void_t`**:
  ```cpp
  template<typename...> using void_t = void;
  ```
- **Пример проверки**:
  ```cpp
  template<typename, typename = void>
  struct HasFoo : std::false_type {};

  template<typename T>
  struct HasFoo<T, void_t<decltype(std::declval<T>().foo())>> : std::true_type {};
  ```

</details>
