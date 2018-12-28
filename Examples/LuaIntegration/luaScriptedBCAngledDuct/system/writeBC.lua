print("Lua Writing")
f=io.open(timeDir.."/".."stateFromLua","w")
for i=1,#cnt do
   f:write(string.format("%d\t%d\n",i,cnt[i]))
end
f:close()
