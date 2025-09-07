## Билет 43. Дизайн многопоточных приложений
### Deadlock, reentrant-функции, `recursive_mutex`, отделение приватного API без блокировок от публичного API с блокировками
#### Deadlock
`Ситуация, когда программа ничего не делает, но чего-то ждёт.`
```c++
#include <iostream>
#include <mutex>
#include <thread>

std::mutex m;

void foo(int x) {  // Атомарная операция, atomic.
    std::unique_lock l(m);
    std::cout << "foo(" << x << ")" << std::endl;
}

void double_foo(int x) {  // Неатомарная операция :(
    foo(x);
    foo(x + 1);
}

int main() {
    const int N = 100'000;
    std::thread t([]() {
        for (int i = 0; i < N; i += 10)
            double_foo(i);
    });
    for (int i = 0; i < N; i += 10)
        double_foo(N + i);
    t.join();
}
```
В коде выше ещё нет дедлока, но есть проблема — `foo()` атомарно, а вот `double_foo()` — нет, потому что между вызовами `foo(x)` и `foo(x+1)` может что-нибудь вклиниться.  
Наивно изменим `double_foo()`:
```c++
void double_foo(int x) {  
    std::unique_lock l(m);  // Берём мьютекс первый раз.
    foo(x);  // Берём мьютекс второй раз, deadlock :(
    foo(x + 1);
}
```
Теперь `double_foo()` действительно атомарен, но вот незадача, программа перестала работать.
Это произошло, потому что мы захватили мьютекс в `double_foo()`, затем пошли в `foo(x)` и пытаемся захватить мьютекс там, но он занят и никогда не освободится — дедлок.  
Пытаемся чинить:  
Можно использовать `recursive_mutex` — такой мьютекс [позволяет](https://cplusplus.com/reference/mutex/recursive_mutex/) делать `.lock()`, если мьютекс уже заблокированн **тем же** потоком, в котором мы сейчас, то есть как раз наша ситуация.
Но обычно это считается плохим стилем, так что идём другим путём.
```c++
#include <iostream>
#include <mutex>
#include <thread>

std::mutex m;
std::mutex m2;

void foo(int x) {  // Атомарная операция, atomic.
    std::unique_lock l(m);
    std::cout << "foo(" << x << ")" << std::endl;
}

void double_foo(int x) { 
    std::unique_lock l(m2);  // Берём другой мьютекс, deadlock отсутствует.
    foo(x);
    foo(x + 1);
}

int main() {
    const int N = 100'000;
    std::thread t([]() {
        for (int i = 0; i < N; i += 10)
            double_foo(i);
    });
    for (int i = 0; i < N; i += 10)
        foo(N + i);
    t.join();
}
```
Теперь `double_foo()` между собой не пересекаются и `foo()` не пересекаются, но `foo()` может влезть в `double_foo()` (это делает цикл в `main`).  
#### Отделение приватного API без блокировок от публичного API с блокировками
```c++
#include <iostream>
#include <mutex>
#include <thread>

std::mutex m;

// Приватный интерфейс.
void foo_lock_held(int x) {
    std::cout << "foo(" << x << ")" << std::endl;
}

// Публичный интерфейс из атомарных операций.
// Над ним надо очень хорошо думать, потому что комбинация двух атомарных операций неатомарна.
void foo(int x) {
    std::unique_lock l(m);
    foo_lock_held(x);
}

void double_foo(int x) {
    std::unique_lock l(m);
    foo_lock_held(x);
    foo_lock_held(x + 1);
}

int main() {
    const int N = 100'000;
    std::thread t([]() {
        for (int i = 0; i < N; i += 10)
            double_foo(i);
    });
    for (int i = 0; i < N; i += 10)
        foo(N + i);
    t.join();
}
```
Теперь можем быть уверены, что `foo()` и `double_foo()` не перемешаются (у них один мьютекс) и при этом не словим дедлок.  
Мораль в том, что нужно сначала очень хорошо подумать и только потом писать.
### Взаимные блокировки и их избегание при помощи контроля порядка взятия блокировок или `scoped_lock`/`unique_lock`
```c++
std::mutex m1, m2;
std::thread t1([&]() {
    for (int i = 0; i < N; i++) {
        std::unique_lock a(m1);
        std::unique_lock b(m2);
    }
});
std::thread t2([&]() {
    for (int i = 0; i < N; i++) {
        std::unique_lock b(m2);
        std::unique_lock a(m1);
    }
});
```
При `N=10'000` уже дедлок, потому что `t1` схватил `a` и ждёт `b`, `t2` схватил `b` и ждёт `a`.
И стоят, как два барана на мосту.  
Решения два:
1) Заводим порядок, в котором берём мьютексы и **везде** придерживаемся его, тогда описанной проблемы не возникнет.
2)  ```c++
    std::mutex m1, m2;
    std::thread t1([&]() {
        for (int i = 0; i < N; i++) {
            std::scoped_lock a(m1, m2);
        }
    });
    std::thread t2([&]() {
        for (int i = 0; i < N; i++) {
            std::scoped_lock b(m2, m1);
        }
    });
    ```
    Про `scoped_lock` известно, что он "довольно тупой, работает долго, но в нужном порядке лочит мьютексы".
    То есть нашу проблему он тоже решает.
    
Про `unique_lock` написано в билете 41.
### Ключевое слово `thread_local` в сравнении с глобальными переменными
`thread_local` переменная имеет `thread storage duration`.
Такая переменная создаётся отдельно для каждого потока.
Аллокация и деаллокация [происходят]((https://en.cppreference.com/w/cpp/language/storage_duration)) при входе и выходе из потока.  
```c++
#include <chrono>
#include <iostream>
#include <thread>

struct Foo {
    Foo(int id) {
        std::cout << "Foo(" << id << ")@" << std::this_thread::get_id() << std::endl;
    }
    ~Foo() {
        std::cout << "~Foo()@" << std::this_thread::get_id() <<  std::endl;
    }
};

thread_local Foo foo(10);  // new kind of storage duration.

int main() {
    std::cout << "T1 &foo=" << &foo <<  std::endl;  // Компилятор может проинициализировать лениво, но может и в момент начала потока.
    std::thread t([&]() {
        thread_local Foo bar(20);  // Инициализирует при проходе через эту строчку.
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        std::cout << "T2 &foo=" << &foo <<  std::endl;  // Компилятор может проинициализировать лениво, но может и в момент начала потока.
        std::cout << "T2 &bar=" << &bar <<  std::endl;  // Уже точно проинициализировано выше.
    });
    std::cout << "Waiting for it..." << std::endl;
    t.join();
    std::cout << "Joined" << std::endl;
    return 0;
}
```
```
T1 &foo=Foo(10)@0x1022cfd40
0x148e068e0
Waiting for it...
Foo(20)@0x16de2f000
T2 &foo=Foo(10)@0x16de2f000
0x148e06960
T2 &bar=0x148e06961
~Foo()@0x16de2f000
~Foo()@0x16de2f000
Joined
```
Видим, что адреса `foo` у основного и второго потоков разные, у каждого своя версия.
```c++
Зачем можно использовать: отслеживать стэк вызовов в каждом потоке.
TEST_CASE() {  // Запомнили в thread-local переменную текущий test case
    SUBCASE() {  // Запомнили текущий SUBCASE
       CHECK(....)  // Можем выводить красивое сообщение
    }
}
```
### Частичная потокобезопасность `shared_ptr`
`std::shared_ptr<int> p` состоит из 3 частей:
1) Сам `p` не является потокобезопасным.
2) Счётчик ссылок внутри `p` потокобезопасен.
3) `*p` не является потокобезопасным.

В 1 и 3 пунктах можно использовать мьютексы, если хотим что-то атомарно с ними делать.
```c++
#include <iostream>
#include <memory>
#include <thread>

int main() {
    std::shared_ptr<int> p = std::make_shared<int>(10);

    std::thread t1([p]() {
        for (int i = 0; i < 100'000; i++) {
            auto p2 = p;  // Thread-safe, even though it increases reference counter.
            ++*p;  // Non thread-safe.
        }
    });

    std::thread t2([p]() {
        for (int i = 0; i < 100'000; i++) {
            auto p2 = p; //так как здесь приравниваем p-шки, то их счетчики указателей тоже приравниваются
            ++*p;
        } //здесь p2 умирает, так как выходит из области видимости, поэтому счетчик ссылок у p уменьшается
    });

    t1.join();
    t2.join();

    std::cout << *p << std::endl;  // race-y.
    std::cout << p.use_count() << std::endl;  // non-race-y, always 1 here.
    p = nullptr;  // No leaks.
}
```

Если что дружеское напоминание про shared_ptr для самых маленьких: объект — это квартира, shared_ptr — это ключ от квартиры, use_count() — количество ключей, квартиру сносят, когда возвращают последний ключ
Итого, изменить `p` или значение, на которое он указывает без мьютексов — небезопасно, а вот создать `auto p1 = p` — на здоровье, хоть он и изменит `p.use_count()`.
### Проблема TOCTOU (Time-Of-Check To Time-Of-Use)
```c++
#include <iostream>
#include <thread>

int balance = 100; // Общий баланс

void withdraw(int amount) {
    if (balance >= amount) {          // Time of Check (TOC)
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Окно уязвимости - баланк за это время мог измениться в другом потоке!! можно чинить мьютексами
        balance -= amount;             // Time of Use (TOU)
        std::cout << "Снято " << amount << ". Баланс: " << balance << "\n";
    }
}

int main() {
    std::thread t1(withdraw, 70);
    std::thread t2(withdraw, 70);
    
    t1.join(); t2.join();
    std::cout << "Итог: " << balance; // Может быть -40!
}
```
