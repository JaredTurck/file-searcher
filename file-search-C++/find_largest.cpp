#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>
#include <windows.h>
#include <string>

using namespace std;

int file_count = 0;
int dir_count = 0;
bool running = true;

DWORD WINAPI output(LPVOID lpParameter) {
    int last_count;
    int fps;
    int actual_file_count = 0;
    while (running == true) {
        last_count = file_count;
        system("timeout 1 > nul");
        fps = file_count - last_count;
        if (running == true) {
            actual_file_count = file_count - (dir_count * 2);
            cout << "Processed " << actual_file_count << " files! FPS=" << fps << endl;
        }
    }
    return 0;
}

void search_for_file(const char* command, string search_term, bool files_only) {
    CreateThread(NULL, 0, output, NULL, 0, NULL);
    char buffer[128];
    string current_dir = "";
    string line = "";
    int filename_length;
    string line_copy = "";
    string filename;
    vector<string> output_dirs = {};
    string path;

    FILE* pipe = _popen(command, "r");

    if (files_only == true) {
        search_term = ' ' + search_term;
    }

    while (fgets(buffer, sizeof buffer, pipe) != NULL) {
        //cout << buffer;

        // convert string to char
        line = string(buffer);

        // check if line contains a directory
        if (line.find(" Directory of") != string::npos) {
            current_dir = line.substr(14);
            dir_count++;

        } else {
            // check if search term in line
            if (line.find(search_term) != string::npos) {

                // get filename
                line_copy = line;                               // copy string
                reverse(line_copy.begin(), line_copy.end());    // reverse copy
                filename_length = line_copy.find(' ');          // length of filename
                filename = line.substr(line.length() - filename_length);

                if (filename.find('.') != string::npos) {
                    path = current_dir.substr(0,current_dir.length()-1)+"\\"+filename.substr(0,filename.length()-1);
                    output_dirs.push_back(path);
                }
            } else if (isdigit(line.at(0))) {
                file_count++; // increase file count
            }
            
        }
    }

    // result
    running = false;
    cout << "\n-------- Search results --------\n" << endl;
    if (output_dirs.size() > 0) {
        for (int i = 0; i < output_dirs.size(); i++) {
            cout << output_dirs.at(i) << endl;
        }
    } else {
        cout << "No search results found!" << endl;
    }
    
    // close handle
    _pclose(pipe);
    return;

}

void search_by_filename() {
    // get user input path
    cout << "Enter path: ";
    string user_input = "";
    getline(cin, user_input);
    string command = "dir \"" + user_input + "\" /s /a";

    // search term
    cout << "Enter Search term: ";
    string search_term = "";
    getline(cin, search_term);

    // convert string to char
    char buffer[270];
    strcpy(buffer, command.c_str());

    // execute command
    search_for_file(buffer, search_term, false);
}

int main() {

    // create threads
    //std::thread t1(output);

    search_by_filename();

    system("pause");
}

/*
- ask user for path

*/