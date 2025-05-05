## Билет 42. Оповещения о событиях
## Producer-consumer и cond_var
Это некоторая модель, где есть *producer* - который отдаёт данные, и *consumer* - который их должен в онлайн режиме читать и обрабатывать. 
Есть несколько реализаций (код Егора):
1. **Promise and future:**
 ```C++
#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>

int main() {
    std::promise<std::string> input_promise;
    std::future<std::string> input_future = input_promise.get_future();

    std::thread producer([&]() {
        std::string input;
        std::cin >> input;
        input_promise.set_value(std::move(input));
    });

    std::thread consumer([&]() {
        std::cout << "Consumer started\n";
        std::string input = input_future.get();
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        std::cout << "Got string: " << input << "\n";
    });

    consumer.join();
    producer.join();
}
```
Здесь можно увидеть, что *producer* читает данные с ввода и передаёт их в специальную переменную *promise* (коробочку), из которой *consumer* с помощью метода `get()` у переменной типа *future*, который не просто считывает содержимое *promise*, а ждёт, пока туда запишут какое-то значение.
Заметим, что *future* констрактится от *promise* и на самом деле, такая конструкция не способна передавать очередь значений, она лишь работает на "единичных передачках".

2. **Async (так писать не рекомендуется):**
```C++
 std::future<std::string> input_future = std::async([]() {
        std::string input;
        std::cin >> input;
        return input;
    });
```
Это другая реализация *producer*, остальной код такой же. 
*async* - по факту "прячет" в себя *thread* и *promise*. То есть мы таким образом говорим, что эта переменная вычислится в соседнем потоке когда-нибудь. 
Это плохая практика, так как *async* - мы соверешенно не контроллируем, не умеем отключать поток(если он нам стал не нужен)  например.

3. **Mutex-ы (так писать не рекомендуется):**
```C++
 std::mutex m;
    std::string input;
    bool input_available = false;

    std::thread producer([&]() {
        while (true) {
            std::cin >> input;

            std::unique_lock l(m);
            input_available = true;
        }
    });

    std::thread consumer([&]() {
        while (true) {
            std::string input_snapshot;
            {
                std::unique_lock l(m);
                if (!input_available) {
                    continue;
                }
                input_snapshot = input;  // Не хотим, чтобы input изменился, пока мы ждём две секунды.
                input_available = false;
            }  // l.unlock();

            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            std::cout << "Got string: " << input_snapshot << "\n";
        }
    });

    consumer.join();
    producer.join();
}
```
Идея таже, но вот в цикле *while* у *consumer*  постоянно берётся *mutex* - это сильно жрёт проц + бесполезно, так как на самом деле мы хотим его взять, только когда *producer* нам что-нибудь отправит, чтобы это исправить есть 4-ый способ.

4. **Cond_var** (the coolest):
Что такое *condition_variable*? Это такая переменная которая обладает методами: *.wait(m)* - ждёт, пока от этой же переменной вызовут *notify* на текущий поток, отпускает *mutex* ( *m* ) пока ждёт
*.notify*  -  будит произвольный поток (может всех, может одного) в котором запущен *wait* на этой переменной.
Таким образом, она позволяет усыплять поток и будить его другим потоком.
```C++
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

int main() {
    std::mutex m;
    std::string input;
    bool input_available = false;
    std::condition_variable cond;

    std::thread producer([&]() {
        while (true) {
            std::cin >> input;
            std::unique_lock l(m);
            input_available = true;
            cond.notify_one();  // Разбудит один случайный поток. Важно, чтобы это было ему по делу.
        }
    });

    std::thread consumer([&]() {
        while (true) {
            std::unique_lock l(m);
            while (!input_available) {  // while, не if!
                cond.wait(l);
            }
            // Эквивалентная while конструкция, чтобы было сложнее набагать и были понятнее намерения.
            // cond.wait(l, []() { return input_available; });  // "ждём выполнения предиката".
            std::string input_snapshot = input;
            input_available = false;
            l.unlock();

            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            std::cout << "Got string: " << input_snapshot << "\n";
        }
    });

    consumer.join();
    producer.join();
}
```
Тут после того, как *producer* считал данные и вызыватся *notify_one* у *cond_var*-а и таким образом запускается один случайный поток с *consumer*-ом.
Важно заметить, что  `cond.wait(l)` происходит внутри `while` а не `if`, т.к. существует `spurious wakeup`, когда система может просто так взять и разбудить поток, в таком случае мы бы вышли из `if` и пошли бы дальше, хотя *input* ещё не поступал. 

## Mutable
Это ключевое слово нужно, когда мы хотим менять какую-то переменную в *const* методах класса, в частности, это бывает полезно для *mutex*:
```C++
#include <iostream>
#include <mutex>

struct user {
    mutable std::mutex m_m;
    int m_balance = 1000;

    int balance_up() const {
        std::unique_lock l(m_m);
        return m_balance;
    }
    void get_salary(){
        std::unique_lock l(m_m);
        m_balance+=30;
    }
};
```
В данном коде, функция `balnce()` - потокобезопасная и с *const* квалифаером, если бы *mutex* был не *mutable* - мы бы не могли так сделать.
## Links:
https://github.com/hse-spb-2021-cpp/exercises/tree/master/18-220207/solution - практика на эту тему
https://github.com/hse-spb-2021-cpp/lectures/tree/master/18-220207 - лекция на эту тему.
