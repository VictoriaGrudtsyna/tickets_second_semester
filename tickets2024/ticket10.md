## Билет 10. Move-семантика и copy elision

Лекция 14.md
### Категории значений: lvalue/xvalue/prvalue; обобщённые glvalue/rvalue

Все выражения в С++ характеризуются двумя свойствами: [типом](https://en.cppreference.com/w/cpp/language/type) и [категорией значения](https://en.cppreference.com/w/cpp/language/value_category): 

- `glvalue`: выражение имеет имя, может быть полиморфным, имеет адрес. 
- `prvalue`: выражение является результатом вычисления операнда (`+`, `-`, ...) или встроенного оператора, либо инциализирует объект.
- `lvalue`: выражение стоит слева от знака равенства (отсюда и название).
- `xvalue`: выражение, чьи ресурсы могут быть переиспользованы.
- `rvalue`: из выражения можно мувать.

```c++
#include <iostream>
#include <string>
#include <utility> // std::move

int foo() { return 42; }

int main() {
    int a = 10;

    // --- lvalue ---
    a = 20;              // a — lvalue (есть имя, адрес)
    std::cout << a << "\n";  

    // --- prvalue ---
    int b = 5 + 7;       // (5 + 7) — prvalue, временное значение
    std::string s = "hi"; // "hi" — prvalue (литерал)

    // --- xvalue ---
    std::string s1 = "hello";
    std::string s2 = std::move(s1); // std::move(s1) — xvalue
    // (s1 в состоянии "moved-from")

    // --- glvalue ---
    // glvalue = lvalue (a) или xvalue (std::move(a))
    int* p = &a;         // &a работает, т.к. a — lvalue (glvalue)
    *p = 33;

    // --- rvalue ---
    int x = foo();       // foo() возвращает 42 → prvalue → rvalue
    int y = std::move(x); // std::move(x) → xvalue → rvalue
}
```

Исключением является выражение `void()` — про него особо ничего не понятно.

Ниже указана схема деления категорий выражений:

```
┌────────────────────────────┐
│         glvalue            │
│      (Generalized Left)    │
│              ┌─────────────┼───────────────────┐
│              │             │      rvalue       │
│              │             │      (Right)      │
│ ┌──────┐     │ ┌─────────┐ │   ┌────────────┐  │
│ │lvalue│     │ │ xvalue  │ │   │  prvalue   │  │
│ │(Left)│     │ │(eXpired)│ │   │(Pure Right)│  │
│ └──────┘     │ └─────────┘ │   └────────────┘  │
└──────────────┴─────────────┴───────────────────┘
```

### rvalue-ссылки и lvalue-ссылки: что к чему привязывается

Тут все довольно просто: lvalue-ссылки могут привязываться только к `lvalue` значениям, а rvalue-ссылки только к `rvalue` значениям (то есть к `xvalue` и `prvalue`). Но есть некоторые нюансы с константными lvalue- и rvalue-ссылками. Ниже примеры кода, а также примеры с константыми ссылками:

```c++
#include <iostream>
#include <utility> // std::move

int foo() { return 42; }

int main() {
    int a = 10;

    // ===== LVALUE REFERENCES =====
    int& lref = a;   // OK: lvalue-ссылка может привязаться к lvalue (переменной с именем)
    std::cout << "lref=" << lref << "\n";

    // int& bad1 = 5; // ❌ ошибка: нельзя привязать lvalue-ссылку к prvalue (литералу)
    // int& bad2 = foo(); // ❌ ошибка: нельзя привязать к временному (prvalue)

    const int& ok1 = 5;  // OK: const lvalue-ссылка может привязаться к rvalue (временное удлиняет жизнь)
    const int& ok2 = foo(); // OK: то же самое

    // ===== RVALUE REFERENCES =====
    int&& rref1 = 5;       // OK: rvalue-ссылка к prvalue
    int&& rref2 = foo();   // OK: rvalue-ссылка к prvalue (результат функции)
    int&& rref3 = std::move(a); // OK: std::move превращает lvalue в xvalue → rvalue-ссылка может привязаться

    // int&& bad3 = a; // ❌ ошибка: rvalue-ссылка не может привязаться напрямую к lvalue (обычной переменной)
}

```


### Почему у move constructor и move assignment именно такая сигнатура, как работает разрешение перегрузок, почему не надо добавить const

Сигнатура copy- и move-constructor:
```c++
Foo(const Foo &other) {  // copy-constructor: direct init, copy init...
	std::cout << "Foo(const Foo&) " << this << " <- " << &other << "\n";
	/* lvalue */ other;
}

Foo(Foo &&other) {  // move-constructor: same, but initialized from rvalue
	std::cout << "Foo(Foo&&) " << this << " <- " << &other << "\n";
	/* lvalue as well */ other;
}
```

Сигнатура copy- и move-assignment:
```c++
Foo& operator=(const Foo &other) { // copy-assignment
	std::cout << "operator=(const Foo&) " << this << " = " << &other << "\n";
	return *this;
}

Foo& operator=(Foo &&other) { // // move-assignment
	std::cout << "operator=(Foo&&) " << this << " = " << &other << "\n";
	return *this;
}
```


Разрешение перегрузок работает следующим образом: не смотря на то, что копирующий конструктор и оператор присваивания могут обработать временные значения, приоритет на таком вызове будет у их мувающих перегрузок.

В мувающих перегрузках не нужно писать const, так как нам может потребоваться изменить объект, который нам передали в параметры (например, сделать `std::swap(this->data, other.data)`).


### std::move: как работает, почему ничего не делает, где используется, как реализовать свой

Согласно [cppreference](https://en.cppreference.com/w/cpp/utility/move), `std::move` эквивалентен следующей конструкции:
```c++
std::move(t) == static_cast<typename std::remove_reference<T>::type&&> (t)
```

Он используется для того, чтобы скастовать значение к категории `xvalue`, это необходимо только для того, чтобы мы умели вызывать нужную перегрузку у функций/методов/конструкторов. Сам `std::move` у объекта, на котором он был вызван, ничего не меняет!


### Примеры copy elision, return value optimization (RVO), named return value optimization (NRVO)

Вот [статья на habr](https://habr.com/ru/company/vk/blog/666330/), в которой также неплохо все это дело расписано. Можно глянуть, если есть время.

Copy elision — ситуация, при которой у объекта, возвращаемого по значению из метода, не создается его копия. Это почти уникальная особенность C++: copy elision допускается даже если у move/copy конструкторов есть сайд-эффекты (то есть если программист их самостоятельно реализовал).

Пусть есть структура:
```c++
class Bar {
public:
	Bar() {
		std::cout << "Bar(): " << this << std::endl;
	}
	
	~Bar() {
		std::cout << "~Bar(): " << this << std::endl;
	}
	
	Bar(const Bar& other) {
		std::cout << "Bar(const Bar&): " << this << " <- " << &other << std::endl;
	}
	
	Bar(Bar&& other) {
		std::cout << "Bar(Bar&&): " << this << " <- " << &other << std::endl;
	}
	
	Bar& operator=(const Bar& other) {
		std::cout << "Bar(const Bar&): " << this << " = " << &other << std::endl;
		return *this;
	}

	Bar& operator=(Bar&& other) {
		std::cout << "Bar(Bar&&): " << this << " = " << &other << std::endl;
		return *this;
	}
};
```

Давайте рассмотрим примеры, в которых происходит copy elision:
- `RVO` (return value optimization) — начиная с C++17 нам гарантируется, что произойдет copy elision, то есть тут даже не нужно иметь сгенерированный/написанный мувающий конструктор (`Bar(Bar&&)`):
	```c++
	Bar get_rvo_bar() {
		return Bar();
	}
	...
	int main() {
		Bar b = get_rvo_bar();
	}
	```
	```js
	Bar(): 0xa6f0dff71f // `b` was constructed only once
	~Bar(): 0xa6f0dff71f 
	```
- `NRVO` (named return value optimization) — здесь нам не гарантируется, что произойдет copy elision. Однако, у меня на GCC даже с флагом -O0 (без оптимизаций), copy elision все равно происходил:
	```c++
	Bar get_nrvo_bar() {
		Bar t;
		return t;
	}
	...
	int main() {
		Bar b = get_nrvo_bar();
	}
	```
	```js
	Bar(): 0xd73b9ff9ef // `b` was constructed only once again
	~Bar(): 0xd73b9ff9ef
	```
	Рассмотрим отличия этих двух оптимизаций. Пусть есть слудующая функция:
	
	```c++
	C f() {
		C local_variable;
		// ...
		return local_variable;
	}
	...
	C result = f();
	```
	Благодаря `NRVO` здесь вместо создания `local_variable` компилятор сразу создаст `result` конструктором по умолчанию в точке вызова функции `f()`. А функция `f()` будет выполнять действия сразу с переменной `result`. То есть в этом случае не будет вызван ни конструктор копии, чтобы скопировать `local_variable` в `result`, ни деструктор `local_variable`.

	Что же касается `RVO`, то это такая же оптимизация, как `NRVO`, но для случаев, когда экземпляр возвращаемого класса создаётся прямо в операторе `return`:

	```c++
	C f() { return C(); }
	```

	Более подробно об особенностях `RVO/NRVO` написано в статье на хабре, ссылка будет в конце билета.

- Конструирование из временного объекта:
	```c++
	void foo(Bar foo_param) {
		std::cout << "foo(Bar): " << &foo_param << std::endl;
	}
	...
	int main() {
		Bar b1 = Bar(); // regular constructor
		std::cout << "=====" << std::endl;

		Bar b2 = Bar(Bar()); // two rounds of elision
		// actually you can put any number of Bar(Bar(Bar(...))) constructor will be called
		// only once.
		std::cout << "=====" << std::endl;
		
		foo(Bar()); // parameter constructed from temporary object
		std::cout << "=====" << std::endl;

		// destructors will be called here
	}	
	``` 
	```js
	Bar(): 0xb6e3bff66e // `b1` - just regular constructor 
	=====
	Bar(): 0xb6e3bff66d // `b2` - double elision
	=====
	Bar(): 0xb6e3bff66f // `foo_param` constructor
	foo(Bar): 0xb6e3bff66f // `foo` function body
	~Bar(): 0xb6e3bff66f // `foo_param` destructor
	=====
	~Bar(): 0xb6e3bff66d // `b2` destructor
	~Bar(): 0xb6e3bff66e // `b1` destructor
	```

### Когда (не) надо писать return std::move(foo);

Вообще никогда не надо писать `std::move(local_object)` в качестве возвращаемого значения функции, так как это не позволяет сработать `NRVO`-оптимизации (более подробно [тут](https://habr.com/ru/company/vk/blog/666330/) - та же статья на habr).

Его имеет смысл писать только тогда, когда мы не муваем локальный объект функции/метода, а муваем, например, аргумент, переданный функции как rvalue-ссылка (в примерах кода выше есть такой случай).
