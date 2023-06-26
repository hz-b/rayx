import sys 
import os 
import subprocess 
import tempfile

#Only execute inside IDE with rayx as root
def checkForTerminal():
    cwd = os.getcwd()
    TerminalApp_Path = 'build/bin/TerminalApp'
    path_to_terminal = 'build/bin/'
    test = os.path.join(cwd,TerminalApp_Path)
    return os.path.exists(test),test,path_to_terminal

def main(): 
    exists, path, path_to_terminal_dir = checkForTerminal()
    if not(exists):
        print("Check for build!")
        return
    else :
        BATCHS = 100
        for i in range(100):
            print(f"Running [{i}]")
            with tempfile.TemporaryFile() as tempf:
                proc = subprocess.Popen([path,"-i",path_to_terminal_dir+str('METRIX_U41_G1_H1_318eV_PS_MLearn_BM.rml'),">"],stdout=tempf)
                proc.wait()
                tempf.seek(0)
                tempf.read()
    return 


# main
if __name__ == '__main__':
    main()