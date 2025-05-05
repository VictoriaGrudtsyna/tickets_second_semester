## Билет 40. Базовая многопоточность
Поток, он же Thread, он же нить — процесс, который выполняет код параллельно с другими потоками.
### Создание потоков в C++11, передача аргументов в функцию потока по значению и ссылкам
#### Создание
```c++
#include <chrono>
#include <iostream>
#include <thread>

int main() {
    int data = 1234;
    std::thread t([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Hello from thread! data=" << data << std::endl;
        data += 10;
    });
    std::cout << "Waiting for it..." << std::endl;
    t.join();  // Освобождает ресурсы потока.
    std::cout << "data is " << data << std::endl;
    // Вызывается ~thread(), к этому моменту обязательно сделать join().
    return 0;
}
```
Тут происходит следующее:  
1) Создали и сразу запустили поток t.
2) Вывели "Waiting..."
3) Параллельно с пунктом 2 поток поспал 500 миллисекунд и вывел data=1234
4) Сделали `t.join()`, чтобы освободить ресурсы. Если не сделали, то ничего хорошего, считай утечка.
При этом `t.join()` дожидается пока поток доделает все свои дела.
5) Когда поток завершился, продолжаем `main`.
#### Передача аргументов
В предыдущем примере используем лямбду, но хочется уметь использовать функции, да ещё и с аргументами.
```c++
void worker(int a, int &b) {
    std::cout << "Thread: " << a << " " << &b << std::endl;
}

int main() {
    int a = 10, b = 20;
    std::thread(worker, a, std::ref(b)).join();
    return 0;
}
```
Обратите внимание:
* `std::ref` позволяет передать аргумент по ссылке
* `std::thread(worker(a, b))` всегда распарсится как "вызвать `worker` от `a, b` и передать результат в конструктор `std::thread`".
В других языках бывает иначе, но мы живём в плюсах.
  
### Joinable/detached потоки
#### [Joinable](https://en.cppreference.com/w/cpp/thread/thread/joinable)
Говорит, активен ли поток.
При этом поток, который доделал все свои дела, но у которого не вызвали `.join()` всё ещё считается joinable.
```c++
#include <iostream>
#include <thread>
#include <chrono>
 
void foo()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
 
int main()
{
    std::thread t;
    std::cout << "before starting, joinable: " << std::boolalpha << t.joinable()
              <<  std::endl;
 
    t = std::thread(foo);
    std::cout << "after starting, joinable: " << t.joinable() 
              <<  std::endl;
 
    t.join();
    std::cout << "after joining, joinable: " << t.joinable() 
              <<  std::endl;
}
```
Выведет:
```
before starting, joinable: false
after starting, joinable: true
after joining, joinable: false
```
#### Detach
Можем сказать программе, что ничего от потока больше не хотим и пусть он живёт своей собственной жизнью отдельно от нас.  
Для этого можно сделать `t.detach()`.
```c++
#include <chrono>
#include <iostream>
#include <thread>

int main() {
    int data = 1234;
    std::thread t([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Hello from thread! data=" << data << std::endl;
        data += 10;
    });
    std::cout << "Detaching..." << std::endl;
    t.detach();  // Обещаем больше не делать join(), теперь ОС отвечает за сборку мусора в потоке.
                 // Возникает только если нам вообще ничего от потока `t` никогда не будет нужно.
                 // Единственный разумный пример: мы создаём сетевой сервер, который в бесконечном цикле плодит потоки для клиентов.
    std::cout << "data is " << data << std::endl;
    // После завершения main() завершается программа, но поток всё ещё работает — UB (обращения к умершим глобальным переменным).
    // Если используем pthread, то можем завершить main(), не завершая программу.
    return 0;
}
```
У Егора хорошие комментарии, самое важное из них:
```
После завершения main() завершается программа, но поток t всё ещё работает 
— UB (обращения к умершим глобальным переменным).
```
### Гонки
`My least favorite race is data-race.`  
#### При выводе
В коде ниже хотим выводить то строчку из main, то из second thread'а.
```c++
#include <iostream>
#include <thread>

void writeln(const char *s) {
    for (int i = 0; s[i]; i++) {
        std::cout << s[i];
    }
    std::cout <<  std::endl;
}

int main() {
    std::thread t([]() {
        for (;;) {
            writeln("Hello from the second thread");
        }
    });
    for (;;) {
        writeln("Hello from the main thread");
    }
}
```
Коза лось бык, что может пойти не так.  
А то, что местами будем видеть забавные вещи типа:
```
Hello from the main thread
Hellfrom the second thread
Ho from the main thread
ello from the second threHello from the main thad
```
Многопоточность устроена так, что мы "прыгаем" от одного потока к другому, выполняя команды то там, то там.
А так как мы в `cout` выводим по одному символу, то не мудрено, что они перемешаются.
#### По данным
В коде ниже плохо всё:
```c++
#include <iostream>
#include <thread>

#pragma GCC optimize ("O0")

const int N = 500'000'000;
const int M = 10'000;

int main() {
    int data = 0;
    auto worker = [&data]() {
        for (int i = 0; i < N; i++) {
            data++;
        }
    };
    std::thread t1(worker);
    std::thread t2(worker);
    for (int i = 0; i < M; i++) {
        if (data % 2 == 0) {
            std::cout << "data is " << data << " (in progress)" << std::endl;
        }
    }
    t2.join();
    t1.join();
    std::cout << "data is " << data << std::endl;
    return 0;
}
```
А именно:
1) Внутри 
    ```c++
    if (data % 2 == 0) {
        std::cout << "data is " << data << " (in progress)" << std::endl;
    }
    ```
    мы проверили, что `data` чётно, но пока дошли до следующей строчки оно уже могло поменяться каким-то другим потоком.
2) У нас два потока пытаются менять `data`, но мы знаем, что на самом деле `data++` состоит из трёх операций и тут нам может как повезти, так и не повезти.
![](../images/ticket41/lucky.jpeg)
![](../images/ticket41/unlucky.jpeg)
И вот если нам не повезло, то мы пропустили один из двух `data++`. Досадно.
   
Пока что оставим, но в конце есть фикс этой радости.
#### Одновременное чтение без записей
Запись + чтение переменной из разных мест = UB.  
Чтение переменной из разных мест, если не меняем её = OK.
### Борьба с гонками
#### Mutex
Для того чтобы бороться с ~~клаво~~дата-гонками придумали mutex.  
Его можно `.lock()`, можно `.unlock()`, причём когда он заблокирован, никто не может его взять.
То есть, если в одном потоке мы сделали `mutex.lock()` и пытаемся сделать то же самое в другом потоке, нам придётся ждать, пока первый поток не отпустит его (не сделает `mutex.unlock()`).  
Пофиксили код с гонкой при выводе:
```c++
std::mutex m;
void writeln(const char *s) {
    m.lock();
    for (int i = 0; s[i]; i++) {
        std::cout << s[i];
    }
    std::cout <<  std::endl;
    m.unlock();
}
```
Обратите внимание:
* `mutex` должен быть глобальным, чтобы его видели разные потоки. Если мы будет создавать `mutex` внутри функции, мы конечно ничего не сломаем, но ничего и не починим.
* неосторожное использование `mutex` может привести к deadlock'у — ситуации, когда "мы ничего не делаем и чего-то ждём".
Подробнее тема будет раскрыта в билете 43.
#### unique_lock
Мьютекс уже решает нашу проблему, но вот незадача — если между `m.lock()` и `m.unlock()` мы по какой либо причине выходим из функции, то наш мьютекс остаётся заблокированным навсегда.  
На помощь приходит `std::unique_lock`, который в конструкторе делает `m.lock()`, а в деструкторе `m.unlock()`. 
Теперь, если мы выйдем из скоупа, где создали `unique_lock`, он вызовет деструктор и освободит мьютекс.
```c++
std::mutex m;
void writeln(const char *s) {
    std::unique_lock l{m};
    for (int i = 0; s[i]; i++) {
        std::cout << s[i];
    }
    std::cout <<  std::endl;
}
```
Обратите внимание:
* `std::unique_lock{m}` — временный объект, который появится и тут же помрёт, ничего полезного не сделав.
Мы хотим именно создать переменную `std::unique_lock l{m}` (разница в 2 символа).
  
#### Атомарность
Атомарная операция — такая операция, что не может случиться ситуации, когда мы "вклинились" в середину её выполнения (выше был пример про "не повезло").  
Считаем, что ничего не атомарно, если нам явно не сказали обратное.  
Например, утверждается, что записать символ в `std::cout` — атомарная операция и мы безопасно можем это сделать.  
Ещё пример:
```c++
std::mutex m;

void foo(int x) {  // Атомарная операция, atomic.
    std::unique_lock l(m);
    std::cout << "foo(" << x << ")" << std::endl;
}
```
Есть даже типы как `std::atomic<int>` у которых, к примеру, оператор `++` атомарен.  
Пример:
```c++
std::atomic<int> data = 0;
auto worker = [&data]() {
  for (int i = 0; i < N; i++) {
      data++; // Атомарно
      // data = data + 1; // Не атомарно 
  }
};
 ```
Замечаем, что `data = data + 1` не атомарен.  
Мнение — atomic лучше не использовать, потому что с большей вероятностью отстрелите себе пятку, чем сделаете что-то полезное.
Хотя у современных процессоров есть поддержка атомарности, поэтому если очень хочется и вы *крайне* аккуратны, то можно.
#### Обещанный фикс data-race
```c++
#include <iostream>
#include <mutex>
#include <thread>

#pragma GCC optimize ("O0")

const int N = 5000000;
const int M = 10000;

// Дополнительное чтение: у clang есть статический анализ: https://clang.llvm.org/docs/ThreadSafetyAnalysis.html
// Можно пометить int data GUARDED_BY(m); и он много чего проверит.

int main() {
    std::mutex m;
    int data = 0;
    auto worker = [&data, &m]() {
        for (int i = 0; i < N; i++) {
            std::unique_lock l{m};
            data++;
        }
    };
    std::thread t1(worker);
    std::thread t2(worker);
    for (int i = 0; i < M; i++) {
        std::unique_lock l(m);
        int data_snapshot = data;
        l.unlock();  // Не m.unlock()! Иначе unique_lock сделает unlock() ещё раз, это UB.

        if (data_snapshot % 2 == 0) {
            std::cout << "data is " << data_snapshot << " (in progress)" << std::endl;
        }
    }
    t2.join();
    t1.join();
    std::cout << "data is " << data << std::endl;
    return 0;
}
```
Фиксы:
1) Научились делать snapshot — прочитали текущее состояние `data`, сохранили в переменную и спокойно используем не боясь, что оно по ходу может измениться.
2) Запихали мьютекс в потоки `worker`, чтобы не потерять ни одного `data++`.
3) Обернули snapshot в мьютекс, потому что вообще-то читать и писать одновременно — это UB.
### Частичная потокобезопасность `cout`
`cout` частично потокобезопасен.  
То есть, ничего не сломается, если мы будем в нескольких потоках что-то в него писать, но при этом никто не гарантирует, что буковки не перемешаются (на это был пример выше).  
При этом `cout << a << b` может быть порезан и между `a` и `b` может что-то вклиниться.
