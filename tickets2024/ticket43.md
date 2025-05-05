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

В 1 и 2 пункте можно использовать мьютексы, если хотим что-то атомарно с ними делать.
```c++
#include <iostream>
#include <memory>
#include <thread>

int main() {
    std::shared_ptr<int> p = std::make_unique<int>(10);

    std::thread t1([p]() {
        for (int i = 0; i < 100'000; i++) {
            auto p2 = p;  // Thread-safe, even though it increases reference counter.
            ++*p;  // Non thread-safe.
        }
    });

    std::thread t2([p]() {
        for (int i = 0; i < 100'000; i++) {
            auto p2 = p;
            ++*p;
        }
    });

    t1.join();
    t2.join();

    std::cout << *p << std::endl;  // race-y.
    std::cout << p.use_count() << std::endl;  // non-race-y, always 1 here.
    p = nullptr;  // No leaks.
}
```
Итого, изменить `p` или значение, на которое он указывает без мьютексов — небезопасно, а вот создать `auto p1 = p` — на здоровье, хоть он и изменит `p.use_count()`.
### Проблема TOCTOU (Time-Of-Check To Time-Of-Use)
Посмотрите на структуру ниже:
```c++
#include <mutex>

struct list_node {
private:
    mutable std::mutex m;
    
    int data;
    list_node *next = nullptr;

public:
    list_node(int new_data, list_node *new_next) : data(new_data), next(new_next) {}
    
    void set_next(list_node *new_next) {
        std::unique_lock l(m);
        next = new_next;
    }
    
    list_node *get_next() {
        // https://twitter.com/Nekrolm/status/1487060735305957379
        // TOC-TOU is imminent!
        std::unique_lock l(m);
        return next;
    }
};
```
В ней всё хорошо.
До момента, пока мы не попробуем начать ей пользоваться:
```c++
void append_after(list_node *x, int data) {
    x->set_next(new list_node(data, x->get_next()));
}
```
Неатомарненько вышло — между тем, как получили `get_next()` и вызвали от него `set_next()`, наш `next` вполне мог измениться из другого потока и мы получим проблему схожую с той, когда два потока пытались делать `data++` и проигрывали.  
Аналогичная проблема:
```c++
#include <cstddef>
#include <mutex>
#include <vector>
#include <utility>

template<typename T>
struct atomic_vector {
private:
    mutable std::mutex m;
    std::vector<T> v;

public:
    void push_back(const T &x) {
        std::unique_lock l(m);
        v.push_back(x);
    }

    void push_back(T &&x) {
        std::unique_lock l(m);
        v.push_back(std::move(x));
    }

    const T &operator[](std::size_t i) const {
        std::unique_lock l(m);
        return v[i];
    }
};

void print(atomic_vector v){
    std::cout << v[0];
}
```
`print()` неатомарен — между получением `v[0]` и его передачей в `cout` он мог поменяться.  
Общее решение — делать такие функции методами класса с продуманной реализацией (и мьютексами).
