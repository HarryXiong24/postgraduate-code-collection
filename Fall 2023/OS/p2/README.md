# README

gcloud compute scp --recurse "/Users/harryxiong24/Code/Study/grad-code-collection/OS/p2" ubuntu:

## How to test

```bash
# general test
./cs238

# strict test
strace ./cs238

# and
valgrind --leak-check=full ./cs238
```
