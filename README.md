# EZLanguage

This is a project of mine where I implement a compiler for my own language called EZ(easy)Language.
The compiler is written in C++ and it takes in an EZ source code file and as output, generates assembly code compatible with 32-bit Linux ABI which means that the compiler generates target assembly code only for Linux based machines.
For getting the final executable binary, you must assemble the output assembly file
using NASM assembler by two terminal commands 
nasm -felf32 <generated-assembly-file-name> -o <object-file-name>
gcc <object-file-name> -o <executable-file-name> -no-pie
on the Linux machine.

# About The Language:
1) supports only two data types - string (str) and unsigned int(int)
2) colon(:) is the assignment operator
3) supports basic arithmetic operations which are +, - , * , / , %.
4) supports the if and if-else conditional construct and also looping constructs like while and do-while.(for loop not supported)
5) boolean operators for conditions which are >(greater than), <(Less than), = (equal to)
6) print statement to print numbers,arithmetic expression results,string variables and string literals
7) input statement to input numbers and string values into variables
8) All statements end with semicolon (like in the C language)                                                                      
9) Single line comments (characters after the @ symbol in a line are treated as comments and ignored by the compiler)                                                                   
The structure of an EZ program is as follows:
                                                                   
begin
variable declarations
...
perform printing,input taking,assignments and arithmetic calcuations
end
                                                                      
Programs start with the begin statement followed by declarations of all used variables,followed by the operations done on the data and then end statement to mark the end of the program.
                                                                      
Sample Programs:
1) EZ Program to print hello world                                                                      
begin
print "Hello World!" ;
end
 
2) Hello world using string variable

begin
str message;  @Declare a string variable message
message : "Hello, World";  @Assign hello world string to the variable
print message;   @Print the variable
end

3) EZ program to input a number and find if its odd or even
begin
int num;
print "Enter a number: ";
input num; @Read input and store the value in num variable
if((num % 2) = 0){
    print "Entered number is even!";
}else{
    print "Entered number is odd";
}; @Yes, you must put a semicolon after the else block in the if-else statement! 
end                                                                      
                                                                     
                                                                      
                                                                      
                                                                      
                                                                      
                                                                      
                                                                      
                                                                      
