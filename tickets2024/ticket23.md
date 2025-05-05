## Билет 23. Шаблонные друзья
Это всего 30 минут одной [лекции](https://drive.google.com/drive/folders/10Ikh4PyQyztk3WhngBsNMjWZdhKOrAHz), лекция 240409 примерно начиная с 1:03:00.

### Шаблонные классы-друзья
Хотим сделать шаблонную структуру-друга для нашей шаблонной структуры.
#### Общий случай:
Можем сделать друзьями все варианты этой структуры независимо от её шаблонных парметров. Объявляется как обычная структура-друг, но с `template<...>` (количество и тип парметров должны совпадать с определением).
```c++
template<typename T>
struct Foo {
private:
    int x = 0;

    // All Bar<> are friends of us.
    template<typename U>  // just template<typename> is also ok
    friend struct Bar;
};

template<typename T>
struct Bar {
    void bar() {
        Foo<T> f;
        f.x = 10;

        Foo<void> g;
        g.x = 10;
    }
};

int main() {
    Bar<int>().bar();
}
```
В этом примере любая структура`Bar`(с любым шаблонным параметром) является другом любой структуры `Foo` (с любым шаблонным параметрам) и имеет доступ к её приватному полю `x`.

#### Полная специализация:
Можем сделать другом *конкретную* структуру, т.е. *конкретную* полную специализацию шаблонной структуры. Объявляется как обычная структура-друг, указываются все шаблонные параметры, `template<...>` не нужен, но нужен forward declaration шаблона чтобы было что специализировать.

```c++
#include <vector>
#include <utility>

// forward declaration is obligatory
template<typename T>
struct Bar;

template<typename T>
struct Foo {
private:
    int x = 0;

    // One specific Bar<> is a friend. No way to partially specialize.
    friend struct Bar<std::vector<T>>;
};

template<typename T /* VecT may be a better name to avoid confusion */>
struct Bar {
    void bar() {
        // Foo<int> has friend struct Bar<vector<int>>
        Foo<typename T::value_type> f;
        f.x = 10;
        
        // Foo<void> has friend struct Bar<vector<void>>??? 
        [[maybe_unused]] Foo<void> g;
        // g.x = 10;
        
        // Foo<T> has friend struct Bar<vector<T>> 
        [[maybe_unused]] Foo<T> h;
        // h.x = 10;
    }
};

int main() {
    Bar</*T=*/std::vector<int>>().bar();
}
```
В этом примере структура `Foo<T>` имеет структуру-друга `Bar<vector<T>>`, т.е. при разных шаблонных параметрах будут разные друзья. В частности, `Bar<vector<int>>` является другом `Foo<int>`, но не `Foo<void>` и `Foo<vector<int>>`.
Здесь можно легко запутаться: шаблонные параметры принято называть однобуквенными именами, но иногда `T` у разных шаблонов могут иметь концептуально разный смысл и уровень вложенности. Возможно в таких случаях стоит давать более подробные имена (ну или просто не пользоваться такими приколами).

#### Частичных специализаций "тупо не бывает".

### Нешаблонные функции-друзья
Теперь хотим нешаблонную функцию-друга для нашей шаблонной структуры.

#### Функции-друзья, не зависящие от шаблонного параметра структуры

Объявляем одну функцию на все варианты шаблонной-структуры, определяем вне класса.

```c++
template<typename T>
struct MyTemplate {
private:
    int x = 0;

    // Non-template declaration
    friend void simple_func();

};

void simple_func() {
    MyTemplate<int> val1;  // Any MyTemplate<T> would work
    val1.x = 10;
}

int main() {
    simple_func();
}
```
Тут всё просто, `simple_func` --- друг всех `MyTemplate<T>`, имеет доступ к приватному полю.

#### Функции-друзья, зависящие от шаблонных параметров структуры

Если один из парметров функции-друга зависит от шаблонного параметра структуры, то на каждый инстанс??? шаблонной-структуры будет сгенерирована своя функция. Если определять их вне класса, то нужно отдельно определить каждую версию (шаблон написать нельзя, т.к. объявление не шаблонное).
```c++
template<typename T>
struct MyTemplate {
private:
    int x = 0;

    // For all T: generate an independent non-template non_simple_func(MyTemplate<T>&) declaration,
    // impossible to define outside the class in general.
    // Warning.
    friend void non_simple_func(MyTemplate&);

    // For all T: generate an independent non-template non_templ_friend(MyTemplate<T>, MyTemplate<void>) which is a friend of MyTemplate<T>.
    friend void non_templ_friend(MyTemplate &val, MyTemplate<void> &weird) {
        val.x = 10;
        weird.x = 10;  // Should not compile, but GCC compiles it anyway. Clang does not.
    }
};

// Template non_simple_func<T>(), does not correspond to a non-template declaration inside MyTemplate.
template<typename T>
void non_simple_func([[maybe_unused]] MyTemplate<T> &val) {
    // val.x = 10;
}

// Non-template non_simple_func
void non_simple_func(MyTemplate<int> &val) {
    val.x = 10;
}

int main() {
    MyTemplate<int> val1;
    MyTemplate<char> val2;
    MyTemplate<void> weird;

    non_simple_func(val1);  // Calls global function, ok
    // non_simple_func(val2);  // Calls global function by its friend declaration, undefined reference.
    non_simple_func<int>(val1);  // Attempts to call the template function, it is not a friend.

    MyTemplate<int> val1b;
    // non_templ_friend(val1, val1b);  // val1b is not <void>
    non_templ_friend(val1, weird);  // T=int
    non_templ_friend(val2, weird);  // T=char
}
```

Здесь сразу много всего.
Во-первых, `non_simple_func` имеет аргумент, зависящий от шаблонного парметра, а значит генеририуется для каждого `MyTemplate` (есть даже warning что устанем определять). Определим её только для `MyTemplate<int>`, от `MyTemplate<char>` вызвать не сможем. Шаблонная версия этой же функции с нешаблонной никак не связана и другом `MyTemplate` не будет.
Во-вторых, `non_templ_friend` также имеет аргумент, зависящий от шаблонного парметра, но определяется внутри класса, а значит все нужные определения сгенерируются сами. Здесь есть бага в GCC: `non_simple_func` можно вызвать даже если первый аргумент не типа `MyTemplate<void>`, хотя внутри есть обращение к приватным полям этого класса (другом этого класса будет только `non_templ_friend(MyTeemplate<void>, MyTemplate<void>`).

### Шаблонные функции-друзья
Наконец, хотим шаблонную функцию-друга для нашей шаблонной структуры.

#### Общий случай:
Делаем другом шаблонную функцию.
* Если нет аргументов, зависящих от шаблонных параметров класса, то определять обязательно вне класса: для всех инстансов это *одна и та же* шаблонная функция-друг, если определить прямо в классе то нарушается ODR.
* Если есть аргументы, зависящие от шаблонных параметров класса, то наоборот, у каждого инстанса будет своя шаблонная функция-друг тогда как и в предыдущем примере, определять лучше внутри класса, иначе нужно руками определять для всех вариантов первого аргумента (но не второго).
```c++
template<typename T>
struct MyTemplate {
private:
    int x = 0;

    // For all T: for all U: foo(MyTemplate<U>&, MyTemplate<void>&) is a friend MyTemplate<T>
    template<typename U>
    friend void foo(MyTemplate<U> &, MyTemplate<void> &);

    // For all T: generate an independent template non_templ_friend<U>(MyTemplate<T>, MyTemplate<U>) 
    // which is a friend of MyTemplate<T> for any U.
    template<typename U>
    friend void bar(MyTemplate &val, MyTemplate<U> &weird) {
        val.x = 10;
        weird.x = 10;  // Should not compile when T != U because it's not a friend of MyTemplate<T>. Both GCC and Clang agree.
    }
};

template<typename U>
void foo(MyTemplate<U> &val, MyTemplate<void> &weird) {
    val.x = 10;
    weird.x = 10;
}

int main() {
    MyTemplate<int> val1;
    MyTemplate<char> val2;
    MyTemplate<void> weird;

    foo(val1, weird);
    foo(val2, weird);

    bar(val1, val1);
    // bar(val1, weird);  // T=int, U=void
}
```

В этом примере `foo` --- друг всех вариантов `MyTemplate` т.к. не зависит от его шаблонных парметров. С `bar` не так --- у каждого варианта `MyTemplate` своя шаблонная функция-друг `bar`.  Поэтому здесь `bar` можно вызвать только с аргментами одинакового типа (т.к. дружба по первому аргументу, а приватный доступ нужен ко второму).

#### Полная специализация
Делаем другом *конкретную* полную специализацию шаблонной функции. Требуется forward declaration, также нужно явно указать все шаблонные параметры (`template<...>` не нужен).
```c++
template<typename T>
struct MyTemplate;

template<typename U>
void foo(MyTemplate<U> &val);

template<typename U>
void bar(MyTemplate<U> &val, MyTemplate<void> &weird);

template<typename T>
struct MyTemplate {
private:
    int x = 0;

    // template<typename T>  // Not needed, we want a full friend specialization.
    friend void foo<T>(MyTemplate<T> &val);  // <T> is mandatory.

    friend void foo<T*>(MyTemplate<T*> &val);
    
    friend void bar<T>(MyTemplate<T> &val, MyTemplate<void> &weird);
};

template<typename U>
void foo(MyTemplate<U> &val) {
    val.x = 10;
}

template<typename U>
void bar(MyTemplate<U> &val, [[maybe_unused]] MyTemplate<void> &weird) {
    val.x = 10;
    weird.x = 10;  // Works with U=void, but not U=int.
}

int main() {
    MyTemplate<int> a;
    MyTemplate<void*> b;
    MyTemplate<void> c;
    
    foo(a);
    foo(b);
    foo(c);
    
    // bar(a, c);  // U=int
    bar(c, c);  // U=void
}
```
В этом примере друзья `MyTemplate<T>` -- специализации `foo<T>`,  `foo<T*>` и `bar<T>`. Однако, в `bar` есть обращение к приватному полю `MyTemplate<void>`, а значит вызываться может только`bar<void>`.

#### Частичных специализаций функций не бывает