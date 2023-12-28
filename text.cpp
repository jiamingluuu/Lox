#include <string>
#include <iostream>
using namespace std;

int main() {
    string s = "123456.0asd";
    int len = s.length()-1;

    if (s[len] == '0' && s[len-1] == '.') s = s.substr(0, len-1);

    cout << s << "\n";

    return 0;
}
