struct entry {
    int tid;
    unsigned long count;
    entry *next;
};

extern thread_local entry *mine;
