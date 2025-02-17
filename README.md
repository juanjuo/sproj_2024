# sproj_2024
Senior-Project repo

## Building instructions

Since I'm using cmake for my build system, you can create IDE specific project files for development, here's an example of how you would do that for Xcode.

first you will need to copy the repository into a local folder, then call

`mkdir build
cd build
cmake -G Xcode ..`

this will create a project file for Xcode. (make sure you have Cmake version 3.22 or above)
