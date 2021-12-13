#ifndef ALLOCATION_TRACKER_H
#define ALLOCATION_TRACKER_H

class AllocationTracker {
    public:
    AllocationTracker() { ++_count; };
    AllocationTracker(const AllocationTracker&) { ++_count; };
    ~AllocationTracker() { --_count; };
    static uint32_t count() { return _count; };
    bool operator<(const AllocationTracker&) const { return false; };

    private:
    static uint32_t _count;
};

uint32_t AllocationTracker::_count = 0;

#endif // ALLOCATION_TRACKER_H