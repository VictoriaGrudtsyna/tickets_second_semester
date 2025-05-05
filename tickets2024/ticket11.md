## Билет 11. Реализация своего `std::vector`:
23.md 22.md
<!-- * Placement new, выравнивание (`alignas`/`alignof`), явный вызов деструктора
* Время жизни объектов (lifetime) и недопустимость операций за пределами lifetime
* Ref-qualifiers: `&`, `&&`, перегрузка по ним и по const-qualifier
* Перегрузка `operator[]`
* Своя реализация `std::vector`
    * Перегрузка `push_back` для `const T&` и `T&&` как альтернатива perfect forwarding; отличие от принятия по константной ссылке или по значению
    * Использование `const_cast` для уменьшения дублирования кода (`27.5-220519/01-c-cpp-extern/02-const-cast/in-vector.cpp`)
    * Трудности с обеспечением базовой гарантии исключений при создании массива объектов (практика `30-220606`, задания `0*-minivector*`) -->



### Placement new, выравнивание (`alignas/alignof`), явный вызов деструктора

1. Placement new:

Нужно разделять моменты выделения памяти для объекта (allocation) и момент создания объекта в выделенной памяти (creation). 

Placement new - это процесс создания объекта в уже выделенной (и правильно выравненной) для него памяти.

Синтаксис: `new ([pointer to buffer]) T(...)`.

Пример:

```cpp
#include <iostream>
#include <memory> // placement new lies here
#include <vector>

struct Foo {
    std::vector<int> v = std::vector{1, 2, 3};
    Foo() {
        std::cout << "Constructed at " << this << std::endl;
    }
    ~Foo() {
        std::cout << "Destructed at " << this << std::endl;
    }
};

int main() {
	// 'unsigned char' is required, not 'char', because UB might happen
    alignas(alignof(Foo)) unsigned char data[sizeof(Foo)];
    std::cout << "Got memory, size=" << sizeof(data) << ", location=" << &data << std::endl;

    // We separate "memory allocation" from "object creation".
    // Previously `new T` did both. Now we only need it to call constructor.

    Foo *f = new (data) Foo();  // Best practice.
    
	// Also possible, but requires that `data` is either `unsigned char` or `std::byte`, not `char`, IIUC.
    // new (data) Foo();
    // Foo *f = reinterpret_cast<Foo*>(data);

    std::cout << f->v.size() << std::endl;
    std::cout << "bytes:";
    for (unsigned char c : data) {
        std::cout << ' ' << static_cast<int>(c);
    }
    std::cout << std::endl;

    f->~Foo();  // Explicit destructor call/pseudodestructor. No memory deallocation, so it can be any memory.
    // delete f;  // Destructor + memory deallocation.
}
```

При этом важно, что мы сами удалаяем объект `Foo`: не смотря на то, что память для `data` выделена на стеке (т.е. автоматически почистится), объект, созданный в данном буфере, удалятся сам не будет (если не удалим - UB). Также нельзя вызывать `delete f`, потому что будет вызываться и удаление объекта, и очистка памяти.

Например, в векторе: мы имеем буфер размера `cap`, и кол-во уже созданных элементов у нас `n` (пусть `n < cap`), тогда при вызове деструктора мы обязаны пройтись по созданным элементам верктора и самостоятельно удалить их (пример условный):

```cpp
template<typename T>
struct vector {
	T* data;
	int n;
	int cap;
	...

	~vector() {
		for(int i = n - 1; i >= 0; i--) {
			data[i].~T();
		}
		Alloc().deallocate(data, cap);
	}

	...
};
```

2. Выравнивание (`alignas/alignof`):

[Информация на cppreference](https://en.cppreference.com/w/cpp/language/alignas).

**Выравнивание** - то, на сколько должен делиться адрес объекта в памяти для корректной работы.

`alignas` - устанавливает выравнивание для структуры. Если указанное значение выравнивания **меньше, чем необходимое** для структуры, т.е. дефолтное, то в данном случае программа считается `ill-formed`.

Если надо установить специфичное выравнивание, то используйте:

```cpp
#pragma pack(push, [desired alignment])

struct Foo {
	int a;
	int b;
};

#pragma pack(pop)
```

В данном примере `ill-formed`:

```cpp 
struct alignas(8) S {};
struct alignas(1) U { S s; }; // error: alignment of U would have been 8 without alignas(1)
```


Далее пример в связке с placement new.

Заметим, что выравнивание (`alignas(alignof(Foo))`) необходимо для того, чтобы правильно хранить структуру Foo, так как некоторые процессоры не умеют обращаться по неправильно выравненным адресам или делают это в 3-4 раза дольше.  


```cpp
struct Foo {
	int x;
	int y;
};
 
// 1
int main() {
	// Выравнивание необходимо для того, чтобы правильно хранить структуру Foo
	alignas(alignof(Foo)) unsigned char data[sizeof(Foo)];
	
	Foo *f = new (data) Foo();

	f->~Foo();

	return 0;
}

// 2
int main() {
	alignas(alignof(Foo)) unsigned char data[sizeof(Foo) * 3];
	
	Foo *f1 = new (data) Foo();
	Foo *f2 = new (data + 1) Foo();
	Foo *f3 = new (data + 2) Foo();

	// order does not matter, but good practice to mimic automatic storage duration destruction order
	f3->~Foo();
	f2->~Foo();
	f1->~Foo();

	return 0;
}

`alignof` - возвращает выравнивание в байтах, необходимое для переданного `type-id`.
Синтаксис: `alignof(int), alignof(Foo)`.

```

3. Явный вызов деструктора:

Как сказано ранее, явный вызов деструктора необходим, так как мы создали объект в уже заалоцированном буфере, поэтому на нас лежит ответственность за его удаление (самостоятельно он не вызовется).

## Время жизни объектов (lifetime) и недопустимость операций за пределами lifetime

Рассмотрим пример (обратите внимание на `const int id`):

```cpp
#include <iostream>
#include <memory>
#include <string>

struct Person {
    std::string first_name;
    const int id;

    Person(int id_) : first_name(10'000, 'x'), id(id_) {
        std::cout << "Person created\n";
    }
};

int main() {
    Person *buf = std::allocator<Person>().allocate(4);

    Person &a = *new (buf) Person(10);
    std::cout << a.id << std::endl;  // 10								(1)
    std::cout << buf[0].id << std::endl;  // 10? I dunno :(				(2)
    a.~Person();

    Person &b = *new (buf) Person(11);
    std::cout << &a << " " << &b << std::endl;
    std::cout << b.id << std::endl;  // ok.
    std::cout << a.id << std::endl;  // 11? UB? I dunno :(				(3)
    std::cout << buf[0].id << std::endl;  // 11? UB? I dunno :(			(4)
    std::cout << std::launder(&buf[0])->id << std::endl;  // 11. Probably not UB.
    b.~Person();

    std::allocator<Person>().deallocate(buf, 4);
	
	return 0;
}
```

1. Обратим внимание на выражения `(1)` и `(3)`: корректно ли использовать ту же ссылку для обращения к первому элементу, если мы сначала создали объект, привязали к нему ссылку, далее удалили объект и в том же месте создали новый? Заметим, что у нас есть `const` полу `id`, оно никогда не меняется в течение жизни объекта, поэтому может произойти так, что компилятор выполнит оптимизации вместо `a.id` всегда будет выводить число 10 (его ведь нельзя менять, значит и обращаться по ссылке в память лишний раз не надо). Из-за этого, после удаления первого и создания второго объектов, может получиться, что по обращению `a.id` мы будем получать все также 10, а не 11. Это потенциальное UB, хотя сам Егор до конца не уверен (на лекции у него все нормально работало: выводилось 11). Подобным образом лучше никогда не писать во издежании сложноотлавимых ошибок.

2. В `(2)` и `(4)` такая же история: мы по одной и той же ссылке (`buf[0]`) обращается к куску памяти, но в котором в разные моменты времени лежат разные объекты `Person`. Идея та же: что если выполнится оптимизация и мы в `(4)` получим 10? Для таких случаев используют `std::launder` - он "отмывает" указатель и заставляет каким-то образом компилятор сделать обращение по ссылке для получения реального значения.


Также всегда, когда в векторе происходит перевыделение памяти и перемещение данных в этот новый кусок памяти, то все созданные ранее ссылки и итераторы/указатели на элементы вектора считаются инвалидированными, и обращение по ним - UB.  
 


### Ref-qualifiers: `&`, `&&`, перегрузка по ним и по `const-qualifier`

1. `&` - метод можно вызывать только на `lvalue` (т.е. невременных) значениях.

2. `const&` - метод вызывается на константных невременных объектах, однако вспомнил про lifetime extention: здесь мы все равно, что создаем константную ссылку на объект, поэтому `const&` может привязаться к любому типу. 

3. `&&` - метод можно вызывать только на `rvalue` (т.е. временных) значениях.

Примеры:

```cpp
#include <iostream>
#include <utility>

using namespace std;


struct Foo {
	void print() & {
		std::cout << "&" << std::endl;
	};

	void print() const& {
		std::cout << "const&" << std::endl;
	};

	void print() && {
		std::cout << "&&" << std::endl;
	};
};


int main() {
	Foo f;
	
	f.print(); // &

	std::as_const(f).print(); // const&

	std::move(f).print(); // &&

	Foo().print(); // &&

	return 0;
}
```

Теперь уберем квалификатор `&&`:

```cpp
#include <iostream>
#include <utility>

using namespace std;


struct Foo {
	void print() & {
		std::cout << "&" << std::endl;
	};

	void print() const& {
		std::cout << "const&" << std::endl;
	};
};


int main() {
	Foo f;
	
	f.print(); // &

	std::as_const(f).print(); // const&

	std::move(f).print(); // const&

	Foo().print(); // const&

	return 0;
}
```

### Перегрузка operator[]:

Хотим иметь `operator[]` в нескольких версиях: для константных, `lvalue` и `xvalue` объектов:

```cpp
template<typename T>
struct vector {

	...

	T &operator[](std::size_t index) &noexcept {
		return m_data[index];
	}

	const T &operator[](std::size_t index) const &noexcept {
		return m_data[index];
	}

	T &&operator[](std::size_t index) &&noexcept {
		return std::move(m_data[index]);
	}

	...

};

int main() {
	vector f;
	
	f[0]; // &

	std::as_const(f)[0]; // const&

	std::move(f)[0]; // &&

	vector<int>()[0]; // &&
}
```


### Своя реализация `std::vector`:


#### Перегрузка `push_back` для `const T&` и `T&&` как альтернатива `perfect forwarding`; отличие от принятия по константной ссылке или по значению:

`perfect forwarding` - это пробрасывание аргументов в функцию с полным сохранением `value`-типов аргументов. Так выглядит паттерн ([`perfect forwarding`](https://en.cppreference.com/w/cpp/utility/forward)):

```cpp
template<class T>
void wrapper(T&& arg) 
{
    // arg is always lvalue
    foo(std::forward<T>(arg)); // Forward as lvalue or as rvalue, depending on T
}
```

Мы также хотим, чтобы `value`-типы аргумента `push_back` сохранился, так мы сможем сделать быстрый `std::move()` вместо копирования, если `push_back` будет вызван от временного объекта:

```cpp
void push_back(const T &elem) & {
	// 1. resize vector if needed
	new (m_data + m_size) T(elem);
	++m_size;
}

void push_back(T &&elem) & {
	// 1. resize vector if needed
	new (m_data + m_size) T(std::move(elem));
	++m_size;
}
```

Если делать перегрузку только от `const T&`, то нам всегда пришлось бы делать копирование в `T(elem)`, или же мы бы вообще не смогли сделать такой вызов, в случае если `ctor` копирования у `T` удален.

Если добавлять перегрузку от `T`, то мы могли бы сделать `T(std::move(elem))`, однако мы все равно вынуждены делать копирование + если `ctor` копирования удален, то мы бы не смогли вызвать данную перегрузку.

Небольшой пример с вызовами:

```cpp
#include <iostream>
#include <utility>

using namespace std;


template<typename T>
struct vector {
	void push_back(const T& elem) & {
		std::cout << "const T&" << std::endl;
		// here we would call 'new (data) T(elem)'
	}

	void push_back(T&& elem) & {
		std::cout << "T&&" << std::endl;
		// here we would call 'new (data) T(std::move(elem))'
	}
};


struct Foo {
	int x;
	int y;
};


int main() {
	vector<Foo> vec;
	Foo elem;

	vec.push_back(elem); // const T&
	vec.push_back(std::as_const(elem)); // const T&
	vec.push_back(std::move(elem)); // T&&
	vec.push_back(Foo()); // T&&

	return 0;
}
```


#### Использование `const_cast` для уменьшения дублирования кода `(27.5-220519/01-c-cpp-extern/02-const-cast/in-vector.cpp)`:

Идея: мы честно пишем оператор `const T& operator[](std::size_t i) const` (заметьте, что он `const-qualified`), далее в неконстантной версии мы вызываем на себе `const T& operator[]`, используя выражение `std::as_const(*this)[i]`. После вызова, так как мы точно знаем, что `this` - неконстантный объект, то можем смело делать `const_cast<T&>` (т.е. отбросить константность) и вернуть уже неконстантную ссылку на элемент под идексом `i`:

```cpp
#include <cstddef>
#include <utility>

// https://stackoverflow.com/a/123995/767632

template<typename T>
struct my_vector {
    T *data;

    const T &operator[](std::size_t i) const {
        return *(data + i);
    }

    T &operator[](std::size_t i) {
        // OK: we know that `this` is actually non-const.
        return const_cast<T&>(std::as_const(*this)[i]);
    }
};

```

Важно, что мы выражаем не-`const` через `const` метод, иначе у нас нет гарантии, что не-`const` метод не меняет объект:

```cpp
template<typename T>
struct my_vector_bad {
    T *data;

    const T &operator[](std::size_t i) const {
        // More dangerous: what if `this` is actually const and `operator[]` actually changes it?
        // Not UB, though.
        return const_cast<my_vector_bad*>(*this)[i];
    }
    T &operator[](std::size_t i) {
        return *(data + i);
    }
};
```


#### Трудности с обеспечением базовой гарантии исключений при создании массива объектов (практика `30-220606`, задания `0*-minivector*`):

Реализация [`minivector` с практики](https://github.com/hse-spb-2021-cpp/exercises/blob/master/21-220228/solution/minivector.h).


Оператор копирующего присваивания `minivector& operator=(const minivector&)` самый сложный в реализации:
	
1. Нужно всегда проверять самоприсвивания.

2. Если мы сначал удалим данные, а затем выделим буфер для новых, чтобы сделать копирование, и при выделении у нас вылетело исключение, то мы испортим объект, поэтому необходимо создавать локальную переменную-указатель, и в нее присваивать указатель на выделенную память.
<!-- Если исключение при выделении вылетело, то мы удалить свой буфер и указать `data = nullptr, len = 0` - так мы поддрежим базовую гарантию. -->

3. Можно воспользоваться `copy-and-swap idiom`: теперь принимаем объект по значению (`minivector& operator=(minivector)`) и делаем `swap` себя и данного объекта.

Реализация базовой гарантии ([Код с лекции](https://github.com/hse-spb-2021-cpp/lectures/blob/master/30-220606/03-more-exceptions/21-assignment-strong-safety.cpp)):

```cpp

#include <cstddef>
#include <utility>

struct minivector {
private:
    int *data = nullptr;
    std::size_t len = 0;

public:
    minivector(std::size_t len_) : data(new int[len_]{}), len(len_) {}

    minivector(minivector &&other) : data(std::exchange(other.data, nullptr)), len(std::exchange(other.len, 0)) {}
    
	minivector(const minivector &other) : minivector(other.len) {
        for (std::size_t i = 0; i < len; i++) {
            data[i] = other.data[i];
        }
    }
    
	~minivector() {
        delete[] data;
    }

    // swap is not needed if we have `operator=(minivector&&)`
    minivector &operator=(minivector &&other) {
        std::swap(data, other.data);
        std::swap(len, other.len);
        return *this;
    }

    minivector &operator=(const minivector &other) {
        if (this == &other) {  // Needed, otherwise we delete data.
            return *this;
        }
        if (len != other.len) {
            int *new_data = new int[other.len]; // allocating first before deleting data
            delete[] data;
            data = new_data;
        }
        for (std::size_t i = 0; i < other.len; i++) {
            data[i] = other.data[i];
        }
        return *this;
    }
};

int main() {
    minivector a(5), b(10);
    a = b;
}
```

`Copy-and-swap idiom` реалиция ([Код с лекции](https://github.com/hse-spb-2021-cpp/lectures/blob/master/30-220606/03-more-exceptions/22-copy-and-swap-idiom.cpp)):

```cpp
struct minivector {

	...

    friend void swap(minivector &a, minivector &b) noexcept {
        std::swap(a.data, b.data);
        std::swap(a.len, b.len);
    }

    // copy-and-swap idiom is about making `operator=` strong exception safe AND simple
    minivector &operator=(minivector other) {  // copy or move to the temporary
        // swap with the temporary via our convenience method
        swap(*this, other);  // we can also write it in-place, but then std::swap will be inefficient
        // std::swap(*this, other);  // do not work, as it calls `operator=`
        return *this;
        // destroy the temporary
        // the only trouble: self-assignment is now not cheap
    }

	...

};
```


Также можем поддержать строгую гарантию для `push_back` путем создания временного объекта в функции и копирования себя у него с добавлением нового элемента, как не странно, это очень дорого ([Код с лекции](https://github.com/hse-spb-2021-cpp/lectures/blob/master/30-220606/03-more-exceptions/23-push-back-strong-safety.cpp)):

```cpp
struct minivector {

	...

    minivector &operator=(minivector other) {
        std::swap(data, other.data);
        std::swap(len, other.len);
        return *this;
    }

    // Simple way to implement strong exception safety for all operations.
    // Probably with a performance penalty.
    void push_back(int x) {
        // Can throw, but `*this`'s state is unchanged.
        // Do everything in a temporary object.
        minivector other(len + 1);
        for (std::size_t i = 0; i < len; i++) {
            other.data[i] = data[i];
        }
        other.data[len] = x;
        // Now we have `other` in the correct state, let's swap.
        *this = std::move(other);
    }

	...

};
```
