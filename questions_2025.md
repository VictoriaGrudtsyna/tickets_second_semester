# Общее
* В билете написано почти всё, что надо рассказать.
* Для каждой синтаксической конструкции должен быть приведён содержательный пример, когда это требуется или делает код проще/удобнее.
* Вас могут активно спрашивать вокруг билета: если вы сказали, что надо писать `Foo(bar)`, вас могут сразу же спросить, что будет при написании `Foo(&bar)`, вы должны ответить.
* "Не было" — не было на лекции, знать не надо, не спрашивают. Даже если было на практике.

# Обязательные знания
Если вы не знаете какие-то термины что-то из списка ниже, это автоматический неуд.

## Термины
* Свободная функция, функция-член
* Время жизни объекта: автоматическое, динамическое, статическое
* Undefined behavior: что может произойти, примеры:
  * Неинициализированная переменная: почему у неё нет никакого значения, почему не стоит говорить что вместо значения "мусор, который иногда меняется".
  * Выход за границы массива.
  * Dangling reference (висячая ссылка) при возврате из функции, при инвалидации в стандартном контейнере.
  * Разыменование `nullptr`.

## Переменные
* статическая типизация
* тип переменной
* размер переменной, размер типа
* типы <code>int</code>, <code>char</code>: типичные размеры. Например, на архитектуре <code>x86_64</code> под Linux в компиляторе <code>GCC</code>.

## Основные конструкции языков C и C++
* объявления переменных
* выражения и операторы:
  * арифметические (`+`, `-`, `*`, `/`, `%`, `+=`, `-=`, `*=`, `/=`, `%=`, `++`, `--`)
  * булевы (`<`, `<=`, `>`, `>=`, `==`, `!=`, `&&`, `||`, `!`)
  * на минимальную оценку необязательно знать: отличия постфиксных и префиксных операторов
* условные операторы `if`, `switch` (с `break` и fallthrough, включая проблему с инициализацией переменных)
* циклы `for`, `while`

## Функции
* объявление и определение функции, требуется отличать между собой (можно случайно перепутать, но после вопроса исправиться)
* вызов функции (синтаксис, не нужно знать mangling, конвенции вызовов)
* возвращаемое значение
* рекурсивный вызов

## Основные конструкции C++
* синтаксис `static_cast`
* синтаксис объявления пространств имён, обращения к элементу внутри пространства имён
* `auto` для объявления переменных
* range-based for, в том числе с `auto&` и `const auto&`
* синтаксис шаблонов, достаточный для написания минимального адаптера `stack<T>` поверх `deque`

## Классы
* определение класса, конструктор, деструктор, методы
* специальные методы (пять штук), правило нуля, правило пяти
* приватные/защищённые/публичные поля и методы
* наследование: базовый синтаксис, включая вызов конструктора базового класса

## Полиморфные классы
* виртуальные методы и их отличия от невиртуальных
* необходимость виртуального деструктора
* чисто виртуальные функции, что происходит при отсутствии `override` или `virtual`
* slicing (срезка объектов) при присваивании полиморфного класса в переменную типа "базовый класс", как защититься (хранить только ссылки/умные указатели)
* хранение полиморфных объектов в контейнерах
* `dynamic_cast` для полиморфных классов (по указателям и ссылкам)

## Const correctness
* синтаксис константных ссылок, применение при передаче аргументов
* невозможность изменять константные объекты и их поля
* const-qualifier у методов (const member function), перегрузка по const-qualifier (const overloading)

## Использование move-семантики
* эффективная инициализация полей класса из аргументов, принятых по значению или по rvalue-ссылке
* moved-from состояние у объектов: может быть не определено, может быть невозможно обнаружить, пример ошибки
* отсутствие необходимости `move` из результата функции, возвращённого по значению
* необходимость move для явной передачи владения `unique_ptr`
* почему `std::move` не выполняет никакого кода

## STL
* использование `vector` как динамического массива фиксированной длины, <code>push_back</code> и <code>emplace_back</code>
* использование `map` со стандартным компаратором, особенность <code>operator[]</code> (создаёт значение даже при чтении)
* `lower_bound`/`upper_bound`: параметры, возвращамое значение

## Си
* использование строк в стиле Си: отличия `char s[10]` и `const char *s`, преобразование в две стороны для `std::string`
* операции со строками в стиле Си: конкатенция, получение длины, безопасная альтернатива для `gets` и `scanf("%s")`
* массивы массивов в языке Си (вроде `int**`): выделение, освобождение, использование

## Исключения
* синтаксис `try`/`catch`/`throw`, включая `catch (...)` и `throw;` (но без function-try-block)
* раскрутка стека: автоматический вызов деструкторов, в том числе в `new`/`new[]`, конструкторе, конструкторах полей
* необходимость ловить исключения по ссылке
* exception safety: определения no/basic/strong/nothrow
* умение добавить базовую гарантию в произвольный код при помощи автоматических выполняемых деструкторов

## Многопоточность
* базовое использование `thread` (включая `join()`), `mutex` и `unique_lock`
* гонки: пример гонки данных и гонки при выводе на экран, как избежать, умение видеть гонки в произвольном несложном коде

# Билеты
## 1x. Продвинутый синтаксис
`10-241121`, `24-240409`, `25-240416`

### 10. Move-семантика и copy elision
* Категории значений: lvalue/xvalue/prvalue; обобщённые glvalue/rvalue
    * Определение в C++11/C++14
    * Новое определение prvalue из C++17 (нужное для guaranteed copy elision) не требуется
* rvalue-ссылки и lvalue-ссылки: что к чему привязывается
* Почему у move constructor и move assignment именно такая сигнатура, как работает разрешение перегрузок, почему не надо добавить `const`
* `std::move`: как работает, почему ничего не делает, где используется, как реализовать свой
* Примеры copy elision, return value optimization (RVO), named return value optimization (NRVO)
* Когда (не) надо писать `return std::move(foo);`

### 11. Реализация своего `std::vector`
* Placement new, выравнивание (`alignas`/`alignof`), явный вызов деструктора
* Время жизни объектов (lifetime) и недопустимость операций за пределами lifetime
* Ref-qualifiers: `&`, `&&`, перегрузка по ним и по const-qualifier
* Перегрузка `operator[]`
* Своя реализация `std::vector`
    * Перегрузка `push_back` для `const T&` и `T&&` как альтернатива perfect forwarding; отличие от принятия по константной ссылке или по значению
    * Использование `const_cast` для уменьшения дублирования кода (`24-240409/01-ref-qualifier/11-const-cast-accessor-good.cpp`
    * Трудности с обеспечением базовой гарантии исключений при создании массива объектов (практика `25-240416`, задания `0*-minivector*`)

### 12. Теоретическая и практическая всячина
* Возвращаемый тип `auto` функций и методов
    * Как выводится просто `auto` (возможно, с модификаторами), в том числе для рекурсии
    * Использование `auto` и `->`, удобство при определении функций-членов снаружи класса
    * Объявления функций с возвращаемым типом `auto`
    * Явное указание возвращаемого типа лямбды
* Оператор `switch` 
    * `break;` и `[[fallthrough]]`
    * Проблемы с инициализацией переменных внутри `switch`
* Глобальные переменные/функции и статические поля-члены классов; слова `extern` и `inline`
    * Всё ещё есть static initialization order fiasco
    * Использование `inline`-переменных/функций/методов для обхода ODR
      * Требования к нескольким определениям.
    * Отличия `inline` от `static` от unnamed namespace.
* Как лучше объявлять глобальные константы и статические константы-члены классов
    * `constexpr` против `const`; порядок инициализации
    * С `inline` и без `inline`; множественное определение

## 2x. Шаблоны
`19-250221`, `24-240409`, `30-250606`

### 20. Шаблоны классов
* Шаблоны классов
    * Синтаксис объявления и определения
    * Параметры шаблона: типы (разница `typename`/`class`), значения простых типов (без строго определения "простоты")
    * Параметры по умолчанию, отсутствие имени у параметра
    * Параметры-шаблоны, передача в них шаблонов с параметрами по умолчанию
    * Ключевые слова `typename`, `template` для обращения внутрь зависимых типов, в том числе при наследовании от шаблонного параметра (задание с практики `*-call-dependent-method.cpp`)
* Отличие шаблона `Foo` и инстанцирования шаблона с параметрами по умолчанию `Foo<>`
    * Когда можно писать `Foo` вместо `Foo<T>`, удобство с `auto Foo<T>::foo() -> Foo`
* Инстанцирование
    * Независимость по методам
    * Когда происходит неявное инстанцирование: полей, методов (статических и нестатических), вложенных типов
    * Когда тип-параметр шаблона может быть incomplete, а когда не может
    * Не было: явное инстанцирование
* Независимость инстанциаций шаблона
* Определение методов и статических членов шаблонного класса внутри и вне класса

### 21. Использование шаблонов
* Передача функторов без `std::function` в качестве: поля, аргумента функции по значению
* Шаблоны функций, базовый автовывод параметров шаблона из аргументов
* Class Template Argument Deduction (CTAD) из конструкторов: при инициализации переменных, при создании временных объектов
* Шаблонные методы/конструкторы в обычных и шаблонных классах
    * Определение внутри и вне класса
* Шаблонные лямбда-выражения
* Проблемы с `>>` до C++11
* Псевдонимы типов: `template<> using`
* Шаблоны переменных (C++17), примеры из стандартной библиотеки
* Переиспользование шаблонов между единицами трансляции, автоматический `inline`, forward declaration

### 22. Специализации
* Полные и частичные специализации шаблона класса: синтаксис, независимость специализаций
    * Специализация по типам, значениям
    * Частичная специализация с уменьшением или увеличением количества шаблонных переменных
* Использование для метапрограммирования: `is_void`, `is_reference`, `is_same`, `conditional`
* Полные специализации функций
    * Ограничения по сравнению с полными специализациями шаблонов класса
    * Влияние на автовывод типов
    * Эмуляция частичных специализаций при помощи структур
* Как использовать `std::swap`, чтобы работали сторонние контейнеры
  * Запрет на добавление перегрузок в `namespace std` и необходимость частичной специализации
  * Костыль через `using std::swap;` и ADL (Argument-Dependent Lookup)

### 23. Шаблонные друзья
* Про всех рассказывать и показать примеры: когда работает, когда нет, к чему у кого есть доступ
    * Будьте осторожны с багами GCC и Clang при демонстрации
* Шаблонный класс-друг произвольного класса
    * Общий случай
    * Когда другом является только полная специализация
* Нешаблонная функция-друг шаблонного класса
    * Определение внутри и снаружи класса
    * Тонкость при зависимости от параметра шаблона
* Шаблонная функция-друг шаблонного класса
    * Определение внутри и снаружи класса
    * Когда другом является только полная специализация
    * Multiple definition внутри шаблонного класса при отсутствии зависимости от параметра шаблонного класса

### 24. Вызов шаблонных функций
* Автовывод шаблонных параметров функций
    * Автовывод параметров шаблона из аргументов, в том числе с `const`, ссылками, наследованием
    * Частичное указание шаблонных параметров при вызове
* Невозможность автовывода:
    * Типа `C` из типа `typename C::iterator`
    * При конфликтующих аргументах
    * При неоднозначном базовом классе
    * При необходимости пользовательских преобразований
* Разрешение перегрузок с шаблонными функциями и автовыводом шаблонных параметров
    * Термины: overload set, viable function, best viable function
    * Этапы разрешения перегрузки и их порядок: построение overload set, выбор viable, best viable, проверка доступа
    * Приоритет нешаблонных перегрузок над шаблонными
    * Отличия вызова `foo()` и `foo<>()` для разрешения перегрузок
    * Отсутствия влияния специализаций функций на выбор перегрузки, пример Димова-Абрамса

## 3x. Исключения
`16-250131`, `17-250207`

### 30. Исключения — основы
* Предусловия и постусловия конструктора и деструктора, инвариант объекта (`17-250207/01-exception-guarantees/10-raii.cpp`)
* Как отличить "ошибки программирования" (undefined behavior, нарушение инвариантов, невозможно предсказать поведение после) и "ошибки окружения" (некорректный ввод пользователя, теоретически можно предсказать и обработать), разные стратегии обработки для двух видов ошибок.
* Техники обработки ошибок без исключений: `assert`, `exit`, коды возврата
    * Способ передачи кода возврата: состояние объекта, глобальная переменная, возвращаемое значение, отдельный аргумент
    * Стратегии обработки: аварийное завершение, проброс ошибки наверх
* Синтаксис `try`/`catch`/`throw`, вложенные `try`/`catch`, несколько `catch` подряд, когда какие исключения ловятся (в том числе с наследованием)
* Слайсинг исключений и как с ним бороться
* Уничтожение локальных (automatic storage) ресурсов (раскрутка стека, stack unwinding)
* Концепция RAII для безопасного владения ресурсами (например, памятью)

### 31. Исключения — детали
* Базовые классы исключений в стандартной библиотеке: `exception`, `runtime_error`, `logic_error`
* Стандартные исключения в стандартной библиотеке: `bad_alloc`, `bad_cast`
* Производительность исключений в happy path/sad path.
* Исключения в конструкторах и деструкторах и их влияние на время жизни объектов (lifetime)
* Исключения при: конструировании массивов, базовых классов и полей, вызове делегирующего конструктора, создании параметров функции, возврата по значению
    * До C++17: проблема с возможным interleaving аргументов
* Причина отсутствия `stack::pop()`

### 32. Гарантии исключений
* Exception safety (безопасность исключений): `noexcept`/no-throw (отсутствия исключений), strong (строгая), basic (базовая), no (отсутствующая)
* Спецификатор `noexcept`, что происходит при выбрасывании исключения
    * Типичная гарантия для оператора перемещения
* Обеспечение базовой гарантии при помощи RAII (практика `17-250211`, задание `13-please-use-vector`)
* Подвохи с базовой гарантией при реализации `operator=(const&)`, если сделать подряд `delete[] data; data = new char[...]` (можно получить UB в деструкторе, чинить — сначала выделить, потом портить поля)
* Обеспечение строгой гарантии исключений при помощи:
    * Полного копирование объекта при последовательности сложных операций (например, при реализации `push_back`)
    * Copy-and-swap idiom для `operator=`
* Возможное отсутствие базовой гарантии у реализации `operator=` по умолчанию при наличии инварианта класса

### 33. Исключения — необычная обработка
* Непойманные исключения и раскрутка стека
* `catch (...)`
* Перебрасывание текущего исключения при помощи `throw;`, возможный slicing
* `exception_ptr`, его создание/перебрасывание/хранение, пустое состояние (`17-240206/02-weird-exceptions/01-exception-ptr.cpp`)
* Function-try-block у конструкторов, деструкторов, методов и функций
    * Невозможность обращения к полям
    * Возможность (или её отсутствие) отменить выброс исключения

## 4x. Многопоточность и сети
`18-250214`, `19-250221`

### 40. Базовая многопоточность
* Создание потоков в C++11, передача аргументов в функцию потока по значению и ссылкам
* Гонки: при выводе на экран, по данным, одновременное чтение без записей
* Борьба с гонками: мьютексы, атомарные снимки, RAII-обёртка над мьютексом (`unique_lock`)
* Частичная потокобезопасность `cout`
* Joinable/detached потоки

### 41. TCP-соединения при помощи блокирующего ввод-вывода в Boost::Asio
* Чем характеризуется TCP-соединение: две пары (хост с IPv4-адресом, порт)
* Почему порт сервера обычно фиксирован, а клиента — случаен
* Использование `boost::asio::ip::tcp::iostream` на сервере и на клиенте для создания простого эхо-сервера
* Отличия `local_endpoint()` от `remote_endpoint()`
* Протокол TCP передаёт байты, а не сообщения: можно получить только кусок сообщения за один вызов чтения.
  Или наоборот, за один вызов получить несколько сообщений.
  Решение: разделять на сообщения самостоятельно.

### 42. Оповещение о событиях
* Условные переменные: как использовать, spurious wakeup
* Реализация producer-consumer
* Ключевое слово `mutable`

### 43. Дизайн многопоточных приложений
* Deadlock, recursive mutex, отделение приватного API без блокировок от публичного API с блокировками
* Взаимные блокировки и их избегание при помощи контроля порядка взятия блокировок или `scoped_lock`/`unique_lock`
* Ключевое слово `thread_local` в сравнении с глобальными переменными
* Частичная потокобезопасность `shared_ptr`
* Проблема TOCTOU

## 5x. Совместимость с языком программирования Си
`20-250228`, `21-250307`, `22-250314`, `23-240319`

### 50. Trivially Copyable
* Существование разных кодировок строк, разного порядка байт в числах
* `reinterpret_cast`, strict aliasing rule и его нарушения в C++, корректное преобразование между `int`/`float` на уровне байт
* Trivially Copyable структуры: определение, пример, использование для (де)сериализации
* Padding (выравнивание) и его отключение, последствия хранения невыровненных нетривиальных типов вроде `vector<>`
* Особенности взятия ссылок и указателей на невыровненную память (пример со `swap` полей)
* Standard Layout и Plain Old Data (POD) структуры: определение из C++11

### 51. Многомерные массивы
* C-style-arrays/массивы в стиле Си
    * Инициализация, невозможность копирования, связь с арифметикой указателей
    * Расширение компилятора variable-length-arrays (VLA)
    * Динамическое выделение/освобождение при помощи `new[]`/`delete[]`
* Тип "массив известного размера", тип "массив неизвестного размера", тип "указатель на массив", `auto`, автовывод размера в шаблонах
* Многомерные массивы, в том числе с неизвестным нулевым измерением
* Массивы массивов как многомерные массивы: выделение и освобождение за константное количество операций, использование
* `std::array<>` для упрощения операций с массивами

### 52. Указатели
* Указатели на указатели
    * Выделение, освобождение, использование: массив строк, output-параметр функции
    * Что означает `const` в разных местах указателя на указатель
    * Явные и неявные преобразования двойных (и выше) указателей: константность и наследование
* `realloc` и его безопасное использование (`s = realloc(s, 4)` небезопасно)
* Ключевое слово `restrict`

### 53. Обобщённые функции
* Указатели на функции
    * Синтаксис, синтаксис, использование как параметров функции (в том числе шаблонной)
    * Отсутствие типа у перегруженной/шаблонной функции
    * Взятие указателя на перегруженную/шаблонную функцию при помощи `static_cast` или присваиванием в параметр/переменную фиксированного типа
    * Неявное преобразование лямдба-выражений в указатель на функцию
    * Преобразования между указателями на функции разных типов
* Указатель `void*`
    * Явные и неявные преобразования
    * Расширение компилятора для арифметики указателей
* Указатели на функцию с параметром `void*` как альтернатива функторам с захватом; пример реализации `for_each`

### 54. Отличия Си и C++
* Комментарии и объявления переменных (особенно в C89)
* `(void)` в объявлении функции, неявные объявления функций
* Объявления своих структур (везде нужно `struct`), конвенция с `typedef`
* Выделение и освобождение памяти через `malloc`/`free` против `new`/`delete`
* Отличия между Си и C++ в преобразованиях `void*`
* Альтернативы для языковых возможностей: unnamed namespace, `*_cast<>`, `int a{}`, namespace, ссылки, `bool`, операторы копирования и перемещения, шаблоны
* Отсутствующие возможности: шаблоны, перегрузка функций и параметры по умолчанию, стандартная библиотека, исключения, RAII

### 55. Особенности и идиомы Си
* `printf`/`scanf`: ограничение буфера для `%s`, вывод `%`, получение текущей позиции в `scanf`, потенциально квадратичное время работы при использовании `scanf`
* Designated initializer для структур и массивов
* Макросы для для констант (вместо `const`)
* `goto` для выхода из циклов и обработки ошибок
* Union, anonymous union, anonymous struct

### 56. Взаимодействие с библиотеками на Си
* Хранение строк в POD: `char[]`, `char*`, `std::string` (`22-230320/04-trivially-copyable-strings`)
* Необходимость вручную выделять буфер под строку, небезопасность `gets`
* Конвенции выделения ресурсов и opaque-структуры: выделение пользователем (строки), выделение библиотекой (`fopen`)
* `const_cast`
* `extern "C"` и линковка программ на Си/C++, линковка со стандартной библиотекой C++, совместимые между Си/C++ заголовки

## 6x. Наследование и динамический полиморфизм
`14-250117`

### 60. Множественное наследование
* Синтаксис, пример
* Возможное представление в памяти, пример изменения адреса при `static_cast` (потому что начало подобъект теперь не всегда совпадает с началом объекта)
* Порядок инициализации/уничтожения подобъектов и полей, как передать параметры конструкторам
* Возможное дублирование базового класса и возникающие неоднозначности при приведении типа
* Cross-cast (он же side-cast, `14-250117/03-multiple-inheritance/10-side-cast.cpp`)
* Как теперь работают `dynamic_cast`/`static_cast`/неявное преобразование/`override`/приватное и защищённое наследование
  * В том числе при наличии одинаковых виртуальных функций в независимых базах

### 61. Виртуальное наследование
* Виртуальный базовый класс: синтаксис, пример, проблема ромба
* Порядок инициализации/уничтожения подобъектов и полей, как и где передать параметры конструктором
* Возможное представление в памяти (только для неполиморфных классов)
  * Необходимость хранения указателя в каждой виртуальной базе
* Что происходит, если один класс объявляется и виртуальным, и невиртуальным
* Взаимодействие с `dynamic_cast` при наличии виртуальных методов (работает) и их отсутствии (невозможно)
* Delegate-to-sister
* Как теперь работают `dynamic_cast`/`static_cast`/неявное преобразование/`override`/приватное и защищённое наследование
  * Невозможность `static_cast` к наследнику от виртуальной базы
  * В том числе при наличии одинаковых виртуальных функций в независимых базах

### 62. Трюки с наследованием классов (необязательно множественными)
* Короткие факты
  * Наследование `private`/`protected`/`public`, отличия слов `struct`/`class`.
  * slicing (срезка объектов) при присваивании полиморфного класса в переменную типа "базовый класс", как защититься (хранить только ссылки/умные указатели)
  * Получение most derived class при помощи `dynamic_cast`
  * Делегирующие конструкторы (поведение при исключениях — отдельный билет)
* Один из способов реализации виртуальных функций: таблица виртуальных функций, в том числе с наследованием
* CRTP (практика `14-250120/01-crtp`)

## 7x. Препроцессор
`15-250124`

### 70. Препроцессор: базовые макросы
* Конкатенация строковых литералов
* Как пользоваться `#ifdef DEBUG`, `#define` и ключом `-DDEBUG` (в случае GCC/Clang).
* Базовый синтаксис для `#if` и `defined()`
* Определение компилятора и компилятороспецифичные `#pragma` (например, игнорирование предупреждений в GCC)
* Проблемы с аргументами макросов: приоритет операторов, повторное вычисление.
* Проблемы с макросами из нескольких statement и трюк с `do { } while(0)`
* Проблемы с `operator,` и вызовом макросов: как добиться, чтобы `assert(true), assert(false);` работало.
* Вариадический макрос: `__VA_ARGS__` и `...`, проблемы с последней запятой (не было: решение этой проблемы).
* Проблемы с `assert(v == std::vector{1, 2, 3})`, `foo({1, 2})`, `foo(map<int, int>{})` и как их решать.

### 71. Препроцессор: генерация кода и `inline`
* Макрос как параметр макроса: пример с автоматическим обходом всех элементов `enum`.
* Базовое использование `##` в макросах
  * Было: генерация случайных имён переменных/функций при помощи `__LINE__` (`exercises/15-250128/02-new-var`)
  * Не было: понимание, почему там надо несколько уровней макросов
* Как использовать `#macro_arg`, `__FILE__`, `__LINE__` для создания своего макроса `CHECK` в тестовом фреймворке `mytest`).
* Возможность писать в макросах лишь куски корректного кода.
  * Общая идея создания фреймворка для автотестов с авторегистрацией тестов: `TEST_CASE("hi") { CHECK(1 == 2); }`.

## 8x. Метапрограммирование
`29-250530`, `30-250606`

### 80. Parameter pack — основы
* Функции с произвольным числом аргументов: синтаксис function parameter pack
* Template parameter pack, variadic template (в том числе специализации — у них может быть несколько паков)
* Значения вместо типов в template parameter pack
* Pack expansion, в том числе параллельный для нескольких паков
* Автовывод типов, автовывод нескольких template parameter pack из аргументов функции
* Fold expression (бинарный и унарный)
* Эмуляция цикла по элементам parameter pack при помощи лямбда-функции, fold expression и `operator,`

### 81. Parameter pack — детали
* Pattern matching + специализации для рекурсивной обработки элементов пака у классов
* Pattern matching + автовывод параметров + перегрузки для обработки элементов пака у функций
* Реализация `std::tuple`, `std::get`, `std::tuple_size`, `std::tuple_element`
* Эмуляция цикла по элементам parameter pack при помощи лямбда-функции и инициализации фиктивного массива (работает в C++14).
* Использование `make_index_sequence`: печать на экран всех элементов `tuple` без рекурсии

### 82. Вспомогательные инструменты метапрограммирования
* `std::apply` для вызова функции с параметрами из кортежа
* Ссылки внутри `std::tuple`, функция `std::tie`
* Необходимость правильной расстановки `noexcept` для эффективной работы.
  Пример: копирование `std::vector<T>` может использовать разные алгоритмы.
* Условный оператор `noexcept`, конструкция `noexcept(noexcept(expr))`
* `std::declval`, где можно вызывать, зачем

### 83. Perfect forwarding
* Использование perfect forwarding: `std::forward`, синтаксис forwarding reference
* Теория perfect forwarding: правила вывода для forwarding reference, правила reference collapse
* Когда `T&&` не является forwarding reference, perfect forwarding в методах шаблонных классов
* Perfect forwarding для множества параметров функции
* Оператор `decltype` (два режима работы)
* Синтаксис `decltype(auto)` для объявления переменных и в возвращаемом типе функций и лямбд
    * Невозможность объявить переменную/поле типа `void` (практика `28-220523`, задание `01-memorizer`)
* Отличия `ref`/`cref` и perfect forwarding

### 84. Вычисления на этапе компиляции — основы
* `constexpr`-вычисления: константы (отличия от `const`), функции (включая принятие константных строковых литералов), структуры
* Тип "функция", его отличия от "указатель на функцию"
* Функции на этапе компиляции из типов или значений в наборы типов и значений
    * Реализация через структуры и их специализации
    * Конвенции `_t`, `_v`
    * Стандартный класс `integral_constant` и его наследники, наследование от них для упрощения реализации
* `iterator_traits<T>`:
    * `::value_type`, отличия от `T::value_type`.
    * Зачем нужен, когда в C++11 есть `auto`.
* Реализация `function_traits`

### 85. SFINAE — основы
* Определение SFINAE для функций и специализаций шаблонов, исходное применение
* Отличия hard compilation error от SFINAE, работа внутри псевдонимов типов и шаблонных переменных, примеры неработающего SFINAE
* SFINAE по возвращаемому типу: без запятой, с оператором `,`, с возвратом `void`
* `enable_if`: требования, реализация, использование для SFINAE в возвращаемом типе
* SFINAE в фиктивных параметрах шаблона
    * В значении по умолчанию, проблемы с переопределением функций
    * В типе
    * Использование `void_t` вместо `enable_if_t` для проверки корректности выражений

## Явно исключено
* иерархия итераторов и их виды
* `::operator new` и их перегрузка
* аллокаторы
* использование виртуального наследования для ABC (abstract base class)
* определения паттернов "Стратегия" и "Фабрика"
* TCP: возможность не получить сообщение целиком за один вызов `receive`, почему это неважно для `tcp::iostream`
* невозможность обеспечения basic guarantee на основе no guarantee
* обеспечение безопасности исключений в шаблонных классах, когда операции с `T` могут бросать исключения
* проблемы от не-`noexcept` конструктора/оператора перемещения
* placement new для массивов
* теоретические проблемы при реализации `std::vector`
* интрузивные списки
* возврат initializer list или `void` из функции с auto-возвращаемым типом
* когда следует передавать параметры по `&&`
* прокси-объекты и их проблемы с `auto`
* type erasure через полиморфные шаблонные классы
* Концепты
* Guaranteed copy elision в C++17 (`*-guaranteed-copy-elision.cpp`)
    * Когда (не) возникает, а когда происходит temporary materialization и создание временного объекта
    * Возврат неперемещаемых объектов из функции
* "Нарушение" правила пяти: когда деструктор может быть тривиальным, а копирование нетривиальным (при ссылках внутрь объекта)
* Мелочи
    * Инициализация при помощи `new T`, `new T()`, `new T{}`, в том числе при вызове с параметрами
    * Инициализация при помощи `new T[]`, `new T[]{}`, в том числе при вызове с меньшим/большим/точным количеством элементов
    * Объявление конкретного метода другом, проблемы с приватными методами (`01-member-friend.cpp`)
* Идиома pImpl: зачем, как реализовать
* Мелочи про метапрограммирование
    * Возврат parameter pack из структуры невозможен
        * Хранение списка типов в `struct type_list<Ts...>`
        * Вспомогательная функция/структура для раскрытия `type_list<Ts...>` на произвольный `F<Ts...>`
    * Конструктор от `initializer_list` (в том числе с рекурсией), невозможность `std::move`
    * `std::addressof`
    * Указатели на члены класса (поля, методы) и их вызов (`.*`, `->*`, `invoke`)
        * Совместимость указателей на члены класса-родителя и класса-наследника
        * Несовместимость с указателями на данные
* Формализм happens-before, неочевидные примеры нарушения и выполнения, возможный reordering
* ABA problem
* Точные правила для protected: через какие указатели можно обращаться к protected-именам родителя/своим/детей; друзья (кроме доступа к protected-полям родителя через друга наследника; тут компиляторы не согласны).
* Hiding, обращение к скрытому имени, using для методов и конструкторов (поведение отличается), добавление перегрузок к методу родителя, изменение видимости полей/методов
* Макросы против inline-функций в языке Си
* Глобальные переменные/функции и статические поля-члены классов; слова `extern` и `inline`
    * Слово `inline` иногда следует из `const`
