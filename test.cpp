#include <iostream>
#include <string>

using namespace std;

int main ()
{

  string in;

  cout << "string:";
  cin >> in;
  while (in.find("$END")== string::npos)
  {
      cout << "\nI'm still looping!\n";
      cin >> in;
  }

  cout << "\n\n\nClosing program";

  return 0;
}
