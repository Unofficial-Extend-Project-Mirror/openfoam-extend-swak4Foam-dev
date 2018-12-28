for d in ["caseDir","systemDir","constantDir","procDir","meshDir"]:
    print "Value of",d,":",
    try:
        print eval(d)
    except NameError:
        print "<undefined>"
        