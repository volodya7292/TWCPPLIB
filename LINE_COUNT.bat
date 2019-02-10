powershell.exe -command "(dir -include *.cs,*.cpp,*.h,*.hlsl,*.idl,*.asmx -recurse | select-string .).Count"
pause