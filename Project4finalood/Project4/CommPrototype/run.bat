

@ECHO OFF &SETLOCAL
for %%x in (%*) do call set "Myvar=%%Myvar%% %%x" 
ECHO %Myvar%
cd Debug
start  "../Debug/MsgClient.exe" MsgClient %Myvar%
start  "../Debug/MsgServer.exe" MsgServer 
cd ..


