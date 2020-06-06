# 6th Asprova Programming Contest Local Tester

This is a script for simulating the judge test in local environment. You can test how well your program will score.

However, this script generates and uses input data that is different from the actual scoring on AtCoder, so the exact same results may not appear on AtCoder.


## Operating Environment

Prepare an environment where the following works.

 * C++ Compiler
 * bash

The behavior can be confirmed in the following environments.

 * Windows 10 + Git Bash
 * Windows 10 + Windows Subsystem for Linux (WSL)
 * macOS Catalina + GNU bash
 * Ubuntu 18.04 LTS + GNU bash

Reference: Git Bash is available by installing Git for Windows (https://gitforwindows.org/).

## testset_generator.sh

This is a script that automatically generates input files for testing. It reads files containing the listed parameters and automatically generates test cases. The output changes for each run.

The following options are available.

 * judge:  Using 30 parameters, 1 test case will be generated for each parameter.
 * test:   Using 30 parameters, 10 test cases will be generated for each parameter.
 * full:   Using 30 parameters, the same number of test cases as that in the system test will be generated for each parameter.
 * -o:     Specifies the directory to write out the test case. The default is "test".
 * -f:     Specifies the file containing the listed parameters. The default is testset_parameters.txt.
 * -p:     Specifies the file name prefix of the output test case. The default is "test".
 * -g:     Specifies the test case generator command. The default is ./testcase_generator. This is assumed as the executable file created by compiling testcase_generator.cpp.

The total output file size will be about 200MB if you specify full.


## testset_parameters.txt

A parameter description file to generate the same number of test cases (30 test cases) as that judged in the contest. You can edit and change the number of test cases.

Each row is written in the format below.

```
<number of test cases>,<command line option>
```

Rows that do not match this format will be ignored. The number of test cases is only applied when there are no specifications of command line argument (judge/test/full) to ./testset_generator.sh.


## local_tester.sh

This is a script that executes your program against test cases, and writes a list of output and scoring results to summary.csv.

Before runing this script, output_checker.cpp has to be modified and compiled. Please see "Example of flow of test" below for details.

From the left, the data in each column of summary.csv is "Input file name" "Gross profit" "Late delivery penalty" "Electricity cost" "Setup cost" and "Score". In the last column, 'WA' is written when the output is invalid, and 'RE' when a runtime error occurs.

All numbers will be set to 0 if the output results do not meet the constraints. In addition, the results may not be output in the correct format if the execution of the answer program does not complete successfully.

The following options are available.

 * -i:   Specifies the directory where the test case to be used is saved. The default is "test". Any file in this directory with a name ending in .txt will be considered as a test case.
 * -o:   Specifies the directory to write out the execution results. The default is the date and time of the script's execution. The output of the program is written to a file with the same name as the input.
 * -e:   Specifies the command to run the solution program. The default is ./a.out. If the command is followed by an argument, use double quotation marks. (Example: "python3 main.py")
 * -c:   Specifies the executable file created by compiling output_checker.cpp. The default is ./output_checker.


## Example of flow of test

**Note: Files with names starting with "-" or containing spaces may not run appropriately**

1. Compile both testcase_generator.cpp and output_checker.cpp. For output_checker.cpp, compile after changing row 4 as follows. Remove the comment in
   ```
   // #define LOCAL_TEST
   ```
   and change it as below.
   ```
   #define LOCAL_TEST
   ```
   In the case of GCC, you can compile it using the following command.
   ```
   g++ -std=c++14 -o testcase_generator testcase_generator.cpp
   g++ -std=c++14 -o output_checker output_checker.cpp
   ```

2. Place the following in the same directory.
   - Executable files created in 1.
   - testset_generator.sh
   - testset_parameters.txt
   - local_tester.sh
   - Executable file of solution program (let the file name be a.out)

3. Run ./testset_generator.sh (If asked for an output destination you can press Enter without specifying anything). 30 test cases will be generated and will be saved under test/. The procedure until this step is unnecessary for the second time onwards.

4. Run ./local_tester.sh (If asked for an output destination you can press Enter without specifying anything). The program will run for the test cases generated in 3.. A csv file of the output and scoring results will be saved in the directory with the date and time of execution as its name.

If the solution program does not finish, for example, due to an infinite loop, the script execution will stop there too. If another window is open when the script is running, Ctrl+C will force the running test case to end, and closing the window will end the entire script execution.
