#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>
#include <windows.h>
#include <string>
#include <fstream>
#include <regex>
#include <map>
#include <cmath>

using namespace std;

int file_count = 0;
int dir_count = 0;
bool running = true;
string current_dir = "";
string line = "";
int filename_length;
string line_copy = "";
string filesize;
string filename;
vector<string> output_dirs = {};
vector<string> search_results = {};
vector<string> systemlinks = {};
string systemlink_path;
string path;
string extension;
vector<string> file_types = {
    ".ods",".mhtml",".ml",".otx",".xlam",".ptm",".xlsb",".ps",".xps",".xlw",
    ".xlsm",".dot",".mht",".dif",".xlt",".doc",".xltm",".txt",".dp",".psm",".csv",".odt",
    ".docx",".xml",".pam",".htm",".dotm",".xls",".xlsx",".ptx",".docm",".rtf",".pdf",".otm",
    ".htm,.html",".xltx",".prn",".wps",".df",".psx",".ps",".html",".slk",".xla",".dotx",
    ".tf",".ot",".dbf",".py"
};
vector<string> largest_folders;
string file_size = "";

map<long long, string> map_filenames;
vector<long long> largest_files;
long long int_file_size;

DWORD WINAPI output(LPVOID lpParameter) {
    int last_count;
    int fps;
    while (running == true) {
        last_count = file_count;
        system("timeout 1 > nul");
        fps = file_count - last_count;
        if (running == true) {
            cout << "Processed " << file_count << " files! FPS=" << fps << endl;
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

vector<string> output_array;
int lowest;
int array_index;
int lowest_value;
int x_value;
int lowest_index;
int x_index;
string temp;

int is_number(string str) {
    if (str.length() == 0) {return false;}
    for (char const &c : str) {
        if (isdigit(c) == 0) {
            return false;
        }
    }
    return true;
}

vector<string> size_labels = {" Bytes", "KB", "MB", "GB", "TB", "PB"};
long double current_number;
int format_counter;
string format_size(long long number) {
    format_counter = 0;
    current_number = number;
    while (current_number > 1024) {
        current_number = current_number / 1024;
        format_counter++;
    }
    current_number = (long long)(current_number * 100) / 100;
    return to_string(current_number) + size_labels.at(format_counter);
}

int spaces_counter;
void get_file_data(string line, string current_dir, string search_term, bool files_only, bool filename_search, bool content_search, 
    bool maxsize, bool systemlink_search) {

    if (line.length() > 15) {
        // check if line contains file
        if (isdigit(line.at(0)) != 0 && isdigit(line.at(15)) != 0 && line.at(2) == '/') {
            if (line.find('<') == string::npos) {

                // get name and size
                spaces_counter = 0;
                for (int i = 18; i < line.length(); i++) {
                    if (line.at(i) != ' ') {
                        break;
                    }
                    spaces_counter++;
                }

                line = line.substr(18 + spaces_counter, line.length());
                filesize = line.substr(0, line.find(' '));
                filename = line.substr(filesize.length() + 1);
                filesize.erase(remove(filesize.begin(), filesize.end(), ','), filesize.end());

                // update vectors
                path = current_dir.substr(0,current_dir.length()-1)+"\\"+filename.substr(0,filename.length()-1);
                int_file_size = stoll(filesize);
                map_filenames.insert(pair<long long, string>(int_file_size, path));

                // filesize search
                if (maxsize == true) {
                    largest_files.push_back(int_file_size);
                }
                
                // filename search
                if (filename_search == true) {
                    if (line.find(search_term) != string::npos) {
                        output_dirs.push_back(path);
                    }
                }

                // content search
                if (content_search == true) {
                    search_file(path, search_term);
                }
                file_count++;

            // search for <SYMLINKD>
            } else if (systemlink_search == true && line.find("<SYMLINKD>") != string::npos) {
                //cout << "line: " << line << endl;
                systemlink_path = line.substr(line.find('['), line.length());
                systemlinks.push_back("<SYMLINKD> " + systemlink_path + "Located in: " + current_dir);
            }

        }
    }

}

void search_for_file(const char* command, string search_term, bool files_only, bool filename_search, bool content_search, bool maxsize,
        bool systemlink_search) {

    CreateThread(NULL, 0, output, NULL, 0, NULL);
    char buffer[256];
    vector<string> file_hashes = {};
    vector<string> file_names = {};
    FILE* pipe = _popen(command, "r");

    if (files_only == true) {
        search_term = ' ' + search_term;
    }

    while (fgets(buffer, sizeof buffer, pipe) != NULL) {
        try {
            // convert char to string
            line = string(buffer);

            // check if line contains a directory
            if (line.find(" Directory of") != string::npos) {
                current_dir = line.substr(14);
                dir_count++;

            } else {
                get_file_data(line, current_dir, search_term, files_only, filename_search, content_search, maxsize, systemlink_search);
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

    // largest files
    if (maxsize == true) {
        running = false;
        cout << "\n-------- Largest files --------\n" << endl;

        sort(largest_files.begin(), largest_files.end());
        reverse(largest_files.begin(), largest_files.end());
        long long current;

        for (int i = 0; i < largest_files.size(); i++) {
            if (i > 200) {break;}
            current = largest_files.at(i);
            cout << format_size(current) << "|\t" << map_filenames.at(current) << endl;

        }
    }

    // systemlink search
    if (systemlink_search == true) {
        running = false;
        cout << "\n-------- Systemlink results" << endl;
        if (systemlinks.size() > 0) {
            for (int i = 0; i < systemlinks.size(); i++) {
                cout << systemlinks.at(i) << endl;
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
    cout << "V2\nMenu:\n1. Search by filename\n2. Search by file content\n3. Search by both filename and content";
    cout << "\n4. Find Largest files\n5. Find System Links\n> ";
    
    string choice;
    getline(cin, choice);

    // get user input path
    cout << "Enter path: ";
    string user_input = "";
    getline(cin, user_input);
    string command = "dir \"" + user_input + "\" /s /a";

    // search term
    string search_term = "";
    if (choice != "4" && choice != "5") {
        cout << "Enter Search term: ";
        getline(cin, search_term);
    }
    
    // convert string to char
    char buffer[270];
    strcpy(buffer, command.c_str());

    // execute command
    if (choice == "1") {
        search_for_file(buffer, search_term, false, true, false, false, false);
    } else if (choice == "2") {
        search_for_file(buffer, search_term, false, false, true, false, false);
    } else if (choice == "3") {
        search_for_file(buffer, search_term, false, true, true, false, false);
    } else if (choice == "4") {
        search_for_file(buffer, search_term, false, false, false, true, false);
    } else if (choice == "5") {
        search_for_file(buffer, search_term, false, false, false, false, true);
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