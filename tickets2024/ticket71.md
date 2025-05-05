## Билет 71 Препроцессор: генерация кода и inline

### Макрос как параметр макроса: пример с автоматическим обходом всех элементов enum.

Спросить на консультации

lectures 15 лекция 2 секция
```c++
#include <iostream>

// X macros: https://en.wikipedia.org/wiki/X_macro

#define LIST_E(f) f(Option1) f(Option2) f(Option3)
enum E {
   #define OPTION(e) e,
   LIST_E(OPTION)
   // OPTION(Option1) OPTION(Option2) OPTION(Option3)
   // Option1, Option2, Option3,
   #undef OPTION
};

int main() {
    E e = Option2;
    switch (e) {
    case Option1: std::cout << "Option1\n"; break;
    case Option2: std::cout << "Option2\n"; break;
    case Option3: std::cout << "Option3\n"; break;
    }

    switch (e) {
    #define PRINT_OPTION(e) case e: std::cout << #e "\n"; break;
    LIST_E(PRINT_OPTION)
    #undef PRINT_OPTION  // undefine, remove macro
    }
}
```

### Базовое использование `##` в макросах:

`##` - склеивает 2 переменные/параметра вместе, например:
```cpp
#define glue(a, b) a ## b // whitespaces between 'a' and 'b' omitted

int xy = 12;
cout << glue(x, y); // expands to: cout << xy; which prints '12' in console
```

1. Было: генерация случайных имён переменных/функций при помощи `__LINE__` (`exercises/05-211004/10-new-var`):

```cpp
#define CONCAT_IMPL(x, y) x##_##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)

// suppose executing on 10th line:
void CONCAT(func, __LINE__)(); // expands to: 'void func_10();'
```


### Как использовать `#macro_arg`, `__FILE__`, `__LINE__` для создания своего макроса `CHECK` в тестовом фреймворке:

1. `#macro_arg` - здесь `#` преобразует переданный параметр макроса `marco_arg` в строку, то есть сделает нам `"marco_arg"`:

```cpp
#define print(x) (cout << #x)

print(output some text); // expands to: (cout << "output some text"); 
``` 

2. `__FILE__` - название текущего файла.

3. `__LINE__` - текущая строка в файле.

4. Как использовать в своем фреймворке для `CHECK`:
```cpp
// printOnError function:

void printOnError(bool hasError, string expr, string filename, int line) {
	if(hasError) {
		cout << expr << "has failed in file: " << filename << ", on line: " << line << endl;
	}
}
```
```cpp
#define CHECK(expr) \
	printOnError((!(expr)), #expr, __FILE__, __LINE__)
```




### Возможность писать в макросах лишь куски корректного кода:

1. Если мы пишем `#if ... #elif ... #else`, то лишь 1 часть из этих `if`-ов выполнится, а остальные будут вырезаны из программы препроцессором до этапа компиляции, поэтому если в каком-нибудь из `if`-ов был написан некорректный код на языке `C++`, но данный `if` не выполнился, то ошибки при компиляции не будет, так как данной части кода просто не будет в файле после работы препроцессора.

2. В лабе `TEST_CASE` выводит нам примерно следующий код `void test()`, а далее ожидается, что вызвавший макрос `TEST_CASE` напишет фигурные скобки и определит в них тело функции-теста, то есть получается, что сам макрос выдает некорректный код: мы обязаны дописывать тело функции, чтобы все компилировалось.



#### Общая идея создания фреймворка для автотестов с авторегистрацией тестов: `TEST_CASE("hi") { CHECK(1 == 2); }`:

1. Мы создаем функцию со статической переменной-вектором, которая будет хранить в себе ссылки на функции (функции - это сами тесты). Эта функция будет по ссылке возвращать нам данный массив, чтобы мы могли класть в него ссылки на тесты (мы делаем это как функцию, так как порядок инициализации переменных между единицами трансляции не задан, поэтому просто статический массив мог не успеть определиться до создания первого теста).

2. `TEST_CASE` делается следующим образом:

```cpp
#define TEST_CASE(name) \
static void CONCAT(func, __LINE__)(); \
const FunctionSaver CONCAT(saver, __LINE__)(name, CONCAT(mytest_func, __LINE__)); \
void CONCAT(mytest_func, __LINE__)()

// suppose calling on line 10
// expands to: 

static void func_10();
const FunctionSaver saver_10(name, func_10);
void func_10()


// use of TEST_CASE
TEST_CASE("test") { // on line 10
	...
}

// expands to 
static void func_10();
const FunctionSaver saver_10(name, func_10);
void func_10() {
	...
}
```

3. Здесь `saver_10` получает в конструктор ссылку на функцию `func_10()`, далее сохраняет ее в глобальный статический массив тестов. Важно, чтобы `saver_10` и `func_10()` были видны только в текущей единице трансляции, это достигается за счет `const` для переменных (здесь это `saver_10`) и `static` для функций (здесь `func_10()`).

4. После того, как все тесты собраны в массив, мы в каком-нибудь `main()` проходимся по всем этим тестам и вызываем каждый из них.