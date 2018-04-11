# EmbeddedHM

http://www.qnx.com/developers/docs/6.6.0.update/#com.qnx.doc.neutrino.lib_ref/topic/p/pthread_exit.html

The pthread_exit() function terminates the calling thread. If the thread is joinable, the value value_ptr is made available to any thread joining the terminating thread (only one thread can get the return status). If the thread is detached, all system resources allocated to the thread are immediately reclaimed.
