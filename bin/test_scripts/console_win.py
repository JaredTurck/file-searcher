import tkinter, time, threading

class output_window():
    def __init__(self):
        self.row_count = 0
        self.win_width = 200

    def create_window(self):
        self.top = tkinter.Tk()
        self.top.title("Output")
        self.txt = tkinter.Text(self.top, height=60, width=self.win_width)
        self.top.mainloop()

    def log(self, text, error=None):
        self.txt.insert(tkinter.END, text+"\n")
        self.txt.pack()

a = output_window()
threading.Thread(target=a.create_window).start()
#a.log("Hello")

def test_with_sleep():
    for i in range(100):
        a.log(f"hello {i}")
        print(i)
        time.sleep(1)
        

#test_with_sleep()
