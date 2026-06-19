# Chapter 11: Advanced Streams & File I/O

This chapter explores the full power of the C++ `<iostream>`, `<fstream>`, and `<sstream>` libraries, deconstructing how they interact with the filesystem and formatted data.

## 10.1 File I/O Foundations

Working with files involves the `std::fstream`, `std::ifstream`, and `std::ofstream` classes.

### 1. Opening Modes

*   `std::ios::in`: Open for reading.
*   `std::ios::out`: Open for writing (overwrites existing).
*   `std::ios::app`: Append to end of file.
*   `std::ios::ate`: Open and seek to end.
*   `std::ios::binary`: Binary mode (no CRLF translation).

### 2. Binary vs Text Mode

In text mode, special characters like `\n` might be translated (e.g., to `\r\n` on Windows). Binary mode ensures that exactly what you write is what appears on disk.
```cpp
std::ofstream file("data.bin", std::ios::binary);
double d = 3.14;
file.write(reinterpret_cast<const char*>(&d), sizeof(d));
```

***

## 10.2 Stream Manipulators

Manipulators allow you to change how data is formatted on the fly.

### 1. Numeric Formatting

*   `std::hex`, `std::oct`, `std::dec`: Change base.
*   `std::setprecision(n)`: Set floating point precision (requires `<iomanip>`).
*   `std::fixed`, `std::scientific`: Change notation.

### 2. Padding and Alignment

*   `std::setw(n)`: Set width of next field.
*   `std::setfill(c)`: Set fill character.
*   `std::left`, `std::right`, `std::internal`: Change alignment.

***

## 10.3 String Streams (`sstream`)

`std::stringstream` allows you to treat a string like a stream, enabling easy conversion between types and strings.
```cpp
#include <sstream>

std::stringstream ss;
ss << "The answer is " << 42;
std::string s = ss.str();
```

***
### Professional Insights: Stream Architecture

#### 1. The `ios_base` State Machine

Every stream inherits from `ios_base`, which maintains internal flags for formatting and errors.
*   **Performance Trap**: Streams are significantly slower than C's `printf`/`scanf` due to the overhead of object construction and virtual function calls.
*   **Optimization**: `std::ios::sync_with_stdio(false);` disables the synchronization between C++ and C streams, making `std::cin` as fast as `scanf`.

#### 2. Stream Buffering (`streambuf`)

The actual data transfer is handled by a buffer object (`rdbuf`).
*   **Redirection**: You can redirect `cout` to a file by swapping its buffer:
```cpp
std::ofstream out("log.txt");
std::streambuf *coutbuf = std::cout.rdbuf();
std::cout.rdbuf(out.rdbuf()); // cout now writes to log.txt
```

#### 3. Custom Manipulators

You can create your own manipulators by defining functions that take and return a reference to a stream:
```cpp
std::ostream& tab(std::ostream& os) {
    return os << "\t";
}
std::cout << "Data1" << tab << "Data2" << std::endl;
```

# VOLUME 01: GODHOOD SUMMARY

Volume 01 established the "Archaic" foundations of C++. By mastering C++98/03, you have learned the manual labor of the language:
1. **Memory Management**: The raw power and danger of pointers and `new/delete`.
2. **OOP Mechanics**: How virtualization and the object model work under the hood.
3. **The Classic STL**: The original containers and algorithms that still form the backbone of modern systems.

**The Golden Rule of C++98**: Everything is explicit. There are no shortcuts. To achieve Godhood, you must respect these roots while preparing to transcend them with the features of the Modern Revolution.

# VOLUME 02 MODERN REVOLUTION C11



## Professional Insights: String Streams (std::ostringstream)


std::ostringstream is a class whose objects look like an output stream (that is, you can write to them via
operator<<), but actually store the writing results, and provide them in the form of a stream.
Consider the following short code:
```cpp
#include <sstream>
#include <string>
using namespace std;
int main()
{
    ostringstream ss;
    ss << "the answer to everything is " << 42;
    const string result = ss.str();
}
```
The line
ostringstream ss;
creates such an object. This object is ﬁrst manipulated like a regular stream:
ss << "the answer to everything is " << 42;
Following that, though, the resulting stream can be obtained like this:
const string result = ss.str();
(the string result will be equal to "the answer to everything is 42").
This is mainly useful when we have a class for which stream serialization has been deﬁned, and for which we want a
string form. For example, suppose we have some class
```cpp
class foo
{
    // All sort of stuff here.
};
ostream &operator<<(ostream &os, const foo &f);
```
To get the string representation of a foo object,
```cpp
foo f;
```
we could use
```cpp
ostringstream ss;
ss << f;
const string result = ss.str();
```

Then result contains the string representation of the foo object.
Basic printing
std::ostream_iterator allows to print contents of an STL container to any output stream without explicit loops.
The second argument of std::ostream_iterator constructor sets the delimiter. For example, the following code:
```cpp
std::vector<int> v = {1,2,3,4};
std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " ! "));
```
will print
1 ! 2 ! 3 ! 4 !
Implicit type cast
std::ostream_iterator allows to cast container's content type implicitly. For example, let's tune std::cout to print
ﬂoating-point values with 3 digits after decimal point:
```cpp
std::cout << std::setprecision(3);
std::fixed(std::cout);
```
and instantiate std::ostream_iterator with float, while the contained values remain int:
```cpp
std::vector<int> v = {1,2,3,4};
std::copy(v.begin(), v.end(), std::ostream_iterator<float>(std::cout, " ! "));
```
so the code above yields
1.000 ! 2.000 ! 3.000 ! 4.000 !
despite std::vector holds ints.
Generation and transformation
std::generate, std::generate_n and std::transform functions provide a very powerful tool for on-the-ﬂy data
manipulation. For example, having a vector:
```cpp
std::vector<int> v = {1,2,3,4,8,16};
```
we can easily print boolean value of "x is even" statement for each element:
```cpp
std::boolalpha(std::cout); // print booleans alphabetically
std::transform(v.begin(), v.end(), std::ostream_iterator<bool>(std::cout, " "),
[](int val) {
    return (val % 2) == 0;
});
```
or print the squared element:
```cpp
std::transform(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "),
[](int val) {
    return val * val;

});
```
Printing N space-delimited random numbers:
```cpp
const int N = 10;
std::generate_n(std::ostream_iterator<int>(std::cout, " "), N, std::rand);
```
Arrays
As in the section about reading text ﬁles, almost all these considerations may be applied to native arrays. For
example, let's print squared values from a native array:
```cpp
int v[] = {1,2,3,4,8,16};
std::transform(v, std::end(v), std::ostream_iterator<int>(std::cout, " "),
[](int val) {
    return val * val;
});
```



##### Stream manipulators

Manipulators are special helper functions that help controlling input and output streams using operator >> or
operator <<.
They all can be included by #include <iomanip>.
Section 14.1: Stream manipulators
std::boolalpha and std::noboolalpha - switch between textual and numeric representation of booleans.
```cpp
std::cout << std::boolalpha << 1;
// Output: true
std::cout << std::noboolalpha << false;
// Output: 0
bool boolValue;
std::cin >> std::boolalpha >> boolValue;
std::cout << "Value \"" << std::boolalpha << boolValue
          << "\" was parsed as " << std::noboolalpha << boolValue;
// Input: true
// Output: Value "true" was parsed as 0
std::showbase and std::noshowbase - control whether preﬁx indicating numeric base is used.
std::dec (decimal), std::hex (hexadecimal) and std::oct (octal) - are used for changing base for integers.
#include <sstream>
std::cout << std::dec << 29 << ' - '
          << std::hex << 29 << ' - '
          << std::showbase << std::oct << 29 << ' - '
          << std::noshowbase << 29  '\n';
int number;
std::istringstream("3B") >> std::hex >> number;
std::cout << std::dec << 10;
// Output: 22 - 1D - 35 - 035
// 59
Default values are std::ios_base::noshowbase and std::ios_base::dec.
If you want to see more about std::istringstream check out the <sstream> header.
std::uppercase and std::nouppercase - control whether uppercase characters are used in ﬂoating-point and
hexadecimal integer output. Have no eﬀect on input streams.
std::cout << std::hex << std::showbase
              << "0x2a with nouppercase: " << std::nouppercase << 0x2a << '\n'
              << "1e-10 with uppercase: " << std::uppercase << 1e-10 << '\n'
}
// Output: 0x2a with nouppercase: 0x2a
// 1e-10 with uppercase: 1E-10
Default is std::nouppercase.
std::setw(n) - changes the width of the next input/output ﬁeld to exactly n.
```

The width property n is resetting to 0 when some functions are called (full list is here).
```cpp
std::cout << "no setw:" << 51 << '\n'
          << "setw(7): " << std::setw(7) << 51 << '\n'
          << "setw(7), more output: " << 13
          << std::setw(7) << std::setfill('*') << 67 << ' ' << 94 << '\n';
char* input = "Hello, world!";
char arr[10];
std::cin >> std::setw(6) >> arr;
std::cout << "Input from \"Hello, world!\" with setw(6) gave \"" << arr << "\"\n";
// Output: 51
// setw(7):      51
// setw(7), more output: 13*****67 94
// Input: Hello, world!
// Output: Input from "Hello, world!" with setw(6) gave "Hello"
Default is std::setw(0).
std::left, std::right and std::internal - modify the default position of the ﬁll characters by setting
std::ios_base::adjustfield to std::ios_base::left, std::ios_base::right and std::ios_base::internal
correspondingly. std::left and std::right apply to any output, std::internal - for integer, ﬂoating-point and
monetary output. Have no eﬀect on input streams.
#include <locale>
std::cout.imbue(std::locale("en_US.utf8"));
std::cout << std::left << std::showbase << std::setfill('*')
          << "flt: " << std::setw(15) << -9.87  << '\n'
          << "hex: " << std::setw(15) << 41 << '\n'
          << "  $: " << std::setw(15) << std::put_money(367, false) << '\n'
          << "usd: " << std::setw(15) << std::put_money(367, true) << '\n'
          << "usd: " << std::setw(15)
          << std::setfill(' ') << std::put_money(367, false) << '\n';
// Output:
// flt: -9.87**********
// hex: 41*************
//   $: $3.67**********
// usd: USD *3.67******
// usd: $3.67          
std::cout << std::internal << std::showbase << std::setfill('*')
          << "flt: " << std::setw(15) << -9.87  << '\n'
          << "hex: " << std::setw(15) << 41 << '\n'
          << "  $: " << std::setw(15) << std::put_money(367, false) << '\n'
          << "usd: " << std::setw(15) << std::put_money(367, true) << '\n'
          << "usd: " << std::setw(15)
          << std::setfill(' ') << std::put_money(367, true) << '\n';
// Output:
// flt: -**********9.87
// hex: *************41
//   $: $3.67**********
// usd: USD *******3.67
// usd: USD        3.67
std::cout << std::right << std::showbase << std::setfill('*')
          << "flt: " << std::setw(15) << -9.87  << '\n'
          << "hex: " << std::setw(15) << 41 << '\n'
          << "  $: " << std::setw(15) << std::put_money(367, false) << '\n'
          << "usd: " << std::setw(15) << std::put_money(367, true) << '\n'
          << "usd: " << std::setw(15)
          << std::setfill(' ') << std::put_money(367, true) << '\n';
// Output:
// flt: **********-9.87
// hex: *************41
//   $: **********$3.67
// usd: ******USD *3.67
// usd:       USD  3.67
Default is std::left.
std::fixed, std::scientific, std::hexfloat [C++11] and std::defaultfloat [C++11] - change formatting for
ﬂoating-point input/output.
std::fixed sets the std::ios_base::floatfield to std::ios_base::fixed,
std::scientific - to std::ios_base::scientific,
std::hexfloat - to std::ios_base::fixed | std::ios_base::scientific and
std::defaultfloat - to std::ios_base::fmtflags(0).
fmtflags
#include <sstream>
std::cout << '\n'
          << "The number 0.07 in fixed:      " << std::fixed << 0.01 << '\n'
          << "The number 0.07 in scientific: " << std::scientific << 0.01 << '\n'
          << "The number 0.07 in hexfloat:   " << std::hexfloat << 0.01 << '\n'
          << "The number 0.07 in default:    " << std::defaultfloat << 0.01 << '\n';
double f;
std::istringstream is("0x1P-1022");
double f = std::strtod(is.str().c_str(), NULL);
std::cout << "Parsing 0x1P-1022 as hex gives " << f << '\n';
// Output:
// The number 0.01 in fixed:      0.070000
// The number 0.01 in scientific: 7.000000e-02
// The number 0.01 in hexfloat:   0x1.1eb851eb851ecp-4
// The number 0.01 in default:    0.07
// Parsing 0x1P-1022 as hex gives 2.22507e-308
Default is std::ios_base::fmtflags(0).
There is a bug on some compilers which causes
double f;
std::istringstream("0x1P-1022") >> std::hexfloat >> f;
std::cout << "Parsing 0x1P-1022 as hex gives " << f << '\n';
// Output: Parsing 0x1P-1022 as hex gives 0
std::showpoint and std::noshowpoint - control whether decimal point is always included in ﬂoating-point
representation. Have no eﬀect on input streams.
std::cout << "7.0 with showpoint: " << std::showpoint << 7.0 << '\n'
          << "7.0 with noshowpoint: " << std::noshowpoint << 7.0 << '\n';
// Output: 1.0 with showpoint: 7.00000
// 1.0 with noshowpoint: 7
Default is std::showpoint.
std::showpos and std::noshowpos - control displaying of the + sign in non-negative output. Have no eﬀect on input
streams.
std::cout << "With showpos: " << std::showpos
          << 0 << ' ' << -2.718 << ' ' << 17 << '\n'
          << "Without showpos: " << std::noshowpos
          << 0 << ' ' << -2.718 << ' ' << 17 << '\n';
// Output: With showpos: +0 -2.718 +17
// Without showpos: 0 -2.718 17
Default if std::noshowpos.
std::unitbuf, std::nounitbuf - control ﬂushing output stream after every operation. Have no eﬀect on input
stream. std::unitbuf causes ﬂushing.
std::setbase(base) - sets the numeric base of the stream.
std::setbase(8) equals to setting std::ios_base::basefield to std::ios_base::oct,
std::setbase(16) - to std::ios_base::hex,
std::setbase(10) - to std::ios_base::dec.
If base is other then 8, 10 or 16 then std::ios_base::basefield is setting to std::ios_base::fmtflags(0). It
means decimal output and preﬁx-dependent input.
As default std::ios_base::basefield is std::ios_base::dec then by default std::setbase(10).
std::setprecision(n) - changes ﬂoating-point precision.
#include <cmath>
#include <limits>
typedef std::numeric_limits<long double> ld;
const long double pi = std::acos(-1.L);
std::cout << '\n'
          << "default precision (6):   pi: " << pi << '\n'
          << "                       10pi: " << 10 * pi << '\n'
          << "std::setprecision(4):  10pi: " << std::setprecision(4) << 10 * pi << '\n'
          << "                    10000pi: " << 10000 * pi << '\n'
          << "std::fixed:         10000pi: " << std::fixed << 10000 * pi << std::defaultfloat <<
'\n'
          << "std::setprecision(10):   pi: " << std::setprecision(10) << pi << '\n'
          << "max-1 radix precicion:   pi: " << std::setprecision(ld::digits - 1) << pi << '\n'
          << "max+1 radix precision:   pi: " << std::setprecision(ld::digits + 1) << pi << '\n'
          << "significant digits prec: pi: " << std::setprecision(ld::digits10) << pi << '\n';
// Output:
// default precision (6):   pi: 3.14159
//                        10pi: 31.4159
// std::setprecision(4):  10pi: 31.42
//                     10000pi: 3.142e+04
// std::fixed:         10000pi: 31415.9265
// std::setprecision(10):   pi: 3.141592654
// max-1 radix precicion:   pi: 3.14159265358979323851280895940618620443274267017841339111328125
// max+1 radix precision:   pi: 3.14159265358979323851280895940618620443274267017841339111328125
// significant digits prec: pi: 3.14159265358979324
Default is std::setprecision(6).
std::setiosflags(mask) and std::resetiosflags(mask) - set and clear ﬂags speciﬁed in mask of
std::ios_base::fmtflags type.
#include <sstream>
std::istringstream in("10 010 10 010 10 010");
int num1, num2;
in >> std::oct >> num1 >> num2;
std::cout << "Parsing \"10 010\" with std::oct gives:   " << num1 << ' ' << num2 << '\n';
// Output: Parsing "10 010" with std::oct gives:   8 8
in >> std::dec >> num1 >> num2;
std::cout << "Parsing \"10 010\" with std::dec gives:   " << num1 << ' ' << num2 << '\n';
// Output: Parsing "10 010" with std::oct gives:   10 10
in >> std::resetiosflags(std::ios_base::basefield) >> num1 >> num2;
std::cout << "Parsing \"10 010\" with autodetect gives: " << num1 << ' ' << num2 << '\n';
// Parsing "10 010" with autodetect gives: 10 8
std::cout << std::setiosflags(std::ios_base::hex |
                              std::ios_base::uppercase |
                              std::ios_base::showbase) << 42 << '\n';
// Output: OX2A
std::skipws and std::noskipws - control skipping of leading whitespace by the formatted input functions. Have no
eﬀect on output streams.
#include <sstream>
char c1, c2, c3;
std::istringstream("a b c") >> c1 >> c2 >> c3;
std::cout << "Default  behavior:  c1 = " << c1 << "  c2 = " << c2 << "  c3 = " << c3 << '\n';
std::istringstream("a b c") >> std::noskipws >> c1 >> c2 >> c3;
std::cout << "noskipws behavior:  c1 = " << c1 << "  c2 = " << c2 << "  c3 = " << c3 << '\n';
// Output: Default  behavior:  c1 = a  c2 = b  c3 = c
// noskipws behavior:  c1 = a  c2 =    c3 = b
Default is std::ios_base::skipws.
std::quoted(s[, delim[, escape]]) [C++14] - inserts or extracts quoted strings with embedded spaces.
s - the string to insert or extract.
delim - the character to use as the delimiter, " by default.
escape - the character to use as the escape character, \ by default.
#include <sstream>
std::stringstream ss;
std::string in = "String with spaces, and embedded \"quotes\" too";
std::string out;
ss << std::quoted(in);
std::cout << "read in     [" << in << "]\n"
          << "stored as   [" << ss.str() << "]\n";
ss >> std::quoted(out);
std::cout << "written out [" << out << "]\n";
// Output:
// read in     [String with spaces, and embedded "quotes" too]
// stored as   ["String with spaces, and embedded \"quotes\" too"]
// written out [String with spaces, and embedded "quotes" too]
```

For more information see the link above.
Section 14.2: Output stream manipulators
std::ends - inserts a null character '\0' to output stream. More formally this manipulator's declaration looks like
```cpp
template <class charT, class traits>
std::basic_ostream<charT, traits>& ends(std::basic_ostream<charT, traits>& os);
and this manipulator places character by calling os.put(charT()) when used in an expression
os << std::ends;
std::endl and std::flush both ﬂush output stream out by calling out.flush(). It causes immediately producing
output. But std::endl inserts end of line '\n' symbol before ﬂushing.
std::cout << "First line." << std::endl << "Second line. " << std::flush
          << "Still second line.";
// Output: First line.
// Second line. Still second line.
std::setfill(c) - changes the ﬁll character to c. Often used with std::setw.
std::cout << "\nDefault fill: " << std::setw(10) << 79 << '\n'
          << "setfill('#'): " << std::setfill('#')
          << std::setw(10) << 42 << '\n';
// Output:
// Default fill:         79
// setfill('#'): ########79
std::put_money(mon[, intl]) [C++11]. In an expression out << std::put_money(mon, intl), converts the
monetary value mon (of long double or std::basic_string type) to its character representation as speciﬁed by the
std::money_put facet of the locale currently imbued in out. Use international currency strings if intl is true, use
currency symbols otherwise.
long double money = 123.45;
// or std::string money = "123.45";
std::cout.imbue(std::locale("en_US.utf8"));
std::cout << std::showbase << "en_US: " << std::put_money(money)
          << " or " << std::put_money(money, true) << '\n';
// Output: en_US: $1.23 or USD  1.23
std::cout.imbue(std::locale("ru_RU.utf8"));
std::cout << "ru_RU: " << std::put_money(money)
          << " or " << std::put_money(money, true) << '\n';
// Output: ru_RU: 1.23 руб or 1.23 RUB
std::cout.imbue(std::locale("ja_JP.utf8"));
std::cout << "ja_JP: " << std::put_money(money)
          << " or " << std::put_money(money, true) << '\n';
// Output: ja_JP: ￥123 or JPY  123
std::put_time(tmb, fmt) [C++11] - formats and outputs a date/time value to std::tm according to the speciﬁed
format fmt.
tmb - pointer to the calendar time structure const std::tm* as obtained from localtime() or gmtime().
fmt - pointer to a null-terminated string const CharT* specifying the format of conversion.
#include <ctime>
std::time_t t = std::time(nullptr);
std::tm tm = *std::localtime(&t);
std::cout.imbue(std::locale("ru_RU.utf8"));
std::cout << "\nru_RU: " << std::put_time(&tm, "%c %Z") << '\n';
// Possible output:
// ru_RU: Вт 04 июл 2017 15:08:35 UTC
```

For more information see the link above.
Section 14.3: Input stream manipulators
std::ws - consumes leading whitespaces in input stream. It diﬀerent from std::skipws.
```cpp
#include <sstream>
std::string str;
std::istringstream("  \v\n\r\t    Wow!There   is no whitespaces!") >> std::ws >> str;
std::cout << str;
// Output: Wow!There   is no whitespaces!
std::get_money(mon[, intl]) [C++11]. In an expression in >> std::get_money(mon, intl) parses the character
input as a monetary value, as speciﬁed by the std::money_get facet of the locale currently imbued in in, and stores
the value in mon (of long double or std::basic_string type). Manipulator expects required international currency
strings if intl is true, expects optional currency symbols otherwise.
#include <sstream>
#include <locale>
std::istringstream in("$1,234.56 2.22 USD  3.33");
long double v1, v2;
std::string v3;
in.imbue(std::locale("en_US.UTF-8"));
in >> std::get_money(v1) >> std::get_money(v2) >> std::get_money(v3, true);
if (in) {
    std::cout << std::quoted(in.str()) << " parsed as: "
              << v1 << ", " << v2 << ", " << v3 << '\n';
}
// Output:
// "$1,234.56 2.22 USD  3.33" parsed as: 123456, 222, 333
std::get_time(tmb, fmt) [C++11] - parses a date/time value stored in tmb of speciﬁed format fmt.
tmb - valid pointer to the const std::tm* object where the result will be stored.
fmt - pointer to a null-terminated string const CharT* specifying the conversion format.
#include <sstream>
#include <locale>
std::tm t = {};
std::istringstream ss("2011-Februar-18 23:12:34");
ss.imbue(std::locale("de_DE.utf-8"));
ss >> std::get_time(&t, "%Y-%b-%d %H:%M:%S");
if (ss.fail()) {
    std::cout << "Parse failed\n";
}
else {
    std::cout << std::put_time(&t, "%c") << '\n';
}
// Possible output:
// Sun Feb 18 23:12:34 2011
```

For more information see the link above.

