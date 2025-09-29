// Assignment1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <regex>
#include <filesystem> // <-- Found on Google search for "c++ check if file has correct extension" First response AI answer
using namespace std;

//prototypes
void GetFileNames(string _inFileLocation, string *_outFileLocation);
void ReadAndWrite(string _inFileLocation, string _outFileLocation);
void Exception_TooManyChars(string _string, int _num);
void ReplaceChars(string _char, ofstream* out);
void OpenWrite();


struct MyException : public exception
{
public:
	const string what()
	{
		return "String exceeds maximum length of characters.";
	}
};

int main() {
    string outFileLocation; // create a string object
    GetFileNames("OriginalCPP.cpp", &outFileLocation); // pass its address
    ReadAndWrite("OriginalCPP.cpp", outFileLocation);  // pass by value
	OpenWrite();
    return 0;
}

void GetFileNames(string _inFileLocation, string *_outFileLocation) {
    string in, out;
	bool inPassed = false;
	int maxChars = 250; // Max characters for input file name (260 is the actal max, but leaving some room for path)

	while (true)
	{
		try {
			if (_inFileLocation == "") { // If programmer hasn't provided an input file name
				throw runtime_error("ERROR: Input file name not provided");
			}

			if (!inPassed) {
				cout << "Enter the input file location: (OriginalCPP.cpp) ";
				getline(cin, in);
				//cin >> in; <-- Had to change to getline to allow for empty input

				// Check if user input matches the provided file names
				if (in != _inFileLocation || in == "") {
					cout << "Input does not match expected file name. Try again." << endl;
					continue;
					//throw runtime_error("Input does not match expected file names. Try again");
				}

				// -- BELOW IS THE NOT-HARDCODED VERSION OF FILE PATH VALIDATION -- //

				// Comment out code above and uncomment below to use non-hardcoded version

				//cout << "Enter the input C++ file name (OriginalCPP.cpp): ";
				//getline(cin, in);

				//ifstream testInFile;
				//testInFile.open(in);

				//if (testInFile.fail())
				//{
				//	cout << "Input file does not exist. Try again." << endl;
				//	continue;
				//}
				//else if (!regex_match(in, regex(R"(.*\.cpp$)"))) { // Tried google search for extension, this link was close: https://stackoverflow.com/questions/31202524/c-regex-pattern-to-check-file-extension but I utlimately used co-pilot to get this right
				//	cout << "Output file must have a .cpp extension. Try again." << endl;
				//	continue;
				//}

				//if (testInFile.is_open())
				//{
				//	testInFile.close();
				//}

				// -- ABOVE IS THE NOT-HARDCODED VERSION OF FILE PATH VALIDATION -- //

			}

			inPassed = true; // Set flag to true after first successful input

			cout << "Enter the output file location: (eg. assignment1.html) ";
			getline(cin, out);
			//cin >> out; <-- Had to change to getline to allow for empty input

			Exception_TooManyChars(out, maxChars); // Check if output file name exceeds max characters

			// REGEX EXPLANATION: (ACCORDING TO CO-PILOT)
			//•^ and $ anchor the regex to the start and end of the string.
				//•[^ <>:\"/\\|?*\s] first character: not an invalid character or whitespace.
				//•[^ <>:\"/\\|?*]{0,252} middle characters: up to 252 valid characters. (REDUNDANT)
				//•[^ .<>:\"/\\|?*] last character before extension: not a space, period, or invalid character.
				//•	\.html must end with.html(case-insensitive).

			// Check if the file name ends with .html
			if (!regex_match(out, regex(R"(^[^<>:\"/\\|?*\s][^<>:\"/\\|?*]{0,248}[^ .<>:\"/\\|?*]\.html$)", regex_constants::icase))) { // Tried google search for extension, this link was close: https://stackoverflow.com/questions/31202524/c-regex-pattern-to-check-file-extension but I utlimately used co-pilot to get this right
				cout << "Output file must be valid and include an .html extension. Try again." << endl;
				continue; //throw runtime_error("Output file must have a .html extension. Try again");
			}

			if (out == "")
			{
				continue;
			}


			*_outFileLocation = out; // Set the output file location via pointer

			break; // Exit the loop if both inputs are valid

		}
		catch (runtime_error& e) { // & to catch by reference
			cerr << e.what() << endl;
			break;
		}
		catch (MyException& e)
		{
			cout << "MyException caught" << endl;
			cout << e.what() << endl;
		}
		catch (exception& e)
		{
			//Other errors
			cout << "other error caught" << endl;
			cout << e.what() << endl;
		}
		catch (...) {
			//Other errors
			cout << "redundant at this point?" << endl;
		}
	}
}

void ReadAndWrite(string _inFileLocation, string _outFileLocation) {
	//declare stream objects
	ifstream inStream;
	ofstream outStream;
	string line;
	
	cout << "DEBUG: Input file: " << _inFileLocation << endl; // DEBUG
	cout << "DEBUG: Output file: " << _outFileLocation << endl; // DEBUG
	
	//connect files to stream objects
	inStream.open(_inFileLocation);
	outStream.open(_outFileLocation);

	//write html header to output file
	outStream << "<PRE>" << endl;
	//read from input file and write to output file
	if (inStream.is_open() && outStream.is_open()) 
	{
		while (!inStream.eof()) { //loop until end of file
			getline(inStream, line); //read a line from input file
			for (char i : line) // <-- Used co-pilot to change from for (int i = 0; i < line.length(); i++)
			{
				string fileChar = string(1, i); // <-- Used co-pilot to change from char to string for comparison
				ReplaceChars(fileChar, &outStream); //replace < and > with html entities	
			}
			outStream << endl; //write a carriage return for last line copied
		}
	}
	else {
		throw runtime_error("One or both files failed to open");
	}
	//write html footer to output file
	outStream << "</PRE>" << endl;
	//Close files
	inStream.close();
	outStream.close();
}

void Exception_TooManyChars(string _string, int _num) {
	if (_string.length() > _num) {
		throw MyException();
	}
}

void ReplaceChars(string _char, ofstream *out) {
	if (_char == "<") //replace < and > with html entities
		(*out) << "&lt";
	else if (_char == ">")
		(*out) << "&gt";
	else
		(*out) << _char;
}

void OpenWrite() {
	ofstream myFileOut;
	myFileOut.open("myFileOut.txt", ios::app); //open for append

	if (!myFileOut.fail()) {
		myFileOut << "stuff" << " and more stuff" << endl;
		myFileOut << "even more stuff" << endl;
		myFileOut.close();
		cout << "File Closed" << endl;
	}
	else {
		cout << "Output file failed to open";
	}
}

