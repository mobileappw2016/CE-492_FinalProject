# CE-492_FinalProject

These are files that are important for the operation of the project.

Here are some helpful notes on how to use the Raspberry Pi for my team members

cd Desktop/Project        //When in the root directory, this changes the directory to the Desktop/Project folder

nano somefilename.txt     // This will open up the nano text editor with that file name. To save to that file name, do crtl-o and type the name you want including the f8ile extension

sudo cp wiegand /var/www  ///This will copy the file to the /var/www directory sudo is needed for this directory

ls                        //list all of the files and folders in the current directory

gcc -o test test.c        //basic compiler for c files, the first part is the name of the executible you want, and the second is the file name

sudo cc -o wiegand wiegand.c -L/usr/local/lib -lwiringPi -lpthread  //This is what is needed to compile the wiegand program specifically

To run the c executable (must be in same directory): sudo ./nameofexecutible
