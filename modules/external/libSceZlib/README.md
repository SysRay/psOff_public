# How to ZLib

## ZLib thread
From what I see, the Library should create separate thread for ZLib data processing. This thread should be put to sleep by condition variable, when there's no requests (and `sceZlibInflate` should signal this variable ofc).

## sceZlibInitialize
Creates the ZLib processing thread. Passed buffer should not be used, moreover should set it to `nullptr`.

## sceZlibFinalize
Terminates ZLib processing thread and destroys every active request in queue.

## sceZlibInflate
Function `sceZlibInflate` creates decompression request and puts it into queue. The thread should process all the requests and store unpacked data for each request to `dst` pointer set by inflate call. The created request should be stored to `reqId` pointer passed to inflate function.

## sceZlibWaitForDone
Game should call `sceZlibWaitForDone` to check if there's any ready to use inflated blocks, the library stores `reqId` for finished block and returns `Ok`. However there's a `timeout` parameter, this function should return `Err::Zlib::TIMEDOUT` if every decompression request is pending (or the queue is empty).

## sceZlibGetResult
This function returns the decompressed data length and decompression status for specified request. Probably it should lock the thread if data is not ready yet.


## Notes
* There's a hard `64 KiB` limit for decompressed block size, if the decompressed block exceeds this limit, the `sceZlibGetResult` should return `Err::Zlib::NOSPACE` error code.
