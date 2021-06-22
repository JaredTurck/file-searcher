import os, time, pathlib, hashlib, random, tkinter
from tkinter import filedialog

# find largest file
class find_largest():
    def __init__(self):
        self.step_size = 100
        self.files_size = []
        self.count = 0
        self.files = []
        self.print_time = 5
        self.fps = 0
        self.paths = {}
        self.log_filename = "output.log"
        self.dirs_count = 0
        self.stop_count = False
        self.dir_first_run = True
        self.search_by_content = False
        self.total_dir_count = 0
        self.search_results = []
        self.file_types = ['.ods', '.mhtml', '.ml', '.otx', '.xlam', '.ptm', '.xlsb', '.ps', '.xps', '.xlw',
            '.xlsm', '.dot', '.mht', '.dif', '.xlt', '.doc', '.xltm', '.txt', '.dp', '.psm', '.csv', '.odt',
            '.docx', '.xml', '.pam', '.htm', '.dotm', '.xls', '.xlsx', '.ptx', '.docm', '.rtf', '.pdf', '.otm',
            '.htm, .html', '.xltx', '.prn', '.wps', '.df', '.psx', '.ps', '.html', '.slk', '.xla', '.dotx',
            '.tf', '.ot', '.dbf', '.log', '.py']
        
    def walk(self, args):
        """ Walk through files in file system, lists all files and sub files"""
        self.files_size = []
        self.count = 0
        self.start_time = time.time()
        name = args[0]
        do_dirs = args[1]
        do_subdirs, self.search_term = args[2], args[3].lower()
        self.walking_in = ["folders" if do_dirs == True else "files"][0]
        self.update_log("", new_log=True)
        self.print_log('[+] walking '+self.walking_in+' in ' + name)
        for path, dirs, files in os.walk(name):
            # process folders
            if do_dirs == True:
                # process the folder
                if do_subdirs == False:
                    if path not in self.paths:
                        sub_files = []
                        for f in files:
                            try:
                                p = os.path.join(path, f)
                                sub_files.append(os.path.getsize(p))
                                self.update_fps()

                                # search file
                                if self.search_term != b"":
                                    # search by content
                                    if self.search_by_content == True:
                                        extension = "." + p.split("\\")[-1].split(".")[-1]
                                        if extension in self.file_types:
                                            # search name
                                            if self.search_term in f.encode("utf-8"):
                                                self.search_results.append(p)
                                            else:
                                                # search contents
                                                searched_file = open(p, "rb").read().lower()
                                                if self.search_term in searched_file:
                                                    self.search_results.append(p)
                                            
                                            
                                    # search by name
                                    else:
                                        if self.search_term in f.encode("utf-8"):
                                            self.search_results.append(p)
                                            
                            except Exception as error:
                                self.print_log("[-] Error thrown when process file!", error)
                        self.paths[path] = sum(sub_files)

                # process sub folders
                elif do_subdirs == True:
                    # get size of sub files
                    sub_folder_size = 0
                    for sub_path, sub_dir, sub_files in os.walk(path):
                        for sub_file in sub_files:
                            try:
                                p = os.path.join(sub_path, sub_file)
                                sub_folder_size += os.path.getsize(p)
                                self.update_fps()
                            except Exception as error:
                                self.print_log("[-] Error thrown when process file!", error)
                        if self.dir_first_run == True:
                            self.total_dir_count += len(sub_dir)

                    # add to dict
                    self.paths[path] = sub_folder_size
                    self.dirs_count += len(dirs)
                    self.dir_first_run = False
                    
            # process files
            else:
                for file in files:
                    try:
                        p = os.path.join(path, file)
                        size = os.path.getsize(p)
                        self.files_size.append([size, p])
                        self.update_fps()
                    except PermissionError:
                        self.print_log("[-] Failed to read file! Access is denied")
                    except Exception as error:
                        self.print_log("[-] Failed to read file!", error)
                    
        self.print_log("[+] Finished walking!")

        if do_dirs == True:
            temp_list = []
            for key in self.paths.keys():
                temp_list.append([self.paths[key], key])
            self.files = list(reversed(sorted(temp_list)))
        else:
            self.files = list(reversed(sorted(self.files_size)))
        self.write_csv()

    def update_fps(self):
        """ Calculate Files Per Second (FPS)"""
        self.count += 1
        self.fps += 1
        if self.dirs_count > 0 and self.stop_count == False:
            self.stop_count = True
            self.final_file_count = self.count
        if self.count % self.step_size == 0:
            if time.time()-self.start_time > self.print_time:
                self.start_time = time.time()
                self.cfps =  "{:,}".format(int(self.fps / self.print_time))
                self.fps = 0
                
                # update log
                if self.stop_count == True:
                    prct = round((self.dirs_count / self.total_dir_count)*100, 2)
                    dir_test = " files! fps="+self.cfps + " dirs="+str(self.dirs_count)+" prct="+str(prct)
                    self.print_log('[+] Walked', "{:,}".format(self.final_file_count) + dir_test)
                else:
                    self.print_log('[+] Walked', "{:,}".format(self.count), "files! fps="+self.cfps)

    def update_log(self, data, new_log=False, close=False):
        """ Create log file if it doesn't exist"""
        if new_log == True:
            self.print_log("[+] New log started!")
            open(self.log_filename, "w").close()
            self.log_handler = open(self.log_filename, "ab")
            
        elif os.path.exists(self.log_filename):
            try:
                self.log_handler.write(data.encode("utf-8") + b"\n")
            except Exception as error:
                print("[-] Failed to write to log!", error)
        elif os.path.exists(self.log_filename) == False:
            open(self.log_filename, "w").close()

        if close == True:
            self.log_handler.close()

    def print_log(self, *text):
        """ Print text and write it to log"""
        txt = " ".join([str(i) for i in list(text)])
        print(txt)
        self.update_log(txt)

    def gb(self, num):
        """ Convert bytes to diffrent sizes of storage"""
        terms = [" bytes", "KB", "MB", "GB", "TB", "PB"]
        count = 0
        if type(num) == str:
            if num.isdigit()==True:
                num = float(num)
            else:
                return num
            
        while num > 1024:
            num = num / 1024
            count += 1

        if "." in str(num):
            return (str(num).split('.')[0]+"."+str(num).split('.')[1][:2]) + terms[count]
        else:
            return str(num) + terms[count]

    def first_100(self, step=100):
        """ print the first X number of results"""
        # folders and files
        self.print_log("\n", step, "largest "+self.walking_in+":\n"+("-"*30))
        count = 1
        for file in self.files[0:step]:
            self.print_log(str(count)+"|\t" + self.gb(file[0]) + "\t" + file[1].replace('\\', '/'))
            count += 1
        self.update_log("", new_log=False, close=True)

        # file search results
        if self.search_term != b"":
            print("\n Search Results for "+str(self.search_term)[1:]+":\n"+("-"*30))
            if len(self.search_results) == 0:
                print("No results found!")
            else:
                for i,result in enumerate(self.search_results):
                    print(str(i+1)+"|\t" + result.replace("\\", "/"))
        os.popen("pause")

    def write_csv(self):
        """ Write the results to a CSV file"""
        # paths
        if len(self.paths.keys()) > 0:
            print("[+] Writing Paths to CSV!")
            path_writer = open("paths.csv", "wb")
            for key in self.paths.keys():
                path_writer.write((str(self.paths[key])+","+key+"\n").encode("utf-8"))
            path_writer.close()

        # files
        if len(self.files) > 0:
            print("[+] Writing Files to CSV!")
            file_writer = open("files.csv", "wb")
            for item in self.files:
                file_writer.write((str(item[0])+","+item[1]+"\n").encode("utf-8"))
            file_writer.close()

    def get_input(self, finder):
        """ Get input from user via console"""
        # return sys drive if input empty
        user_dir = input('Dir Path: ')
        if user_dir == "":
            user_dir = os.getenv("SystemDrive") + "\\"
        else:
            # check if input is valid path
            if os.path.exists(user_dir) == False:
                self.print_log("Nov a valid directory, please enter a valid dir e.g. C:\\")
            while os.path.exists(user_dir) == False:
                user_dir = input('Invalid Directory\nDir Path: ')

        # files or folders
        self.step_size = 100
        user_files = input("Menu:\n1. Files\n2. Folders\n3. Folders and sub folders (very slow)" +\
                           "\n4. Search files by content (very slow)\n5. Search files by name (fast)\n> ")
        if user_files[0:6].lower() in ["folder", "2"]:
            return [user_dir, True, False, b""]
        elif user_files[0:6].lower() in ["sub", "subfolders", "3"]:
            return [user_dir, True, True, b""]
        elif user_files[0:6].lower() in ["s", "search", "4"]:
            self.step_size = 1
            self.search_by_content = True
            search = input("search term: ").encode("utf-8")
            return [user_dir, True, False, search]
        elif user_files[0:6].lower() in ["n", "search name", "5"]:
            search = input("search term: ").encode("utf-8")
            return [user_dir, True, False, search]
        else:
            return [user_dir, False, False, b""]

def finder_main():
    """ The main function for finder"""
    finder = find_largest()
    while True:
        finder.walk(finder.get_input(finder))
        finder.first_100(step=10)
        menu = input("Press enter to continue or type e to exit...")
        if menu == "e":
            break

# Sort files
class sort_files():
    def __init__(self):
        self.peper = b"JaredBot1234_"
        self.check_count = 200
        self.chunk_size = 65536

    def hash_file(self, path):
        """ Generates MD5 hash of file"""
        md5 = hashlib.md5()
        with open(path, 'rb') as f:
            while True:
                data = f.read(self.chunk_size)
                if not data:
                    break
                md5.update(data)
                
        return md5.hexdigest()

    def remove_duplicates(self, path):
        """ Deletes duplicate files in a directory"""
        count = 0
        unique = []
        for filename in os.listdir(path):
            full_path = os.path.join(path, filename)
            if os.path.isfile(full_path):
                filehash = self.hash_file(full_path)
                if filehash not in unique: 
                    unique.append(filehash)
                else: 
                    os.remove(full_path)
                    print("[+] removed duplicate " + filename)
            count += 1
            if count % self.check_count == 0:
                print("checking file " + str(filename))
                
    def rename_files(self, path):
        """ Renames files in a directory"""
        temp_name = "temp_" + str(int(random.random()*10000))
        for fname in [temp_name, ""]:
            files = os.listdir(path)
            count = 0
            for index, file in enumerate(files):
                extension = file.split(".")[-1]
                if extension in ["jpg", "png", "gif", "mp4"]:
                    current_fname = (fname + "%05d"%count).encode("utf-8")
                    hashed = hashlib.md5(self.peper + current_fname).hexdigest()
                    os.rename(os.path.join(path, file), os.path.join(path, hashed+"."+extension))
                    count += 1

                    if count % 200 == 0:
                        print("["+str(count)+"]renamed file " + file)

def remove_dupes_main():
    # path
    sort = sort_files()
    path = input("Dir Path: ")
    if path == "":
        path = pathlib.Path(__file__).parent.absolute()
    input("[+] Duplicate files in '"+str(path)+"' will be removed!\nPress enter to continue...")

    # settings
    itter = input("No. itterations: ")
    itter = ["1" if len(itter)==0 else itter][0]
    while itter.isdigit() == False:
        itter = input("Invalid Input!\nNo. itterations: ")

    print("Starting!")
    for i in range(1, int(itter)+1):
        sort.remove_duplicates(path)
        print("[+] Itteration "+str(i)+"/"+str(itter)+" Complete!")

    # rename files
    sort.rename_files(path)

class GUI():
    def __init__(self):
        self.row_counter = 0
        self.select_dir = False
        self.text_input = None

    def get_input_cb(self, win, tk, cb, args):
        result = tk.get(1.0, "end")
        self.text_input = result
        win.withdraw()
        self.top.withdraw()

        txt = (lambda x : "\n".join(x[:-1]) if x[-1]=="" else "\n".join(x))(self.text_input.split("\n"))
        txt = txt.encode("utf-8")
        cb.walk([args[0], args[1], args[2], txt])
        cb.first_100(step=10)

    def get_input(self, cb, args):
        win = tkinter.Tk()
        txt = tkinter.Text(win, height=10)
        txt.pack()

        command = lambda: self.get_input_cb(win, txt, cb, args)
        btn1 = tkinter.Button(win, height=1, width=10, text="Submit", command=command)
        btn1.pack()
    
    def button_callback(self, arg):
        finder = find_largest()

        if self.select_dir == True:
            print("Please Select a directory!")
            return
        
        self.select_dir = True
        user_dir = filedialog.askdirectory()
        
        if arg == 1:
            self.top.withdraw()
            finder.walk([user_dir, False, False, b""])
            finder.first_100(step=10)
        
        elif arg == 2:
            self.top.withdraw()
            finder.walk([user_dir, True, False, b""])
            finder.first_100(step=10)
        
        elif arg == 3:
            self.top.withdraw()
            finder.walk([user_dir, True, True, b""])
            finder.first_100(step=10)
        
        elif arg == 4:
            finder.step_size = 1
            finder.search_by_content = True
            self.get_input(finder, [user_dir, True, False, self.text_input])
            
        elif arg == 5:
            self.get_input(finder, [user_dir, True, False, self.text_input])
            
        elif arg == 6:
            pass
        

    def add_title(self, top, text, colour="black", font_family="Arial", size=32):
        title1 = tkinter.Label(top, fg="black", anchor="n")
        title1.config(font=(font_family, size), text=text)
        title1.grid(row=self.row_counter, sticky=tkinter.W)
        self.row_counter += 1

    def add_button(self, top, text, callback, width=30, arg=[]):
        b_size_1 = tkinter.Button(top, text=text, command=lambda: callback(arg))
        b_size_1.config(width=width)
        b_size_1.grid(row=self.row_counter, sticky=tkinter.W)
        self.row_counter += 1

    def menu(self):
        self.top = tkinter.Tk()
        self.top.title("Datadase Scripts")

        #File/folder sizes
        self.add_title(self.top, "Largest files and folders", font_family="Arial", size=14)
        self.add_button(self.top, "Search by Files", self.button_callback, arg=1)
        self.add_button(self.top, "Search by Folders", self.button_callback, arg=2)
        self.add_button(self.top, "Folders and sub folders (Slow)", self.button_callback, arg=3)
        self.add_title(self.top, " ")

        #Search for file by query
        self.add_title(self.top, "File Search", font_family="Arial", size=14)
        self.add_button(self.top, "Search files by content (Slow)", self.button_callback, arg=4)
        self.add_button(self.top, "Search files by name (fast)", self.button_callback, arg=5)
        self.add_title(self.top, " ")

        #Manage files
        self.add_title(self.top, "Manage Files", font_family="Arial", size=14)
        self.add_button(self.top, "Remove duplicate files by hash", self.button_callback, arg=6)
        self.add_title(self.top, " ")

menu = GUI()
menu.menu()
