Homework 0
----------

### Watch the videos and write up your answers to the following questions

**Important!**

The virtual machine-in-your-browser and the videos you need for HW0 are here:

<http://cs-education.github.io/sys/>

Questions? Comments? Use Piazza: <https://piazza.com/illinois/spring2019/cs241>

The in-browser virtual machine runs entirely in Javascript and is fastest in Chrome. Note the VM and any code you write is reset when you reload the page, **so copy your code to a separate document.** The post-video challenges are not part of homework 0 but you learn the most by doing rather than just passively watching - so we suggest you have some fun with each end-of-video challenge.

HW0 questions are below. Please use this document to write the answers. This will be hand graded.

### Chapter 1

In which our intrepid hero battles standard out, standard error, file descriptors and writing to files

1.  **Hello, World! (system call style)** Write a program that uses `write()` to print out “Hi! My name is &lt;Your Name&gt;”.

```c
// Your code here

#include <unistd.h>

int main() {
   write(1, "Hi! My name is Suk Min Hwang.", 30;   return 0;
}
```

2.  **Hello, Standard Error Stream!** Write a function to print out a triangle of height `n` to standard error. Your function should have the signature `void write_triangle(int n)` and should use `write()`. The triangle should look like this, for n = 3:

```
*
**
***
```

```c
// Your code here
#include <stdio.h>
#include <unistd.h>

void write_triangle(int n){
	int i;
	int j;
	for(i=1; i<=n; i++){
		for(j=1; j<=i; j++){
			write(STDERR_FILENO,"*",1);
		}
		write(STDERR_FILENO,"\n",1);
	}
}

int main() {
	write_triangle(3);
	return 0;
}

```


3.  **Writing to files** Take your program from “Hello, World!” modify it write to a file called `hello_world.txt`. Make sure to to use correct flags and a correct mode for `open()` (`man 2 open` is your friend).

```c
// Your code here

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>

int main() {
	mode_t mode = S_IRUSR;
	int fildes = open("hello_world.txt", O_CREAT|O_TRUNC|O_RDWR, mode);
	write(fildes, "Hello, World!" ,13);
	close(fildes);
	return 0;
}
```

5. **Not everything is a system call** Take your program from “Writing to files” and replace `write()` with `printf()`. *Make sure to print to the file instead of standard out!*

```c
// Your code here

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main() {
	mode_t mode = S_IRUSR | S_IWUSR;
	close(1);
	int fildes = open("hello_world4.txt", O_CREAT|O_TRUNC|O_RDWR, mode);
	printf("Hello, World!\n");
	close(fildes);
	
	
	return 0;
	
}

```

6.  What are some differences between `write()` and `printf()`?

```c
// Your code here
Write() could be used something into the system or into a program and it is designed to only write a sequence of bytes. But printf() is used to write data in many different formats. 
It will print something out but it will only be in GUI/command line and will not effect system.
```

### Chapter 2

Sizing up C types and their limits, `int` and `char` arrays, and incrementing pointers

1.  How many bits are there in a byte?

```c
// Your answer here
at least 8 bits.
```

2.  How many bytes are there in a `char`?

```c
// Your answer here
1byte
```

3.  How many bytes the following are on your machine? 

* `int`: 4 
* `double`: 8 
* `float`: 4
* `long`: 4
* `long long`: 8 

4.  On a machine with 8 byte integers, the declaration for the variable `data` is `int data[8]`. If the address of data is `0x7fbd9d40`, then what is the address of `data+2`?

```c
// Your answer here
0x7fbd9d30 (if stack)
0x7fbd9d50(if heap)
```

5.  What is `data[3]` equivalent to in C? Hint: what does C convert `data[3]` to before dereferencing the address? Remember, the type of a string constant `abc` is an array.

```c
// Your answer here
*(data+3)
```

6.  Why does this segfault?

```c
char *ptr = "hello";
*ptr = 'J';
```
//answer 
If hello is already stored in each of address, in this case, it is like constant you cannot alter.

7.  What does `sizeof("Hello\0World")` return?

```c
// Your answer here
12
```

8.  What does `strlen("Hello\0World")` return?

```c
// Your answer here
5
```

9.  Give an example of X such that `sizeof(X)` is 3.

```c
// Your code here
char X[3] = "abc"
```

10. Give an example of Y such that `sizeof(Y)` might be 4 or 8 depending on the machine.

```c
// Your code here
Y is a pointer. 
int x = 4;
int* Y = &x;
```

### Chapter 3

Program arguments, environment variables, and working with character arrays (strings)

1.  What are two ways to find the length of `argv`?
	1) see argc to find the length of argv.
        2) count argv until return null to find the length.  

2.  What does `argv[0]` represent?
	name of the excution program.

3.  Where are the pointers to environment variables stored (on the stack, the heap, somewhere else)?
	Above the stack, they are stored in the commandline arguement.

4.  On a machine where pointers are 8 bytes, and with the following code:

    ``` c
    char *ptr = "Hello";
    char array[] = "Hello";
    ```

    What are the values of `sizeof(ptr)` and `sizeof(array)`? Why?

```c
// Your answer here
sizeof(ptr) = 4. sizeof(array)=6 
Because size of pointer that points to the first letter, which is first case is just 4.
However size of the array is 6 because it holds 5+NULL space.
```

5.  What data structure manages the lifetime of automatic variables?
	stack
### Chapter 4

Heap and stack memory, and working with structs

1.  If I want to use data after the lifetime of the function it was created in ends, where should I put it? How do I put it there?
    You can use malloc() to allocate memory in heap and free after it is used.

2.  What are the differences between heap and stack memory?
    Heap is for dynamic memory allocation. It can be always allocated and freed at anytime. Accessing to this memory is a bit slower than stack memory.
    Stack is for static memory allocation. It is stored directly to the memory so accessingto this memory and run time is a bit faster than heap memory.

3.  Are there other kinds of memory in a process?
	Yes, text segment and data segment

4.  Fill in the blank: “In a good C program, for every malloc, there is a free”.
    //answer: free

5.  What is one reason `malloc` can fail?
	When the size trying to malloc is larger than the maximum or when zero bytes to malloc, then it fails.

6.  What are some differences between `time()` and `ctime()`?
	time() returns the time time since the Epoch (00:00:00 UTC, January 1, 1970).
	ctime() returns string that indicates the date and time information. (Human readable)
 
7.  What is wrong with this code snippet?

``` c
free(ptr);
free(ptr);
```
//answer
freeing twice the same pointer. It will occur undefined behavior
second line will occur error because memory of ptr is already freed(deallocated).

8.  What is wrong with this code snippet?

``` c
free(ptr);
printf("%s\n", ptr);
```
//answer
After free ptr (deallocate the pointer), trying to access ptr again.

9.  How can one avoid the previous two mistakes?
//answer
When it is freed, make sure set to NULL as well.
and keeping track of memories.

10. Use the following space for the next four questions

```c
// 10, 11, 12, 13
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

struct Person{
	char* name;
	int age;
	struct Person* friends[10];
	
};

typedef struct Person person_t;

person_t* create(char* name_, int age_ ){
	person_t* p1 = (person_t*)malloc(sizeof(person_t));
	p1->name = name_;
	p1->age = age_;
	return p1;
}

void setFriends(person_t* p1, person_t* p2){
	
	p1->friends[0] = p2;
	p2->friends[0] = p1;
}

void destroy(person_t* p) {
	
	memset(p, 0, sizeof(person_t));
	free(p);
	p = NULL;
	
}

int main() {
	
	person_t* AS = create("Agent Smith", 128);
	person_t* SM = create("Sonny Moore", 256);
	setFriends(AS, SM);
	
	
	printf("His age : %d\n", AS->age);
	printf("His name : %s\n", AS->name);
	printf("His friend's age : %d\n", AS->friends[0]->age);
	
	
	destroy(AS);
	destroy(SM);
	return 0;
}

```

* Create a `struct` that represents a `Person`. Then make a `typedef`, so that `struct Person` can be replaced with a single word. A person should contain the following information: their name (a string), their age (an integer), and a list of their friends (stored as a pointer to an array of pointers to `Person`s). 

*  Now, make two persons on the heap, “Agent Smith” and “Sonny Moore”, who are 128 and 256 years old respectively and are friends with each other. Create functions to create and destroy a Person (Person’s and their names should live on the heap).

* `create()` should take a name and age. The name should be copied onto the heap. Use malloc to reserve sufficient memory for everyone having up to ten friends. Be sure initialize all fields (why?).

* `destroy()` should free up not only the memory of the person struct, but also free all of its attributes that are stored on the heap. Destroying one person should not destroy any others.


### Chapter 5

Text input and output and parsing using `getchar`, `gets`, and `getline`.

1.  What functions can be used for getting characters from `stdin` and writing them to `stdout`?
	gets(), puts()

2.  Name one issue with `gets()`.
	it can overwrites stack beyond the boundary so cannot control the memory if overwrites.

3.  Write code that parses the string “Hello 5 World” and initializes 3 variables to “Hello”, 5, and “World”
```c
// Your code here
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

int main() {
	
	char* data = "Hello 5 World";
	char str1[10], str2[10];
	int num1 = 0;
	
	sscanf(data, "%s %d %s", str1, &num1, str2);
	printf("%s : %d : %s", str1, num1, str2);
	return 0;
}
```

4.  What does one need to define before including `getline()`?
	#define _GNU_SOURCE_

5.  Write a C program to print out the content of a file line-by-line using `getline()`.

```c
// Your code here
#define _GNU_SOURCE_

#include <stdlib.h>
#include <stdio.h>

int main() {
	
	char *buffer = NULL;
	size_t capacity = 0;
	FILE *f_ = fopen("random_file.txt", "w+");
	ssize_t result = getline(&buffer, &capacity, f_);
	
	while(result!=-1) {
		printf("%s\n", buffer);
		result = getline(&buffer, &capacity, f_);
	}
	
	return 0;
}
```

### C Development

These are general tips for compiling and developing using a compiler and git. Some web searches will be useful here


1.  What compiler flag is used to generate a debug build?
	-g

2.  You fix a problem in the Makefile and type `make` again. Explain why this may be insufficient to generate a new build.
	If the program failed to make, the files or outputs to make the program are partially left uncompleted. Therefore it could be unsafe and problematic to generate a new build.

3.  Are tabs or spaces used to indent the commands after the rule in a Makefile?
	'Tab' is used. 'Tab' is differentiated from 'space' because it is a part of the syntax.

4.  What does `git commit` do? What’s a `sha` in the context of git?
	git commit is to put your change on file to local repository and 'sha' is a unique ID about this saving or changing.

5.  What does `git log` show you?
	Record of commits that are checked.


6.  What does `git status` tell you and how would the contents of `.gitignore` change its output?
	The git status command tells the state of the working directory and the staging area. '.gitignore' is used to to determine which files and directories to ignore, before you make a commit. A .gitignore file should be committed into your repository, in order to share the ignore rules with any other users that clone the repository.

7.  What does `git push` do? Why is it not just sufficient to commit with `git commit -m ’fixed all bugs’ `?
	git commit is just to change and save the file. But git push is to push or send it to remote github repository. So the local and git files are the same.

8.  What does a non-fast-forward error `git push` reject mean? What is the most common way of dealing with this?
	git cannot make any update without losing commits. Fetching and merging the branch or git pull to perform both action at the same time to deal with this.

### Optional: Just for fun

-   Convert your a song lyrics into System Programming and C code covered in this wiki book and share on Piazza.

-   Find, in your opinion, the best and worst C code on the web and post the link to Piazza.

-   Write a short C program with a deliberate subtle C bug and post it on Piazza to see if others can spot your bug.

-   Do you have any cool/disastrous system programming bugs you’ve heard about? Feel free to share with your peers and the course staff on piazza.
