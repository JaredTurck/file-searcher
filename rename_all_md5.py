import os, random, hashlib

peper = b"JaredBot1234_"
temp_name = "temp_" + str(int(random.random()*10000))

def rename_files():
    for fname in [temp_name, ""]:
        path = "/".join(__file__.split("\\")[:-1])
        files = os.listdir(path)


        count = 0
        for index, file in enumerate(files):
            extension = file.split(".")[-1]
            if extension in ["jpg", "png", "gif", "mp4"]:
                current_fname = (fname + "%05d"%count).encode("utf-8")
                hashed = hashlib.md5(peper + current_fname).hexdigest()
                os.rename(os.path.join(path, file), os.path.join(path, hashed+"."+extension))
                count += 1

                if count % 100 == 0:
                    print("["+str(count)+"]renamed file " + file)

        input("Press enter to close...")

rename_files()
