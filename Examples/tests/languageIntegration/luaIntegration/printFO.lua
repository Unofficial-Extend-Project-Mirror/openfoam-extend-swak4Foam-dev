print([[
functions {
    luaDynamicOutput
    {
        type luaIntegration;

        startCode "print('Dynamic Starting')";
        executeCode "print('Dynamic Executing')";
        endCode "print('Never been here')";
        //        tolerateExceptions true;
        parallelMasterOnly true;
    }
    luaDynamicOutput2
    {
         $luaDynamicOutput;
         parallelMasterOnly false;
         isParallelized true;
    }
}
]])
