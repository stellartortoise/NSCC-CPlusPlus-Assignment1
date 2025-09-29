// Assignment1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <string>
#include <fstream>
#include <stdexcept>
#include <regex>
#include <filesystem> // <-- Found on Google search for "c++ check if file has correct extension" AI answer
using namespace std;

//prototypes
void OpenFile();
bool isValidWindowsFileName(const string& name);
void OpenPrint();
void OpenWrite();
void GetFileNames(string _inFileLocation, string *_outFileLocation);
void ReadAndWrite(string _inFileLocation, string _outFileLocation);
void Exception_TooManyChars(string _string, int _num);
void ReplaceChars(string _char, ofstream* out);


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
    return 0;
}

// Helper function to check for invalid Windows filename characters
bool isValidWindowsFileName(const string& name) { //<--- Co-pilot generated function; could not figure this out myself
	// Invalid characters: < > : " / \ | ? *
	return !name.empty() && name.find_first_of("<>:\"/\\|?*") == string::npos && name.length() <= 255;
}

// less than &#060;
// greater than &#062;

void OpenFile() {
	cout << "Files Open" << endl;
	//declare stream objects
	ifstream inStream;
	ofstream outStream; // Fixed typo
	//connect files to stream objects
	inStream.open("OriginalCPP.cpp");
	outStream.open("assignment1.html");
	//Close files
	inStream.close();
	outStream.close();
	cout << "Files Closed" << endl;
}

void OpenPrint() {
	string line;
	ifstream myFileIn;

	myFileIn.open("test.txt");

	if (myFileIn.is_open()) {
		while (!myFileIn.eof()) {
			getline(myFileIn, line);
			cout << line << endl;
		}
		myFileIn.close();
	}
	else {
		cout << "Input file failed to open";
	}
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

void GetFileNames(string _inFileLocation, string *_outFileLocation) {
    string in, out;
	bool inPassed = false;
	int maxChars = 15; // Max characters for input file name

	while (true)
	{
		try {
			if (_inFileLocation == "") {
				throw runtime_error("Input file name not provided");
			}

			if (!inPassed) {
				//cout << "Enter the input file location: (OriginalCPP.cpp) ";
				//getline(cin, in);
				////cin >> in; <-- Had to change to getline to allow for empty input

				//// Check if user input matches the provided file names
				//if (in != _inFileLocation || in == "") {
				//	cout << "Input does not match expected file names. Try again." << endl;
				//	continue;
				//	//throw runtime_error("Input does not match expected file names. Try again");
				//}

				cout << "Enter the input C++ file name (e.g., OriginalCPP.cpp): ";
				getline(cin, in);

				if (!isValidWindowsFileName(in) || in.length() < 5 || in.substr(in.length() - 4) != ".cpp") {
					cout << "Invalid input file name. Must be a valid Windows file name ending with .cpp and not contain < > : \" / \\ | ? *" << endl;
					continue;
				}

			}

			inPassed = true; // Set flag to true after first successful input

			cout << "Enter the output file location: (assignment1.html) ";
			getline(cin, out);
			//cin >> out; <-- Had to change to getline to allow for empty input

			Exception_TooManyChars(out, maxChars); // Check if output file name exceeds 255 characters

			// Check if the file name ends with .html
			if (!regex_match(out, regex(R"(.*\.html$)"))) { // Tried google search for extension, this link was close: https://stackoverflow.com/questions/31202524/c-regex-pattern-to-check-file-extension but I utlimately used co-pilot to get this right
				cout << "Output file must have a .html extension. Try again." << endl;
				continue; //throw runtime_error("Output file must have a .html extension. Try again");
			}

			if (out == "")
			{
				continue;
			}

			*_outFileLocation = out; // Set the output file location via pointer

			break; // Exit the loop if both inputs are valid

			// Validate Windows file path
			//cout << "File names and paths are valid." << endl;
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

