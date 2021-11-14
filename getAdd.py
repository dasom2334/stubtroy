import sys
from ctypes import *
import ctypes

def usage():
	print "\n Get Function Address v1.0\n"
	print "Usage : %s [dll] [proc]" % sys.argv[0]
	print "ex) getAdd.py kernel32.dll WinExec"
	sys.exit()

if len(sys.argv) < 2 : 
	usage()


target_dll = sys.argv[1]
target_function = sys.argv[2]
dll = windll.LoadLibrary(target_dll)
function = windll.kernel32.GetProcAddress(dll._handle, target_function)
print "[##] Find Address %s(%s) : 0x%08x" % (target_dll, target_function, function)