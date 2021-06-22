import hashlib, os

h = "9ede90fa1ce68e22f34e3b359dae4bfb"

def hash_file(filename):
    with open(filename, "rb") as file:
        md5 = hashlib.md5()
        while True:
            data = file.read(65536)
            if not data:
                break
            md5.update(data)
            
    return md5.hexdigest()

def delete_duplicates(hash_to_delete): 
    for file in os.listdir():
        h = hash_file(file)
        if h == hash_to_delete:
            os.remove(file)
            print(f"deleted file {file}! {h}")

def delete_muli_files(files):
    for file in files:
        try:
            hash_2_delete = hash_file(file)
            print(f"Deleting files: {file} '{hash_2_delete}'!")
            delete_duplicates(hash_2_delete)
        except: pass
