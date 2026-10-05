import pyfalcon
import sys

if len(sys.argv) < 3:
    print('{} <path to CsMl.dll> <path to PE file>'.format(sys.argv[0]))
    sys.exit(1)

def MyLogger(message):
    r = message.static_analysis_response
    if r is not None:
        print("Hash      : " + r.response_hash)
        print("Verdict   : " + str(r.verdict))
        print("Subverdict: " + str(r.subverdict))
        print("Score     : " + str(r.score))
        
try:
    scanner = pyfalcon.Scanner(sys.argv[1])
    scanner.logger = MyLogger
    file = pyfalcon.File(sys.argv[2])
    scanner.scan_file(file)
except pyfalcon.FalconException as error:
    print(str(error), file=sys.stderr)
    sys.exit(1)
