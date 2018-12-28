names = {"caseDir","systemDir","constantDir","procDir","meshDir"}
for i, name in ipairs(names) do
   print ("Value of",name,":",_G[name])
end
