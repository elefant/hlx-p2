# Build
  $ make all

# Run
  $ ./hlx-p2-b

# Notes

1. Synchronization between parent and childA is fairly simple. The parent just
   close the pipe to let childA know it's time to exit.

2. Synchronization between parent and childB is a little bit complicated. The
   state-of-the-art approach should be using a semaphore to lock and unlock
   the shared memory. However, I take the simplest one: polling.