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
int last_file_count = 1;
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
vector<string> folders_and_subdirs = {};
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
string file_size = "";
map<long long, string> map_filenames;
vector<long long> largest_files;
map<long long, string> map_folders;
vector<long long> largest_folders;
map<long long, string> map_folders_and_subdirs;
vector<long long> largest_folders_and_subdirs;
long long int_file_size;
int is_number(string str) {
    if (str.length() == 0) {return false;}
    for (char const &c : str) {
        if (isdigit(c) == 0) {
            return false;
        }
    }
    return true;
}

string get_last_path() {
    // get temp path
    string line;
    string user_input = "";
    char buffer[256] = "echo %temp%";
    FILE* pipe = _popen(buffer, "r");
    while (fgets(buffer, sizeof buffer, pipe) != NULL) {}
    string temp_path = string(buffer);
    temp_path.erase(remove(temp_path.begin(), temp_path.end(), '\n'), temp_path.end());
    //cout << string(buffer) << endl;

    // read file
    ifstream file(string(temp_path) + "\\find_largest_path.txt");
    while (getline(file, line)) {}

    if (line.length() == 0) {
        // ask user for input
        cout << "Enter path: ";
        getline(cin, user_input);

        // write to file
        cout << "wrote to file" << endl;
        ofstream file(string(temp_path) + "\\find_largest_path.txt");
        file << user_input;
        return user_input;

    } else {
        cout << "enter path or press enter to use " << line << ": ";
        getline(cin, user_input);
        if (user_input.length() == 0) {
            return line;
        } else {
            return user_input;
        }
    }
}

DWORD WINAPI output(LPVOID lpParameter) {
    int last_count;
    int fps;
    int current_percent;
    while (running == true) {
        last_count = file_count;
        system("timeout 1 > nul");
        fps = file_count - last_count;
        if (running == true) {
            if (last_file_count > 1) {
                int percent = (file_count * 100) / last_file_count;
                cout << "Processed " << file_count << " files! FPS=" << fps << ", " << percent << " complete" << endl;
            } else {
                cout << "Processed " << file_count << " files! FPS=" << fps << endl;
            }
            
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
void search_folders_and_subdirs() {
    char buffer[256];
    int logging;
    string output;
    string command;
    int index;
    float percent;
    //folders_and_subdirs
    for (int i = 0; i < folders_and_subdirs.size(); i++) {
        // convert string to char
        current_dir = folders_and_subdirs.at(i);
        command = "dir \"" + current_dir.substr(0, current_dir.length() - 1) + "\" /s /a";
        strcpy(buffer, command.c_str());
        FILE* pipe = _popen(buffer, "r");
        logging = false;
        output = "";
        // get size
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            file_count++;
            if (string(buffer).find("Total Files Listed") != string::npos) {
                fgets(buffer, sizeof buffer, pipe);
                break;
            }
        }
        // extract size
        filesize = string(buffer);
        filesize = filesize.substr(0, filesize.find("bytes"));
        filesize = filesize.substr(filesize.find("File(s)") + 7, filesize.length());
        filesize.erase(remove(filesize.begin(), filesize.end(), ' '), filesize.end());
        filesize.erase(remove(filesize.begin(), filesize.end(), ','), filesize.end());
        if (is_number(filesize) == true) {
            int_file_size = stoll(filesize);
            map_folders_and_subdirs.insert(pair<long long, string>(int_file_size, current_dir));
            largest_folders_and_subdirs.push_back(int_file_size);
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
    bool maxsize_files, bool systemlink_search, bool maxsize_folder) {
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
                if (maxsize_files == true) {
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
        // largest folders search
        if (maxsize_folder == true && line.find("File(s)") != string::npos) {
            line = line.substr(0, line.find("bytes"));
            line = line.substr(line.find("File(s)") + 7, line.length());
            line.erase(remove(line.begin(), line.end(), ' '), line.end());
            line.erase(remove(line.begin(), line.end(), ','), line.end());
            int_file_size = stoll(line);
            map_folders.insert(pair<long long, string>(int_file_size, current_dir));
            largest_folders.push_back(int_file_size);
        }
    }
}
void search_for_file(const char* command, string search_term, bool files_only, bool filename_search, bool content_search, bool maxsize_files,
        bool systemlink_search, bool maxsize_folder, bool folders_and_subdirs_search) {
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
                if (folders_and_subdirs_search == true) {
                    folders_and_subdirs.push_back(current_dir);
                }
                dir_count++;
            } else {
                get_file_data(line, current_dir, search_term, files_only, filename_search, content_search, maxsize_files, 
                    systemlink_search, maxsize_folder);
            }
        } catch (int error) {
            cout << "error thrown!" << endl;
        }
    }
    // start folders sub dir search
    if (folders_and_subdirs_search == true) {
        search_folders_and_subdirs();
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
    if (maxsize_files == true) {
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
    // largest folders
    if (maxsize_folder == true) {
        running = false;
        cout << "\n-------- Largest folders --------\n" << endl;
        sort(largest_folders.begin(), largest_folders.end());
        reverse(largest_folders.begin(), largest_folders.end());
        long long current;
        for (int i = 1; i < largest_folders.size(); i++) {
            if (i > 200) {break;}
            current = largest_folders.at(i);
            cout << format_size(current) << "|\t" << map_folders.at(current);
        }
    }
    // largest folders and subdirs
    if (folders_and_subdirs_search == true) {
        running = false;
        cout << "\n-------- Largest folders and sub folders --------\n" << endl;
        sort(largest_folders_and_subdirs.begin(), largest_folders_and_subdirs.end());
        reverse(largest_folders_and_subdirs.begin(), largest_folders_and_subdirs.end());
        long long current;
        for (int i = 1; i < largest_folders_and_subdirs.size(); i++) {
            if (i > 200) {break;}
            current = largest_folders_and_subdirs.at(i);
            cout << format_size(current) << "|\t" << map_folders_and_subdirs.at(current);
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
    cout << "Menu:\n1. Search by filename\n2. Search by file content\n3. Search by both filename and content";
    cout << "\n4. Find Largest files\n5. Find Largest folders\n6. Find Largest folders (and sub folders)\n7. Find System Links\n> ";
    
    string choice;
    getline(cin, choice);

    // get user input path
    string command = "dir \"" + get_last_path() + "\" /s /a";

    // search term
    string search_term = "";
    if (choice != "4" && choice != "5" && choice != "6") {
        cout << "Enter Search term: ";
        getline(cin, search_term);
    }
    
    // convert string to char
    char buffer[270];
    strcpy(buffer, command.c_str());
    // execute command
    if (choice == "1") {
        search_for_file(buffer, search_term, false, true, false, false, false, false, false);
    } else if (choice == "2") {
        search_for_file(buffer, search_term, false, false, true, false, false, false, false);
    } else if (choice == "3") {
        search_for_file(buffer, search_term, false, true, true, false, false, false, false);
    } else if (choice == "4") {
        search_for_file(buffer, search_term, false, false, false, true, false, false, false);
    } else if (choice == "5") {
        search_for_file(buffer, search_term, false, false, false, false, false, true, false);
    } else if (choice == "6") {
        search_for_file(buffer, search_term, false, false, false, false, false, false, true);
    } else if (choice == "7") {
        search_for_file(buffer, search_term, false, false, false, false, true, false, false);
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