**Python Requests**

A library that is analogous to the python requests module for C (for windows). This library has a dependency on openssl, and must be compiled and run with openssl dlls.

**Example**
```C
int main() {
    char buffer[50000];
    Http_Args args;
    wget_https("learn.microsoft.com/en-us/", buffer, 50000, args);
    printf("%i", get_content(buffer)); //will print html content
    return 0;
}
```
**Setup**

Download the zip in the releases. Compile using GCC or Mingw with the following command:

`gcc -o file -L. -lwget_https code.c`

Code must be run from same directory as `libcrypto-3-x64.dll`, `libssl-3-x64.dll` and `wget_https.dll`.

**Self Compiling**

To compile, download openssl for window from a source such as https://slproweb.com/products/Win32OpenSSL.html. Then install to the directory containing the files above code. 
Compile with the following commands:

`gcc -c main.c`

`gcc -shared -o wget_https.dll main.o -lws2_32 -IOpenSSL-Win64/include -LOpenSSL-Win64 -lssl-3-x64 -lcrypto-3-x64
`
