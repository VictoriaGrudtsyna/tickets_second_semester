## Билет 41. TCP-соединения при помощи блокирующего ввод-вывода в Boost::Asio

### Чем характеризуется TCP-соединение: две пары (хост с IPv4-адресом, порт)

#### IP (Internet Protocol)
Немного определений и концепций: \
* Каждый девайс имеет 0 или более IP адресов
    * IPv4 - 4 байта на адрес, обычно в десятичной системе, разделены точками, их мало (4 миллиарда)
    * IPv4 - локальный хост - `127.0.0.1`, обозначает "это устройство", то есть на другом устройстве означает другое, как следствие все не уникально
    * IPv6 - 16 байт на адрес, придумали в 98-ом году, но до конца полностью не перешли
* DNS (Domain Name System) - набор протоколов, договоров, чтобы переводить домены (например `hse.ru`) в IP адрес
    * Все происходит с помощью `nslookup` (не изучаем)
    * Перевод неоднозначен, в частности зависит от локации - так легче распределять нагрузку
* IP в одностороннем порядке могут отправлять маленькие пакеты

### TCP (Transmission Control Protocol)
Клиент инициирует соединение, сервер соединение принимает. \
На каждом устройстве выделяется порт. `TCP port` это число от 1 до 65535. \
Создается сокет. `TCP socket` - это пара (IP, Port).
В итоге, уникальный идентификатор соединения - это два сокета. \
Рисунок, описывающий соединение:
```
  client (initiates)           server (accepts)
┌───────────────────┐  bytes  ┌────────────────┐
│      ip:port      ├────────►│     ip:port    │
│95.161.246.38:65124│◄────────┤186.2.163.228:80│
└───────────────────┘  bytes  └────────────────┘
```
## Почему порт сервера обычно фиксирован, а клиента — случаен
IP, с которого инициировать соединение, обычно выбирается операционной системой как более подходящий. \
Порт клиента вообще не важен, поэтому выбирается рандомно - чаще от 40000 до 60000. При этом, чтобы подключиться к серверу нужно знать и его IP, и его порт, поэтому обычно у сервера порт фиксирован. Поэтому, например, если отрывать `google.com`, то IP мы получаем через DNS, а порт получаем стандартный.

## Использование `boost::asio::ip::tcp::iostream` на сервере и на клиенте для создания простого эхо-сервера
Рассмотрим простой сервер, который принимает от клиента строки и отправляет их обратно.
```c++
using boost::asio::ip::tcp;

int main(int argc, char *argv[]) {
    assert(argc == 2);

    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), std::atoi(argv[1])));
    std::cout << "Listening at " << acceptor.local_endpoint() << "\n";

    tcp::iostream client([&]() {
        tcp::socket s = acceptor.accept();
        std::cout << "Connected " << s.remote_endpoint() << " --> " << s.local_endpoint() << "\n";
        return s;
    }());

    while (client) {
        std::string s;
        client >> s;
        client << "string: " << s << "\n";
    }
    std::cout << "Completed\n";
    return 0;
}
```
`io_context` - просто переменная окружения, хранит информацию о многопоточности просто нужна boost (больше знать не надо) \
`acceptor` - штука, которая принимает соединения от клиентов, вторым параметром ей передается место, где принимат (`tcp::v4()` - значит все локальные IP, std::atoi(argv[1]) - порт, который берется из аргументов). \
`tcp::iostream` - поток ввода-вывода (работает как `stringstream`), но соеденен с очередным клиентом. В его параметр нужно передать сокет (причем в полное владение, например с помощью `std::move`). В данном случае это делается с помощью лямбды, а сокет появляется из функции `acceptor.accept()` - жди, пока подключится очередной клиент и установи с ним соединение (в этот момент сервер стал блокирующим). \
Примерно тоже самое, но теперь клиент: \
```c++
using boost::asio::ip::tcp;

int main(int argc, char *argv[]) {
    assert(argc == 3);

    boost::asio::io_context io_context;

    auto create_connection = [&]() {
        tcp::socket s(io_context);
        boost::asio::connect(
            s,
            tcp::resolver(io_context).resolve(argv[1], argv[2]));
        return tcp::iostream(std::move(s));
    };
    tcp::iostream conn = create_connection();
    std::cout << "Connected " << conn.socket().local_endpoint() << " --> " << conn.socket().remote_endpoint() << "\n";

    conn << "hello world 123\n";
    conn.socket().shutdown(tcp::socket::shutdown_send);
    std::cout << "Shut down\n";

    int c;
    while ((c = conn.get()) != std::char_traits<char>::eof()) {
        std::cout << static_cast<char>(c);
    }

    std::cout << "Completed\n";
    return 0;
}
```
Сначала создается какой-то сокет, и затем вызывается функция `connect`, которая говорит присоедени этот сокет к вот этому серверу, `tcp::resolver` как раз собирает этот сервер, используя DNS. Лямбда возвращает `tcp::iostream`, как раз мувая сокет. \
`shutdown` - мы может сказать другой стороне (в данном случае серверу), что больше не будем отправлять данные, для этого отправляем `tcp::socket::shutdown_send`. Сервер может посмотреть на это и например больше не принимать данные, сделав похожую штуку.

### Отличия `local_endpoint()` от `remote_endpoint()`
У сокета есть методы: \
`local_endpoint()` - на какой адрес к серверу подключились локально. \
`remote_endpoint()` - с какого адреса подключились.

Лекции Егора:
* [IP и TCP](https://youtu.be/xp0645fTXUE?list=PL8a-dtqmQc8ph7sgkWKlgoAHIw2WIaNDZ&t=1398) 
* [Server](https://youtu.be/xp0645fTXUE?list=PL8a-dtqmQc8ph7sgkWKlgoAHIw2WIaNDZ&t=3109)

### Протокол TCP передаёт байты, а не сообщения:

Передача данных – это передача _БАЙТОВ_, при этом нет гарантий, что байты приходят заявленными группами/сообщениями.

```c++
for (;;) {
    std::vector<char> buf(50);
    std::size_t read = client.receive(buffer(buf), 0);  // reads _something_
    std::string s(buf.begin(), buf.begin() + read);
    std::cout << "string of length " << read << ": " << s << "\n";
}
```

`send()` отправляет сколько-то байт, `receive()` – принимает сколько-то байт.

**NB:** в TCP нет понятия сообщения, поэтому нужно специально договариваться/обрабатывать, что является корректным вводом.

```c++
for (int i = 0; i < 20; i++) {
        s.send(buffer("hello\n"));  // bad
        // write(s, buffer("hello\n"));  // good, but does not matter
    }
```