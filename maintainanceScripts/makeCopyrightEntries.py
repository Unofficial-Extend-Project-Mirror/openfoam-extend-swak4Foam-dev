#! /usr/bin/env python

import sys,re
from os import listdir,path
import argparse
from subprocess import Popen,PIPE,STDOUT
from collections import defaultdict

# This part is lifted from six.py (https://pythonhosted.org/six/) to
# make sure that this script runs with Python 2 and Python 3

# True if we are running on Python 3.
PY3 = sys.version_info[0] == 3

if PY3:
    import builtins
    print_ = getattr(builtins, "print")
    del builtins
else:
    def print_(*args, **kwargs):
        """The new-style print function."""
        fp = kwargs.pop("file", sys.stdout)
        if fp is None:
            return
        def write(data):
            if not isinstance(data, basestring):
                data = str(data)
            fp.write(data)
        want_unicode = False
        sep = kwargs.pop("sep", None)
        if sep is not None:
            if isinstance(sep, unicode):
                want_unicode = True
            elif not isinstance(sep, str):
                raise TypeError("sep must be None or a string")
        end = kwargs.pop("end", None)
        if end is not None:
            if isinstance(end, unicode):
                want_unicode = True
            elif not isinstance(end, str):
                raise TypeError("end must be None or a string")
        if kwargs:
            raise TypeError("invalid keyword arguments to print()")
        if not want_unicode:
            for arg in args:
                if isinstance(arg, unicode):
                    want_unicode = True
                    break
        if want_unicode:
            newline = unicode("\n")
            space = unicode(" ")
        else:
            newline = "\n"
            space = " "
        if sep is None:
            sep = space
        if end is None:
            end = newline
        for i, arg in enumerate(args):
            if i:
                write(sep)
            write(arg)
        write(end)

parser = argparse.ArgumentParser(description='Check repository for contributors and add them to the initial comment')
parser.add_argument('files', metavar='file/dir', type=str, nargs='+',
                    help='files or directories to be treated')
parser.add_argument('--dry-run', dest='dryRun', action='store_true',
                    default=False,
                    help="Don't change the files")
parser.add_argument('--extensions', dest='extensions', type=str, action='append',
                    default=["py","C","sh","H","ll","yy"],
                    help="Valid extension to investigate. Add to default list: %(default)s")
parser.add_argument('--ignore-files', dest='ignoreList', type=str, action='append',
                    default=["lnInclude","Make"],
                    help="Directories and files that should not be handled. Add to default list: %(default)s")
parser.add_argument('--special-file', dest='special', type=str, action='append',
                    default=["Allwmake","Allwclean","files","options"],
                    help="Files that should be investigated without the right extension. Add to default list: %(default)s")
parser.add_argument('--all-files', dest='allFiles', action='store_true',
                    default=False,
                    help="Check all files even if they don't fit the extension list'")
parser.add_argument('--add-contributor', dest='addContrib', type=str, action='append',
                    default=[],
                    help="Add a contributor entry of the form '<year>,<name>' to the file even if he is not yet in the source file")

args = parser.parse_args()

exts=["."+e for e in args.extensions]

def runHg(*args):
    proc=Popen(("hg",)+args,stdout=PIPE,stderr=STDOUT)
    out=proc.communicate()[0].decode()
    if proc.returncode:
        print_("Problem with: hg "+" ".join(args))
        print_(out)
        sys.exit(0)
    return out

root=runHg("root").strip()
print_(root)
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
swakRevision=" SWAK Revision:"

contribLine=re.compile("\s*(?P<years>[0-9]{4}((-|, )[0-9]{4})*) (?P<name>.+)$")

modifiedFiles=[]

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
                print_("Unparsable year-string",m.groupdict()["years"],"Chocking at",yearString)
            assert start<=end
            for year in range(start,end+1):
                result.add((user,year))
        return result
    else:
        return set()

def buildContributorLine(name,data,prefix="    "):
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

    return prefix+result+" "+name

def processFile(f,data):
    global allContrib

    localContrib=set()
    for c in args.addContrib:
        l=c.split(",")
        if len(l)!=2:
            print_("Entry",c,"is not of the required form <year>,<name>")
            sys.exit(-1)
        user=l[1]
        if user in aliases:
            user=aliases[user]
        localContrib.add((user,int(l[0])))

    for d in data:
        m=churnOut.match(d)
        if m:
            user,year,lines=m.groups()
            if user in aliases:
                user=aliases[user]
            allLines[user]+=int(lines)
            localContrib.add((user,int(year)))
    allContrib |= localContrib

    contrStart=None
    swakLine=None

    lines=open(f).readlines()

    for i,l in enumerate(lines):
        if l.startswith(swakRevision):
            swakLine=i
            if contrStart==None:
                contrStart=i
                lines=lines[:i]+[contribStart+"\n","\n"]+lines[i:]
                swakLine+=2
            break
        elif l.startswith(contribStart):
            contrStart=i
        elif contrStart!=None:
            localContrib|=getContributorsFromLine(l)
            allContrib|=getContributorsFromLine(l)

    if contrStart!=None:
        if swakLine==None:
            print_("No finishing '"+swakRevision+"' found. No contributors added")
            return
        users=list(set([u for u,y in localContrib]))
        cLines=[]
        for u in users:
            cLines.append(buildContributorLine(u,localContrib))
        cLines.sort()
        print_("Adding",len(cLines),"contributor lines to",f)
        if not args.dryRun:
            f=open(f,"w")
            f.writelines(lines[:contrStart+1])
            f.writelines([l+"\n" for l in cLines])
            f.writelines(lines[swakLine-1:])
            f.close()
            modifiedFiles.append(f)

def handleFiles(files):
    for f in files:
        if path.basename(f) in args.ignoreList:
            continue
        if not path.exists(f):
            print_("WARNING: File",f,"does not exist. Don't try this again with me")
        elif path.isdir(f):
            print_("Going into directory",f)
            handleFiles([path.join(f,g) for g in listdir(f)])
        else:
            if args.allFiles or path.basename(f) in args.special or path.splitext(f)[1] in exts:
                if runHg("status","-A",f)[0] not in ["?","I"]:
                    print_("File",f)
                    data=runHg("--config",
                               "extensions.swak="+path.join(root,"maintainanceScripts/lib/swak_mercurial.py"),
                               "churn","-t","'{author} {date|year}'","-q",
                               f).split("\n")
                    processFile(f,data)

handleFiles(args.files)

print
print_("Summary: Lines")
nameLen=max([len(n) for n in allLines.keys()])
tabFormat="%"+str(nameLen)+"s : %s"
print_(tabFormat %("Name","Nr"))
print_("-"*(nameLen+12))
for k in allLines:
    print_(tabFormat % (k,allLines[k]))

if len(modifiedFiles)>0:
    print
    print_(len(modifiedFiles),"files modified")
    print

print_("Years - Contributors")
print_("--------------------")
years=list(set([y for u,y in allContrib]))
years.sort()
for y in years:
    print_(y,":",)
    contributors=set([u for u,yr in allContrib if y==yr])
    print_(", ".join(contributors))
    print

users=list(set([u for u,y in allContrib]))
print_("Contributor lines")
print_("-----------------")
clines=[]
for u in users:
    clines.append(buildContributorLine(u,allContrib))
clines.sort()
print_("\n".join(clines))
