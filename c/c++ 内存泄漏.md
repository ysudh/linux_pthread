---
title: c/c++ 内存泄漏
tags: c/c++,内存泄漏
grammar_cjkRuby: true
---

### 内存泄漏（memory leakage）概述

定义：在编写应用程序的时候，程序分配了一块内存，但已经不再持有引用这块内存的对象（通常是指针），虽然这些内存被分配出去，但是无法收回，将无法被其他的进程所使用，我们说这块内存泄漏了，被泄漏的内存将在整个程序声明周期内都不可使用。
主要原因：是在使用==new==或==malloc==动态分配堆上的内存空间，而并未使用==delete==或==free==及时释放掉内存。

### 内存泄漏的场景

#### 1. ==malloc==和==free==未成对出现

``` cpp
{
    char *p=(char *)malloc(sizeof(char) *10);
    memset(p,0,sizeof(char)*10);
　  ...
    return 0;
}
```
上面的程序在编译，运行阶段都没有问题，但是由malloc分配的内存去无法回收。可以利用valrind定位内存泄漏的原因，如图用malloc分配了10 bytes内存，但是并未回收，==valgrind --tool=memcheck --leak-check=full ./test==

![enter description here](./images/valgrind01.png)

#### 2. 通过局部分配的内存，未在调用者函数体内释放

这种错误在大型代码中常常出现，而且出现该问题是不容易定位，因为内存分配与释放是在不同的函数体内。
``` cpp
char* getMemory()
{
	char *p = (char *)malloc(30); 
	return p;
}
 
int main()
{
	char *p = getMemory();
	
	return 0;
}
```

![enter description here](./images/valgrind01_1.png)

在函数调用者的内部正确释放内存，代码如下。
``` cpp
char* getMemory()
{
	char *p = (char *)malloc(30); 
	return p;
}
 
int main()
{
	char *p = getMemory();

	if(p != NULL)
    {
        free(p);
        p=NULL;
    }
	return 0;
}
```

![enter description here](./images/valgrind02.png)

#### 3. 在局部分配的内存未释放

``` cpp
void getheap(int *p)//p是NULL的地址
{
	p = malloc(sizeof(int) * 10); //p重新指向了分配在堆中的空间
    memset(p,0,sizeof(int) * 10);
}//形式参数int *p在栈空间内，函数结束后就释放了，malloc分配的空间也丢失了.  
int main()
{
	int *p = NULL;  //NULL就是（void *）0
	printf("p=%p\n", p);//p是null的地址
	printf("p=%p\n", &p);//&p是p本身的地址
 
	getheap(p);//值传递，将NULL的地址传递给形参
 
	p[0] = 10;//相当与给NULL指针赋值
	p[1] = 20;
	printf("p[0]=%d,p[1]=%d\n",p[0],p[1]);	
	
	free(p);//p中不是堆中分配的空间的首地址，故free(p)也有问题
    //int *ptr = (int *)malloc(10);
    return 0;
}
```
程序在编译阶段并未报错，当时会出现段错误（内存未能正确被访问，如访问位初始化的指针，访问已经释放的指针变量，数组越界等），可以使用gdb调试coredump定位错误原因。

![enter description here](./images/seg01.png)

上述代码中，在函数==getheap==中分配的内存，但并未能得到释放，当该函数返回时，因为未返回分配的内存的首地址，所以程序将失去对这块内存的控制。

![enter description here](./images/valgrind04_1.png)

#### 4. 由于程序运行时出现不可遇见的错误，导致内存泄漏。

``` cpp
int fun1()
{
    p = malloc(sizeof(int) * 10); 
    fun2();
    ...
    free(p);
}
```
上述代码段在运行中时，若fun2内部出现错误，但是==free==函数不能正常执行，所以出现的内存泄漏的情况。

>其他出现段错误的情况，==使用未初始化的内存；在内存被释放后进行读/写；从已分配内存块的尾部进行读/写；不匹配地使用malloc/new/new[] 和 free/delete/delete[]；两次释放内存==等。并[利用valgrind检测](https://www.oschina.net/translate/valgrind-memcheck?cmp)

### [c++中内存泄漏情况](https://blog.csdn.net/lovely20085901/article/details/39050085)

#### 1. 不匹配使用==new[]== 和 ==delete[]==

``` cpp
int *p = new int[100];
delete []p;//new[],delete []不匹配，导致99对象的内存空间被泄漏。
```
#### 2. ==delet== void * 的指针，导致没有调用到对象的析构函数，析构的所有清理工作都没有去执行从而导致内存的泄露； 

``` cpp
class Object {
private:
    void* data;
    const int size;
    const char id;
public:
    Object(int sz, char c):size(sz), id(c){
        data = new char[size];
        cout << "Object() " << id << " size = " << size << endl;
    }
    ~Object(){
        cout << "~Object() " << id << endl;
        delete []data;      
    }
};
int main() {
    Object* a = new Object(10, 'A');//Object*指针指向一个Object对象；
    void* b = new Object(20, 'B');//void*指针指向一个Object对象；
    delete a;//执行delete，编译器自动调用析构函数；
    delete b;//执行delete，编译器不会调用析构函数，导致data占用内存没有得到回收；

    return 0;
}
```
执行结果：可见对象ｂ的析构函数并未调用，在用delete删除b的操作中，我们只是对b指向的对象进行了释放，并没有调用析构函数，也就是说data指向的内存并没有被释放，而此时又没有指针指向刚才data指针指向的内存，从而造成了内存的丢失，更会造成内存的泄露。

![enter description here](./images/valgrind11.png)
valgrind分析的结果：可见出现了20 bytes的内存泄漏

![enter description here](./images/valgrind12.png)

#### 3. 没有将基类的析构函数定义为虚函数，当基类的指针指向子类时，==delete==该对象时，不会调用子类的析构函数

``` cpp
class person{
public:
    person(){
        cout<<"基类构造函数运行中.....\n";
    }
    ~person(){
        cout<<"基类析构函数运行中.....\n";
    }//需要设置为virtual函数
}; 
class child:public person{
private:
    void* data;
    const int size;
    const char id;
public:
    child(int sz, char c):size(sz), id(c){
        data = new char[size];
        cout<<"派生类构造函数运行中.....\n";
        cout << "child() " << id << " size = " << size << endl;
    }
    ~child(){
        delete []data;
        cout<<"派生类析构函数运行中.....\n";
    }
};
int main(int argc, char** argv) {
	
	person *p = new child(20,'B');
    delete p;
	return 0;
}
```
发现派生类的析构函数并未运用，这时析构函数不能够将构造函数建立的动态资源，进行有效的释放。有效的做法是将父类的析构函数的设为虚函数（==vartual==）

![enter description here](./images/valgrind13.png)
