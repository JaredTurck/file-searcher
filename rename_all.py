import os, random

temp_name = "temp_" + str(int(random.random()*10000))

def rename_files():
    for fname in [temp_name, ""]:
        path = "/".join(__file__.split("\\")[:-1])
        files = os.listdir(path)


        count = 0
        for index, file in enumerate(files):
            extension = file.split(".")[-1]
            if extension in ["jpg", "png", "gif", "mp4"]:
                os.rename(os.path.join(path, file), os.path.join(path, fname + "%05d"%count+"."+extension))
                count += 1

                if count % 100 == 0:
                    print("["+str(count)+"]renamed file " + file)

        input("Press enter to close...")

rename_files()
