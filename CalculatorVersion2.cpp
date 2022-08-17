//
// This is example code from Chapter 6.7 "Trying the second version" of
// "Software - Principles and Practice using C++" by Bjarne Stroustrup
//

/*
    This file is known as calculator02buggy.cpp

    I have inserted 5 errors that should cause this not to compile
    I have inserted 3 logic errors that should cause the program to give wrong results

    First try to find an remove the bugs without looking in the book.
    If that gets tedious, compare the code to that in the book (or posted source code)

    Happy hunting!

*/

#include "std_lib_facilities.h"

//------------------------------------------------------------------------------

const char number = '8';
const char quit = 'q';
const char print = ';';
const char result = '=';
const char prompt = '>';
const char let = 'L';
const string declkey = "let";
const char name = 'a';

//------------------------------------------------------------------------------

class Token
{
public:
    char kind;    // what kind of token
    double value; // for numbers: a value
    string name;
    Token(char ch) // make a Token from a char
        : kind(ch), value(0)
    {
    }
    Token(char ch, double val) // make a Token from a char and a double
        : kind(ch), value(val)
    {
    }
    Token(char ch, string s)
        : kind{ch}, name{s}
    {
    }
};

//------------------------------------------------------------------------------

class Token_stream
{
public:
    Token_stream();        // make a Token_stream that reads from cin
    Token get();           // get a Token (get() is defined elsewhere)
    void putback(Token t); // put a Token back
    void ignore(char);

private:
    bool full;    // is there a Token in the buffer?
    Token buffer; // here is where we keep a Token put back using putback()
};

//------------------------------------------------------------------------------

// The constructor just sets full to indicate that the buffer is empty:
Token_stream::Token_stream()
    : full(false), buffer(0) // no Token in buffer
{
}

//------------------------------------------------------------------------------

// The putback() member function puts its argument back into the Token_stream's buffer:
void Token_stream::putback(Token t)
{
    if (full)
        error("putback() into a full buffer");
    buffer = t;  // copy t to buffer
    full = true; // buffer is now full
}

//------------------------------------------------------------------------------

Token Token_stream::get()
{
    if (full)
    { // do we already have a Token ready?
        // remove token from buffer
        full = false;
        return buffer;
    }

    char ch;

    cin >> ch; // note that >> skips whitespace (space, newline, tab, etc.)

    switch (ch)
    {
    case print: // for "print"
    case quit:  // for "quit"
    case '(':
    case ')':
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '=':
        return Token(ch); // let each character represent itself
    case '.':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
        cin.putback(ch); // put digit back into the input stream
        double val;
        cin >> val; // read a floating-point number
        return Token(number, val);
    }
    default:
        if (isalpha(ch))
        {
            string s;
            s += ch;
            while (cin.get(ch) && (isalpha(ch) || isdigit(ch)))
                s += ch;
            cin.putback(ch);
            if (s == declkey)
                return Token{let}; // declaration keyword
            return Token{name, s};
        }
        error("Bad token");
    }
}

//------------------------------------------------------------------------------

// ignores input stream upto character ch
void Token_stream::ignore(char ch)
{

    for (char a = 0; cin >> a, a != ch;)
    {
        // ignores upto ch
    }
}
//------------------------------------------------------------------------------

Token_stream ts; // provides get() and putback()

//------------------------------------------------------------------------------

class variable
{
public:
    string name;
    double value;
    variable(string, double);
};

//------------------------------------------------------------------------------

variable::variable(string s, double x)
{
    name = s;
    value = x;
}

//------------------------------------------------------------------------------

vector<variable> var_table;

//------------------------------------------------------------------------------

double get_value(string s) // get the value of the predefined variable
{
    for (int i = 0; i <= var_table.size(); i++)
    {
        if (var_table[i].name == s)
        {
            return var_table[i].value;
        }
    }
    error("get:undefined variable", s);
}

//------------------------------------------------------------------------------

void set_value(string s, double x) // change the value of the predefined variable
{
    for (int i = 0; i <= var_table.size(); i++)
    {
        if (var_table[i].name == s)
        {
            var_table[i].value = x;
        }
    }
    error("set:undefined variable", s);
}

//------------------------------------------------------------------------------

bool is_declared(string var)
{
    for (int i = 0; i < var_table.size(); i++)
    {
        if (var_table[i].name == var)
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------

double define_name(string var, double val)
{
    if (is_declared(var))
    {
        error("declared twice", var);
    }
    var_table.push_back(variable{var, val});
    return val;
}

//------------------------------------------------------------------------------

double expression(); // declaration so that primary() can call expression()

//------------------------------------------------------------------------------

// deal with numbers and parentheses
double primary()
{
    Token t = ts.get();
    switch (t.kind)
    {
    case '(': // handle '(' expression ')'
    {
        double d = expression();
        t = ts.get();
        if (t.kind != ')')
            error("')' expected");
        return d;
    }
    case number:
        return t.value; // return the number's value
    case '+':
        return primary();
    case '-':
        return -primary();
    default:
        if(is_declared(t.name))
        {
            return get_value(t.name);
        }
        error("primary expected");
    }
}

//------------------------------------------------------------------------------

// deal with *, /, and %
double term()
{
    double left = primary();
    Token t = ts.get(); // get the next token from token stream

    while (true)
    {
        switch (t.kind)
        {
        case '*':
            left *= primary();
            t = ts.get();
            break;
        case '/':
        {
            double d = primary();
            if (d == 0)
                error("divide by zero");
            left /= d;
            t = ts.get();
            break;
        }
        case '%':
        {
            double d = primary();
            if (d == 0)
                error("divide by zero");
            left = fmod(left, d);
            t = ts.get();
            break;
        }
        default:
            ts.putback(t); // put t back into the token stream
            return left;
        }
    }

    return left;
}

//------------------------------------------------------------------------------

// deal with + and -
double expression()
{
    double left = term(); // read and evaluate a Term
    Token t = ts.get();   // get the next token from token stream

    while (true)
    {
        switch (t.kind)
        {
        case '+':
            left += term(); // evaluate Term and add
            t = ts.get();
            break;
        case '-':
            left += term(); // evaluate Term and subtract
            t = ts.get();
            break;
        default:
            ts.putback(t); // put t back into the token stream
            return left;   // finally: no more + or -: return the answer
        }
    }
}

//------------------------------------------------------------------------------

double declaration()
{
    Token t1 = ts.get();
    if (t1.kind != name)
    {
        error("name expexted");
    }
    string var_name = t1.name;

    Token t2 = ts.get();
    if (t2.kind != '=')
        error("missing ssignment operation '=' ", var_name);

    double d = expression();
    define_name(var_name, d);
    return d;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

double statement()
{
    Token t = ts.get();
    {
        if (t.kind == let)
        {
            return declaration();
        }
        ts.putback(t);
        return expression();
    }
}

//------------------------------------------------------------------------------

// cleans up the error
void clean_up_mess()
{
    ts.ignore(print);
}

//------------------------------------------------------------------------------

// general calculation loop
void calculate()
{
    while (cin)
        try
        {
            cout << prompt;
            Token t = ts.get();
            while (t.kind == print)
                t = ts.get();
            if (t.kind == quit)
            {
                break;
            }
            ts.putback(t);
            cout << result << statement() << '\n';
        }
        catch (exception &e)
        {
            cerr << e.what() << '\n';
            clean_up_mess();
        }
}

//------------------------------------------------------------------------------

int main()
try
{
    define_name("pi", 3.1415926535);
    define_name("e", 2.7182818284);

    calculate();

    keep_window_open();
      return 0;
}
catch (exception &e)
{
    cerr << "error: " << e.what() << '\n';

    keep_window_open();
    return 1;
}
catch (...)
{
    cerr << "Oops: unknown exception!\n";
    keep_window_open();
    return 2;
}
