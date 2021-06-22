import cv2, os
from PIL import Image
import numpy as np

class file_sorter():
    def __init__(self):
        self.allowed_extensions = ["png", "jpg", "jpeg", "gif"]
        self.start_fname = "sorted_"
    
    def get_hist(self, filename):
        try:
            im = Image.open(filename)
            if im.mode != "RGB":
                im = im.convert(mode="RGB")
            file = np.frombuffer(im.tobytes(), dtype="uint8").reshape((im.size[1], im.size[0], 3))
            return cv2.calcHist([cv2.cvtColor(file, cv2.COLOR_BGR2GRAY)], [0], None, [16], [0, 256])
        except Exception as error:
            print(f"[-] Failed to read '{filename}'! " + err)

    def compare_photos(self, srt_file):
        srt_hist = self.get_hist(srt_file)
        hists = {}
        count = 0
        for file in os.listdir():
            if file.split(".")[-1] in self.allowed_extensions:
                # get diffrence
                hist = self.get_hist(file)
                diff = sum([abs(int(hist[i] - srt_hist[i])) for i in range(len(hist))])
                hists[diff] = file
                count += 1
                if count % 100 == 0:
                    print(f"[+] compared {count} files!")
        print("[+] Finished comparing files!")
        return hists

    def sort_photos(self):
        photos = self.compare_photos(input('enter comparison filename: '))
        for count, file in enumerate(sorted(photos.keys())):
            try:
                fname = f"{self.start_fname}{count}_" + photos[file]
                os.rename(photos[file], fname)
                if count % 100 == 0:
                    print(f"[+] renamed {count} files!")
            except Exception as error:
                print(f"[-] Failed to rename file {file}! " + error)
        print("[+] Finished renaming files!")
            
    def get_original_names(self):
        count = 0
        for file in os.listdir():
            if file[:7] == self.start_fname:
                if file.split(".")[-1] in self.allowed_extensions:
                    original_name = "_".join(file.replace(self.start_fname, "").split("_")[1:])
                    os.rename(file, original_name)
                    count += 1
                    if count % 100 == 0:
                        print(f"[+] Renamed {count} files!")

while True:
    s = file_sorter()
    menu = input("Menu:\n1. Sort Photos\n2. Get original name\n3. exit\n> ")
    while menu not in ["1", "2", "3"]:
        menu = input("Invalid Input!\n> ")

    if menu == "1":
        s.sort_photos()
    elif menu == "2":
        s.get_original_names()
    elif menu == "3":
        break
