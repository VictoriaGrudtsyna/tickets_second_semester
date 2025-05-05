## Билет 52. Указатели
## Указатели на указатели
### Массив строк (плюсовых)
В `C++` есть специальных синтаксис для создания массива, обьекты которого выделяются на куче (пример - массив строк):
```C++
std::string *arr = new std::string[n];
```
Но потом надо будет самим почистить содержимое:
```C++
delete[] arr;
```
Но вообще это тупо и почти всегда лучше использовать *vector*, кроме случая, когда вы не хотите его инклудить.

### Output-параметр функции
Иногда удобно передавать в функцию параметром указатель, по которому будет записана структура, являющаяся результатом вывода данной функции, а сама функция будет возвращать номер ошибки например:
```c
typedef struct calc_result {
    int value;
} calc_result;

int calc_evaluate(const int *expr,calc_result *res) {
    int ERROR_OF_EXPR = 0;
    if (res && expr) {
        res->value = expr[0];
    } else if (!res) {
        ERROR_OF_EXPR = 1;
    } else {
        ERROR_OF_EXPR = 2;
    }
    return ERROR_OF_EXPR;
}
```
Если у нас пустой *expr* - то это один тип ошибки, если пустой *res* - другой. В *res* - записывается результат (если нужные параметры не пустые).
### Const в разных местах указателя на указатель
1. Если мы пишем:
```C
const int *p = &x;
```
То это означает, что то, на что теперь указывает *p* - мы менять не можем (то есть он указывает на константный *int*). Такой указатель каститься к любому другому указателю, то есть эта константность легко снимается, например:
```c
int *p_new = &x;
```
*p_new* - можем менять его как хотим.

2. Если мы пишем:
```C
int *const p = &x;
```
То мы не можем менять указатель (то есть сама переменная *p* - константа). Но такая константность уже не снимается:
```c
  [[maybe_unused]] int cx[10];
  [[maybe_unused]] const int *pcx = &cx[0];
//[[maybe_unused]] int *px = pcx; так делать нельзя
//[[maybe_unused]] int *const cpx = pcx; так делать нельзя
  [[maybe_unused]] const int *const cpcx = pcx; //можно навешивать константность указателя и забирать
```
Мы не можем делать преобразования от константного обьекта к неконстантному.

3. Ну и конечно можно писать:
```C
const int *const p = &x;
```
Тут у нас всё константное - и указатель, и то, на что он указывает. 

Ещё бывают интересные конструкции вида:
```c
const int *const *const p = &x;
const int **const p = &x;
```
Где мы можем контроллировать константность каждой отдельной "размерности" массивов. 

**link:** https://cdecl.org/  - можно удобно конструировать подобное словами, и наоборот: переводить подобное в связный текст.
### Наследование 
Код Егора:
```c
struct Base {};
struct Derived : Base {};
struct Derived2 : Base {};

int main() {
    Base b;
    Derived d;
    [[maybe_unused]] Derived2 d2;

    {
        [[maybe_unused]] Base *pb = &b;
        [[maybe_unused]] Derived *pd = &d;
        pb = pd;  // Derived* --> Base*: OK

        pb = &d2; // OK
        // pd = pb;  // Base* --> Derived*: error, otherwise pd points to Derived2.
    }
    {
        Base *pb = &b;
        [[maybe_unused]] Base **ppb = &pb;
        Derived *pd = &d;
        [[maybe_unused]] Derived **ppd = &pd;

        *ppb = &d2;  // OK
        // ppd = ppb;  // Base** --> Derived**: error, otherwise *ppd points to Derived2.

        // Surprise: https://isocpp.org/wiki/faq/proper-inheritance#derivedptrptr-to-baseptrptr
        // ppb = ppd;  // Derived** --> Base**: error, otherwise assertion fails:
        *ppb /* Base* */ = &d2;  // OK
        // assert(*ppd /* Derived* */ != &d2 /* Derived2* */);
    }
}
```
Здесь показывается, что обычные указатели наследуются как обычно: можем каститься только к базе и более близким к базе классам (от которых мы, непосредственно, наследовались).
А в двойных указателях запрещены любые касты вообще. Это показывается в строчках:
```c
ppb = ppd;  // Derived** --> Base**: error, otherwise assertion fails:
*ppb /* Base* */ = &d2;  // OK
```
Что если бы вдруг первая строчка компилировалась, то мы могли бы второй строчкой присвоить указателю на *Derived* указатель на *Derived2* - а такого допускать нельзя.
## realloc
### Принцип работы
Это перевыделение памяти, пишется как
```C
realloc(p, len)
```
*p* - указатель, где выделить; *len* - сколько байт. 
Вернёт указатель на аллоцированную память заданного размера. Работает так: смотрит сколько у нас есть уже аллоцированной памяти после *p*, если там ещё можно довыделить до *len* (или наоборот уменьшить до *len*), то довыделит и вернёт тот же указатель (то есть копирования не произойдёт), в противном случае выделит требуемый размер в другом месте и перекопирует туда лежащее в *p*.

### Безопасность
Небезопасно:
```C
b = realloc(b, num)
```
В случае фейла (что бы не произошло) *realloc* вернёт *null*, таким образом мы потеряем старые данные, которые лежали в *b*.
Нормальный способ:
```C
char *new_b = realloc(b, num);
if(!new_b){
    что-то сделать, у тебя память не выделилась :(
}
else{
    b = new_b;
}
```

## restrict
Часто используют с параметрами функций (код Егора):
```C
void my_memcpy1(void *restrict dst, const void *restrict src, size_t n) {
    while (n-- > 0)
        *(char *)dst++ = *(const char *)src++;
}

```
Означает, что такой указатель (обьект по нему) точно с другими не пересекается (в рамках блока, то есть {}, то есть любое обращение к обьекту или его части идёт через этот указатель, иначе UB) и компилятор может применять всякие интересные оптимизации (которые, в случае с данным примером, не будут учитывать порялок копирования). Можно указывать и не  в функциях, но там есть свои тонкости, лучше почитайте cppreference (ссылка внизу), нам про это не говорили, так что думаю это не обязательно. В плюсах такой штуки нет, там мы не можем указать, что какой-то указатель ни с кем больше не пересекается.
(**ДОП пример** его не обязательно показывать)
На самом деле может использоваться даже такой синтаксис (cppreference):
```C
void f(int m, int n, float a[restrict m][n], float b[restrict m][n]);
void g12(int n, float (*p)[n]) {
   f(10, n, p, p+10); // OK
   f(20, n, p, p+10); // possibly undefined behavior (depending on what f does)
}
```
Здесь мы в функцию передаём массивы и таким образом указываем тип указателя на них. 

## Собственные opaque-указатели
Opaque-указатели - это указатели, где нам не особо важно, как устроенны обьекты, на которые они указывают, мы просто пользуемся возможностями этих обьектов.
Иногда хотим точно специфицировать тип, с которым мы работаем: 
```C
typedef struct array_data_ *ArrayData;
```
Таким образом теперь *ArrayData* можно обозначать указатели на *array_data_*, то есть писать конструкции вида:
```C
ArrayData data = NULL;
```
Пример Егора, как это можно использовать:
```C++
#include <stddef.h>
#include <iostream>

// C library
typedef struct array_data_ *ArrayData;
typedef struct user_data_ *UserData;

void foreach(
    ArrayData begin,
    size_t element_size,
    size_t n,
    void (*f)(UserData user_data, ArrayData element),
    UserData user_data
) {
    for (size_t i = 0; i < n; i++, begin = (ArrayData)((char*)begin + element_size)) {
        f(user_data, begin);
        // f(begin, user_data);  // compilation error now
    }
}

// C++ code
int main() {
    int arr[] = {1, 2, 3};
    int x = 123;
    foreach(
        reinterpret_cast<ArrayData>(arr),
        sizeof(int),
        3,
        [](UserData user_data, ArrayData element) {
            std::cout << *reinterpret_cast<int*>(user_data) << " " << *reinterpret_cast<int*>(element) << "\n";
        },
        reinterpret_cast<UserData>(&x)
   );
}
```
Таким образом мы защитились от случая, что мы случайно вызовем *f(begin,user_data)* тем, что мы точнее специфицировали типы, если бы мы везде написали *void* * такой код бы сработал.

## Links
https://en.cppreference.com/w/c/language/restrict - про restrict

https://www.tutorialspoint.com/c_standard_library/c_function_realloc.htm - про realloc

https://github.com/hse-spb-2021-cpp/lectures/tree/master/24-220411 - лекция Егора по указатели

https://github.com/hse-spb-2021-cpp/lectures/tree/master/26-220425/01-c-pointer-ops-idioms - лекция Егора про realloc, restrict

https://github.com/hse-spb-2021-cpp/lectures/tree/master/27.5-220519/01-c-cpp-extern/03-opaque-reinterpret-cast - пример со своим opaque указателем
