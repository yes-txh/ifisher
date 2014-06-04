# Using the os module to rename and remove files
 
import os, string
 
def replace(fileName, text, replace_text):
    back = os.path.splitext(fileName)[0] + ".bak"
    temp = os.path.splitext(fileName)[0] + ".tmp"
 
    try:
        fi = open(fileName)
        fo = open(temp, "w")
    except os.error:
        print "No such file, or something error"
    else:
        for s in fi:
            fo.write(string.replace(s, text, replace_text))
        fi.close()
        fo.close()
 
    try:
        os.remove(back)
    except os.error:
        print "remove file error."
 
    try:
        os.rename(fileName, back)
    except os.error:
        print "rename file error."
 
    try:
        os.rename(temp, fileName)
    except os.error:
        print "write failed"

if __name__ == "__main__":
        file = "kvm.xml"
        replace(file,"T_IMG_LOCATION","hello")
