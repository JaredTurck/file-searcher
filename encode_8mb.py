import os

fname = "2020-04-05_17-02-19_UTC.mp4"

def gen_cmd(fname):
    cmd = 'ffmpeg -y -i "'+fname+'" -b:v {bitrateV} -b:a 20k -qscale 0 "new_'+fname+'"  -af "volumedetect" -vn -sn -dn -f null -c:v h264_nvenc -r 8 -filter:v fps=fps=8'
    file_size = os.popen('ffprobe -i "'+fname+'" -show_entries format=duration -v quiet -of csv="p=0"').read()
    bitrate = lambda file_size : str(int(((8 * 1024 * 1024 * 8) / float(file_size.replace('\n',''))) / 1024)- 20) + "k"
    return cmd.replace("{bitrateV}", bitrate(file_size))

for file in os.listdir():
    if file.split(".")[-1] in ["mp4", "mov", "webm"]:
        command = gen_cmd(file)
        os.system(command)
        print("finished processing file '" + file + "'!")
