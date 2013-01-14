from mercurial import templatefilters

from time import localtime

def year(s):
    return localtime(s[0])[0]

def extsetup(ui):
    templatefilters.filters["year"] = year

#  hg --config extensions.swak=maintainanceScripts/lib/swak_mercurial.py
