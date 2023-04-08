# Operating System Homework 2 Github Repository

This is the repository for HW2 of Operating System class.

[Assignment PDF](https://github.com/hongshin/OperatingSystem/blob/master/assignments/homework2.pdf)

---

### Team Members:
- 김 성 민
- 21900112
---
- 조 성 준
- 21900699

---

# CIMIN: Crashing Input Minimizer
CIMIN is a program that minimizes the given input that makes the target program crash. By using delta debugging algorithm, CIMIN checkes if the input causes crash of the target program and reduces it as short as possible. In this way, we can identify what certain input makes the target program fail.



# Usage

clone this repository using:
```
$ git clone https://github.com/joon0017/OS-Homework2
```

There will be some executable files exisisting. Those are examples that can be run with CIMIN.

Since there is no executable file for cimin.c, you must compile by using makefile. There are two options provided : original CIMIN, and debugging mode CIMIN which prints out all the details.

1. Original CIMIN
```
$ make
```
In this way, ordinary CIMIN will be compiled, and its name will be 'cimin'.

2. debugging mode CIMIN
```
$ make -f make.debug
```
In this way, CIMIN that prints out all the details will be compiled as 'cimin_debug'


---

### CIMIN
This is the main program of this assignment. The basic usage is:

```
./cimin -i [inputPath] -m [errorString] -o [outputPath] [programToRun]
```
Arguments inside [] as well as all flags are needed to run the program.

- -i inputPath: The file path of the crashing input (i.e., the subject of the reduction)
- -m errorString: A string whose appearance in standard error determines whether the expected crash occurs or not
- -o outputPath: A new file path to store the reduced crashing input
- programToRun: The file path to the executable binary of the target program

The order of the three options doesn't matter if and only if you had put all the options.

### test
This is the debug tool we used to simplify the building process of cimin. The basic usage is:

```
./test [Error String] [String]
```
Arguments inside [] are necessary for the program to run without troubles.

- [Error String]: The sequence of characters to look for
- [String]: The string where the program will look for the error string.
