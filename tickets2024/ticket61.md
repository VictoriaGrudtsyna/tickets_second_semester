# 61. Виртуальное наследование
* Виртуальный базовый класс: синтаксис, пример, проблема ромба
* Порядок инициализации/уничтожения подобъектов и полей, как и где передать параметры конструктором
* Возможное представление в памяти (только для неполиморфных классов)
    * Необходимость хранения указателя в каждой виртуальной базе
* Что происходит, если один класс объявляется и виртуальным, и невиртуальным
* Взаимодействие с `dynamic_cast` при наличии виртуальных методов (работает) и их отсутствии (невозможно)
* Delegate-to-sister
* Как теперь работают `dynamic_cast`/`static_cast`/неявное преобразование/`override`/hiding/приватное и защищённое наследование
    * Невозможность `static_cast` к наследнику от виртуальной базы
    * В том числе при наличии одинаковых виртуальных функций в независимых базах

---

## Виртуальный базовый класс: синтаксис, пример, проблема ромба

Может возникнуть вот такая _diamond problem_ (несколько одинаковых базовых классов).

```c++
/*
Class diagram:

 Person   Person
    ^        ^
    |        |
 Employee Student
    ^        ^
     \      /
      \    /
   MagicStudent

But we want a single Person!

      Person
       ^  ^
      /    \
     /      \
 Employee Student
    ^        ^
     \      /
      \    /
   MagicStudent
*/
```

Тогда получается, что в таком случае у `MagicStudent` нет однозначного имени.

```c++
struct Person {
    std::string name;
    Person(std::string name_) : name(std::move(name_)) {}
};
struct Employee : Person {
    std::string employer;
    Employee(std::string name_, std::string employer_) : Person(std::move(name_)), employer(std::move(employer_)) {}
};
struct Student : Person {
    std::string group;
    Student(std::string name_, std::string group_) : Person(std::move(name_)), group(std::move(group_)) {}
};
struct MagicStudent : /*Person, */ Employee, Student {
    MagicStudent(std::string name_, std::string employer_, std::string group_)
        : Employee(name_, std::move(employer_))
        , Student(name_, std::move(group_)) {} 
};

void hi_employee(Employee &e) {
    std::cout << "Hi, " << e.name << " employed by " << e.employer << "!\n";
}

void hi_student(Student &s) {
    std::cout << "Hi, " << s.name << " from group " << s.group << "!\n";
}

void hi_magic(MagicStudent &ms) {
    std::cout << "Wow, " << ms.name << " does not exist!\n"; 
}
```

**Решение:** давайте «склеим» базовые классы в один и получим виртуальное наследование. То есть мы заранее избавимся
от проблемы наличия `Person` в двух экземплярах.

```c++
/*
Class diagram:

      Person
       ^  ^
    v./    \v.
     /      \
 Employee Student
    ^        ^
     \      /
      \    /
   MagicStudent
*/
```

## Порядок инициализации/уничтожения подобъектов и полей, как и где передать параметры конструктором

При этом конструктор `Person()` должен вызвать самый вложенный класс. В менее вложенных классах этот
конструктор пропустится, но все равно параметры передать нужно (остается верить, что компилятор это оптимизирует).

Синтаксис будет следующим:

```c++
struct Person {
    std::string name;
    Person(std::string name_) : name(std::move(name_)) {
        std::cout << "Person(" << name << ")\n";
    }
};
struct Employee : virtual /* !!! */ Person {
    std::string employer;
    Employee(std::string name_, std::string employer_) : Person(std::move(name_)), employer(std::move(employer_)) {}
};
struct Student : virtual /* !!! */ Person {
    std::string group;
    Student(std::string name_, std::string group_) : Person(std::move(name_)), group(std::move(group_)) {}
};
struct MagicStudent : Employee, Student {
    MagicStudent(std::string name_, std::string employer_, std::string group_)
        : Person(std::move(name_))
        , Employee("", std::move(employer_))
        , Student("", std::move(group_)) {}  
};

void hi_employee(Employee &e) {
    std::cout << "Hi, " << e.name << " employed by " << e.employer << "!\n";
}

void hi_student(Student &s) {
    std::cout << "Hi, " << s.name << " from group " << s.group << "!\n";
}

void hi_magic(MagicStudent &ms) {
    std::cout << "Wow, " << ms.name << " does not exist!\n";
}
```

## Возможное представление в памяти (только для неполиморфных классов)

Как это устроено внутри (как нам кажется):

```c++
/*
Class diagram:

      Person
       ^  ^
    v./    \v.
     /      \
 Employee Student
    ^        ^
     \      /
      \    /
   MagicStudent

Proposed layout (careful: in reality Person is in the end):
+-----------------------------------------------------------------------------+
| +--------+  +------------------------------+  +---------------------------+ |
| | Person |  | +---------------+            |  | +---------------+         | |
| | +name  |  | | PersonVirtual |            |  | | PersonVirtual |         | |
| +--------+  | | +person       |            |  | | +person       |         | |
|             | +---------------+            |  | +---------------+         | |
|             | EmployeeWithVirtual          |  | StudentWithVirtual        | |
|             |                    +employer |  |                    +group | |
|             +------------------------------+  +---------------------------+ |
| MagicStudent                                                                |
+-----------------------------------------------------------------------------+
or:
+-------------------------------------------+
| +--------+  +---------------------------+ |
| | Person |  | +---------------+         | |
| | +name  |  | | PersonVirtual |         | |
| +--------+  | | +person       |         | |
|             | +---------------+         | |
|             | StudentWithVirtual        | |
|             |                    +group | |
|             +---------------------------+ |
| Student                                   |
+-------------------------------------------+
*/
```

А в реальности:

```c++
/*
Proposed layout (careful: in reality Person is in the end):
+-----------------------------------------------------------------------------+
| +--------+  +------------------------------+  +---------------------------+ |
| | Person |  | +---------------+            |  | +---------------+         | |
| | +name  |  | | PersonVirtual |            |  | | PersonVirtual |         | |
| +--------+  | | +person       |            |  | | +person       |         | |
|             | +---------------+            |  | +---------------+         | |
|             | EmployeeWithVirtual          |  | StudentWithVirtual        | |
|             |                    +employer |  |                    +group | |
|             +------------------------------+  +---------------------------+ |
| MagicStudent                                                                |
+-----------------------------------------------------------------------------+
or:
+-------------------------------------------+
| +--------+  +---------------------------+ |
| | Person |  | +---------------+         | |
| | +name  |  | | PersonVirtual |         | |
| +--------+  | | +person       |         | |
|             | +---------------+         | |
|             | StudentWithVirtual        | |
|             |                    +group | |
|             +---------------------------+ |
| Student                                   |
+-------------------------------------------+
*/
```

### Необходимость хранения указателя в каждой виртуальной базе

## Что происходит, если один класс объявляется и виртуальным, и невиртуальным

Виртуальное и не виртуальное наследование. Вопрос: какого черта происходит?

```c++
struct Base { int data = 0; };
struct X : virtual Base {};
struct Y : Base {};
struct Z : virtual Base {};
struct Derived : X, Y, Z {};  // warning: multiple 'Base'

/*
    Base 
  v.|  |v.
  -/   \
 /      -----\
X             Z
|     Base   /
|      |    /
|      Y   /
 \--\  |  /
    Derived
*/
```

Не можем напрямую обращаться к `data` и скастоваться к базовому классу (их два). Но есть два независимых поля `X::data`
и `Y::data`.

```c++
Derived d;
// d.data = 5;  // ambiguous
// Base &b = d;  // ambiguous
d.X::data = 10;
d.Y::data = 20;
// d.Base::data = 123;  // ambiguous

std::cout << d.X::data << "\n";
std::cout << d.Y::data << "\n";
std::cout << d.Z::data << "\n";

[[maybe_unused]] Base &bxz = static_cast<X&>(d);
[[maybe_unused]] Base &by = static_cast<Y&>(d);
```

**Итого:**

1. Для каждого класса хранится максимум одна виртуальная версия и произвольное количество невиртуальных.
2. Сначала инициализируются все виртуальные (от базовых к наследникам), потом все остальное.
3. Порядок деинитиализации – обратный.

## Взаимодействие с `dynamic_cast` при наличии виртуальных методов (работает) и их отсутствии (невозможно)

## Delegate-to-sister

В одном классе переписали только `foo`, а в другом – `bar`. Тогда в `Derived` можно вызвать обе функции через обоих
наследников, вау.

```c++
struct Base {
    virtual void foo() = 0;
    virtual void bar() = 0;
};

struct X : virtual Base {
    void foo() override {
        std::cout << "X::foo()\n";
        bar(); 
    }
};

struct Y : virtual Base {
    void bar() override {
        std::cout << "Y::bar()\n";
    }
};

struct Derived : X, Y {
};

int main() {
    Derived a;
    a.foo();  // X::foo() --> Base::bar() ~~ Y::bar()
}
```

А вот так (в одном переписать только `foo`, а в другом – обе):

```c++
struct Base {
    virtual void foo() = 0;
    virtual void bar() = 0;
};

struct X : virtual Base {
    void foo() override {
        std::cout << "X::foo()\n";
    }
};

struct Y : virtual Base {
    void foo() override {
        std::cout << "Y::foo()\n";
    }
    void bar() override {
        std::cout << "Y::bar()\n";
    }
};

struct Derived : X, Y {  // no unique final overrider for 'virtual void Base::foo()'
};
```

Можно переписать в наследнике:

```c++
struct Derived : X, Y {  // ok: Derived::foo() calls X::foo() + Y::foo(), Y::bar()
    void foo() override {
        std::cout << "Derived::foo()\n";
        X::foo();  // non virtual call
        Y::foo();  // non virtual call
    }
};

int main() {
    Derived d;
    d.foo();
    std::cout << "=====\n";
    d.bar();
    std::cout << "=====\n";
    static_cast<Base&>(d).foo();  // Derived::foo()
    std::cout << "=====\n";
    d.X::foo();  // non virtual call
}
```

А можно переписать в базовых классах (убрать из `Y` реализацию `foo` и написать ее в `Base`):

```c++
struct Base {
    // Base() { foo(); }
    virtual void foo() { std::cout << "Base::foo()\n"; }
    virtual void bar() = 0;
};

struct X : virtual Base {
    void foo() override {
        std::cout << "X::foo()\n";
    }
};

struct Y : virtual Base {
//    void foo() override {
//        std::cout << "Y::foo()\n";
//     }
    void bar() override {
        std::cout << "Y::bar()\n";
    }
};

struct Derived : X, Y {  // ok: X::foo(), Y::bar()
};
```

## Как теперь работают `dynamic_cast`/`static_cast`/неявное преобразование/`override`/hiding/приватное и защищённое наследование

### Невозможность `static_cast` к наследнику от виртуальной базы

### В том числе при наличии одинаковых виртуальных функций в независимых базах