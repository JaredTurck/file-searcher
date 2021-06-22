import os, pathlib, hashlib, random

def hash_file(filename):
    md5 = hashlib.md5()
    with open(filename, 'rb') as f:
        while True:
            data = f.read(65536)
            if not data:
                break
            md5.update(data)
            
    return md5.hexdigest()

def remove_duplicates(dir):
    count = 0
    unique = []
    for filename in os.listdir(dir):
        if os.path.isfile(filename):
            filehash = hash_file(filename)
            if filehash not in unique: 
                unique.append(filehash)
            else: 
                os.remove(filename)
                print("[+] removed duplicate " + filename)
        count += 1
        if count % 200 == 0:
            print("checking file " + str(filename))

def rename_files():
    temp_name = "temp_" + str(int(random.random()*10000))
    for fname in [temp_name, ""]:
        path = "/".join(__file__.split("\\")[:-1])
        files = os.listdir(path)

        count = 0
        for index, file in enumerate(files):
            extension = file.split(".")[-1]
            name = ".".join(file.split(".")[0: len(file.split("."))-1])
            if extension in ["jpg", "png", "gif"]:
                os.rename(os.path.join(path, file), os.path.join(path, name+".png"))
                count += 1

                if count % 200 == 0:
                    print("["+str(count)+"]renamed file " + file)
            
# main
path = pathlib.Path(__file__).parent.absolute()
print("[+] Duplicate files in '"+str(path)+"' will be removed!\nPress enter to continue...")

# settings
print("Press CTRL+C within the next 5 seconds to change settings!")
itter = input("No. itterations: ")
if itter == "": itter = "1"
while itter.isdigit() == False:
    itter = input("Invalid Input!\nNo. itterations: ")

print("Starting!")
for i in range(1, int(itter)+1):
    remove_duplicates(path)
    print("[+] Itteration "+str(i)+"/"+str(itter)+" Complete!")

# rename files
rename_files()
