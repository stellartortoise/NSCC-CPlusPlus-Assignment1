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
bool ValidatePath(string _file);


struct MyException : public exception
{
public:
	const string what()
	{
		return "String exceeds maximum length of characters.";
	}
};

int main() {

	char buffer[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buffer);
	string dir = string(buffer);

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

				if (!ValidatePath(in)) // Validate the file path
				{
					continue;
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
			getline(cin, out); //cin >> out; <-- Had to change to getline to allow for empty input
			
			Exception_TooManyChars(out, maxChars); // Check if output file name exceeds max characters

			// REGEX EXPLANATION: (ACCORDING TO CO-PILOT)
			//•^ and $ anchor the regex to the start and end of the string.
				//•[^ <>:\"/\\|?*\s] first character: not an invalid character or whitespace.
				//•[^ <>:\"/\\|?*]{0,252} middle characters: up to 252 valid characters. (REDUNDANT)
				//•[^ .<>:\"/\\|?*] last character before extension: not a space, period, or invalid character.
				//•	\.html must end with.html(case-insensitive).

			// Check if the file name ends with .html
			if (!regex_match(out, regex(R"(^[^<>:\"/\\|?*\s][^<>:\"/\\|?*]{0,248}[^ .<>:\"/\\|?*]\.html$)", regex_constants::icase))) { // Tried google search for extension, this link was close: https://stackoverflow.com/questions/31202524/c-regex-pattern-to-check-file-extension but I utlimately used co-pilot to get the right pattern
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
		catch (runtime_error& e) { 
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

bool ValidatePath(string _file)
{

	if (_file.empty()) {
		throw runtime_error("File path cannot be empty.");
	}

	ifstream fStream(_file);

	fStream.open(_file);

	if (!fStream.is_open()) {
		throw runtime_error("File does not exist.");
		return false;
	}
	else
	{
		fStream.close();
	}
	// For some reason the code above in this function crashes the program when it should throw an exception. I'm leaving this for now.

	//Get the working directory
	char buffer[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buffer);
	
	//Concatenate the working directory with the file name
	string path = string(buffer) + "\\" + _file;

	// Pattern explanation:
	//(ACCORDING TO CO - PILOT)
	// 
	//	Examples That Match
	//	C : \Users\Alexander\Documents\file.cpp
	//	file.html
	//	folder\subfolder\file.txt
	// 
	//	Examples That Don't Match
	//	C: / path / to / file.cpp(uses / instead of \)
	//	file ? .cpp(contains ? )
	//	file(no extension)

	if (!regex_match(path, regex(R"(^([a-zA-Z]:\\)?([^<>:\"/\\|?*\r\n]+\\)*[^<>:\"/\\|?*\r\n]+\.[a-zA-Z0-9]+$)", regex_constants::icase))) { // Tried google search for extension, this link was close: https://stackoverflow.com/questions/31202524/c-regex-pattern-to-check-file-extension but I utlimately used co-pilot to get the right pattern
		throw runtime_error("File path is not valid. Ensure it uses backslashes and has a valid extension.");
		return false;
	}

	cout << "File path is valid: " << path << endl;
	return true;
}


