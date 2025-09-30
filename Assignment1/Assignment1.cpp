// Assignment1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <regex>
using namespace std;

//prototypes
void GetFileNames(string _inFileLocation, string *_outFileLocation);
void ReadAndWrite(string _inFileLocation, string _outFileLocation);
void Exception_TooManyChars(string _string, int _num);
void ReplaceChars(string _char, ofstream* out);
bool ValidatePath(string _path, string _fileExtension);
string GetWorkingDirectory();


struct MyException : public exception
{
public:
	const string what()
	{
		return "String exceeds maximum length of characters.";
	}
};

int main() {

	string dir = GetWorkingDirectory();
	string outFileLocation; // create a string for the file location of the html output file
	GetFileNames("OriginalCPP.cpp", &outFileLocation); // Get the file names from user, use a pointer to set the output file name
	ReadAndWrite("OriginalCPP.cpp", outFileLocation);  // Read from the input file and write to the output file
	cout << "File conversion complete. Find .html file here: " << dir << endl;
    return 0;
}

void GetFileNames(string _inFileLocation, string *_outFileLocation) {
	string in, out; //user input for file names
	bool inPassed = false; // Flag to indicate if input file name has been successfully passed; if passed then skip asking for input file if the user fails at entering the output file
	int maxChars = 250; // Max characters for input file name (260 is the actal max, but leaving some room for path)

	while (true)
	{
		try {
			if (_inFileLocation == "") { // If programmer hasn't provided an input file name
				throw runtime_error("ERROR: Input file name not provided");
			}

			if (!inPassed) {
				cout << "Enter the input file location: (OriginalCPP.cpp) ";
				getline(cin, in); //cin >> in; <-- Had to change to getline to allow for empty input

				// Check if user input matches the provided file names
				if (in != _inFileLocation || in == "") {
					cout << "Input does not match expected file name. Try again." << endl;
					continue;
				}

				if (ValidatePath(in, ".cpp")) { // Redundant but included anyway to showcase versatility of ValidatePath function
					cout << "Input file is valid. Please continue." << endl;
				}
			}

			inPassed = true; // Set flag to true after first successful input

			cout << "Enter the output file location: (eg. assignment1.html) ";
			getline(cin, out); //cin >> out; <-- Had to change to getline to allow for empty input
			
			Exception_TooManyChars(out, maxChars); // Check if output file name exceeds max characters

			if (!ValidatePath(out, ".html")) {
				cout << "Output file must be valid and include an .html extension. Try again." << endl;
				continue;
			}

			*_outFileLocation = out; // Set the output file location via pointer

			break; // Exit the loop if both inputs are valid

		}
		catch (runtime_error& e) { 
			cerr << e.what() << endl;
			break;
		}
		catch (MyException& e)
		{
			cerr << "MyException caught" << endl;
			cerr << e.what() << endl;
		}
		catch (exception& e)
		{
			//Other errors
			cerr << "other error caught" << endl;
			cerr << e.what() << endl;
		}
		catch (...) {
			//Other errors
			cerr << "redundant at this point?" << endl;
		}
	}
}

void ReadAndWrite(string _inFileLocation, string _outFileLocation) {
	//declare streams
	ifstream inStream;
	ofstream outStream;
	string line;

	//connect files to streams
	inStream.open(_inFileLocation);
	outStream.open(_outFileLocation);

	//write html header to output file
	outStream << "<PRE>" << endl;
	//read from input file and write to output file
	if (inStream.is_open() && outStream.is_open()) 
	{
		while (!inStream.eof()) { //loop until end of file
			getline(inStream, line); //read a line from input file
			for (char i : line)
			{
				string fileChar = string(1, i);
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

string GetWorkingDirectory()
{
	//The contents of this function I programmed with assistance from Co-Pilot; I'm using VS as an IDE so I don't have access to <filesystem> in C++14 which is the default for some reason, I'm probably going to use CLion in the future

	char buffer[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buffer);

	return string(buffer);
}

bool ValidatePath(string _path, string _fileExtension)
{
	// REGEX EXPLANATION: (I had difficulty finding a good regex expression online so I used co-pilot to get it)
	//•^ and $ anchor the regex to the start and end of the string.
	//•[^ <>:\"/\\|?*\s] first character: not an invalid character or whitespace.
	//•[^ <>:\"/\\|?*]{0,248} middle characters: up to 248 valid characters. (REDUNDANT BECAUSE OF Exception_TooManyChars)
	//•[^ .<>:\"/\\|?*] last character before extension: not a space, period, or invalid character.
	//•	\.html must end with.html(case-insensitive).

    // Ensure fileExtension starts with a dot (e.g., ".html")
    if (_fileExtension.empty() || _fileExtension[0] != '.')
        throw runtime_error("File extension must start with a dot (e.g., .html)");

    // Escape the dot for regex
    string escapedExt = "\\" + _fileExtension;

    // Build the regex pattern dynamically
    string pattern = R"(^[^<>:\"/\\|?*\s][^<>:\"/\\|?*]{0,248}[^ .<>:\"/\\|?*])" + escapedExt + R"($)"; // I asked co-pilot how to concatenate a string to a regex pattern

    if (!regex_match(_path, regex(pattern, regex_constants::icase))) {
        return false;
    }
    return true;
}
