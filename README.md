# Operating System Homework 2 Girhub Repository

This is the repository for HW2 of Operating System class.

[Assignment PDF](https://github.com/hongshin/OperatingSystem/blob/master/assignments/homework2.pdf)

---

### Team Nembers:
- 김 성 민
- 21900112
---
- 조 성 준
- 21900699

---
# Usage

clone this repository using:
```
git clone https://github.com/joon0017/OS-Homework2
```

compile using a gcc:
```
make
```
This will create 2 executable files
- cimin
- test

---

### cimin
This is the main program of this assignment. The basic usage is:

```
./cimin -i [inputPath] -m [errorString] -o [outputPath] [programToRun]
```
Arguments inside [] as well as all flags are needed to run the program.

- -i inputPath: The file path of the crashing input (i.e., the subject of the reduction
- -m errorString: A string whose appearance in standard error determines whether the expected crash occurs or not
- -o outputPath: A new file path to store the reduced crashing input
- programToRun: The file path to the executable binary of the target program

### test
This is the debug tool we used to simplify the building process of cimin. The basic usage is:

```
./test [Error String] [String]
```
