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
