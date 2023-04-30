typedef struct LLNode {
    void * data;
    struct LLNode * next;
    struct LLNode * prev;
} LLNode;

typedef struct LinkedList {
    LLNode *head;
    LLNode *tail;
    int count;
} LinkedList;

LinkedList * createLinkedList();
void llInsert(LinkedList * list, void * data);
void llDelete(LinkedList * ll, LLNode * node);