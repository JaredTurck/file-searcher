#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>
#include <windows.h>
#include <string>
#include <fstream>

using namespace std;

int file_count = 0;
int dir_count = 0;
bool running = true;
string current_dir = "";
string line = "";
int filename_length;
string line_copy = "";
string filename;
vector<string> output_dirs = {};
vector<string> search_results = {};
string path;
string extension;
vector<string> file_types = {
    ".ods",".mhtml",".ml",".otx",".xlam",".ptm",".xlsb",".ps",".xps",".xlw",
    ".xlsm",".dot",".mht",".dif",".xlt",".doc",".xltm",".txt",".dp",".psm",".csv",".odt",
    ".docx",".xml",".pam",".htm",".dotm",".xls",".xlsx",".ptx",".docm",".rtf",".pdf",".otm",
    ".htm,.html",".xltx",".prn",".wps",".df",".psx",".ps",".html",".slk",".xla",".dotx",
    ".tf",".ot",".dbf",".log",".py"
};

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

void search_file(string filename, string term) {
    // checks if a file contains a search term

    if (filename.find('.') != string::npos) {
        // check if extension in allowed file types
        extension = filename.substr(filename.find('.'));
        if (std::find(file_types.begin(), file_types.end(), extension) != file_types.end()) {
            // read file
            ifstream file(filename);
            int line_num = 0;
            while (getline(file, line)) {
                if (line.find(term) != std::string::npos) {
                    search_results.push_back(filename + " at line " + std::to_string(line_num));
                    return;
                }
                line_num++;
            }
        }
    }
}

string get_file_path(string search_term) {
    // get filename
    if (line.length() > 0) {
        line_copy = line;                               // copy string
        reverse(line_copy.begin(), line_copy.end());    // reverse copy
        filename_length = line_copy.find(' ');          // length of filename
        filename = line.substr(line.length() - filename_length);

        if (filename.find('.') != string::npos) {
            path = current_dir.substr(0,current_dir.length()-1)+"\\"+filename.substr(0,filename.length()-1);
            return path;
        }
    }
    return "";
}

void search_for_file(const char* command, string search_term, bool files_only, bool filename_search, bool content_search) {
    CreateThread(NULL, 0, output, NULL, 0, NULL);
    char buffer[128];
    vector<string> file_hashes = {};
    vector<string> file_names = {};
    FILE* pipe = _popen(command, "r");

    if (files_only == true) {
        search_term = ' ' + search_term;
    }

    while (fgets(buffer, sizeof buffer, pipe) != NULL) {
        try {
            //cout << buffer;

            // convert string to char
            line = string(buffer);

            // check if line contains a directory
            if (line.find(" Directory of") != string::npos) {
                current_dir = line.substr(14);
                dir_count++;

            } else {
                // check if search term in line
                if (filename_search == true) {
                    if (line.find(search_term) != string::npos) {

                        // get filename
                        if (line.length() > 0) {
                            line_copy = line;                               // copy string
                            reverse(line_copy.begin(), line_copy.end());    // reverse copy
                            filename_length = line_copy.find(' ');          // length of filename

                            if (line.length() > filename_length) {
                                filename = line.substr(line.length() - filename_length);
                                if (filename.find('.') != string::npos) {
                                    path = current_dir.substr(0,current_dir.length()-1)+"\\"+filename.substr(0,filename.length()-1);
                                    output_dirs.push_back(path);
                                }
                            }

                        }

                    } 
                    
                    else if (line.size() > 0) {
                        file_count++;
                    }
                }
                
                if (content_search == true) {
                    // get filename
                    if (line.length() > 0) {
                        line_copy = line;                               // copy string
                        reverse(line_copy.begin(), line_copy.end());    // reverse copy
                        filename_length = line_copy.find(' ');          // length of filename

                        if (line.length() > filename_length) {
                            filename = line.substr(line.length() - filename_length);
                            if (filename.find('.') != string::npos) {
                                path = current_dir.substr(0,current_dir.length()-1)+"\\"+filename.substr(0,filename.length()-1);
                                search_file(path, search_term);
                            }
                        }

                    }

                }
                
            }
        } catch (int error) {
            cout << "error thrown!" << endl;
        }
    }

    // filename result
    if (filename_search == true) {
        running = false;
        cout << "\n-------- Search results --------\n" << endl;
        if (output_dirs.size() > 0) {
            for (int i = 0; i < output_dirs.size(); i++) {
                cout << output_dirs.at(i) << endl;
            }
        } else {
            cout << "No search results found!" << endl;
        }
    }

    // file content search
    if (content_search == true) {
        running = false;
        cout << "\n-------- File content search results --------\n" << endl;
        if (search_results.size() > 0) {
            for (int i = 0; i < search_results.size(); i++) {
                cout << search_results.at(i) << endl;

            }
        } else {
            cout << "No search results found!" << endl;
        }
    }
    
    // close handle
    _pclose(pipe);
    return;

}

void search_by_filename() {
    // menu
    cout << "V2\nMenu:\n1. Search by filename\n2. Seatch by file content\n3. search by both filename and content\n> ";
    string choice;
    getline(cin, choice);

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
    if (choice == "1") {
        search_for_file(buffer, search_term, false, true, false);
    } else if (choice == "2") {
        search_for_file(buffer, search_term, false, false, true);
    } else if (choice == "3") {
        search_for_file(buffer, search_term, false, true, true);
    }
    
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