<<<<<<< HEAD
#!/bin/bash

gcc -Wall $final.c -lpthreads && ./a.out 100 1000
echo
=======
#!/bin/bash
gcc -pthread final.c
./a.out 100 1000
>>>>>>> f2530e5ca135c74885c98798fcdd967601750dd2
