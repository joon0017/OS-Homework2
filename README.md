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



## Preprocess

clone this repository using:
```
$ git clone https://github.com/joon0017/OS-Homework2
```

There will be some executable files exisisting. Those are examples that can be run with CIMIN.

Since there is no executable file for `cimin.c`, you must compile by using makefile. There are two options provided : original CIMIN, and debugging mode CIMIN which prints out all the details.

1. Original CIMIN
>```
>$ make
>```
>In this way, ordinary CIMIN will be compiled, and its name will be `cimin`.

2. debugging mode CIMIN
>```
>$ make -f make.debug
>```
>In this way, CIMIN that prints out all the details will be compiled as `cimin_debug`  


If you want to remove executable files, use
```
$ make clean
```
to clear files for original CIMIN, and use
```
$ make -f make.debug clean
```
for wiping out `cimin_debug`.


## To run CIMIN
This is the main program of this assignment. The basic usage is:

```
./cimin -i [inputPath] -m [errorString] -o [outputPath] [programToRun]
```
Arguments inside [] as well as all flags are needed to run the program.

- -i [inputPath]: The file path of the crashing input (i.e., the subject of the reduction)    

- -m [errorString]: A string whose appearance in standard error determines whether the expected crash occurs or not   

- -o [outputPath]: A new file path to store the reduced crashing input   

- programToRun: The file path to the executable binary of the target program 


The order of the three options doesn't matter if and only if you had put all the options.

## Example usage
*   For `jsondump`, its crashing input is `crash.json`, and the crash can be indicated by `"heap-buffer-overflow"`. Since it does not require any parameter for running, the command line input will be :
> ```
>$ ./cimin -i crash.json -m "heap-buffer-overflow" -o reduced_crash.json jsondump
>``` 
> When `cimin` is done, the shortened version of crashing input will be stored in file named `reduced_crash.json`. All the other examples could be run in this way.

* For `xmllint`, crashing input is `crash.xml`, and it identifies crash with `"AddressSanitizer: SEGV on unknown address"`. In this case, it requires parameters for `xmllint` which are `--recover --postvalid -`. Make sure you put all the parameters, or you will get wrong result.

* For `balance`, crashing input is `fail`, and since it falls into infinte loop when chashed, it does not generate any error message. So the error detecting message can be anything. `cimin` detects this case by signal handling. 

 * For `test_pngfix`, crashing input is `crash.png`, and the error message to identify crash is `"WARNING: MemorySanitizer"`. 

## Tips
if you press `ctrl+c`, `cimin` will stop and save the result so far to the output file.

---