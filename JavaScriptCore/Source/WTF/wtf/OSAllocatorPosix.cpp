/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "OSAllocator.h"

#include "PageAllocation.h"
#include <errno.h>
#include <sys/mman.h>
#include <wtf/Assertions.h>
#include <wtf/UnusedParam.h>

namespace WTF {

#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
OSAllocator::Callback sAllocate = 0;
OSAllocator::Callback sRelease = 0;
OSAllocator::Callback sCommit = 0;
OSAllocator::Callback sDecommit = 0;
OSAllocator::StartCallback sStart = 0;
#endif

void* OSAllocator::reserveUncommitted(size_t bytes, Usage usage, bool writable, bool executable, bool includesGuardPages)
{
#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
    if (sStart)
        sStart();
#endif
#if OS(QNX)
    // Reserve memory with PROT_NONE and MAP_LAZY so it isn't committed now.
    void* result = mmap(0, bytes, PROT_NONE, MAP_LAZY | MAP_PRIVATE | MAP_ANON, -1, 0);
    if (result == MAP_FAILED)
        CRASH();
#else // OS(QNX)

    void* result = reserveAndCommit(bytes, usage, writable, executable, includesGuardPages);
#if OS(LINUX)
    madvise(result, bytes, MADV_DONTNEED);
#elif HAVE(MADV_FREE_REUSE)
    // To support the "reserve then commit" model, we have to initially decommit.
    while (madvise(result, bytes, MADV_FREE_REUSABLE) == -1 && errno == EAGAIN) { }
#endif

#endif // OS(QNX)
#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
    if (sDecommit)
        sDecommit(result, bytes);
#endif

    return result;
}

void* OSAllocator::reserveAndCommit(size_t bytes, Usage usage, bool writable, bool executable, bool includesGuardPages)
{
#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
    if (sStart)
        sStart();
#endif
    // All POSIX reservations start out logically committed.
    int protection = PROT_READ;
    if (writable)
        protection |= PROT_WRITE;
    if (executable)
        protection |= PROT_EXEC;

    int flags = MAP_PRIVATE | MAP_ANON;
#if PLATFORM(IOS)
    if (executable)
        flags |= MAP_JIT;
#endif

#if (OS(LINUX) && CPU(X86_64))
    // Linux distros usually do not allow overcommit by default, so
    // JSC's strategy of mmaping a large amount of memory upfront
    // won't work very well on some systems. Fortunately there's a
    // flag we can pass to mmap to disable the overcommit check for
    // this particular call, so we can get away with it as long as the
    // overcommit flag value in /proc/sys/vm/overcommit_memory is 0
    // ('heuristic') and not 2 (always check). 0 is the usual default
    // value, so this should work well in general.
    flags |= MAP_NORESERVE;
#endif

#if OS(DARWIN)
    int fd = usage;
#else
    int fd = -1;
#endif

    void* result = 0;
#if (OS(DARWIN) && CPU(X86_64))
    if (executable) {
        ASSERT(includesGuardPages);
        // Cook up an address to allocate at, using the following recipe:
        //   17 bits of zero, stay in userspace kids.
        //   26 bits of randomness for ASLR.
        //   21 bits of zero, at least stay aligned within one level of the pagetables.
        //
        // But! - as a temporary workaround for some plugin problems (rdar://problem/6812854),
        // for now instead of 2^26 bits of ASLR lets stick with 25 bits of randomization plus
        // 2^24, which should put up somewhere in the middle of userspace (in the address range
        // 0x200000000000 .. 0x5fffffffffff).
        intptr_t randomLocation = 0;
        randomLocation = arc4random() & ((1 << 25) - 1);
        randomLocation += (1 << 24);
        randomLocation <<= 21;
        result = reinterpret_cast<void*>(randomLocation);
    }
#endif

    result = mmap(result, bytes, protection, flags, fd, 0);
    if (result == MAP_FAILED) {
#if ENABLE(CLASSIC_INTERPRETER) || ENABLE(LLINT)
        if (executable)
            result = 0;
        else
    #endif
            CRASH();
    }
    if (result && includesGuardPages) {
        // We use mmap to remap the guardpages rather than using mprotect as
        // mprotect results in multiple references to the code region.  This
        // breaks the madvise based mechanism we use to return physical memory
        // to the OS.
        mmap(result, pageSize(), PROT_NONE, MAP_FIXED | MAP_PRIVATE | MAP_ANON, fd, 0);
        mmap(static_cast<char*>(result) + bytes - pageSize(), pageSize(), PROT_NONE, MAP_FIXED | MAP_PRIVATE | MAP_ANON, fd, 0);
    }

#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
    if (sAllocate)
        sAllocate(result, bytes);
#endif

    return result;
}

void OSAllocator::commit(void* address, size_t bytes, bool writable, bool executable)
{
#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
    if (sStart)
        sStart();
#endif
#if OS(QNX)
    int protection = PROT_READ;
    if (writable)
        protection |= PROT_WRITE;
    if (executable)
        protection |= PROT_EXEC;
    if (MAP_FAILED == mmap(address, bytes, protection, MAP_FIXED | MAP_PRIVATE | MAP_ANON, -1, 0))
        CRASH();
#elif OS(LINUX)
    UNUSED_PARAM(writable);
    UNUSED_PARAM(executable);
    madvise(address, bytes, MADV_WILLNEED);
#elif HAVE(MADV_FREE_REUSE)
    UNUSED_PARAM(writable);
    UNUSED_PARAM(executable);
    while (madvise(address, bytes, MADV_FREE_REUSE) == -1 && errno == EAGAIN) { }
#else
    // Non-MADV_FREE_REUSE reservations automatically commit on demand.
    UNUSED_PARAM(address);
    UNUSED_PARAM(bytes);
    UNUSED_PARAM(writable);
    UNUSED_PARAM(executable);
#endif

#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
    if (sCommit)
        sCommit(address, bytes);
#endif
}

void OSAllocator::decommit(void* address, size_t bytes)
{
#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
    if (sStart)
        sStart();
#endif
#if OS(QNX)
    // Use PROT_NONE and MAP_LAZY to decommit the pages.
    mmap(address, bytes, PROT_NONE, MAP_FIXED | MAP_LAZY | MAP_PRIVATE | MAP_ANON, -1, 0);
#elif OS(LINUX)
    madvise(address, bytes, MADV_DONTNEED);
#elif HAVE(MADV_FREE_REUSE)
    while (madvise(address, bytes, MADV_FREE_REUSABLE) == -1 && errno == EAGAIN) { }
#elif HAVE(MADV_FREE)
    while (madvise(address, bytes, MADV_FREE) == -1 && errno == EAGAIN) { }
#elif HAVE(MADV_DONTNEED)
    while (madvise(address, bytes, MADV_DONTNEED) == -1 && errno == EAGAIN) { }
#else
    UNUSED_PARAM(address);
    UNUSED_PARAM(bytes);
#endif

#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
    if (sDecommit)
        sDecommit(address, bytes);
#endif
}

void OSAllocator::releaseDecommitted(void* address, size_t bytes)
{
#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
    if (sStart)
        sStart();
#endif
    int result = munmap(address, bytes);
    if (result == -1)
        CRASH();

#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
    if (sRelease)
        sRelease(address, bytes);
#endif
}

#ifdef NRDP_JSC_ENABLE_OSALLOCATOR_CALLBACKS
void OSAllocator::setCallbacks(Callback allocate, Callback release, Callback commit, Callback decommit, StartCallback start)
{
    sAllocate = allocate;
    sRelease = release;
    sCommit = commit;
    sDecommit = decommit;
    sStart = start;
}
#endif
} // namespace WTF
