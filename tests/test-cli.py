#!/usr/bin/python

import sys, os
from subprocess import Popen, PIPE

# This test case mimicks QTestLib output.
print '********* Start testing of TestCLI *********'

pathname = os.path.dirname(sys.argv[0])
cmd = pathname + "/../src/miniature"
p = Popen([cmd], bufsize=0, stdin=PIPE, stdout=PIPE, shell = False, env={"LD_LIBRARY_PATH" : pathname + "/../game", "DISPLAY" : ":0"})

testgreen = p.stdout.readline().startswith('Welcome')
if not testgreen :
    print 'FAIL   : TestCLI - start Miniature'
    p.terminate()

if testgreen : 
    p.stdin.write("start\n")
    testgreen = testgreen and p.stdout.readline().startswith("Turn started")

if not testgreen :
    print 'FAIL   : TestCLI - start game'
    p.terminate()
    sys.exit()

if testgreen : 
    p.stdin.write("garbage\n")
    p.stdin.write('move f2-f4\n')
    testgreen = testgreen and p.stdout.readline().startswith("Turn started for")

if not testgreen :
    print 'FAIL   : TestCLI - no new turn'
    p.terminate()

if testgreen :
    p.stdin.write('move f7-f5\n')
    testgreen = testgreen and p.stdout.readline().startswith("Turn started for")

if not testgreen :
    print 'FAIL   : TestCLI - no change of side in turn'
    p.terminate()

if testgreen :
    p.stdin.write('quit\n')
    testgreen = testgreen and p.stdout.readline().startswith("Shutting down")

if not testgreen :
    print 'FAIL   : TestCLI - could not quit'
    p.terminate()

if testgreen :
    print 'PASS   : TestCLI'
    print 'Totals:  1 passed, 0 failed, 0 skipped'
else :
    print 'Totals:  0 passed, 1 failed, 0 skipped'

p.terminate()
print '********* Finished testing of TestCLI *********'
