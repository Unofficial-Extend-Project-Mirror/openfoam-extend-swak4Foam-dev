#! /usr/bin/env python

import sys,re
from os import listdir,path
import argparse
from subprocess import Popen,PIPE,STDOUT
from collections import defaultdict

parser = argparse.ArgumentParser(description='Check repository for contributors and add them to the initial comment')
parser.add_argument('files', metavar='file/dir', type=str, nargs='+',
                    help='files or directories to be treated')
parser.add_argument('--dry-run', dest='dryRun', action='store_true',
                    default=False,
                    help="Don't change the files")
parser.add_argument('--extensions', dest='extensions', type=str, action='append',
                    default=["py","C","sh","H","ll","yy"],
                    help="Valid extension to investigate. Add to default list: %(default)s")
parser.add_argument('--special-file', dest='special', type=str, action='append',
                    default=["Allwmake","Allwclean","files","options"],
                    help="Files that should be investigated without the right extension. Add to default list: %(default)s")
parser.add_argument('--all-files', dest='allFiles', action='store_true',
                    default=False,
                    help="Check all files even if they don't fit the extension list'")

args = parser.parse_args()

exts=["."+e for e in args.extensions]

def runHg(*args):
    proc=Popen(("hg",)+args,stdout=PIPE,stderr=STDOUT)
    out=proc.communicate()[0]
    if proc.returncode:
        print "Problem with: hg "+" ".join(args)
        print out
        sys.exit(0)
    return out

root=runHg("root").strip()
aliases={}
allLines=defaultdict(lambda:0)

with open(path.join(root,".hgchurn")) as chFile:
    for l in chFile.readlines():
        lst=[e.strip() for e in l.split("=")]
        if len(lst)==2:
            aliases[lst[0]]=lst[1]

churnOut=re.compile("'(.+) ([0-9]+)'\s+([0-9]+).*")

allContrib=set()

def processFile(f,data):
    global allContrib

    localContrib=set()
    for d in data:
        m=churnOut.match(d)
        if m:
            user,year,lines=m.groups()
            if user in aliases:
                user=aliases[user]
            allLines[user]+=int(lines)
            localContrib.add((user,int(year)))
    allContrib |= localContrib

def handleFiles(files):
    for f in files:
        if not path.exists(f):
            print "WARNING: File",f,"does not exist. Don't try this again with me"
        elif path.isdir(f):
            print "Going into directory",f
            handleFiles([path.join(f,g) for g in listdir(f)])
        else:
            if args.allFiles or path.basename(f) in args.special or path.splitext(f)[1] in exts:
                if runHg("status","-A",f)[0] not in ["?","I"]:
                    print "File",f
                    data=runHg("--config",
                               "extensions.swak="+path.join(root,"maintainanceScripts/lib/swak_mercurial.py"),
                               "churn","-t","'{author} {date|year}'","-q",
                               f).split("\n")
                    processFile(f,data)

handleFiles(args.files)

print
print "Summary: Lines"
nameLen=max([len(n) for n in allLines.keys()])
tabFormat="%"+str(nameLen)+"s : %s"
print tabFormat %("Name","Nr")
print "-"*(nameLen+12)
for it in allLines.iteritems():
    print tabFormat % it

print
print "Years - Contributors"
print "--------------------"
years=list(set([y for u,y in allContrib]))
years.sort()
for y in years:
    print y,":",
    contributors=set([u for u,yr in allContrib if y==yr])
    print ", ".join(contributors)
    print