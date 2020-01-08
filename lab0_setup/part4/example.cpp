#include <iostream>
#include <string>

int main() {
	int myArray[10];
	
	for (int ii = 0; ii < 10; ++ii) {
		myArray[ii] = ii;
		std::cout << ii << " ";
	}
	
	std::string s="hello";
	
	for (char c : s) {
		std::cout << c;
	}
}
