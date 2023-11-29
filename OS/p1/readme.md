# How to test

gcloud compute scp --recurse "/Users/harryxiong24/Code/Study/grad-code-collection/OS/p1" ubuntu:

```bash
# general test
./cs238 "1*2+1/4"

# strict test
strace ./cs238 "1*2+1/4"

# and
valgrind --leak-check=full ./cs238 "1*2+1/4"

strace -ff ./cs238 "1*2+1/4" 2>&1 | grep gcc

strace -ff ./cs238 "1*2+1/4" 2>&1 | grep WIFEXITED
```
