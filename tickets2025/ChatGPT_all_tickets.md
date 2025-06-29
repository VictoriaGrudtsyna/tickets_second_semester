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
