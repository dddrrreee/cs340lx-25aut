### 340lx setup

Similar to 240LX you need to edit your shell configuration file
(e.g., `.tcshrc`, `.bash_profile`, etc) to set a shell environment variable
`CS340LX_2025_PATH` to the absolute path of where your 340lx class repository
is on your laptop.  


NOTE:
  - Do not put the class repo in a directory has a space!
    This will break various things with unclear errors.  This happened
    to a few MacOS users.


E.g., for me, since I'm running `tcsh` I edit my `.tcshrc` file and add:

    setenv CS340LX_2025_PATH /home/engler/class/cs340lx-25win/

Note the path ends with a `/`.

To the end of it.  

If you use `bash` you'd write this as:

    export CS340LX_2025_PATH=<path to your repo>

And, as before, don't forget to source your configuration file.

As a simple test:

        % cd hello-test
        % make
        ... bunch of stuff ...
        hello: things worked!
        DONE!!!

If this doesn't work:
  1. You likely didn't source the appropriate shell file (easiest is to just
     logout and log back in).
  2. You mispelled `CS340LX_2025_PATH` or gave it the wrong path.

NOTE:
  - Once you set this variable, don't re-install your 340LX repo somewhere
    else without updating it!  We had a hard bug last quarter when a
    student did this and didn't delete the old repo (so all makefiles
    used it and ignored updates to the new one).
