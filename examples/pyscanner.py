import pyfalcon
import sys

if len(sys.argv) < 3:
    print('{} <path to CsMl.dll> <path to PE file>'.format(sys.argv[0]))
    sys.exit()

def MyLogger(message):
    r = message.static_analysis_response
    if r != None:
        print("Verdict: " + str(r.verdict))
        print("Score  : " + str(r.score))
        
servlet = pyfalcon.Servlet(sys.argv[1])
servlet.logger = MyLogger

file = pyfalcon.File(sys.argv[2])
servlet.scan_file(file)
del file