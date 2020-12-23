#!/bin/zsh

helpFunction()
{
   echo ""
   echo "Usage: $0 -f filename -p projectname"
   echo -e "\t-f Name of the file to be compiled"
   echo -e "\t-p Name of the project being compiled"
   exit 1 # Exit script after printing help
}

while getopts "f:p:" opt
do
   case "$opt" in
      f ) filename="$OPTARG" ;;
      p ) projectname="$OPTARG" ;;
      ? ) helpFunction ;; # Print helpFunction in case parameter is non-existent
   esac
done

# Print helpFunction in case parameters are empty
if [ -z "$filename" ] || [ -z "$projectname" ]
then
   echo "Some or all of the parameters are empty";
   helpFunction
fi

cmd.exe /C "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat" \&\& \
    cl.exe /D_USRDLL /D_WINDLL /D${projectname:u}_EXPORTS /Zi /EHsc ${filename} helpers\\\*.cpp /link /DLL /OUT:${filename%.*}.dll
