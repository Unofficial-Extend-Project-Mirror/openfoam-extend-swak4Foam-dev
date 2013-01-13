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

contribStart="Contributors/Copyright:"
contribLine=re.compile("\s*(?P<years>[0-9]{4}((-|, )[0-9]{4})*) (?P<name>.+)$")

def getContributorsFromLine(line):
    m=contribLine.match(line)
    if m:
        user=m.groupdict()["name"]
        if user in aliases:
            user=aliases[user]
        yearString=m.groupdict()["years"]
        result=set()
        while len(yearString)>0:
            start=int(yearString[0:4])
            yearString=yearString[4:]
            if len(yearString)==0:
                end=start
            elif yearString[0:2]==", ":
                end=start
                yearString=yearString[2:]
            elif yearString[0]=="-":
                end=int(yearString[1:5])
                yearString=yearString[5:]
                if len(yearString)>0:
                    assert yearString[0:2]==", "
                    yearString=yearString[2:]
            else:
                print "Unparsable year-string",m.groupdict()["years"],"Chocking at",yearString
            assert start<=end
            for year in range(start,end+1):
                result.add((user,year))
        return result
    else:
        return set()

def buildContributorLine(name,data):
    years=list(set((y for u,y in data if u==name)))
    years.sort()
    result=""
    while len(years)>0:
        start=years[0]
        end=start
        years=years[1:]
        for i,y in enumerate(years):
            if y!=end+1:
                years=years[i:]
                break
            else:
                end=y
                if i==len(years)-1:
                    years=[]
        result+=str(start)
        if start!=end:
            result+="-"+str(end)
        if len(years)>0:
            result+=", "

    return result+" "+name

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

users=list(set([u for u,y in allContrib]))
print "Contributor lines"
print "-----------------"
clines=[]
for u in users:
    clines.append(buildContributorLine(u,allContrib))
clines.sort()
print "\n".join(clines)
