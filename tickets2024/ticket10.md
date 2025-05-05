## Билет 10. Move-семантика и copy elision

Лекция 14.md
### Категории значений: lvalue/xvalue/prvalue; обобщённые glvalue/rvalue

Все выражения в С++ характеризуются двумя свойствами: [типом](https://en.cppreference.com/w/cpp/language/type) и [категорией значения](https://en.cppreference.com/w/cpp/language/value_category): 

- `glvalue`: выражение имеет имя, может быть полиморфным, имеет адрес. 
- `prvalue`: выражение является результатом вычисления операнда (`+`, `-`, ...) или встроенного оператора, либо инциализирует объект.
- `lvalue`: выражение стоит слева от знака равенства (отсюда и название).
- `xvalue`: выражение, чьи ресурсы могут быть переиспользованы.
- `rvalue`: из выражения можно мувать.

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


Примеры xvalue, rvalue, lvalue.



```c++
int get_value() { // value
    return 10;
}

int &get_ref() { // reference
    static int x;
    return x;
}

int &&get_rvalue_ref() { // rvalue-reference
    static int x;
    return std::move(x);
}

const int &get_cref() { // const reference
    static int x;
    return x;
}

int *get_ptr() { // pointer
    static int x;
    return &x;
}

int main() {
    int a = 10;
    int arr[5]{};
    struct { int x, y; } s;
    std::vector<int> vec(10);

    // lvalue: "has name", "cannot be moved from because lives long enough"
    std::cin;
    a;
    get_ref();
    get_cref();  // type: const int
    *get_ptr();
    arr;
    arr[4];  // `arr` should be lvalue
    s.x;  // `s` should be lvalue

    // prvalue: does not have name, "temporary" object
    10;
    get_value();
    get_value() + 20;
    &a;
    static_cast<int>(10.5);
    // `this` is also a prvalue.
    []() { return 23; };

    // xvalue: will expire soon, but has name
    std::move(vec);
    std::move(vec)[9];
    std::move(s).x;
    static_cast<std::vector<int>&&>(vec);  // std::move is almost exactly 
that.
    get_rvalue_ref();
}
```



### rvalue-ссылки и lvalue-ссылки: что к чему привязывается

Тут все довольно просто: lvalue-ссылки могут привязываться только к `lvalue` значениям, а rvalue-ссылки только к `rvalue` значениям (то есть к `xvalue` и `prvalue`). Но есть некоторые нюансы с константными lvalue- и rvalue-ссылками. Ниже примеры кода, а также примеры с константыми ссылками:

```c++
int& foo() {
	static int z = 0;
	return z;
}

int bar() {
	static int z = 0;
	return z;
}

int& bad_foo() {
	int x = 10;
	return x;  // no lifetime extension! dangling reference.
}


...

int x = /* prvalue */ 10;
x;  // lvalue
std::move(x);  // xvalue

// lvalue references, can only bind to lvalue
// lvalue references можно привязать только к lvalue, нельзя к prvalue и к std::move(x).

int &x1 = /* should be lvalue */ ((((x))));
int &x2 = /* should be lvalue */ foo();
// int &x3 = /* prvalue, CE */ bar();
// int &x4 = /* prvalue */ (/* lvalue */ x + /* prvalue */ 10 + /* prvalue */ bar());  // CE
// int &x5 = /* xvalue is rvalue, CE */ std::move(x);


// rvalue references, can only bind to rvalue (xvalue + prvalue)
// Lifetime extension ––> объект будет жить, пока жива ссылка.
// std::move(/* prvalue */) ––> бессмысленно.

int &&y1 = /* prvalue */ 10;
int &&y2 = /* prvalue */ bar();  // lifetime extension: lifetime of the temporary is extended to lifetime of y2
int &&y2b = /* xvalue */ std::move(/* prvalue */ bar());  // no lifetime extension, accessing y2b is UB.
		// One should never move(prvalue).
int &&y3 = /* xvalue */ std::move(x);
// int &&y4 = /* lvalue, CE */ x;

// const lvalue references, can bind anywhere for historical reasons
const int &z1 = /* lvalue */ x;
const int &z2 = /* prvalue */ 10;  // lifetime extension
const int &z3 = /* xvalue */ std::move(x);  // move(x) == static_cast<int&&>(x) (well, almost)
const int &z4 = bar();  // lifetime extension

// Also possible, but mostly useless.
const int &&zz1 = 10;
// const int &&zz2 = /* lvalue, CE */ x1;
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


### Guaranteed copy elision в C++17 (как 'отключить' copy elision, temporary materialization, возврат неперемещаемых объектов из функции)

Давайте также рассмотрим способы избежать (хотя неизвестно зачем, но ладно) copy elision (кстати, чтобы отключить `RVO/NRVO` совсем, можно использовать флаг компиляции `-fno-elide-constructors`):
- Если у нас несколько return выражений в фукнции/методе, тогда `NRVO` скорее всего не сработает:
	```c++
	Bar create_bar_no_nrvo(int x) {
		Bar f, g;
		// probably no copy elision
		if (x == 0) {
			return f;
		} else {
			return g;
		}
	}
	void consume(Bar consume_param) {
		std::cout << "consumed " << &f << "\n";
	}
	...
	int main() {
		consume(create_bar_no_nrvo(0));
	}
	``` 
	```js
	Bar(): 0xc5579ff67f // inside create_bar_no_nrvo: constructed `f`
	Bar(): 0xc5579ff67e // inside create_bar_no_nrvo: constructed `g`
	Bar(Bar&&): 0xc5579ff6cf <- 0xc5579ff67f // create `consume_param` with move constructor and `f`
	~Bar(): 0xc5579ff67e // inside create_bar_no_nrvo: destructed `g`
	~Bar(): 0xc5579ff67f // inside create_bar_no_nrvo: destructed `f`
	consumed 0xc5579ff6cf // `consume_param`
	~Bar(): 0xc5579ff6cf // destructed `consume_param`
	```
- [Temporary materialization](https://en.cppreference.com/w/cpp/language/implicit_conversion#Temporary_materialization) — представляет собой автоматическую конвертацию значения объекта из `prvalue` категории в `xvalue`. 
Рассмотрим ситуации, когда это проявляется:
	```c++
	Bar create_bar_rvo() {
		return Bar();  // copy/move from prvalue is elided (RVO, return value optimization)
	}


	Bar &&pass_ref(Bar &&f) {  // Forces "temporary materialization" by binding a reference, now it's xvalue at best
		std::cout << "pass_ref(Bar&&): arg is " << &f << "\n";
		return std::move(f);
	}

	Bar pass_copy(Bar f) {  // Forces "temporary materialization" by binding a reference inside Bar(Bar&&)
		std::cout << "pass_copy(Bar): arg is " << &f << "\n";
		return f;  // no std::move to enable NRVO
	}

	void consume(Bar f) {
		std::cout << "consumed " << &f << "\n";
	}
	```
	Пример, в котором мы привязали rvalue-ссылку к аргументу функции:
	```c++
	...
	int main() {
		consume(pass_ref(create_bar_rvo()));
	}
	```
	```js
	Bar(): 0x4a7f9ffabf // RVO did its magic
	pass_ref(Bar&&): arg is 0x4a7f9ffabf // here we have the same Bar
	Bar(Bar&&): 0x4a7f9ffabe <- 0x4a7f9ffabf // temporary materialization happened here
	consumed 0x4a7f9ffabe 
	~Bar(): 0x4a7f9ffabe // when we quit all function calls destructors invoke
	~Bar(): 0x4a7f9ffabf
	```
	Пример, в котором мы привязываем ссылку во время вызова move-конструктора (обратите внимание, что temporary materialization возник именно в мувающем конструкторе!):
	```c++
	...
	int main() {
		consume(pass_copy(create_bar_rvo()));
	}
	```
	```js
	Bar(): 0x6bfc9ff65f // RVO magic again
	pass_copy(Bar): arg is 0x6bfc9ff65f // same Bar as on the line before
	Bar(Bar&&): 0x6bfc9ff65e <- 0x6bfc9ff65f // temporary materialization happened here
	consumed 0x6bfc9ff65e 
	~Bar(): 0x6bfc9ff65e // when quit all function calls destructors invoke
	~Bar(): 0x6bfc9ff65f
	``` 
	Более простой пример (на лекциях были те примеры, что я расписал выше, а этот взят из cpprefence), обращение к полям временных объектов:
	```c++
	struct S {
		int m = 0;
	};
	int i = S().m; // member access expects glvalue as of C++17;
									// S() prvalue is converted to xvalue
	```
	Подробный список случаев, в которых возникает temporary materialization, можно найти по ссылке на cppreference (ссылка в самом начале этого буллет-поинта).


Возврат неперемещаемого объекта из функции возможен только копированием или с помощью guaranteed copy elision (частный случай `RVO`).

### Когда (не) надо писать return std::move(foo);

Вообще никогда не надо писать `std::move(local_object)` в качестве возвращаемого значения функции, так как это не позволяет сработать `NRVO`-оптимизации (более подробно [тут](https://habr.com/ru/company/vk/blog/666330/) - та же статья на habr).

Его имеет смысл писать только тогда, когда мы не муваем локальный объект функции/метода, а муваем, например, аргумент, переданный функции как rvalue-ссылка (в примерах кода выше есть такой случай).