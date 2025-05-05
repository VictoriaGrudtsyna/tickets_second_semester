## Билет 60. Множественное наследование

Ссылка на репо практики - `13-211208/01-mi-virtual` - [клик](https://github.com/hse-spb-2021-cpp/lectures/tree/master/13-211208).
<!-- 1 -->
### Синтаксис, пример
Когды мы делаем множес## Билет 43. Множественное наследование

Ссылка на репо практики - `13-211208/01-mi-virtual` - [клик](https://github.com/hse-spb-2021-cpp/lectures/tree/master/13-211208).
<!-- 1 -->
### Синтаксис, пример
Когды мы делаем множественное наследование, у нас (в данном примере) существует два разных `Base` у `Derived`:
```cpp
struct Base {
	int data;
	Base(int data_): data(data_) {}
};

struct X : Base {
	X(): Base(1) {}
	void print() {
		std::cout << "X: " << data << std::endl; 
	}
};

struct Y : Base {
	Y(): Base(-1) {}
	void print() {
		std::cout << "Y: " << data << std::endl; 
	}
};
struct Derived: X, Y {};
/*
   Base         Base
    ^            ^
    |            |
    X            Y
    ^            ^
     \          /
        \    /
       Derived
*/
int main() {
	Derived d;
	// d.print(); // CE - print() is ambiguous
	d.X::print(); // X: -1
	d.Y::print(); // Y: 1
}
```
<!-- 2 -->
### Возможное представление в памяти, пример изменения адреса при static_cast (потому что начало подобъект теперь не всегда совпадает с началом объекта)
- Возможное представление в памяти выглядит следующим образом:
```cpp
// Layout is guaranteed, exact sizes and padding vary:
+---------------------------------------------------------+
| +---------------------+ +----------------------+        |
| | +------------+      | | +------------+       |        |
| | | Base       |      | | | Base       |       |        |
| | | +data      |      | | | +data      |       |        |
| | +------------+      | | +------------+       |        |
| |   X                 | |   Y                  |        |
| |                     | |                      |        |
| +---------------------+ +----------------------+        |
| Derived                                                 |
|                                                         |
+---------------------------------------------------------+
```
- Пример изменения адреса при `static_cast`: при касте к `X` все будет как раньше, при касте к `Y` сдвиг на нужный адрес.
```cpp
struct Base {
	int data = 10;
};

struct X : Base {};
struct Y : Base {};

struct Derived : X, Y {};

int main() {
	Derived d;
	X& x = static_cast<X&>(d);
	std::cout << &x << ' ' << &d << std::endl;
	// 0x7ffcb09b8530 0x7ffcb09b8530

	Y& y = static_cast<Y&>(d);
	// Тут отступ на размер int, так как в Derived X и Y лежат один за другим в таком порядке 
	std::cout << &y << ' ' << &d << std::endl;
	// 0x7ffcb09b8534 0x7ffcb09b8530
}
```
<!-- 3 -->
### Порядок инициализации/уничтожения подобъектов и полей, как передать параметры конструкторам
- Инициализация: при инциализации у нас вначале вызовутся конструторы родительских классов (в порядке перечисления наследования), затем наш конструтор. Чтобы передать параметры в конструкторы родителей, мы передаем их в наш конструктор, а в его списке инциализации членов (member initialization list) мы вызываем нужные конструкторы родителей.
- Удаление: вначале вызывается наш деструтор, внутри он вызывает дескрукторы родителей.
```cpp
struct Base {
	int data;
	Base(int data_): data(data_) { std::cout << "Base()\n"; }
	~Base() { std::cout << "~Base()\n"; }
};
struct X : Base {
	X(): Base(1) { std::cout << "X()\n"; }
	~X() { std::cout << "~X()\n"; }
};
struct Y : Base {
	Y(): Base(-1) { std::cout << "Y()\n"; }
	~Y() { std::cout << "~Y()\n"; }
};
struct Derived: Y, X {
	Derived() { std::cout << "Derived()\n"; }
	~Derived() { std::cout << "~Derived()\n"; }
};

int main() {
	Derived d;
}
/*
Base()
Y()
Base()
X()
Derived()
~Derived()
~X()
~Base()
~Y()
~Base()
*/
```
<!-- 4 -->
### Возможное дублирование базового класса и возникающие неоднозначности при приведении типа
Если мы пытаемся скастовать класс, у которого имеется дубликаты среди родителей, то мы получим CE (все классы в примере кода имееют ту же структуру, что и в примерах выше): 
```cpp
/*
   Base         Base
    ^            ^
    |            |
    X            Y
    ^            ^
     \          /
        \    /
       Derived
*/
...
Derived d;
Base& b = static_cast<Base&> (d); // CE - ‘Base’ is an ambiguous base of ‘Derived’
X& x = static_cast<X&> (d); // OK
Y& y = static_cast<Y&> (d); // OK
...
```
<!-- 5 -->
### Cross-cast (он же side-cast, `13-211208`/`01-mi-virtual`/`25-side-cast.cpp`)
Ссылка на код упражнения - [тык](https://github.com/hse-spb-2021-cpp/lectures/blob/master/13-211208/01-mi-virtual/25-side-cast.cpp).

Основная идея в том, что если у нас базовый класс полиморфный (равносильно тому, что есть хотя бы один виртуальный метод), то с помощью `dynamic_cast<...>()` мы можем получить ссылку на своего брата:
```cpp
struct Base1 { virtual ~Base1() {} };
struct Base2 { virtual ~Base2() {} };
struct Base3 { virtual ~Base3() {} };

struct Derived1 : virtual Base1 {};
struct Derived12 : virtual Base1, virtual Base2 {};
struct Derived23 : virtual Base2, virtual Base3 {};
struct Derived3 : virtual Base3 {};

struct DerivedX : Derived1, Derived12, Derived23, Derived3 {};
/*
  B1  B2  B3
  /\  /\  /\
 /  \/  \/  \
D1  12  23  D3
  \  \  /   /
   --\\//---
      DX
*/
...
void foo(Derived1 &d1) {
	// NOTE: 'virtual' inheritance is not important here.
	std::cout << dynamic_cast<Derived3*>(&d1) << std::endl;
}
...
DerivedX d;
foo(d); // 0x7ffc4733fa68 - some memory address
```
Что тут произошло: в `foo()` мы передали объект типа `DerivedX`, при передачи параметра случился каст к родителю (к `Derived1`), после чего мы с помощью `dynamic_cast` снова перешли в `DerivedX` и уже у него поискали `Derived3`.
<!-- 6 -->
### Как теперь работают dynamic_cast/static_cast/неявное преобразование/override/hiding/приватное и защищённое наследование
- `dynamic_cast`: можем кастовать классы по иерархии, но при этом базовый класс обязан быть полиморфным.
- `static_cast`: если используем обычное множественное наследование то все ок. Если используем виртуальное наследование, то можно делать `static_cast` от ребенка к родителю, но мы не можем сделать `static_cast` в обратную сторону, поскольку представление объектов в памяти будет отличаться в зависимости от иерархии наследования, и мы просто не сможем понять, кем на самом деле являемся:
```cpp
/*
   Base
   ^  ^
  /    \
 /      \
X        Y
^        ^
 \      /
  \    /
 Derived
*/
struct Base {
	int base_data = 0;
	virtual ~Base() {}
};
struct X : virtual Base { int x_data = 0; };
struct Y : virtual Base { int y_data = 0; };
struct Derived : X, Y { };

int main() {
	Derived d1;
	X &x1 = static_cast<X&>(d1); // статический каст к родителю - OK
	Base &b1 = d1; // неявное преобразование к родителю - OK
	// d1: (baseptr x_data) (baseptr y_data) base_data

	X x2;
	Base &b2 = x2; // static_cast<Base&>(x2); is also ok
	// x2: (baseptr x_data) base_data

	dynamic_cast<X*>(&b1); // OK
	static_cast<X&>(b1);   // CE - cannot convert from pointer to base class ‘Base’ to pointer to derived class ‘X’ because the base is virtual
}
``` 
- Неявное преобразование: тут рили хз, что писать (на лекциях такого отдельно не проговаривали)
- override: Если мы наследуемся от двух классов, у которых есть виртуальные функции с одинаковой сигнатурой, то при перезаписи мы перезапишем оба метода (смотреть пример) - [ссылка на упражнение с кодом](https://github.com/hse-spb-2021-cpp/lectures/blob/master/13-211208/01-mi-virtual/02-override-independent.cpp).  
- hiding: 
	- перекрывает методы с таким же названием (не сигнатурой) у родителей:
	```cpp
	struct Base1 {
		void foo(int) {
			std::cout << "foo(int)\n";
		}
	};
	struct Base2 {
		void foo(double) {
			std::cout << "foo(double)\n";
		}
		void foo() {
			std::cout << "foo()\n";
		}
	};
	struct Derived : Base1, Base2 {
		void foo(char) {
			std::cout << "foo(char)\n";
		}
	};

	int main() {
		Derived d;
		d.foo(1.1); // foo(char)
	}
	```

	- Возникает неоднозначность, если в нескольких родителях есть методы с одинаковым именем:
	```cpp
	// Order:
	// 1. Name resolution. Output: "overload set".
	// 2. Overload resolution. Output: a single overload.
	// 3. Access check.
	// 4. Call, can be virtual or non-virtual.
	struct Base1 {
		void foo(int) {
			std::cout << "foo(int)\n";
		}
	};
	struct Base2 {
		void foo(double) {
			std::cout << "foo(double)\n";
		}
		void foo() {
			std::cout << "foo()\n";
		}
	};
	struct Derived : Base1, Base2 {};

	int main() {
		Derived d;
		d.foo(1.1);
	}
	``` 
	- При все при этом можем использовать `using` - никто не запрещает, также если все перегрузки лежали в одном из родителей, то все скомпилируется без проблем и разрешится корректно.
- Приватное и защищённое наследование: квалификаторы перечисляются вместе с родительскими классами: `struct Class: public Base1, private Base2, protected Base3 {};`. В целом тут все очев, но есть локальные приколы, [например](https://github.com/hse-spb-2021-cpp/lectures/blob/master/13-211208/01-mi-virtual/21-most-accessible-path.cpp):
```cpp
struct Base {
	int data;
};
struct X : private virtual Base {};
struct Y : public virtual Base {};
struct X1 : X {
	void foo() {
		data = 10;  // access through X, private - bad, you'll suck a ****!!!
	}
};

struct Derived : X, Y {
	void foo() {
		data = 10;  // access is public through Y
		Y::data = 20;  // accessible as well, not surprising
		X::data = 30;  // accessible as well, wow
	}
};
```
В этом примере, так как наследование виртуальное, то у нас один экземпляр `Base`, поэтому у нас есть доступ к его полям через `Y`, который наследуется от `Base` публично. 

#### В том числе при наличии одинаковых виртуальных функций в независимых базах
При наличии независимых баз с одинаковыми виртуальными методами при перезаписывании мы перекроем сразу оба:
```cpp
struct Base1 {
	virtual void foo() {
		std::cout << "Base1: foo()\n";
	}
};

struct Base2 {
	virtual void foo() {
		std::cout << "Base2: foo()\n";
	}
};

struct Derived : Base1, Base2 {
	void foo() override {
		std::cout << "Derived: foo()\n";
	}
};


int main() {
	Derived d;
	d.foo(); // Derived: foo()

	Base1 &b1 = d;
	b1.foo(); // Derived: foo()

	Base1 &b2 = d;
	b2.foo(); // Derived: foo()
}
```
твенное наследование, у нас (в данном примере) существует два разных `Base` у `Derived`:
```cpp
struct Base {
	int data;
	Base(int data_): data(data_) {}
};

struct X : Base {
	X(): Base(1) {}
	void print() {
		std::cout << "X: " << data << std::endl; 
	}
};

struct Y : Base {
	Y(): Base(-1) {}
	void print() {
		std::cout << "Y: " << data << std::endl; 
	}
};
struct Derived: X, Y {};
/*
   Base         Base
    ^            ^
    |            |
    X            Y
    ^            ^
     \          /
        \    /
       Derived
*/
int main() {
	Derived d;
	// d.print(); // CE - print() is ambiguous
	d.X::print(); // X: -1
	d.Y::print(); // Y: 1
}
```
<!-- 2 -->
### Возможное представление в памяти, пример изменения адреса при static_cast (потому что начало подобъект теперь не всегда совпадает с началом объекта)
- Возможное представление в памяти выглядит следующим образом:
```cpp
// Layout is guaranteed, exact sizes and padding vary:
+---------------------------------------------------------+
| +---------------------+ +----------------------+        |
| | +------------+      | | +------------+       |        |
| | | Base       |      | | | Base       |       |        |
| | | +data      |      | | | +data      |       |        |
| | +------------+      | | +------------+       |        |
| |   X                 | |   Y                  |        |
| |                     | |                      |        |
| +---------------------+ +----------------------+        |
| Derived                                                 |
|                                                         |
+---------------------------------------------------------+
```
- Пример изменения адреса при `static_cast`: при касте к `X` все будет как раньше, при касте к `Y` сдвиг на нужный адрес.
```cpp
struct Base {
	int data = 10;
};

struct X : Base {};
struct Y : Base {};

struct Derived : X, Y {};

int main() {
	Derived d;
	X& x = static_cast<X&>(d);
	std::cout << &x << ' ' << &d << std::endl;
	// 0x7ffcb09b8530 0x7ffcb09b8530

	Y& y = static_cast<Y&>(d);
	// Тут отступ на размер int, так как в Derived X и Y лежат один за другим в таком порядке 
	std::cout << &y << ' ' << &d << std::endl;
	// 0x7ffcb09b8534 0x7ffcb09b8530
}
```
<!-- 3 -->
### Порядок инициализации/уничтожения подобъектов и полей, как передать параметры конструкторам
- Инициализация: при инциализации у нас вначале вызовутся конструторы родительских классов (в порядке перечисления наследования), затем наш конструтор. Чтобы передать параметры в конструкторы родителей, мы передаем их в наш конструктор, а в его списке инциализации членов (member initialization list) мы вызываем нужные конструкторы родителей.
- Удаление: вначале вызывается наш деструтор, внутри он вызывает дескрукторы родителей.
```cpp
struct Base {
	int data;
	Base(int data_): data(data_) { std::cout << "Base()\n"; }
	~Base() { std::cout << "~Base()\n"; }
};
struct X : Base {
	X(): Base(1) { std::cout << "X()\n"; }
	~X() { std::cout << "~X()\n"; }
};
struct Y : Base {
	Y(): Base(-1) { std::cout << "Y()\n"; }
	~Y() { std::cout << "~Y()\n"; }
};
struct Derived: Y, X {
	Derived() { std::cout << "Derived()\n"; }
	~Derived() { std::cout << "~Derived()\n"; }
};

int main() {
	Derived d;
}
/*
Base()
Y()
Base()
X()
Derived()
~Derived()
~X()
~Base()
~Y()
~Base()
*/
```
<!-- 4 -->
### Возможное дублирование базового класса и возникающие неоднозначности при приведении типа
Если мы пытаемся скастовать класс, у которого имеется дубликаты среди родителей, то мы получим CE (все классы в примере кода имееют ту же структуру, что и в примерах выше): 
```cpp
/*
   Base         Base
    ^            ^
    |            |
    X            Y
    ^            ^
     \          /
        \    /
       Derived
*/
...
Derived d;
Base& b = static_cast<Base&> (d); // CE - ‘Base’ is an ambiguous base of ‘Derived’
X& x = static_cast<X&> (d); // OK
Y& y = static_cast<Y&> (d); // OK
...
```
<!-- 5 -->
### Cross-cast (он же side-cast, `13-211208`/`01-mi-virtual`/`25-side-cast.cpp`)
Ссылка на код упражнения - [тык](https://github.com/hse-spb-2021-cpp/lectures/blob/master/13-211208/01-mi-virtual/25-side-cast.cpp).

Основная идея в том, что если у нас базовый класс полиморфный (равносильно тому, что есть хотя бы один виртуальный метод), то с помощью `dynamic_cast<...>()` мы можем получить ссылку на своего брата:
```cpp
struct Base1 { virtual ~Base1() {} };
struct Base2 { virtual ~Base2() {} };
struct Base3 { virtual ~Base3() {} };

struct Derived1 : virtual Base1 {};
struct Derived12 : virtual Base1, virtual Base2 {};
struct Derived23 : virtual Base2, virtual Base3 {};
struct Derived3 : virtual Base3 {};

struct DerivedX : Derived1, Derived12, Derived23, Derived3 {};
/*
  B1  B2  B3
  /\  /\  /\
 /  \/  \/  \
D1  12  23  D3
  \  \  /   /
   --\\//---
      DX
*/
...
void foo(Derived1 &d1) {
	// NOTE: 'virtual' inheritance is not important here.
	std::cout << dynamic_cast<Derived3*>(&d1) << std::endl;
}
...
DerivedX d;
foo(d); // 0x7ffc4733fa68 - some memory address
```
Что тут произошло: в `foo()` мы передали объект типа `DerivedX`, при передачи параметра случился каст к родителю (к `Derived1`), после чего мы с помощью `dynamic_cast` снова перешли в `DerivedX` и уже у него поискали `Derived3`.
<!-- 6 -->
### Как теперь работают dynamic_cast/static_cast/неявное преобразование/override/hiding/приватное и защищённое наследование
- `dynamic_cast`: можем кастовать классы по иерархии, но при этом базовый класс обязан быть полиморфным.
- `static_cast`: если используем обычное множественное наследование то все ок. Если используем виртуальное наследование, то можно делать `static_cast` от ребенка к родителю, но мы не можем сделать `static_cast` в обратную сторону, поскольку представление объектов в памяти будет отличаться в зависимости от иерархии наследования, и мы просто не сможем понять, кем на самом деле являемся:
```cpp
/*
   Base
   ^  ^
  /    \
 /      \
X        Y
^        ^
 \      /
  \    /
 Derived
*/
struct Base {
	int base_data = 0;
	virtual ~Base() {}
};
struct X : virtual Base { int x_data = 0; };
struct Y : virtual Base { int y_data = 0; };
struct Derived : X, Y { };

int main() {
	Derived d1;
	X &x1 = static_cast<X&>(d1); // статический каст к родителю - OK
	Base &b1 = d1; // неявное преобразование к родителю - OK
	// d1: (baseptr x_data) (baseptr y_data) base_data

	X x2;
	Base &b2 = x2; // static_cast<Base&>(x2); is also ok
	// x2: (baseptr x_data) base_data

	dynamic_cast<X*>(&b1); // OK
	static_cast<X&>(b1);   // CE - cannot convert from pointer to base class ‘Base’ to pointer to derived class ‘X’ because the base is virtual
}
``` 
- Неявное преобразование: тут рили хз, что писать (на лекциях такого отдельно не проговаривали)
- override: Если мы наследуемся от двух классов, у которых есть виртуальные функции с одинаковой сигнатурой, то при перезаписи мы перезапишем оба метода (смотреть пример) - [ссылка на упражнение с кодом](https://github.com/hse-spb-2021-cpp/lectures/blob/master/13-211208/01-mi-virtual/02-override-independent.cpp).  
- hiding: 
	- перекрывает методы с таким же названием (не сигнатурой) у родителей:
	```cpp
	struct Base1 {
		void foo(int) {
			std::cout << "foo(int)\n";
		}
	};
	struct Base2 {
		void foo(double) {
			std::cout << "foo(double)\n";
		}
		void foo() {
			std::cout << "foo()\n";
		}
	};
	struct Derived : Base1, Base2 {
		void foo(char) {
			std::cout << "foo(char)\n";
		}
	};

	int main() {
		Derived d;
		d.foo(1.1); // foo(char)
	}
	```

	- Возникает неоднозначность, если в нескольких родителях есть методы с одинаковым именем:
	```cpp
	// Order:
	// 1. Name resolution. Output: "overload set".
	// 2. Overload resolution. Output: a single overload.
	// 3. Access check.
	// 4. Call, can be virtual or non-virtual.
	struct Base1 {
		void foo(int) {
			std::cout << "foo(int)\n";
		}
	};
	struct Base2 {
		void foo(double) {
			std::cout << "foo(double)\n";
		}
		void foo() {
			std::cout << "foo()\n";
		}
	};
	struct Derived : Base1, Base2 {};

	int main() {
		Derived d;
		d.foo(1.1);
	}
	``` 
	- При все при этом можем использовать `using` - никто не запрещает, также если все перегрузки лежали в одном из родителей, то все скомпилируется без проблем и разрешится корректно.
- Приватное и защищённое наследование: квалификаторы перечисляются вместе с родительскими классами: `struct Class: public Base1, private Base2, protected Base3 {};`. В целом тут все очев, но есть локальные приколы, [например](https://github.com/hse-spb-2021-cpp/lectures/blob/master/13-211208/01-mi-virtual/21-most-accessible-path.cpp):
```cpp
struct Base {
	int data;
};
struct X : private virtual Base {};
struct Y : public virtual Base {};
struct X1 : X {
	void foo() {
		data = 10;  // access through X, private - bad, you'll suck a ****!!!
	}
};

struct Derived : X, Y {
	void foo() {
		data = 10;  // access is public through Y
		Y::data = 20;  // accessible as well, not surprising
		X::data = 30;  // accessible as well, wow
	}
};
```
В этом примере, так как наследование виртуальное, то у нас один экземпляр `Base`, поэтому у нас есть доступ к его полям через `Y`, который наследуется от `Base` публично. 

#### В том числе при наличии одинаковых виртуальных функций в независимых базах
При наличии независимых баз с одинаковыми виртуальными методами при перезаписывании мы перекроем сразу оба:
```cpp
struct Base1 {
	virtual void foo() {
		std::cout << "Base1: foo()\n";
	}
};

struct Base2 {
	virtual void foo() {
		std::cout << "Base2: foo()\n";
	}
};

struct Derived : Base1, Base2 {
	void foo() override {
		std::cout << "Derived: foo()\n";
	}
};


int main() {
	Derived d;
	d.foo(); // Derived: foo()

	Base1 &b1 = d;
	b1.foo(); // Derived: foo()

	Base1 &b2 = d;
	b2.foo(); // Derived: foo()
}
```
